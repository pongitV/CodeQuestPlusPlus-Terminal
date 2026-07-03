#include "TelaRacaIDE.h"
#include "TelaMenuIDE.h"
#include <iostream>
#include <vector>
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/FuncoesDialogo.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../TelasBase/Menu/TelaMenuLayout.h"
#include "../../../Entidades/Racas/RacaBase.h"

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

TelaRaca::Resultado TelaRacaIDE::exibir(const std::string& nomeJogador) {
    std::vector<std::string> opcoes = {"Dwarf", "Elfo", "Humano", "Ork"};
    Aparencia::ordenarAlfabeticamente(opcoes);
    opcoes.push_back("VOLTAR (selecao de nome)");
    int escolha = exibirPromptGenerico("SELECAO DE RACA", "| JOGADOR: " + nomeJogador + " |",
        {FuncoesDialogo::formatarMsgNarracao("Qual sua origem?")},
        opcoes);

    if (escolha >= 4) {
        TelaRaca::Resultado r;
        r.voltou = true;
        return r;
    }

    int indiceReal = escolha;
    std::vector<std::string> racasOrdenadas = {"Dwarf", "Elfo", "Humano", "Ork"};
    Aparencia::ordenarAlfabeticamente(racasOrdenadas);
    std::string racaNome = racasOrdenadas[indiceReal];

    std::vector<std::string> arteRaca;
    std::vector<std::string> infoRaca = {
        "Raca: " + racaNome,
    };
    if (racaNome == "Dwarf") {
        arteRaca = { "  (")_.-\"\"-.  ", "   `-'      `-.)", "  (o_o)       ", "   /|\\        ", "  _/ \\_       " };
        infoRaca.push_back("Resistencia +2");
        infoRaca.push_back("Constituicao +2");
    } else if (racaNome == "Elfo") {
        arteRaca = { "     ^     ", "    / \\    ", "   /   \\   ", "  ( o_o )  ", "   \\   /   ", "    '-'    " };
        infoRaca.push_back("Destreza +2");
        infoRaca.push_back("Sabedoria +1");
    } else if (racaNome == "Humano") {
        arteRaca = { "   \"\"\"  ", "  (o_o) ", "   /|\\  ", "   / \\  " };
        infoRaca.push_back("Forca +1");
        infoRaca.push_back("Inteligencia +1");
        infoRaca.push_back("Sabedoria +1");
    } else if (racaNome == "Ork") {
        arteRaca = { "  .-\"\"\"-.", " /       \\", "|  (o_o) |", " \\   |   /", "  `-' `-'", "    / \\  " };
        infoRaca.push_back("Forca +3");
        infoRaca.push_back("Destreza -1");
    }

    bool confirmou = TelaMenuIDE::exibirConfirmacaoDeEscolhaComArteLadoALado("RACA", racaNome, infoRaca, arteRaca);
    if (!confirmou) {
        return exibir(nomeJogador);
    }

    TelaRaca::Resultado r;
    r.indice = indiceReal;
    return r;
}