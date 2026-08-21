#include "World/Kingdom/Map4Kingdom.h"
#include "Core/Engine/Debug.h"
#include "World/Kingdom/Map4KingdomLayout.h"
#include "Core/Engine/GameMenu.h"
#include "Core/Utils/Appearance.h"
#include "World/MapControl.h"
#include "World/Systems/MapAnimator.h"
#include "World/Systems/MapLoader.h"
#include "Core/Utils/InputControl.h"
#include "Domain/NPCs/NPCMerchant.h"
#include "Domain/NPCs/NPCBlacksmith.h"
#include "Domain/NPCs/NPCAppearance.h"
#include "Domain/NPCs/NPCGenericKnight.h"
#include "Domain/NPCs/NPCAlchemist.h"
#include "Domain/NPCs/NPCPriest.h"
#include "Systems/Progression/Diary.h"
#include "Systems/Progression/Progression.h"
#include "Systems/Progression/ProgressionFlags.h"
#include "Systems/Combat/Combat.h"
#include "World/Systems/CommonMapInteractions.h"

#include <unordered_map>
#include <functional>
#include <algorithm>
#include <iostream>

Map4Kingdom::Map4Kingdom(Character* playerCharacter) :
    playerPositionX(41), 
    playerPositionY(41),
    currentPlayer(playerCharacter), 
    isExplorationActive(true), 
    currentMapTitle("REINO"),
    nextMap(NextMapTransition::None),
    playerIsInsideSubMap(false),
    churchAlreadyVisited(false)
{
    currentMapMatrix = Map4KingdomLayouts::getKingdomLayout();
    MapLoader::standardizeMapSize(currentMapMatrix);
    // [PT-BR] Salva a matriz principal para restauracao ao sair de submapas
    // [EN-US] Saves main matrix for restoration when exiting submaps
    savedMainMapMatrix = currentMapMatrix;
}

Map4Kingdom::~Map4Kingdom() = default;

