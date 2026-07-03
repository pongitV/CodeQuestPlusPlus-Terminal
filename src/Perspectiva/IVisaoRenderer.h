#pragma once

#include <vector>
#include <string>
#include <functional>
#include "../Core/Utilidades/Aparencia.h" // Apenas para os enums de Cor, temporariamente

// Interface abstrata para renderização.
// Qualquer componente de jogo (NPC, Controle de Mapa) deve usar esta interface
// ao invés de chamar `Aparencia` ou `std::cout` diretamente.
class IVisaoRenderer {
public:
    virtual ~IVisaoRenderer() = default;

    // --- MÉTODOS DE POPUP E DIÁLOGO ---
    virtual void exibirPopup(const std::string& titulo, const std::vector<std::string>& texto, Cor corTema = Cor::BRANCO, const std::vector<std::string>& arteAscii = {}) = 0;
    virtual void iniciarInteracaoPopup() = 0;
    virtual int lerSelecaoMenuEmPopup(const std::string& titulo, const std::vector<std::string>& texto, const std::vector<std::string>& opcoes, Cor corTema = Cor::BRANCO, const std::vector<std::string>& arteLogo = {}, bool voltarHabilitado = true) = 0;
    
    // --- MÉTODOS DE TELA E RENDERIZAÇÃO GERAL ---
    virtual void limparTela() = 0;
    virtual void exibirPainelTexto(const std::string& texto, Cor cor) = 0;
};
