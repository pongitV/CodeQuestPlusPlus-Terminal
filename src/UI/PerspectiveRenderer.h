#pragma once

#include "UI/Interfaces/IPopupRenderer.h"
#include "UI/Interfaces/IScreenRenderer.h"

class PerspectiveRenderer : public IRendererPopup, public IRendererScreen {
public:
    ~PerspectiveRenderer() override = default;
};
