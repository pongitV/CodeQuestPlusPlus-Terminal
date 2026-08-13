#pragma once

#include "UI/PerspectiveRenderer.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Core/Utils/AnsiFormatter.h"

class GORenderer : public PerspectiveRenderer {
public:
    std::vector<std::string> formatIDEText(const std::vector<std::string>& text) {
        return AnsiFormatter::formatIDEText(text);
    }

    std::vector<std::string> formatIDEArt(const std::vector<std::string>& art) {
        return AnsiFormatter::formatIDEArt(art);
    }

    std::string formatIDETitle(const std::string& title) {
        return AnsiFormatter::formatIDETitle(title);
    }

    void displayPopup(const std::string& title, const std::vector<std::string>& lines, Color colorHeader, const std::vector<std::string>& logoArt = {}) override {
        Appearance::displayPopup(formatIDETitle(title), formatIDEText(lines), colorHeader, formatIDEArt(logoArt));
    }

    void startPopupInteraction() override {}

    int readMenuSelectionInPopup(const std::string& title, const std::vector<std::string>& descriptions, const std::vector<std::string>& options, Color colorHeader, const std::vector<std::string>& logoArt = {}, bool returnEnabled = true) override {
        return InputControl::readMenuSelectionInPopup(
            formatIDETitle(title), formatIDEText(descriptions),
            AnsiFormatter::formatGOOptions(options),
            colorHeader, formatIDEArt(logoArt), returnEnabled);
    }

    void clearScreen() override { Appearance::clearScreen(); }

    void displayTextPanel(const std::string& text, Color color) override {
        Appearance::displayTextPanel(AnsiFormatter::formatYESDText(text), color);
    }
};
