#include "World/Kingdom/Map3KingdomBridge.h"
#include "Core/Engine/Debug.h"

#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <unordered_map>

#include "Core/Engine/GameMenu.h"
#include "UI/Screens/Menu/MenuScreen.h"
#include "Core/Utils/Appearance.h"
#include "World/MapControl.h"
#include "World/Systems/MapAnimator.h"
#include "World/Systems/MapLoader.h"
#include "Core/Engine/EnemyCreator.h"
#include "World/Kingdom/Map3KingdomBridgeLayout.h"
#include "Domain/NPCs/NPCGenericKnight.h"


Map3KingdomBridge::Map3KingdomBridge(Character* playerCharacter) :
    playerPositionX(47), 
    playerPositionY(32),
    currentPlayer(playerCharacter), 
    isExplorationActive(true), 
    currentMapTitle("PONTE DO REINO"),
    nextMap(NextMapTransition::None)
{
    currentMapMatrix = Map3KingdomBridgeLayouts::getKingdomBridgeLayout();
    MapLoader::standardizeMapSize(currentMapMatrix);
}

Map3KingdomBridge::~Map3KingdomBridge() = default;

NextMapTransition Map3KingdomBridge::startExplorationLoop()
{
    bool trollDefeated = false;
    bool invitationReceived = false;

    MapLoader::standardizeMapSize(currentMapMatrix);

    Appearance::hideCursor();

    auto formatter = [&](char cell, int x, int y) -> std::string {
        if (x == playerPositionX && y == playerPositionY) {
            char ic = Appearance::customPlayerIcon;
            // [PT-BR] Garante que o icone do jogador seja um caractere visivel
    // [EN-US] Ensures player icon is a visible character
    if (ic <= 32 || ic > 126) ic = '@';
            return Appearance::color(Appearance::customPlayerColor) + std::string(1, ic) + Appearance::color(Color::RESET);
        }
        return MapControl::formatCell(cell, x, y, currentMapTitle, currentMapMatrix, false);
    };

    int initialLineToDrawMap = 0;

    auto restoreScreen = [&]() {
        initialLineToDrawMap = MapAnimator::animateMapIntroduction(currentMapTitle, {}, 0, {}, 0, Color::CYAN, currentMapMatrix, playerPositionX, playerPositionY, formatter, false, true, nullptr);
    };

    auto animateScreen = [&]() {
        std::vector<std::string> titleArt;
        int artWidth = 0;
        std::vector<std::string> transArt;
        int transWidth = 0;

        if (currentMapTitle == "PONTE DO REINO" || currentMapTitle == "CAMINHO DO Reino") {
            titleArt = Map3KingdomBridgeLayouts::getKingdomBridgeLogo();
            // [PT-BR] Largura da arte ASCII da ponte (150 colunas)
    // [EN-US] Width of bridge ASCII art (150 columns)
    artWidth = 150;
            transArt = Map3KingdomBridgeLayouts::getKingdomBridgeTransitionArt();
            transWidth = 75;
        }

        initialLineToDrawMap = MapAnimator::animateMapIntroduction(currentMapTitle, titleArt, artWidth, transArt, transWidth, Color::CYAN, currentMapMatrix, playerPositionX, playerPositionY, formatter, true, true, nullptr);
    };

    animateScreen();

    std::unordered_map<char, std::function<void(int, int, int)>> interactions;

    interactions['^'] = [&]([[maybe_unused]] int px, int py, [[maybe_unused]] int width) {
        // [PT-BR] Transicao 1: Acesso ao Reino / Castelo
        // [EN-US] Transition 1: Access to Kingdom / Castle
        if (py < 20) {
            if (!invitationReceived) {
                Appearance::startPopupInteraction();
                std::vector<std::string> msg = { "Os portoes estao trancados.", "Voce precisa de uma permissao real." };
                Appearance::displayPopup("ACESSO NEGADO", msg, Color::CYAN);
            } else {
                Appearance::startPopupInteraction();
                std::vector<std::string> msg = {
                    "Voce apresentou o Convite Real e os portoes se abriram!",
                    "Entrando no Reino do Reino..."
                };
                Appearance::displayPopup("ACESSO CONCEDIDO", msg, Color::YELLOW);
                isExplorationActive = false;
                nextMap = NextMapTransition::Kingdom;
            }
        }
        // [PT-BR] Transicao 2: Retornar para a Floresta
        // [EN-US] Transition 2: Return to Forest
        else if (py >= 20) {
            isExplorationActive = false;
            nextMap = NextMapTransition::Forest;
        }
    };

    interactions['G'] = [&]([[maybe_unused]] int px, [[maybe_unused]] int py, [[maybe_unused]] int width) {
        Appearance::startPopupInteraction();
        std::vector<std::string> msg = {
            "Alto la! Somente o Rei pode conceder passagem.",
            "(O Reino ainda esta em construcao pelos devs)"
        };
        Appearance::displayPopup("GUARDA REAL", msg, Color::CYAN);
        if (isExplorationActive && !MapControl::is3DExplorationActive()) restoreScreen();
    };

    auto interactKnight = [&](int px, int py, int width) {
        NPCGenericKnight::interact(currentPlayer, trollDefeated, invitationReceived, width, currentMapMatrix, isExplorationActive, restoreScreen, currentMapMatrix[py][px], px, py);
    };
    interactions['T'] = interactKnight;
    interactions['C'] = interactKnight;

    auto processInteraction = [&](int px, int py, int width) {
        char destinationCell = currentMapMatrix[py][px];
        auto it = interactions.find(destinationCell);
        if (it != interactions.end()) {
            it->second(px, py, width);
        } else if ((destinationCell != '*' && destinationCell != '#' && destinationCell != '=' && destinationCell != '|' && destinationCell != '[' && destinationCell != ']' && destinationCell != 'A' && destinationCell != 'S' && destinationCell != 'E' && destinationCell != 'L' && destinationCell != 'O' && destinationCell != ' ') || Debug::isNoclipActive) {
            playerPositionX = px;
            playerPositionY = py;
        }
    };

    bool needsRender = true;
    NextMapTransition fastTravelDestination = MapControl::executeExplorationLoop(
        currentPlayer, currentMapMatrix, playerPositionX, playerPositionY,
        isExplorationActive, currentMapTitle, []() { return ""; },
        []() -> std::vector<std::string> { return {}; },
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
