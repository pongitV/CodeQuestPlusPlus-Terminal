#pragma once

#include "UI/Interfaces/IDiaryUI.h"

class IDEDiaryScreen : public IDiaryUI {
public:
    IDEDiaryScreen() = default;
    ~IDEDiaryScreen() override = default;

    void renderBackground() override;
    void displayHeader(int startY) override;
    void renderBox(const std::vector<std::string>& lines, const std::string& title, Color colorBox, int minY, int startYOverride) override;
    void renderPopupMessage(const std::string& title, const std::vector<std::string>& text) override;
    void renderPopupInspectionWithArt(const std::string& title, const std::vector<std::string>& art, const std::vector<std::string>& info, const std::string& subtitle) override;

    static IDEDiaryScreen& instance() {
        static IDEDiaryScreen s_instance;
        return s_instance;
    }
};
