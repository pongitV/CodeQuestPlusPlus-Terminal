#pragma once
#include <string>
#include "UI/Screens/Menu/ParryScreen.h"

class ScreenParryRaycaster {
public:
    static ParryScreen::Result display(const std::string& namePlayer, const std::string& raceName, const std::string& className);
};
