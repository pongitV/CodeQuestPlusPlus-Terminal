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
#include "World/Systems/MapCameraController.h"
#include "World/Systems/MapCellFormatter.h"

void MapControl::signal3DMapChange() { MapCameraController::signal3DMapChange(); }
bool MapControl::is3DExplorationActive() { return MapCameraController::is3DExplorationActive(); }
float MapControl::getCameraPostX3D() { return MapCameraController::getCameraPostX3D(); }
float MapControl::getCameraPostY3D() { return MapCameraController::getCameraPostY3D(); }
float MapControl::getCameraAngle3D() { return MapCameraController::getCameraAngle3D(); }
std::string MapControl::getCurrentMapTitle() { return MapCameraController::getCurrentMapTitle(); }
std::vector<std::string> MapControl::getCurrentMapMatrix() { return MapCameraController::getCurrentMapMatrix(); }

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
            combat.set3DContext(true, currentMapMatrix, MapCameraController::getCameraPostX3D(), MapCameraController::getCameraPostY3D(), MapCameraController::getCameraAngle3D(), MapCameraController::getCurrentMapTitle());
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
// [EN-US] Map introduction animation delegated to MapAnimator

// [PT-BR] Efeito visual de flashbang delegado para AnimadorMapa
// [EN-US] Flashbang visual effect delegated to MapAnimator

// [PT-BR] Funcoes de camera e renderizacao 3D abstraidas para RenderizadorMapa
// [EN-US] 3D camera and rendering functions abstracted to MapRenderer

std::string MapControl::formatCell(char cell, int x, int y, const std::string& mapTitle, const std::vector<std::string>& mapMatrix, bool isMinimap) {
    return MapCellFormatter::formatCell(cell, x, y, mapTitle, mapMatrix, isMinimap);
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
    float cameraAngle3D = 0.0f;
    float cameraPosX3D = -1.0f;
    float cameraPosY3D = -1.0f;
    MapCameraController::setCameraState(cameraPosX3D, cameraPosY3D, cameraAngle3D, currentMapTitle, currentMapMatrix);

    while (isExplorationActive && currentPlayer->getHealth() > 0)
    {
        MapCameraController::setCameraState(cameraPosX3D, cameraPosY3D, cameraAngle3D, currentMapTitle, currentMapMatrix);
        
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
            if (MapCameraController::hasJustChangedMap() && !changingPerspective) {
                animationType = 2;
            }
            
            if (previousTitle != currentMapTitle) {
                if (previousTitle != "" && !changingPerspective) {
                    animationType = 2;
                }
                previousTitle = currentMapTitle;
            }

            MapCameraController::resetMapChangeFlag();
            if (!PerspectiveManager::getInstance().is3DViewActive()) {
                PerspectiveManager::getInstance().toggleView();
            }

            if (cameraPosX3D == -1.0f || static_cast<int>(cameraPosX3D) != playerPositionX || static_cast<int>(cameraPosY3D) != playerPositionY) {
                cameraPosX3D = static_cast<float>(playerPositionX) + 0.5f;
                cameraPosY3D = static_cast<float>(playerPositionY) + 0.5f;
            }
            
            int hitX = -1, hitY = -1;
            char pendingAction = Raycaster::start3DExploration(currentMapMatrix, cameraPosX3D, cameraPosY3D, cameraAngle3D, currentMapTitle, currentPlayer, hitX, hitY, animationType);
            
            MapCameraController::setCameraState(cameraPosX3D, cameraPosY3D, cameraAngle3D, currentMapTitle, currentMapMatrix);
            playerPositionX = static_cast<int>(cameraPosX3D);
            playerPositionY = static_cast<int>(cameraPosY3D);
            
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
                    cameraPosX3D = static_cast<float>(hitX) + 0.5f - cos(cameraAngle3D) * 1.5f;
                    cameraPosY3D = static_cast<float>(hitY) + 0.5f - sin(cameraAngle3D) * 1.5f;
                    MapCameraController::setCameraPos(cameraPosX3D, cameraPosY3D);
                    playerPositionX = static_cast<int>(cameraPosX3D);
                    playerPositionY = static_cast<int>(cameraPosY3D);
                } else if (static_cast<int>(cameraPosX3D) != playerPositionX || static_cast<int>(cameraPosY3D) != playerPositionY) {
                    // Sincroniza a camera 3D com o jogador caso a interacao (ex: teleporte/troca de mapa) tenha mudado as coordenadas
                    cameraPosX3D = static_cast<float>(playerPositionX) + 0.5f;
                    cameraPosY3D = static_cast<float>(playerPositionY) + 0.5f;
                    MapCameraController::setCameraPos(cameraPosX3D, cameraPosY3D);
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
