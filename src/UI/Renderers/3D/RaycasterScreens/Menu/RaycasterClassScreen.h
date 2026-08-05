#pragma once
#include <string>
#include "UI/Screens/Menu/ClassScreen.h"

class ScreenClassRaycaster {
public:
    static ClassScreen::Result display(const std::string& namePlayer, const std::string& raceName);
};
