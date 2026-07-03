#pragma once

#include "../../IVisaoRenderer.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"

class IDERenderer : public IVisaoRenderer {
public:
    std::vector<std::string> formatarTextoIDE(const std::vector<std::string>& texto) {
        std::vector<std::string> formatado;
        formatado.push_back("\033[38;2;86;156;214mpublic:\033[0m");
        for (const auto& linha : texto) {
            if (linha.empty() || linha == " ") {
                formatado.push_back("");
            } else {
                formatado.push_back("    \033[38;2;86;156;214mstring\033[0m msg = \033[38;2;214;157;133m\"" + linha + "\"\033[0m;");
            }
        }
        formatado.push_back("};");
        return formatado;
    }
    
    std::vector<std::string> formatarArteIDE(const std::vector<std::string>& arte) {
        if (arte.empty()) return arte;
        std::vector<std::string> formatado;
        formatado.push_back("\033[38;2;96;139;78m/* ==================================\033[0m");
        for (const auto& l : arte) {
            formatado.push_back(l);
        }
        formatado.push_back("\033[38;2;96;139;78m================================== */\033[0m");
        return formatado;
    }

    std::string formatarTituloIDE(const std::string& titulo) {
        return "\033[38;2;86;156;214mclass\033[0m \033[38;2;78;201;176m" + titulo + "\033[0m {";
    }

    void exibirPopup(const std::string& titulo, const std::vector<std::string>& falas, Cor corCabecalho, const std::vector<std::string>& arteLogo = {}) override {
        Aparencia::exibirPopup(formatarTituloIDE(titulo), formatarTextoIDE(falas), corCabecalho, formatarArteIDE(arteLogo));
    }

    void iniciarInteracaoPopup() override {
    }

    int lerSelecaoMenuEmPopup(const std::string& titulo, const std::vector<std::string>& descricoes, const std::vector<std::string>& opcoes, Cor corCabecalho, const std::vector<std::string>& arteLogo = {}, bool voltarHabilitado = true) override {
        std::vector<std::string> opcoesIDE;
        for (size_t i = 0; i < opcoes.size(); ++i) {
            opcoesIDE.push_back("\033[38;2;220;220;170m" + opcoes[i] + "\033[0m()");
        }
        return ControleDeInput::lerSelecaoMenuEmPopup(formatarTituloIDE(titulo), formatarTextoIDE(descricoes), opcoesIDE, corCabecalho, formatarArteIDE(arteLogo), voltarHabilitado);
    }

    void limparTela() override {
        Aparencia::limparTela();
    }
    
    void exibirPainelTexto(const std::string& texto, Cor cor) override {
        std::string textoIDE = "\033[38;2;96;139;78m// " + texto + "\033[0m";
        Aparencia::exibirPainelTexto(textoIDE, cor);
    }
};
