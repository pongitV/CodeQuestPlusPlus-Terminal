#pragma once

#include "UI/PerspectiveRenderer.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Core/Utils/AnsiFormatter.h"

class GORenderer : public PerspectiveRenderer {
public:
    std::vector<std::string> formatGOText(const std::vector<std::string>& text) {
        return AnsiFormatter::formatGOText(text);
    }

    std::vector<std::string> formatGOArt(const std::vector<std::string>& art) {
        return AnsiFormatter::formatGOArt(art);
    }

    std::string formatGOTitle(const std::string& title) {
        return AnsiFormatter::formatGOTitle(title);
    }

    void displayPopup(const std::string& title, const std::vector<std::string>& lines, Color colorHeader, const std::vector<std::string>& artSoon = {}) override {
        Appearance::displayPopup(formatGOTitle(title), formatGOText(lines), colorHeader, formatGOArt(artSoon));
    }

    void startPopupInteraction() override {}

    int readMenuSelectionInPopup(const std::string& title, const std::vector<std::string>& descriptions, const std::vector<std::string>& options, Color colorHeader, const std::vector<std::string>& artSoon = {}, bool returnEnabled = true) override {
        return InputControl::readMenuSelectionInPopup(
            formatGOTitle(title), formatGOText(descriptions),
            AnsiFormatter::formatGOOptions(options),
            colorHeader, formatGOArt(artSoon), returnEnabled);
    }

    void clearScreen() override { Appearance::clearScreen(); }

    void displayTextPanel(const std::string& text, Color color) override {
        Appearance::displayTextPanel(AnsiFormatter::formatYESDText(text), color);
    }
};
