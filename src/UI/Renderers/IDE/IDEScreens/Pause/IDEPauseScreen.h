#pragma once

#include "UI/Interfaces/IPauseUI.h"

class IDEPauseScreen : public IPauseUI {
public:
    IDEPauseScreen() = default;
    ~IDEPauseScreen() override = default;

    int renderMenuPause() override;
    int renderSettingsMenu(Character* player) override;
    int renderMenuAppearance(Character* player) override;
    int renderMenuBackground(int colorBackgroundCurrentIndex) override;
    int renderMenuSensitivity(int percentX, int percentY) override;

    static IDEPauseScreen& instance() {
        static IDEPauseScreen s_instance;
        return s_instance;
    }
};
