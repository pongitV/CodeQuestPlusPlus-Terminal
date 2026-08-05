#pragma once
#include <string>
#include "UI/Screens/Menu/DifficultyScreen.h"

class ScreenDifficultyRaycaster {
public:
    static DifficultyScreen::Result display(const std::string& namePlayer, const std::string& raceName, const std::string& className);
};
