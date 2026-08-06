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
    igrejaJaFoiVisitada(false)
{
    currentMapMatrix = Map4KingdomLayouts::getKingdomLayout();
    MapLoader::standardizeMapSize(currentMapMatrix);
    savedMainMapMatrix = currentMapMatrix; // Caso necessario
}

Map4Kingdom::~Map4Kingdom() = default;

NextMapTransition Map4Kingdom::startExplorationLoop()
{
    MapLoader::standardizeMapSize(currentMapMatrix);
    Appearance::hideCursor();

    auto formatter = [&](char cell, int x, int y) -> std::string {
        if (x == playerPositionX && y == playerPositionY) {
            char ic = Appearance::customPlayerIcon;
            if (ic <= 32 || ic > 126) ic = '@'; // Garante caractere visivel
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
        
        // Garante que o titulo seja estritamente REINO para a bandeira
        if (currentMapTitle == "REINO" || currentMapTitle.find("Reino") != std::string::npos || currentMapTitle.find("REINO") != std::string::npos) {
            currentMapTitle = "REINO";
            titleArt = Map4KingdomLayouts::getKingdomSoon();
            artWidth = 77;
        }
        initialLineToDrawMap = MapAnimator::animateMapIntroduction(currentMapTitle, titleArt, artWidth, {}, 0, Color::PURPLE, currentMapMatrix, playerPositionX, playerPositionY, formatter, true, true, nullptr);
    };

    animateScreen();

    std::unordered_map<char, std::function<void(int, int, int)>> interactions;

    // Triggers e Teleportes
    interactions['^'] = [&](int px, int py, int width) {
        if (playerIsInsideSubMap) {
            // Saindo da Igreja (submapa) de volta para o patio do Reino
            if (px == 18 && py == 3) {
                currentMapMatrix = savedMainMapMatrix;
                playerPositionX = savedPositionXBeforeEnteringSubMap;
                playerPositionY = savedPositionYBeforeEnteringSubMap;
                playerIsInsideSubMap = false;
                currentMapTitle = "REINO";
                restoreScreen();
            }
        } else {
            // Retornar para o Reino (Ponte)
            if (py > 30) {
                isExplorationActive = false;
                nextMap = NextMapTransition::Kingdom;
            }
            // Entrada do Palacio (agora no X=43, Y=1)
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

    // Entrada da Igreja
    interactions['I'] = [&](int px, int py, int width) {
        if (!playerIsInsideSubMap) {
            MapLoader::enterSubMap(
                currentMapMatrix, savedMainMapMatrix,
                savedPositionXBeforeEnteringSubMap, savedPositionYBeforeEnteringSubMap,
                playerPositionX, playerPositionY, playerIsInsideSubMap,
                currentMapTitle, savedChurchMapMatrix, igrejaJaFoiVisitada,
                Map4KingdomLayouts::getChurchLayout(), 17, 3, "IGREJA DO REINO", restoreScreen
            );
        }
    };

    // Padre da Igreja (so funciona dentro do submapa da igreja)
    interactions['P'] = [&](int px, int py, int width) {
        if (playerIsInsideSubMap) {
            NPCPriest priest;
            priest.interact(currentPlayer);
            Diary::instance().registerNPC("Padre Benedito");
            if (isExplorationActive && !MapControl::is3DExplorationActive()) restoreScreen();
        }
    };

    // Loja do Franchesco
    interactions['F'] = [&](int px, int py, int width) {
        NPCMerchant franchesco;
        franchesco.interact(currentPlayer);
        Diary::instance().registerNPC("Franchesco (Mercador)");
        if (isExplorationActive && !MapControl::is3DExplorationActive()) restoreScreen();
    };

    // Forja do Bjorn
    interactions['B'] = [&](int px, int py, int width) {
        NPCBlacksmith bjorn;
        bjorn.interact(currentPlayer);
        Diary::instance().registerNPC("Bjorn (Ferreiro)");
        if (isExplorationActive && !MapControl::is3DExplorationActive()) restoreScreen();
    };

    // Cavaleiro Generico (Treino)
    interactions['C'] = [&](int px, int py, int width) {
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

            // Restaura o progresso de XP e Ouro para garantir que nao ganhe nada permanente
            currentPlayer->setCurrentXp(xpBefore);
            int goldAfter = currentPlayer->getInventory()->getGold();
            currentPlayer->getInventory()->addGold(goldBefore - goldAfter);
        }
        
        if (isExplorationActive && !MapControl::is3DExplorationActive()) restoreScreen();
    };

    // Loja de aparencia (Anok)
    interactions['N'] = [&](int px, int py, int width) {
        NPCAppearance appearance;
        appearance.interact(currentPlayer);
        Diary::instance().registerNPC("Anok (Estilista)");
        if (isExplorationActive && !MapControl::is3DExplorationActive()) restoreScreen();
    };

    // Alquimista
    interactions['Q'] = [&](int px, int py, int width) {
        NPCAlchemist alchemist;
        alchemist.interact(currentPlayer);
        Diary::instance().registerNPC("Alquimista Real");
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
            
            // Transicao para a Ponte do Reino (saida sul do mapa)
            if (!playerIsInsideSubMap && py >= currentMapMatrix.size() - 3) {
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
