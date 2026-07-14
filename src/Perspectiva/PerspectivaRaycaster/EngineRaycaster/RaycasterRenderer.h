#pragma once

#include "../../../Perspectiva/RenderizadorPerspectiva.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"

class RaycasterRenderer : public RenderizadorPerspectiva {
public:
    void exibirPopup(const std::string& titulo, const std::vector<std::string>& falas, Cor corCabecalho, const std::vector<std::string>& arteLogo = {}) override {
        Aparencia::exibirPopup(titulo, falas, corCabecalho, arteLogo);
    }

    void iniciarInteracaoPopup() override {
        Aparencia::iniciarInteracaoPopup();
    }

    int lerSelecaoMenuEmPopup(const std::string& titulo, const std::vector<std::string>& descricoes, const std::vector<std::string>& opcoes, Cor corCabecalho, const std::vector<std::string>& arteLogo = {}, bool voltarHabilitado = true) override {
        return ControleDeInput::lerSelecaoMenuEmPopup(titulo, descricoes, opcoes, corCabecalho, arteLogo, voltarHabilitado);
    }

    void limparTela() override { 
        Aparencia::limparTela(); 
    }

    void exibirPainelTexto(const std::string& texto, Cor cor) override {
        Aparencia::exibirPainelTexto(texto, cor);
    }
};
