#pragma once

#include "IVisaoRenderer.h"
#include <memory>

class GerenciadorVisao {
public:
    static GerenciadorVisao& obterInstancia() {
        static GerenciadorVisao instancia;
        return instancia;
    }

    void inicializar();
    void alternarVisao();
    bool isVisao3DAtiva() const;

    IVisaoRenderer* obterRendererAtivo() const;

private:
    GerenciadorVisao();
    ~GerenciadorVisao() = default;

    GerenciadorVisao(const GerenciadorVisao&) = delete;
    GerenciadorVisao& operator=(const GerenciadorVisao&) = delete;

    bool m_visao3DAtiva;
    std::unique_ptr<IVisaoRenderer> m_rendererTerminal;
    std::unique_ptr<IVisaoRenderer> m_renderer3D;
};
