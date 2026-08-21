#include "World/MapControl.h"
#include "Systems/Inventory/CombatInventory.h"
#include "UI/Screens/Attributes/AttributesScreen.h"
#include "UI/Screens/Diary/DiaryScreen.h"
#include "UI/Screens/Menu/MenuScreen.h"
#include "UI/Screens/Pause/PauseScreen.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputDispatcher.h"

#include "UI/Screens/WorldMap/WorldMapScreen.h"
#include "Systems/Combat/Combat.h"
#include "Systems/Combat/CombatRaycasterUIImpl.h"
#include "Systems/Progression/Progression.h"
#include "Core/Engine/Debug.h"
#include "Core/Utils/InputControl.h"
#include "Core/Utils/RandomGenerator.h"
#include "Core/Utils/RendererProvider.h"
#include "Domain/Characters/Races/BaseRace.h"
#include "World/Systems/MapPhysics.h"
#include "UI/Renderers/3D/EngineRaycaster/Raycaster.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterWorld.h"
#include "UI/PerspectiveManager.h"
#include "Systems/Minigames/HackConsole.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <fstream>
#include <cmath>

#include "World/Systems/MapRenderer.h"
#include "World/Systems/MapInputController.h"

namespace {
    std::string extractBaseColorFromRaycaster(char cell, const std::string& mapTitle, bool isForest) {
        // [PT-BR] Sampleia a textura no meio do bloco (tx=33, ty=33) para evitar linhas escuras de rejunte
// [EN-US] Samples texture in the middle of block (tx=33, ty=33) to avoid dark grout/shadow lines
        std::vector<std::tuple<int, int, int>> emptyLights;
        Pixel3D px = RaycasterWorld::getInternalWallPixel(mapTitle, isForest, 0.0f, 10.0f, cell, 33, 0, 64, 33.0f/64.0f, 0.0f, emptyLights, 0.0f, 0.0f);
        return "\033[38;2;" + std::to_string(px.r) + ";" + std::to_string(px.g) + ";" + std::to_string(px.b) + "m";
    }
}

static bool s_justChangedMap = false;
static float s_cameraPosX3D = -1.0f;
static float s_cameraPosY3D = -1.0f;
static float s_cameraAngle3D = 0.0f;
static std::string s_currentMapTitle = "";
static std::vector<std::string> s_currentMapMatrix;

void MapControl::signal3DMapChange() { s_justChangedMap = true; }
bool MapControl::is3DExplorationActive() { return PerspectiveManager::getInstance().is3DViewActive(); }
float MapControl::getCameraPostX3D() { return s_cameraPosX3D; }
float MapControl::getCameraPostY3D() { return s_cameraPosY3D; }
float MapControl::getCameraAngle3D() { return s_cameraAngle3D; }
std::string MapControl::getCurrentMapTitle() { return s_currentMapTitle; }
std::vector<std::string> MapControl::getCurrentMapMatrix() { return s_currentMapMatrix; }

// [PT-BR] Processamento de input e comandos delegado ao modulo correspondente
// [EN-US] Input and command processing delegated to corresponding module

