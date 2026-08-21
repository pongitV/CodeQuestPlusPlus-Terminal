#include "UI/Screens/WorldMap/WorldMapScreen.h"
#include "UI/Screens/WorldMap/WorldMapScreenLayout.h"
#include "UI/PerspectiveManager.h"
#include "Core/Utils/InputControl.h"
#include "Systems/Progression/Progression.h"
#include "Systems/Progression/ProgressionFlags.h"

#include <thread>
#include <chrono>
#include <algorithm>
#include "Core/Utils/Appearance.h"

NextMapTransition WorldMapScreen::display(Character* currentPlayer, MapLocation locationCurrent, int villageProgress, int forestProgress, int kingdomBridgeProgress, int kingdomProgress) {
    (void)currentPlayer;
    (void)villageProgress;
    (void)forestProgress;
    (void)kingdomBridgeProgress;
    (void)kingdomProgress;

    auto locations = ArtsMapWorld::getLocations();
    std::vector<std::string> places;

    for (const auto& l : locations) {
        if (l.flag == nullptr) {
            places.push_back(l.nameDisplay);
        } else if (Progression::instance().getFlag(l.flag)) {
            places.push_back(l.nameDisplay);
        }
    }

    std::string nameLocationCurrent;
    switch (locationCurrent) {
        case MapLocation::InitialVillage: nameLocationCurrent = "Vila Inicial"; break;
        case MapLocation::Forest:   nameLocationCurrent = "Floresta Sombria"; break;
        case MapLocation::KingdomBridge: nameLocationCurrent = "Ponte do Reino"; break;
        case MapLocation::Kingdom:      nameLocationCurrent = "Reino Distante"; break;
    }

    int selection = 0;
    bool running = true;
    bool redesignComplete = true;

    InputControl::clearBuffer();

    while (running) {
        PerspectiveManager::getMapWorldUI().renderPopup(ArtsMapWorld::planetEarth, places, selection, redesignComplete);
        redesignComplete = false;

        int totalOptions = 1 + static_cast<int>(places.size());

#ifdef _WIN32
        int mouseX = -1, mouseY = -1;
        bool isLeft = false, isRight = false;
        if (InputControl::pollMouseState(mouseX, mouseY, isLeft, isRight)) {
            int heightTerm = Appearance::getTerminalHeight();
            int artH = static_cast<int>(ArtsMapWorld::planetEarth.size());
            int listH = 1 + static_cast<int>(places.size());
            int contH = std::max(artH, listH) + 2;
            int outH = contH + 2;
            int startY = std::max(0, (heightTerm - outH) / 2);
            int optionsStartY = startY + 2;

            if (mouseY >= optionsStartY && mouseY < optionsStartY + totalOptions) {
                int hovered = mouseY - optionsStartY;
                if (hovered >= 0 && hovered < totalOptions) {
                    selection = hovered;
                    if (isLeft) {
                        while (true) {
                            int mx, my; bool l, r;
                            InputControl::pollMouseState(mx, my, l, r);
                            if (!l) break;
                            std::this_thread::sleep_for(std::chrono::milliseconds(10));
                        }
                        if (selection == 0) {
                            return NextMapTransition::None;
                        } else {
                            std::string& selectedName = places[selection - 1];
                            if (selectedName == nameLocationCurrent) return NextMapTransition::None;
                            else if (selectedName.find("Vila") != std::string::npos) return NextMapTransition::Village;
                            else if (selectedName.find("Floresta") != std::string::npos) return NextMapTransition::Forest;
                            else if (selectedName.find("Ponte") != std::string::npos) return NextMapTransition::KingdomBridge;
                            else if (selectedName.find("Reino") != std::string::npos) return NextMapTransition::Kingdom;
                        }
                    }
                }
            }
        }
#endif

        if (InputControl::pressedKey()) {
            unsigned char key = static_cast<unsigned char>(InputControl::readKey());
            if (key == 27) {
                if (!InputControl::pressedKey()) {
                    return NextMapTransition::None;
                }
                unsigned char nextKey = static_cast<unsigned char>(InputControl::readKey());
                if (nextKey == '[') nextKey = static_cast<unsigned char>(InputControl::readKey());
                if (nextKey == 72 || nextKey == 'A') key = 'w';
                else if (nextKey == 80 || nextKey == 'B') key = 's';
                else return NextMapTransition::None;
            } else if (key == 224 || key == 0) {
                unsigned char nextKey = static_cast<unsigned char>(InputControl::readKey());
                if (nextKey == 72) key = 'w';
                else if (nextKey == 80) key = 's';
            }

            if (key == 'q' || key == 'Q' || key == 'm' || key == 'M' || key == 'c' || key == 'C') {
                return NextMapTransition::None;
            }

            if (key == 'w' || key == 'W') {
                selection--;
                if (selection < 0) selection = totalOptions - 1;
            } else if (key == 's' || key == 'S') {
                selection++;
                if (selection >= totalOptions) selection = 0;
            } else if (key == '\n' || key == '\r' || key == ' ') {
                if (selection == 0) {
                    return NextMapTransition::None;
                } else {
                    std::string& selectedName = places[selection - 1];
                    if (selectedName == nameLocationCurrent) return NextMapTransition::None;
                    else if (selectedName.find("Vila") != std::string::npos) return NextMapTransition::Village;
                    else if (selectedName.find("Floresta") != std::string::npos) return NextMapTransition::Forest;
                    else if (selectedName.find("Ponte") != std::string::npos) return NextMapTransition::KingdomBridge;
                    else if (selectedName.find("Reino") != std::string::npos) return NextMapTransition::Kingdom;
                }
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }
    return NextMapTransition::None;
}
