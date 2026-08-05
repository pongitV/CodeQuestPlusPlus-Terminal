#pragma once

#include <string>
#include "Core/Utils/Appearance.h"

class IRendererScreen {
public:
    virtual ~IRendererScreen() = default;
    virtual void clearScreen() = 0;
    virtual void displayTextPanel(const std::string& text, Color color) = 0;
};