// [PT-BR] Aplicacao de limites de mapa delegada para FisicaMapa
// [EN-US] Application of map boundaries delegated to MapPhysics
void MapControl::processCombat(
    Character* currentPlayer, std::vector<std::string>& currentMapMatrix, 
    int& playerPositionX, int& playerPositionY, bool& isExplorationActive,
    const std::string& combatTitle, const std::string& warningMessage, std::vector<std::unique_ptr<Character>> enemiesForBattle, 
    int positionXAfterCombat, int positionYAfterCombat, int initialEnemyPositionX, int occupiedCellsQuantity, int /*terminalWidth*/, const std::function<void()>& restoreScreen)
{
    Appearance::startPopupInteraction();
    std::vector<std::string> text = { 
        Appearance::color(Color::YELLOW) + "[!] " + warningMessage
    };
    std::vector<std::string> combatOptions = { "Nao, recuar", "Sim, batalha!" };
    
    int optionChosenByPlayer = 0;
    if (RendererProvider::get()) {
        optionChosenByPlayer = RendererProvider::get()->readMenuSelectionInPopup(combatTitle, text, combatOptions, Color::RED);
    } else {
        optionChosenByPlayer = InputControl::readMenuSelectionInPopup(combatTitle, text, combatOptions, Color::RED);
    }

    if (optionChosenByPlayer == 1) {
        std::unique_ptr<ICombatUI> ui = nullptr;
        if (PerspectiveManager::getInstance().is3DViewActive()) {
            ui = std::make_unique<CombatRaycasterUIImpl>();
        }
        
        Combat combat(currentPlayer, std::move(enemiesForBattle), std::move(ui));
        if (PerspectiveManager::getInstance().is3DViewActive()) {
            combat.set3DContext(true, currentMapMatrix, s_cameraPosX3D, s_cameraPosY3D, s_cameraAngle3D, s_currentMapTitle);
        }
        combat.startCombat();

        if (currentPlayer->getHealth() > 0) {
            for (int i = 0; i < occupiedCellsQuantity; ++i) currentMapMatrix[positionYAfterCombat][initialEnemyPositionX + i] = '.';
            playerPositionX = positionXAfterCombat;
            playerPositionY = positionYAfterCombat;
        }
    }

    if (isExplorationActive && !PerspectiveManager::getInstance().is3DViewActive()) restoreScreen();
}

// [PT-BR] Animacao de introducao do mapa delegada para AnimadorMapa
// [EN-US] Map introduction animation delegated to MapAnimator}


// [PT-BR] Efeito visual de flashbang delegado para AnimadorMapa
// [EN-US] Flashbang visual effect delegated to MapAnimator

// [PT-BR] Funcoes de camera e renderizacao 3D abstraidas para RenderizadorMapa
// [EN-US] 3D camera and rendering functions abstracted to MapRenderer

