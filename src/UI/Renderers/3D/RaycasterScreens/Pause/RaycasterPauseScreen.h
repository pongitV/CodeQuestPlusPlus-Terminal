#pragma once

#include <string>
#include <vector>

class Character;

class ScreenPauseRaycaster {
public:
    static int renderMenuPause();
    static int renderMenuConfiguracoes(Character* player);
    static int renderMenuAppearance(Character* player);
    static int renderMenuBackground(int colorBackgroundCurrentIndex);
    static int renderMenuSensitivity(int percX, int percY);
};
