#pragma once

#include <string>
#include <vector>

class Character;

class RaycasterPauseScreen {
public:
    static int renderMenuPause();
    static int renderSettingsMenu(Character* player);
    static int renderMenuAppearance(Character* player);
    static int renderMenuBackground(int colorBackgroundCurrentIndex);
    static int renderMenuSensitivity(int percentX, int percentY);
};