std::string MapControl::formatCell(char cell, int x, int y, const std::string& mapTitle, const std::vector<std::string>& mapMatrix, bool isMinimap) {
    thread_local std::string lastTitle = "";
    thread_local std::string upperTitle = "";
    thread_local bool isKingdom = false, isInterior = false, isForest = false, isVillage = false, isSpawn = false;

    if (lastTitle != mapTitle) {
        lastTitle = mapTitle;
        upperTitle = mapTitle;
        for (char& ch : upperTitle) ch = std::toupper(static_cast<unsigned char>(ch));
        
        isKingdom = (upperTitle.find("Reino") != std::string::npos || upperTitle.find("REINO") != std::string::npos);
        isInterior = (upperTitle.find("LABIRINTO") != std::string::npos || upperTitle.find("CHEFE") != std::string::npos || upperTitle.find("CORACAO") != std::string::npos || upperTitle.find("CAVERNA") != std::string::npos);
        isForest = (upperTitle.find("FLORESTA") != std::string::npos);
        isVillage = (upperTitle.find("VILA") != std::string::npos);
        isSpawn = (upperTitle.find("INICIO") != std::string::npos);
    }
    
    // [PT-BR] --- ESTETICA ENGINE IDE (VISAO TERMINAL) ---
// [EN-US] --- IDE ENGINE AESTHETICS (TERMINAL VIEW) ---
    bool isEngineIDE = !isMinimap && !PerspectiveManager::getInstance().is3DViewActive();
    if (isEngineIDE) {
        std::string npcs = "GOBFPMSTRCH";
        if (npcs.find(cell) == std::string::npos && cell != ' ' && !RaycasterWorld::isMapLabel(x, y, mapMatrix)) {
            
            if (cell == '.' && (!isInterior || upperTitle.find("CHEFE") != std::string::npos || upperTitle.find("CORACAO") != std::string::npos)) {
                return "\033[38;2;40;40;40m·\033[0m"; // Rastro do chao para a IDE
            }

            const char syntaxChars[] = "{};/*<>&|!=";
            int idx = (x * 7 + y * 13) % (sizeof(syntaxChars) - 1);
            char ideChar = syntaxChars[idx];
            
            // [PT-BR] Paleta de cores de sintaxe (Tema Dark+)
// [EN-US] Syntax Colors (VSCode Dark+ Theme)
            const char* colors[] = {
                "\033[38;2;86;156;214m",   // Blue
                "\033[38;2;197;134;192m",  // Purple
                "\033[38;2;220;220;170m",  // Yellow
                "\033[38;2;78;201;176m",   // Teal
                "\033[38;2;214;157;133m"   // Orange
            };
            int cIdx = (x * 3 + y * 11) % 5;
            
            if (isForest || cell == '*' || cell == '#') {
                return "\033[38;2;96;139;78m" + std::string(1, ideChar) + "\033[0m"; // Comentario Verde
            }
            if (cell == '~') {
                return "\033[38;2;86;156;214m" + std::string(1, '~') + "\033[0m"; // Agua continua azul mas com string base
            }
            if (cell == '^') {
                return "\033[38;2;197;134;192m" + std::string(1, '^') + "\033[0m"; // Teleporte roxo IDE
            }
            
            return std::string(colors[cIdx]) + std::string(1, ideChar) + "\033[0m";
        }
    }

    // Teleporte
    if (cell == '^') return Appearance::color(Color::BOLD, Color::TELEPORT) + "^" + Appearance::color(Color::RESET);
    
    // Agua
    if (cell == '~') return Appearance::colorRGB(50, 150, 255) + "≈" + Appearance::color(Color::RESET);
    
    // [PT-BR] Arvores e Vegetacao
    // [EN-US] Trees and Vegetation
    if (cell == '*') {
        bool isTrunk = false;
        if (y > 0 && mapMatrix[y-1][x] == '*') {
            int countHorizontal = 0;
            if (x > 0 && mapMatrix[y][x-1] == '*') countHorizontal++;
            if (x + 1 < static_cast<int>(mapMatrix[y].length()) && mapMatrix[y][x+1] == '*') countHorizontal++;
            if (countHorizontal <= 1) isTrunk = true;
        }
        if (isTrunk) return Appearance::color(Color::WOOD) + "█" + Appearance::color(Color::RESET);
        return Appearance::color(Color::GREEN) + "▲" + Appearance::color(Color::RESET);
    }
    
    // [PT-BR] Verifica caracteres de rotulo de mapa antes de processar entidades
    // [EN-US] Checks map label characters before processing entities
    if (RaycasterWorld::isMapLabel(x, y, mapMatrix)) {
        return Appearance::color(Color::GRAY) + std::string(1, cell) + Appearance::color(Color::RESET);
    }
    
    // [PT-BR] Renderizacao de Entidades no mapa
    // [EN-US] Entity Rendering on map
    if (isVillage || isSpawn) {
        if (cell == 'G' || cell == 'O') return Appearance::color(Color::BOLD, Color::RED) + std::string(1, cell) + Appearance::color(Color::RESET); // Inimigos Vermelhos
        if (cell == 'B') return Appearance::color(Color::BOLD, Color::CYAN) + "B" + Appearance::color(Color::RESET); // Bjorn Ciano
        if (cell == 'F' && x > 0 && mapMatrix[y][x-1] == '{') return Appearance::color(Color::BOLD, Color::YELLOW) + "F" + Appearance::color(Color::RESET); // Franchesco Amarelo
        if (cell == 'P') return Appearance::color(Color::BOLD, Color::BROWN) + "P" + Appearance::color(Color::RESET); // Placa Marrom
    } else if (isForest) {
        if (cell == 'S' && (!isInterior || upperTitle.find("CHEFE") != std::string::npos)) return Appearance::color(Color::BOLD, Color::RED) + "S" + Appearance::color(Color::RESET);
        if (cell == 'F' || cell == 'A') return Appearance::color(Color::BOLD, Color::RED) + std::string(1, cell) + Appearance::color(Color::RESET);
        if (cell == 'M') return Appearance::color(Color::BOLD, Color::MAGENTA) + "M" + Appearance::color(Color::RESET);
        if (cell == 'B') return Appearance::color(Color::BOLD, Color::GOLD) + "B" + Appearance::color(Color::RESET);
    } else if (isKingdom) {
        if (cell == 'T') return Appearance::color(Color::BOLD, Color::RED) + "T" + Appearance::color(Color::RESET);
        if (cell == 'G') return Appearance::color(Color::BOLD, Color::YELLOW) + "G" + Appearance::color(Color::RESET);
        if (cell == 'C') return Appearance::color(Color::BOLD, Color::SILVER) + "C" + Appearance::color(Color::RESET);
    }
    
    if (upperTitle == "SALA DO CHEFE" && (cell == 'M' || cell == 'A' || cell == 'H' || cell == 'O' || cell == 'R' || cell == 'G')) {
        return Appearance::color(Color::BOLD, Color::WHITE) + std::string(1, cell) + Appearance::color(Color::RESET);
    }
    
    // [PT-BR] Casas e Estruturas no mapa
    // [EN-US] Houses and Structures on map
    if (!isInterior && !isKingdom) {
        std::string structureColor = extractBaseColorFromRaycaster('|', upperTitle, isForest);
        
        if (cell == '_') return structureColor + "▄" + Appearance::color(Color::RESET);
        if (cell == '|' || cell == '[' || cell == ']') return structureColor + "█" + Appearance::color(Color::RESET);
        std::string structures = "{}/\\<>;=-:+";
        if (structures.find(cell) != std::string::npos) return structureColor + std::string(1, cell) + Appearance::color(Color::RESET);
        
        if (cell == '#') {
            if (isForest) return Appearance::color(Color::GREEN) + "█" + Appearance::color(Color::RESET); // Arvores continuam verdes
            std::string wallColor = extractBaseColorFromRaycaster('#', upperTitle, isForest);
            return wallColor + "█" + Appearance::color(Color::RESET);
        }
    }
    
    // [PT-BR] Elementos do Reino
    // [EN-US] Kingdom Elements
    if (isKingdom) {
        if (cell == '|') return Appearance::color(Color::WOOD) + "█" + Appearance::color(Color::RESET); // Portao de madeira
        std::string structures = "_[]{}/\\<>;=-+#";
        if (structures.find(cell) != std::string::npos) {
            std::string kingdomColor = extractBaseColorFromRaycaster(cell, upperTitle, isForest);
            return kingdomColor + "█" + Appearance::color(Color::RESET);
        }
    }
    
    // [PT-BR] Elementos do Labirinto
    // [EN-US] Maze Elements
    if (isInterior) {
        if (upperTitle.find("LABIRINTO") != std::string::npos) {
            std::string mazeColor = extractBaseColorFromRaycaster('|', upperTitle, isForest);
            auto isHWall = [](char c) { return c == '=' || c == '.' || c == '\''; };
            auto isVWall = [](char c) { return c == '|' || c == '+' || c == 'S' || c == 'E'; };

            if (cell == '=') return mazeColor + "─" + Appearance::color(Color::RESET);
            if (cell == '|') {
                bool right = (x + 1 < static_cast<int>(mapMatrix[y].length()) && isHWall(mapMatrix[y][x+1]));
                bool left = (x > 0 && isHWall(mapMatrix[y][x-1]));
                if (right && left) return mazeColor + "┼" + Appearance::color(Color::RESET);
                if (right) return mazeColor + "├" + Appearance::color(Color::RESET);
                if (left) return mazeColor + "┤" + Appearance::color(Color::RESET);
                return mazeColor + "│" + Appearance::color(Color::RESET);
            }
            if (cell == '.') {
                bool right = (x + 1 < static_cast<int>(mapMatrix[y].length()) && isHWall(mapMatrix[y][x+1]));
                bool left = (x > 0 && isHWall(mapMatrix[y][x-1]));
                bool down = (y + 1 < static_cast<int>(mapMatrix.size()) && isVWall(mapMatrix[y+1][x]));
                
                if (left && right && down) return mazeColor + "┬" + Appearance::color(Color::RESET);
                if (right && down) return mazeColor + "┌" + Appearance::color(Color::RESET);
                if (left && down) return mazeColor + "┐" + Appearance::color(Color::RESET);
                if (left && right) return mazeColor + "─" + Appearance::color(Color::RESET);
                return mazeColor + "█" + Appearance::color(Color::RESET);
            }
            if (cell == '\'') {
                bool right = (x + 1 < static_cast<int>(mapMatrix[y].length()) && isHWall(mapMatrix[y][x+1]));
                bool left = (x > 0 && isHWall(mapMatrix[y][x-1]));
                bool up = (y > 0 && isVWall(mapMatrix[y-1][x]));
                
                if (left && right && up) return mazeColor + "┴" + Appearance::color(Color::RESET);
                if (right && up) return mazeColor + "└" + Appearance::color(Color::RESET);
                if (left && up) return mazeColor + "┘" + Appearance::color(Color::RESET);
                if (left && right) return mazeColor + "─" + Appearance::color(Color::RESET);
                return mazeColor + "█" + Appearance::color(Color::RESET);
            }
            if (cell == '+') {
                bool right = (x + 1 < static_cast<int>(mapMatrix[y].length()) && isHWall(mapMatrix[y][x+1]));
                bool left = (x > 0 && isHWall(mapMatrix[y][x-1]));
                bool down = (y + 1 < static_cast<int>(mapMatrix.size()) && isVWall(mapMatrix[y+1][x]));
                bool up = (y > 0 && isVWall(mapMatrix[y-1][x]));
                
                if (left && right && down && up) return mazeColor + "┼" + Appearance::color(Color::RESET);
                if (left && right && down) return mazeColor + "┬" + Appearance::color(Color::RESET);
                if (left && right && up) return mazeColor + "┴" + Appearance::color(Color::RESET);
                if (up && down && left) return mazeColor + "┤" + Appearance::color(Color::RESET);
                if (up && down && right) return mazeColor + "├" + Appearance::color(Color::RESET);
                return mazeColor + "┼" + Appearance::color(Color::RESET);
            }
        }
        else if (upperTitle.find("CAVERNA") != std::string::npos) {
            std::string caveColor = extractBaseColorFromRaycaster('#', upperTitle, isForest);
            if (cell == '#') return caveColor + "█" + Appearance::color(Color::RESET);
            if (cell == '.') {
                if (isMinimap) return "\033[38;2;50;50;50m.\033[0m";
                return "\033[38;2;40;40;40m·\033[0m";
            }
        }
    }
    
    // [PT-BR] Renderizacao do chao e rotulos
    // [EN-US] Ground and label rendering
    if (cell == '.' && (!isInterior || upperTitle.find("CHEFE") != std::string::npos || upperTitle.find("CORACAO") != std::string::npos)) {
        if (isMinimap) return "\033[38;2;50;50;50m.\033[0m";
        return "\033[38;2;40;40;40m·\033[0m";
    }
    
    if (std::isalpha(cell) && cell != ' ' && cell != 'S' && cell != 'F' && cell != 'A' && cell != 'M' && cell != 'B' && cell != 'T' && cell != 'G' && cell != 'C') {
        return Appearance::color(Color::GRAY) + std::string(1, cell) + Appearance::color(Color::RESET);
    }
    
    return std::string(1, cell);
}

