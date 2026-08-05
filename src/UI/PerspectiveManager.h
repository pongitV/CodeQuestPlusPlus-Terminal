/*
 * Arquivo: PerspectiveManager.h
 * Propósito: Singleton gerenciador de alternância de perspectiva (Modo IDE/Console 2D vs Raycaster 3D).
 */

#pragma once

#include "UI/PerspectiveRenderer.h"
#include "UI/Screens/IScreenManager.h"
#include "UI/IDiaryUI.h"
#include "UI/IInventoryUI.h"
#include "UI/IAttributesUI.h"
#include "UI/IBestiaryUI.h"
#include "UI/ICombatScreenUI.h"
#include "UI/IDefeatUI.h"
#include "UI/IVictoryUI.h"
#include "UI/IPauseUI.h"
#include "UI/IWorldMapUI.h"
#include <memory>

/*
 * Gerenciador central de perspectiva visual e fábricas de interfaces.
 */
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
    static IScreenCombatUI& getScreenCombatUI();
    static IDefeatUI& getDefeatUI();
    static IVictoryUI& getVictoryUI();
    static IPauseUI& getPauseUI();
    static IMapWorldUI& getMapWorldUI();

    static float getSensitivityMouseX();
    static float getSensitivityMouseY();
    static void setSensitivityMouse(float x, float y);

private:
    PerspectiveManager();
    ~PerspectiveManager() = default;

    PerspectiveManager(const PerspectiveManager&) = delete;
    PerspectiveManager& operator=(const PerspectiveManager&) = delete;

    bool m_visa3DActive;
    std::unique_ptr<PerspectiveRenderer> m_renderer3D;

    std::unique_ptr<IManagerScreens> m_screens3D;
};