NextMapTransition Map4Kingdom::startExplorationLoop()
{
    // [PT-BR] Resgata a posicao do jogador caso tenha usado Viagem Rapida de dentro de um submapa
    // [EN-US] Rescues player position if Fast Travel was used while inside a submap
    if (playerIsInsideSubMap) {
        currentMapMatrix = savedMainMapMatrix;
        playerPositionX = (savedPositionXBeforeEnteringSubMap > 0) ? (savedPositionXBeforeEnteringSubMap + 1) : 33;
        playerPositionY = (savedPositionYBeforeEnteringSubMap > 0) ? savedPositionYBeforeEnteringSubMap : 60;
        playerIsInsideSubMap = false;
        currentMapTitle = "REINO";
    }

    MapLoader::standardizeMapSize(currentMapMatrix);
    Appearance::hideCursor();

    auto formatter = [&](char cell, int x, int y) -> std::string {
        if (x == playerPositionX && y == playerPositionY) {
            char ic = Appearance::customPlayerIcon;
            // [PT-BR] Garante caractere visivel para o icone do jogador
    // [EN-US] Ensures visible character for player icon
    if (ic <= 32 || ic > 126) ic = '@';
            return Appearance::color(Appearance::customPlayerColor) + std::string(1, ic) + Appearance::color(Color::RESET);
        }
        return MapControl::formatCell(cell, x, y, currentMapTitle, currentMapMatrix, false);
    };

    int initialLineToDrawMap = 0;

    auto restoreScreen = [&]() {
        initialLineToDrawMap = MapAnimator::animateMapIntroduction(currentMapTitle, {}, 0, {}, 0, Color::PURPLE, currentMapMatrix, playerPositionX, playerPositionY, formatter, false, true, nullptr);
    };

    auto animateScreen = [&]() {
        std::vector<std::string> titleArt;
        int artWidth = 0;
        
        // [PT-BR] Garante que o titulo seja estritamente REINO para a bandeira
    // [EN-US] Ensures title is strictly KINGDOM for the banner
        if (currentMapTitle == "REINO" || currentMapTitle.find("Reino") != std::string::npos || currentMapTitle.find("REINO") != std::string::npos) {
            currentMapTitle = "REINO";
            titleArt = Map4KingdomLayouts::getKingdomLogo();
            artWidth = 77;
        }
        initialLineToDrawMap = MapAnimator::animateMapIntroduction(currentMapTitle, titleArt, artWidth, {}, 0, Color::PURPLE, currentMapMatrix, playerPositionX, playerPositionY, formatter, true, true, nullptr);
    };

    animateScreen();

    std::unordered_map<char, std::function<void(int, int, int)>> interactions;

    // [PT-BR] Gatilhos e Teleportes do Reino
    // [EN-US] Kingdom Triggers and Teleports
    interactions['^'] = [&](int px, int py, [[maybe_unused]] int width) {
        if (playerIsInsideSubMap) {
            // [PT-BR] Transicao: Saindo da Igreja de volta para o patio do Reino
            // [EN-US] Transition: Exiting Church back to Kingdom courtyard
            if (px == 18 && py == 3) {
                currentMapMatrix = savedMainMapMatrix;
                playerPositionX = savedPositionXBeforeEnteringSubMap + 1; // Coluna 33 (avenida aberta fora da porta)
                playerPositionY = savedPositionYBeforeEnteringSubMap;     // Linha 60
                playerIsInsideSubMap = false;
                currentMapTitle = "REINO";
                restoreScreen();
            }
        } else {
            // [PT-BR] Transicao: Retornar para a Ponte do Reino
            // [EN-US] Transition: Return to Kingdom Bridge
            if (py > 30) {
                isExplorationActive = false;
                nextMap = NextMapTransition::KingdomBridge;
            }
            // [PT-BR] Transicao: Entrada do Palacio Real
            // [EN-US] Transition: Royal Palace Entrance
            else if (py == 1 && (px >= 40 && px <= 45)) {
                Appearance::startPopupInteraction();
                std::vector<std::string> msg = {
                    "Os grandes portoes do Palacio Real estao selados por runas magicas.",
                    "Uma barreira intransponivel impede sua passagem por enquanto.",
                    "A aventura continuara em breve..."
                };
                Appearance::displayPopup("PORTAO DO PALACIO", msg, Color::YELLOW);
                playerPositionX = px;
                playerPositionY = py + 1; // Recua um passo
                restoreScreen();
            }
        }
    };

    // [PT-BR] Transicao: Entrada da Igreja da Capital
    // [EN-US] Transition: Capital Church Entrance
    interactions['I'] = [&]([[maybe_unused]] int px, [[maybe_unused]] int py, [[maybe_unused]] int width) {
        if (!playerIsInsideSubMap) {
            MapLoader::enterSubMap(
                currentMapMatrix, savedMainMapMatrix,
                savedPositionXBeforeEnteringSubMap, savedPositionYBeforeEnteringSubMap,
                playerPositionX, playerPositionY, playerIsInsideSubMap,
                currentMapTitle, savedChurchMapMatrix, churchAlreadyVisited,
                Map4KingdomLayouts::getChurchLayout(), 17, 3, "IGREJA DO REINO", restoreScreen
            );
        }
    };

    // [PT-BR] Interacao: Padre da Igreja (so funciona dentro do submapa da igreja)
    // [EN-US] Interaction: Church Priest (only functions inside church submap)
    interactions['P'] = [&]([[maybe_unused]] int px, [[maybe_unused]] int py, [[maybe_unused]] int width) {
        if (playerIsInsideSubMap) {
            NPCPriest priest;
            priest.interact(currentPlayer);
            Diary::instance().registerNPC("Padre Benedito");
            if (isExplorationActive && !MapControl::is3DExplorationActive()) restoreScreen();
        }
    };

    // [PT-BR] Registro de interacoes padrao de NPCs (Franchesco, Bjorn, Anok, Alquimista)
    // [EN-US] Registration of standard NPC interactions (Franchesco, Bjorn, Anok, Alchemist)
    CommonMapInteractions::registerStandardNPCs(interactions, currentPlayer, isExplorationActive, restoreScreen);

    // [PT-BR] Interacao: Cavaleiro Real para Treino
    // [EN-US] Interaction: Royal Knight for Training
    interactions['C'] = [&]([[maybe_unused]] int px, [[maybe_unused]] int py, [[maybe_unused]] int width) {
        Appearance::startPopupInteraction();
        std::vector<std::string> lines = {
            "Saudacoes, guerreiro!",
            "Deseja treinar suas habilidades em um combate amistoso?",
            "Esta luta nao concede experiencia (XP) ou recompensas permanentes,",
            "mas serve como um otimo teste de suas taticas."
        };
        int choice = InputControl::readMenuSelectionInPopup("TREINO DE COMBATE", lines, {"Aceitar Treino", "Recusar"}, Color::GRAY);
        if (choice == 0) {
            std::vector<std::unique_ptr<Character>> enemies;
            enemies.push_back(NPCGenericKnight::createKnight("Cavaleiro de Treino"));

            int xpBefore = currentPlayer->getCurrentXp();
            int goldBefore = currentPlayer->getInventory()->getGold();

            Combat combat(currentPlayer, std::move(enemies));
            if (MapControl::is3DExplorationActive()) {
                combat.set3DContext(
                    true, 
                    currentMapMatrix, 
                    MapControl::getCameraPostX3D(), 
                    MapControl::getCameraPostY3D(), 
                    MapControl::getCameraAngle3D(), 
                    MapControl::getCurrentMapTitle()
                );
            }
            combat.startCombat();

            // [PT-BR] Restaura XP e Ouro para garantir combate puramente de treino
            // [EN-US] Restores XP and Gold to ensure training-only combat
            currentPlayer->setCurrentXp(xpBefore);
            int goldAfter = currentPlayer->getInventory()->getGold();
            currentPlayer->getInventory()->addGold(goldBefore - goldAfter);
        }
        
        if (isExplorationActive && !MapControl::is3DExplorationActive()) restoreScreen();
    };

    auto processInteraction = [&](int px, int py, int width) {
        char destinationCell = currentMapMatrix[py][px];
        auto it = interactions.find(destinationCell);
        if (it != interactions.end()) {
            it->second(px, py, width);
        } else if ((destinationCell != '*' && destinationCell != '#' && destinationCell != '=' && destinationCell != '|' && destinationCell != '[' && destinationCell != ']' && destinationCell != ' ') || Debug::isNoclipActive) {
            playerPositionX = px;
            playerPositionY = py;
            
            // [PT-BR] Transicao para a Ponte do Reino (saida sul do mapa)
            // [EN-US] Transition to Kingdom Bridge (southern map exit)
            if (!playerIsInsideSubMap && py >= static_cast<int>(currentMapMatrix.size()) - 3) {
                nextMap = NextMapTransition::KingdomBridge;
                isExplorationActive = false;
            }
        }
    };

    bool needsRender = true;
    NextMapTransition fastTravelDestination = MapControl::executeExplorationLoop(
        currentPlayer, currentMapMatrix, playerPositionX, playerPositionY,
        isExplorationActive, currentMapTitle, []() { return ""; },
        [this]() -> std::vector<std::string> { 
            if (playerIsInsideSubMap) return Map4KingdomLayouts::getChurchLayout();
            return Map4KingdomLayouts::getKingdomLayout();
        },
        processInteraction, formatter, restoreScreen,
        initialLineToDrawMap, needsRender
    );

    if (fastTravelDestination != NextMapTransition::None) {
        return fastTravelDestination;
    }

    if (currentPlayer->getHealth() <= 0 || currentPlayer->getReturnToMenu()) {
        return NextMapTransition::ReturnMenu;
    }
    return nextMap;
}