// [PT-BR] Rotina renderizarMapa delegada para RenderizadorMapa
// [EN-US] renderMap routine delegated to MapRenderer

NextMapTransition MapControl::executeExplorationLoop(
    Character* currentPlayer,
    std::vector<std::string>& currentMapMatrix,
    int& playerPositionX,
    int& playerPositionY,
    bool& isExplorationActive,
    const std::string& currentMapTitle,
    const std::function<std::string()>& getEnemySymbols,
    const std::function<std::vector<std::string>()>& getOriginalLayout,
    const std::function<void(int, int, int)>& processInteraction,
    const std::function<std::string(char, int, int)>& formatter,
    const std::function<void()>& restoreScreen,
    int& initialLineToDrawMap,
    bool& needsRender
) {
    auto lastEnemyMovement = std::chrono::steady_clock::now();
    NextMapTransition fastTravelDestination = NextMapTransition::None;
    s_cameraAngle3D = 0.0f;
    s_cameraPosX3D = -1.0f;
    s_cameraPosY3D = -1.0f;
    s_currentMapTitle = currentMapTitle;
    s_currentMapMatrix = currentMapMatrix;

    while (isExplorationActive && currentPlayer->getHealth() > 0)
    {
        s_currentMapTitle = currentMapTitle;
        
        auto now = std::chrono::steady_clock::now();
        bool timeToMoveEnemies = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastEnemyMovement).count() >= 800;

        if (timeToMoveEnemies) {
            MapPhysics::moveEnemiesRandomly(currentMapMatrix, getOriginalLayout(), getEnemySymbols(), playerPositionX, playerPositionY);
            lastEnemyMovement = std::chrono::steady_clock::now();
            needsRender = true;
        }

        int terminalWidth = Appearance::getTerminalWidth();
        
        if (needsRender && !PerspectiveManager::getInstance().is3DViewActive()) {
            int terminalHeight = Appearance::getTerminalHeight();

            MapRenderer::renderMap(currentMapMatrix, playerPositionX, playerPositionY, terminalWidth, terminalHeight, initialLineToDrawMap, formatter);

            needsRender = false;
        }

        char keyPressedByPlayer = '\0';
        bool processInput = false;

            if (!PerspectiveManager::getInstance().is3DViewActive() && InputControl::pressedKey()) {
            keyPressedByPlayer = InputControl::readKey();
            processInput = true;
        }

        if (PerspectiveManager::getInstance().is3DViewActive() || (processInput && (keyPressedByPlayer == 'v' || keyPressedByPlayer == 'V'))) {
            static std::string previousTitle = "";
            int animationType = 0;
            
            bool changingPerspective = !PerspectiveManager::getInstance().is3DViewActive();
            if (changingPerspective) {
                animationType = 1;
            }
            if (s_justChangedMap && !changingPerspective) {
                animationType = 2;
            }
            
            if (previousTitle != currentMapTitle) {
                if (previousTitle != "" && !changingPerspective) {
                    animationType = 2;
                }
                previousTitle = currentMapTitle;
            }

            s_justChangedMap = false;
            if (!PerspectiveManager::getInstance().is3DViewActive()) {
                PerspectiveManager::getInstance().toggleView();
            }

                if (s_cameraPosX3D == -1.0f || static_cast<int>(s_cameraPosX3D) != playerPositionX || static_cast<int>(s_cameraPosY3D) != playerPositionY) {
                    s_cameraPosX3D = static_cast<float>(playerPositionX) + 0.5f;
                    s_cameraPosY3D = static_cast<float>(playerPositionY) + 0.5f;
                }
                
                int hitX = -1, hitY = -1;
                char pendingAction = Raycaster::start3DExploration(currentMapMatrix, s_cameraPosX3D, s_cameraPosY3D, s_cameraAngle3D, currentMapTitle, currentPlayer, hitX, hitY, animationType);
                
                playerPositionX = static_cast<int>(s_cameraPosX3D);
                playerPositionY = static_cast<int>(s_cameraPosY3D);
                
                bool isTrigger = false;
                if (hitX != -1 && hitY != -1) {
                    MapPhysics::applyMapLimits(hitX, hitY, currentMapMatrix);
                    
                    char cell = currentMapMatrix[hitY][hitX];
                    // [PT-BR] Verifica se o jogador parou sobre um gatilho (inimigos, teleporte ou terminal)
                    // [EN-US] Checks if player landed on a trigger (enemies, teleport, or terminal)
                    std::string triggers = "^GOBFSAMTHRPCIQ@";
                    if (triggers.find(cell) != std::string::npos) {
                        isTrigger = true;
                    }
                    
                    int posXBefore = playerPositionX;
                    int posYBefore = playerPositionY;
                    
                    if (cell == '@') {
                        if (HackConsole::startHack(currentPlayer)) {
                            // [PT-BR] Remove o terminal do mapa apos hackeado com sucesso
                            // [EN-US] Removes terminal from map after successful hack
                            currentMapMatrix[hitY][hitX] = '.';
                            for(int dy = -5; dy <= 5; dy++) {
                                for(int dx = -5; dx <= 5; dx++) {
                                    if(hitY+dy >= 0 && hitY+dy < static_cast<int>(currentMapMatrix.size()) && hitX+dx >= 0 && hitX+dx < static_cast<int>(currentMapMatrix[0].size())) {
                                        if (currentMapMatrix[hitY+dy][hitX+dx] == '!' || currentMapMatrix[hitY+dy][hitX+dx] == '%') {
                                            currentMapMatrix[hitY+dy][hitX+dx] = '.';
                                        }
                                    }
                                }
                            }
                            RaycasterWorld::updateMapHash(currentMapMatrix);
                        }
                    } else {
                        // [PT-BR] Processa interacao ou combate caso o jogador tenha colidido com entidade
                        // [EN-US] Processes interaction or combat if player collided with entity
                        processInteraction(hitX, hitY, terminalWidth);
                    }
                    
                    // [PT-BR] Empurra o jogador para tras para evitar ficar preso na celula da entidade
                    // [EN-US] Pushes player back to prevent getting stuck on entity cell
                    if (isTrigger && PerspectiveManager::getInstance().is3DViewActive() && playerPositionX == posXBefore && playerPositionY == posYBefore) {
                        s_cameraPosX3D = static_cast<float>(hitX) + 0.5f - cos(s_cameraAngle3D) * 1.5f;
                        s_cameraPosY3D = static_cast<float>(hitY) + 0.5f - sin(s_cameraAngle3D) * 1.5f;
                        playerPositionX = static_cast<int>(s_cameraPosX3D);
                        playerPositionY = static_cast<int>(s_cameraPosY3D);
                    }
                }
                
                if (pendingAction == 'M') {
                    keyPressedByPlayer = 'M';
                    processInput = true;
                } else if (!isTrigger) {
                    restoreScreen();
                    needsRender = true;
                    continue;
                } else {
                    continue;
                }
        }

        if (processInput) {
            if (keyPressedByPlayer == 'v' || keyPressedByPlayer == 'V') {
                continue;
            }

            if (keyPressedByPlayer == 'm' || keyPressedByPlayer == 'M') {
                MapLocation loc = MapLocation::InitialVillage;
                std::string upperTitle = currentMapTitle;
                std::transform(upperTitle.begin(), upperTitle.end(), upperTitle.begin(), ::toupper);
                
                if (upperTitle.find("FLORESTA") != std::string::npos || 
                    upperTitle.find("BOSQUE") != std::string::npos ||
                    upperTitle.find("LABIRINTO") != std::string::npos ||
                    upperTitle.find("CHEFE") != std::string::npos ||
                    upperTitle.find("ARVORE") != std::string::npos) {
                    loc = MapLocation::Forest;
                } else if (upperTitle.find("PONTE") != std::string::npos) {
                    loc = MapLocation::KingdomBridge;
                } else if (upperTitle.find("REINO") != std::string::npos || upperTitle.find("IGREJA") != std::string::npos) {
                    loc = MapLocation::Kingdom;
                }
                
                int villageProgress = Progression::instance().getVillageProgress(currentPlayer);
                int forestProgress = Progression::instance().getForestProgress(currentPlayer);
                int kingdomBridgeProgress = Progression::instance().getKingdomBridgeProgress(currentPlayer);
                int kingdomProgress = Progression::instance().getKingdomProgress(currentPlayer);

                NextMapTransition destination = WorldMapScreen::display(currentPlayer, loc, villageProgress, forestProgress, kingdomBridgeProgress, kingdomProgress);

                if (destination != NextMapTransition::None) {
                    fastTravelDestination = destination;
                    // [PT-BR] Sinaliza encerramento da exploracao para processar a viagem
                    // [EN-US] Signals exploration exit to process fast travel
                    isExplorationActive = false;
                    break;
                }
                // [PT-BR] Se nenhum destino foi selecionado, restaura a tela e prossegue exploracao
                // [EN-US] If no destination was selected, restores screen and continues exploration
                if (!PerspectiveManager::getInstance().is3DViewActive()) {
                    restoreScreen();
                    needsRender = true;
                }
                continue;
            }

            int nextPositionX = playerPositionX;
            int nextPositionY = playerPositionY;

            if (MapInputController::processInputAndCommands(keyPressedByPlayer, currentPlayer, nextPositionX, nextPositionY, restoreScreen)) continue;
            
            if (currentPlayer->getReturnToMenu()) break;

            MapPhysics::applyMapLimits(nextPositionX, nextPositionY, currentMapMatrix);
            processInteraction(nextPositionX, nextPositionY, terminalWidth);
            
            needsRender = true;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }

    return fastTravelDestination;
}
