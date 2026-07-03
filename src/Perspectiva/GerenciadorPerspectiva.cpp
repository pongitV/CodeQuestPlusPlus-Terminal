#include "GerenciadorPerspectiva.h"
#include "PerspectivaIDE/EngineIDE/IDERenderer.h"
#include "PerspectivaRaycaster/EngineRaycaster/Raycaster.h"
#include "../Core/Utilidades/RendererProvider.h"

GerenciadorPerspectiva::GerenciadorPerspectiva() : m_visao3DAtiva(false) {
}

void GerenciadorPerspectiva::inicializar() {
    m_rendererTerminal = std::make_unique<IDERenderer>();
    // Por enquanto a Engine3D usará as mesmas mecânicas de popup (TelasBase), mas futuramente terá a própria!
    m_renderer3D = std::make_unique<IDERenderer>(); // TODO: Implementar RaycasterRenderizador como IVisaoRenderer 
    
    // Configura o Provider inicial baseado no estado (3D é o padrão da nossa fantasia)
    RendererProvider::set(obterRendererAtivo());
}

void GerenciadorPerspectiva::alternarVisao() {
    m_visao3DAtiva = !m_visao3DAtiva;
    RendererProvider::set(obterRendererAtivo());
}

bool GerenciadorPerspectiva::isVisao3DAtiva() const {
    return m_visao3DAtiva;
}

IVisaoRenderer* GerenciadorPerspectiva::obterRendererAtivo() const {
    if (m_visao3DAtiva) {
        return m_renderer3D.get();
    }
    return m_rendererTerminal.get();
}

float GerenciadorPerspectiva::obterSensibilidadeMouseX() {
    return Raycaster::sensibilidadeX;
}

float GerenciadorPerspectiva::obterSensibilidadeMouseY() {
    return Raycaster::sensibilidadeY;
}

void GerenciadorPerspectiva::definirSensibilidadeMouse(float x, float y) {
    Raycaster::sensibilidadeX = x;
    Raycaster::sensibilidadeY = y;
}
