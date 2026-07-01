#include "GerenciadorVisao.h"
#include "Terminal/TerminalRenderer.h"
#include "../Core/Utilidades/RendererProvider.h"

GerenciadorVisao::GerenciadorVisao() : m_visao3DAtiva(true) {
}

void GerenciadorVisao::inicializar() {
    m_rendererTerminal = std::make_unique<TerminalRenderer>();
    // Por enquanto a Engine3D usará as mesmas mecânicas de popup (TelasBase), mas futuramente terá a própria!
    m_renderer3D = std::make_unique<TerminalRenderer>(); // TODO: Implementar RaycasterRenderer como IVisaoRenderer 
    
    // Configura o Provider inicial baseado no estado (3D é o padrão da nossa fantasia)
    RendererProvider::set(obterRendererAtivo());
}

void GerenciadorVisao::alternarVisao() {
    m_visao3DAtiva = !m_visao3DAtiva;
    RendererProvider::set(obterRendererAtivo());
}

bool GerenciadorVisao::isVisao3DAtiva() const {
    return m_visao3DAtiva;
}

IVisaoRenderer* GerenciadorVisao::obterRendererAtivo() const {
    if (m_visao3DAtiva) {
        return m_renderer3D.get();
    }
    return m_rendererTerminal.get();
}
