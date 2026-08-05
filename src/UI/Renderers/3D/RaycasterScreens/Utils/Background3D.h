#pragma once

#include "UI/Renderers/3D/RaycasterScreens/Utils/FrameOverlayUtils.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterCombatRenderer.h"

namespace MenuRaycasterUtils {

    inline void curlBackground3D(const std::string& biome, Character* player) {
        if (biome != s_lastBiomeMenu || s_background3DMenu.empty()) {
            std::vector<Character*> empty;
            s_background3DMenu = RaycasterRendererCombat::renderFrame(biome, player, empty);
            s_lastBiomeMenu = biome;
        }
    }

    inline void cleanBackground3D() {
        s_background3DMenu.clear();
        s_lastBiomeMenu.clear();
    }

}
