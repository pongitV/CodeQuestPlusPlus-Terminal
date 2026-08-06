#pragma once

#include <vector>
#include <string>
#include "Core/Utils/Appearance.h"

class IRendererPopup {
public:
    virtual ~IRendererPopup() = default;
    virtual void displayPopup(const std::string& title, const std::vector<std::string>& text, Color themeColor = Color::WHITE, const std::vector<std::string>& asciiArt = {}) = 0;
    virtual void startPopupInteraction() = 0;
    virtual int readMenuSelectionInPopup(const std::string& title, const std::vector<std::string>& text, const std::vector<std::string>& options, Color themeColor = Color::WHITE, const std::vector<std::string>& artSoon = {}, bool returnEnabled = true) = 0;
};
