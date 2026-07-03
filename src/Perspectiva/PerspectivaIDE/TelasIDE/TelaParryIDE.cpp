#include "TelaParryIDE.h"
#include "TelaMenuIDE.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/FuncoesDialogo.h"
#include "../../../Core/Utilidades/ControleDeInput.h"

namespace {
    int exibirPromptGenerico(const std::string& titulo, const std::string& infoBox, const std::vector<std::string>& narracao, const std::vector<std::string>& opcoes) {
        Aparencia::limparTela();
        TelaMenuIDE::exibirPainelLogoJogo(titulo, false);
        if (infoBox.empty()) std::cout << "\n";
        else {
            std::string cleanInfoBox = infoBox;
            size_t pos = 0;
            while ((pos = cleanInfoBox.find('|', pos)) != std::string::npos) {
                cleanInfoBox.replace(pos, 1, "\342\225\221");
                pos += 3;
            }
            int infoLength = Aparencia::obterComprimentoVisual(infoBox);
            std::string tracos = "";
            for (int i = 0; i < infoLength - 2; ++i) tracos += "\342\225\220";
            std::string margem = Aparencia::espacosParaCentralizar(infoLength);
            std::cout << margem << "\342\225\224" << tracos << "\342\225\227\n";
            std::cout << margem << cleanInfoBox << "\n";
            std::cout << margem << "\342\225\232" << tracos << "\342\225\235\n\n";
        }
        Aparencia::imprimirBlocoCentralizadoDigitando(narracao);
        std::cout << "\n";
        return ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
    }
}

TelaParry::Resultado TelaParryIDE::exibir(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse) {
    int escolha = exibirPromptGenerico("CONFIGURACOES DO JOGO", "| JOGADOR: " + nomeJogador + " | RACA: " + nomeRaca + " | CLASSE: " + nomeClasse + " |",
        {
            FuncoesDialogo::formatarMsgSistema("Deseja ativar o sistema de PARRY?"),
            "(Permite reduzir danos ao digitar uma sequencia de numeros num tempo limite)",
            "(O tutorial esta disponivel apenas neste momento, mas voce pode ligar/desligar depois)"
        },
        {"Parry LIGADO (Tutorial disponivel)", "Parry DESLIGADO (Pula o tutorial)", "VOLTAR (selecao de dificuldade)"});

    TelaParry::Resultado r;
    if (escolha == 0) {
        r.parryAtivado = true;
        r.fazerTutorial = true;
    } else if (escolha == 1) {
        r.parryAtivado = false;
        r.fazerTutorial = false;
    } else {
        r.voltou = true;
    }
    return r;
}