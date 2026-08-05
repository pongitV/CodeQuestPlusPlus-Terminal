#pragma once
#include <string>
#include "UI/Screens/Menu/RaceScreen.h"

class ScreenRaceGO {
public:
    static RaceScreen::Result display(const std::string& namePlayer);
};
