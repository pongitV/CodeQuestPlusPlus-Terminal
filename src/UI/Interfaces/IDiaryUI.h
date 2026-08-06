#pragma once
#include <string>
#include <vector>
#include "Core/Utils/Appearance.h"

class IDiaryUI {
public:
    virtual ~IDiaryUI() = default;
    virtual void renderBackground() = 0;
    virtual void displayHeader(int startY) = 0;
    virtual void renderBox(const std::vector<std::string>& lines, const std::string& title, Color colorBox, int minY, int startYOverride) = 0;
    virtual void renderPopupMessage(const std::string& title, const std::vector<std::string>& text) = 0;
    virtual void renderPopupInspectionWithArt(const std::string& title, const std::vector<std::string>& art, const std::vector<std::string>& info, const std::string& subtitle) = 0;
};
