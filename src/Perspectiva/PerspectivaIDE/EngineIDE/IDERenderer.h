#pragma once

#include "../../RenderizadorPerspectiva.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Core/Utilidades/FormatadorAnsi.h"

class IDERenderer : public RenderizadorPerspectiva {
public:
    std::vector<std::string> formatarTextoIDE(const std::vector<std::string>& texto) {
        return FormatadorAnsi::formatarTextoIDE(texto);
    }

    std::vector<std::string> formatarArteIDE(const std::vector<std::string>& arte) {
        return FormatadorAnsi::formatarArteIDE(arte);
    }

    std::string formatarTituloIDE(const std::string& titulo) {
        return FormatadorAnsi::formatarTituloIDE(titulo);
    }

    void exibirPopup(const std::string& titulo, const std::vector<std::string>& falas, Cor corCabecalho, const std::vector<std::string>& arteLogo = {}) override {
        Aparencia::exibirPopup(formatarTituloIDE(titulo), formatarTextoIDE(falas), corCabecalho, formatarArteIDE(arteLogo));
    }

    void iniciarInteracaoPopup() override {}

    int lerSelecaoMenuEmPopup(const std::string& titulo, const std::vector<std::string>& descricoes, const std::vector<std::string>& opcoes, Cor corCabecalho, const std::vector<std::string>& arteLogo = {}, bool voltarHabilitado = true) override {
        return ControleDeInput::lerSelecaoMenuEmPopup(
            formatarTituloIDE(titulo), formatarTextoIDE(descricoes),
            FormatadorAnsi::formatarOpcoesIDE(opcoes),
            corCabecalho, formatarArteIDE(arteLogo), voltarHabilitado);
    }

    void limparTela() override { Aparencia::limparTela(); }

    void exibirPainelTexto(const std::string& texto, Cor cor) override {
        Aparencia::exibirPainelTexto(FormatadorAnsi::formatarTextoSIMD(texto), cor);
    }
};
