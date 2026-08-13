#pragma once
#include "Domain/Characters/Character.h"

class IPauseUI {
public:
    virtual ~IPauseUI() = default;
    virtual int renderMenuPause() = 0;
    virtual int renderSettingsMenu(Character* player) = 0;
    virtual int renderMenuAppearance(Character* player) = 0;
    virtual int renderMenuBackground(int colorBackgroundCurrentIndex) = 0;
    virtual int renderMenuSensitivity(int percentX, int percentY) = 0;
};
