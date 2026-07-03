#include "TelaNomeIDE.h"
#include "TelaMenuIDE.h"
#include <iostream>
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/FuncoesDialogo.h"
#include "../../../Core/Utilidades/ControleDeInput.h"

TelaNome::Resultado TelaNomeIDE::exibir() {
    std::cout << "\033[?25l";
    Aparencia::limparTela();
    TelaMenuIDE::exibirPainelLogoJogo("INTRODUCAO AO RPG", false);
    std::cout << "\n";

    std::vector<std::string> narracao = {
        FuncoesDialogo::formatarMsgNarracao("O mundo clama por um novo destino..."),
        FuncoesDialogo::formatarMsgNarracao("E todas lendas possuem um nome.")
    };
    Aparencia::imprimirBlocoCentralizadoDigitando(narracao);
    std::cout << "\n";
    Aparencia::exibirPrompt(" > Escolha o nome do seu personagem [0 para voltar]: ");

    std::cout << "\033[?25h";
    std::string nome = ControleDeInput::lerEntradaProtegida();

    if (nome == "0") {
        TelaNome::Resultado r;
        r.voltou = true;
        return r;
    }

    while (nome.empty() || nome.length() > 20) {
        Aparencia::limparTela();
        TelaMenuIDE::exibirPainelLogoJogo("INTRODUCAO AO RPG", false);
        std::cout << "\n";
        Aparencia::imprimirBlocoCentralizado(narracao);
        std::cout << "\n";
        if (nome.empty()) {
            Aparencia::exibirPrompt(" > Nome invalido! Digite novamente [0 para voltar]: ");
        } else {
            Aparencia::exibirPrompt(" > Nome muito longo (max 20 caracteres). Digite novamente [0 para voltar]: ");
        }
        std::cout << "\033[?25h";
        nome = ControleDeInput::lerEntradaProtegida();
        if (nome == "0") {
            TelaNome::Resultado r;
            r.voltou = true;
            return r;
        }
    }

    Aparencia::limparTela();
    TelaMenuIDE::exibirPainelLogoJogo("CONFIRMACAO", false);
    std::cout << "\n";
    Aparencia::imprimirCentralizado("O nome escolhido foi: " + Aparencia::cor(Cor::VERDE) + nome + Aparencia::cor(Cor::RESET));
    std::cout << "\n";
    std::vector<std::string> opcoes = { "VOLTAR", "CONFIRMAR" };
    int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
    if (escolha == 0) {
        return exibir();
    }

    TelaNome::Resultado r;
    r.nome = nome;
    return r;
}