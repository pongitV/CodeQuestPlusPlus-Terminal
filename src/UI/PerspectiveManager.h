// [PT-BR] Arquivo: PerspectiveManager.h
// [PT-BR] Proposito: Singleton gerenciador de alternancia de perspectiva (Modo IDE/Console 2D vs Raycaster 3D).
// [EN-US] File: PerspectiveManager.h
// [EN-US] Purpose: Perspective switching singleton manager (2D IDE/Console Mode vs 3D Raycaster).

#pragma once

#include "UI/PerspectiveRenderer.h"
#include "UI/Screens/IScreenManager.h"
#include "UI/Interfaces/IDiaryUI.h"
#include "UI/Interfaces/IInventoryUI.h"
#include "UI/Interfaces/IAttributesUI.h"
#include "UI/Interfaces/IBestiaryUI.h"
#include "UI/Interfaces/ICombatScreenUI.h"
#include "UI/Interfaces/IDefeatUI.h"
#include "UI/Interfaces/IVictoryUI.h"
#include "UI/Interfaces/IPauseUI.h"
#include "UI/Interfaces/IWorldMapUI.h"
#include <memory>

// [PT-BR] Gerenciador central de perspectiva visual e fabricas de interfaces.
// [EN-US] Central manager for visual perspective and interface factories.
class PerspectiveManager {
public:
    static PerspectiveManager& getInstance() {
        static PerspectiveManager instance;
        return instance;
    }

    void boot();
    void toggleView();
    bool is3DViewActive() const;

    PerspectiveRenderer* getRendererActive() const;
    IManagerScreens* getManagerScreens() const;

    static IDiaryUI& getDiaryUI();
    static IInventoryUI& getInventoryUI();
    static IAttributesUI& getAttributesUI();
    static IBestiaryUI& getBestiaryUI();
    static ICombatScreenUI& getScreenCombatUI();
    static IDefeatUI& getDefeatUI();
    static IVictoryUI& getVictoryUI();
    static IPauseUI& getPauseUI();
    static IWorldMapUI& getMapWorldUI();

    static float getSensitivityMouseX();
    static float getSensitivityMouseY();
    static void setSensitivityMouse(float x, float y);

private:
    PerspectiveManager();
    ~PerspectiveManager() = default;

    PerspectiveManager(const PerspectiveManager&) = delete;
    PerspectiveManager& operator=(const PerspectiveManager&) = delete;

    bool m_view3DActive;
    std::unique_ptr<PerspectiveRenderer> m_renderer3D;

    std::unique_ptr<IManagerScreens> m_screens3D;
};
