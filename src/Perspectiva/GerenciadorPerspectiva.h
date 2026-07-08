#pragma once

#include "RenderizadorPerspectiva.h"
#include "TelasBase/IGerenciadorTelas.h"
#include "IDiarioUI.h"
#include "IInventarioUI.h"
#include "IAtributosUI.h"
#include "IBestiarioUI.h"
#include "ITelaCombateUI.h"
#include "IDerrotaUI.h"
#include "IVitoriaUI.h"
#include "IPauseUI.h"
#include "IMapaMundoUI.h"
#include <memory>

class GerenciadorPerspectiva {
public:
    static GerenciadorPerspectiva& obterInstancia() {
        static GerenciadorPerspectiva instancia;
        return instancia;
    }

    void inicializar();
    void alternarVisao();
    bool isVisao3DAtiva() const;

    RenderizadorPerspectiva* obterRendererAtivo() const;
    IGerenciadorTelas* obterGerenciadorTelas() const;

    static IDiarioUI& obterDiarioUI();
    static IInventarioUI& obterInventarioUI();
    static IAtributosUI& obterAtributosUI();
    static IBestiarioUI& obterBestiarioUI();
    static ITelaCombateUI& obterTelaCombateUI();
    static IDerrotaUI& obterDerrotaUI();
    static IVitoriaUI& obterVitoriaUI();
    static IPauseUI& obterPauseUI();
    static IMapaMundoUI& obterMapaMundoUI();

    static float obterSensibilidadeMouseX();
    static float obterSensibilidadeMouseY();
    static void definirSensibilidadeMouse(float x, float y);

private:
    GerenciadorPerspectiva();
    ~GerenciadorPerspectiva() = default;

    GerenciadorPerspectiva(const GerenciadorPerspectiva&) = delete;
    GerenciadorPerspectiva& operator=(const GerenciadorPerspectiva&) = delete;

    bool m_visao3DAtiva;
    std::unique_ptr<RenderizadorPerspectiva> m_rendererTerminal;
    std::unique_ptr<RenderizadorPerspectiva> m_renderer3D;

    std::unique_ptr<IGerenciadorTelas> m_telasTerminal;
    std::unique_ptr<IGerenciadorTelas> m_telas3D;
};
