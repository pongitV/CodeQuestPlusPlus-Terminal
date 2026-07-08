#pragma once

#include "IRenderizadorPopup.h"
#include "IRenderizadorTela.h"

class RenderizadorPerspectiva : public IRenderizadorPopup, public IRenderizadorTela {
public:
    ~RenderizadorPerspectiva() override = default;
};
