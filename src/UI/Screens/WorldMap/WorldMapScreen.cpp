#include "UI/Screens/WorldMap/WorldMapScreen.h"
#include "UI/Screens/WorldMap/WorldMapScreenLayout.h"
#include "UI/PerspectiveManager.h"
#include "Core/Utils/InputControl.h"
#include "Systems/Progression/Progression.h"
#include "Systems/Progression/ProgressionFlags.h"

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

    while (running) {
        PerspectiveManager::getMapWorldUI().renderPopup(ArtsMapWorld::planetEarth, places, selection, redesignComplete);

        redesignComplete = false;
        char key = InputControl::readKey();
        int totalOptions = 1 + (int)places.size();

        if (key == 'w' || key == 'W') {
            selection--;
            if (selection < 0) selection = totalOptions - 1;
        } else if (key == 's' || key == 'S') {
            selection++;
            if (selection >= totalOptions) selection = 0;
        } else if (key == '\n' || key == '\r') {
            redesignComplete = true;
            if (selection == 0) {
                running = false;
            } else {
                std::string& selectedName = places[selection - 1];
                if (selectedName == nameLocationCurrent)
                    running = false;
                else if (selectedName.find("Vila") != std::string::npos)
                    { running = false; return NextMapTransition::Village; }
                else if (selectedName.find("Floresta") != std::string::npos)
                    { running = false; return NextMapTransition::Forest; }
                else if (selectedName.find("Ponte") != std::string::npos)
                    { running = false; return NextMapTransition::KingdomBridge; }
                else if (selectedName.find("Reino") != std::string::npos)
                    { running = false; return NextMapTransition::Kingdom; }
            }
        }
    }
    return NextMapTransition::None;
}
