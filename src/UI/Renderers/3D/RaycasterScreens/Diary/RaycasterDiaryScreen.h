#pragma once

#include <string>
#include <vector>
#include "UI/Screens/BaseScreen.h"
#include "UI/Interfaces/IDiaryUI.h"

class Character;

class RaycasterDiaryScreen : public IDiaryUI {
public:
    void renderBackground() override;
    void displayHeader(int startY) override;
    void renderBox(const std::vector<std::string>& lines, const std::string& title, Color colorBox, int minY, int startYOverride) override;
    void renderPopupMessage(const std::string& title, const std::vector<std::string>& text) override;
    void renderPopupInspectionWithArt(const std::string& title, const std::vector<std::string>& art, const std::vector<std::string>& info, const std::string& subtitle) override;
};
