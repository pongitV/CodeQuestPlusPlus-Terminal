#pragma once

#include "UI/PerspectiveRenderer.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"

class RaycasterRenderer : public PerspectiveRenderer {
public:
    void displayPopup(const std::string& title, const std::vector<std::string>& lines, Color colorHeader, const std::vector<std::string>& artSoon = {}) override {
        Appearance::displayPopup(title, lines, colorHeader, artSoon);
    }

    void startPopupInteraction() override {
        Appearance::startPopupInteraction();
    }

    int readMenuSelectionInPopup(const std::string& title, const std::vector<std::string>& descriptions, const std::vector<std::string>& options, Color colorHeader, const std::vector<std::string>& artSoon = {}, bool returnEnabled = true) override {
        return InputControl::readMenuSelectionInPopup(title, descriptions, options, colorHeader, artSoon, returnEnabled);
    }

    void clearScreen() override { 
        Appearance::clearScreen(); 
    }

    void displayTextPanel(const std::string& text, Color color) override {
        Appearance::displayTextPanel(text, color);
    }
};
