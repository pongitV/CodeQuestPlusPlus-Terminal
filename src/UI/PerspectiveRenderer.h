#pragma once

#include "UI/IPopupRenderer.h"
#include "UI/IScreenRenderer.h"

class PerspectiveRenderer : public IRendererPopup, public IRendererScreen {
public:
    ~PerspectiveRenderer() override = default;
};
