#pragma once

#include "FrameOverlayUtils.h"
#include "../../EngineRaycaster/RaycasterRenderizadorCombate.h"

namespace MenuRaycasterUtils {

    inline void cachearFundo3D(const std::string& bioma, Personagem* jogador) {
        if (bioma != s_ultimoBiomaMenu || s_fundo3DMenu.empty()) {
            std::vector<Personagem*> vazio;
            s_fundo3DMenu = RaycasterRenderizadorCombate::renderizarQuadro(bioma, jogador, vazio);
            s_ultimoBiomaMenu = bioma;
        }
    }

    inline void limparFundo3D() {
        s_fundo3DMenu.clear();
        s_ultimoBiomaMenu.clear();
    }

}
