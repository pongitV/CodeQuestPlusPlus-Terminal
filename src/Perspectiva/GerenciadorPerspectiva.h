#pragma once

#include "IVisaoRenderer.h"
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

    IVisaoRenderer* obterRendererAtivo() const;

    static float obterSensibilidadeMouseX();
    static float obterSensibilidadeMouseY();
    static void definirSensibilidadeMouse(float x, float y);

private:
    GerenciadorPerspectiva();
    ~GerenciadorPerspectiva() = default;

    GerenciadorPerspectiva(const GerenciadorPerspectiva&) = delete;
    GerenciadorPerspectiva& operator=(const GerenciadorPerspectiva&) = delete;

    bool m_visao3DAtiva;
    std::unique_ptr<IVisaoRenderer> m_rendererTerminal;
    std::unique_ptr<IVisaoRenderer> m_renderer3D;
};
