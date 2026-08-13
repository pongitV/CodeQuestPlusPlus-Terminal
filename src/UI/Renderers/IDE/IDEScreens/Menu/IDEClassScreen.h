#pragma once
#include <string>
#include "UI/Screens/Menu/ClassScreen.h"

class IDEClassScreen {
public:
    static ClassScreen::Result display(const std::string& namePlayer, const std::string& raceName);
};
