#include "TelaClasseIDE.h"
#include "TelaMenuIDE.h"
#include <iostream>
#include <vector>
#include <sstream>
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/FuncoesDialogo.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../TelasBase/Menu/TelaMenuLayout.h"

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

TelaClasse::Resultado TelaClasseIDE::exibir(const std::string& nomeJogador, const std::string& nomeRaca) {
    std::vector<std::string> opcoes = {"Arqueiro", "Bardo", "Guerreiro", "Mago", "Necromante"};
    Aparencia::ordenarAlfabeticamente(opcoes);
    opcoes.push_back("VOLTAR (selecao de raca)");
    int escolha = exibirPromptGenerico("SELECAO DE CLASSE", "| JOGADOR: " + nomeJogador + " | RACA: " + nomeRaca + " |",
        {FuncoesDialogo::formatarMsgNarracao("Qual seu caminho?")},
        opcoes);

    if (escolha >= 5) {
        TelaClasse::Resultado r;
        r.voltou = true;
        return r;
    }

    int indiceReal = escolha;
    std::vector<std::string> classesOrdenadas = {"Arqueiro", "Bardo", "Guerreiro", "Mago", "Necromante"};
    Aparencia::ordenarAlfabeticamente(classesOrdenadas);
    std::string classeNome = classesOrdenadas[indiceReal];

    std::vector<std::string> arteClasse;
    std::vector<std::string> infoClasse = { "Classe: " + classeNome };

    Atributos statsBase;
    if (classeNome == "Arqueiro") {
        arteClasse = { "  ^", " /|\\", " / \\", "  |", " / \\", "    ~>===>>>" };
        statsBase = { 8, 6, 10, 7, 6, 8, 7 };
        infoClasse.push_back("Habilidade: Tiro Certeiro (+50% dano, 75% chance)");
    } else if (classeNome == "Bardo") {
        arteClasse = { "  ^", " /|\\_\\266983", " / \\" };
        statsBase = { 8, 5, 8, 6, 8, 10, 9 };
        infoClasse.push_back("Habilidade: Melodia Curativa (cura aliados)");
    } else if (classeNome == "Guerreiro") {
        arteClasse = { "  O", " /|\\", " / \\", "  |", " / \\" };
        statsBase = { 12, 9, 7, 9, 9, 5, 5 };
        infoClasse.push_back("Habilidade: Golpe Imparavel (ignora defesa)");
    } else if (classeNome == "Mago") {
        arteClasse = { "  ^", " /|\\*", " / \\" };
        statsBase = { 6, 4, 6, 5, 12, 10, 12 };
        infoClasse.push_back("Habilidade: Bola de Fogo (dano em area)");
    } else if (classeNome == "Necromante") {
        arteClasse = { "  ^", " /|\\+", " / \\" };
        statsBase = { 7, 4, 7, 5, 10, 9, 11 };
        infoClasse.push_back("Habilidade: Invocar Esqueleto (cria aliado)");
    }

    std::vector<std::string> quadroAtributos = TelaMenuIDE::comporQuadroDeAtributos(
        statsBase, "ATRIBUTOS BASE", "HABILIDADE UNICA", classeNome,
        infoClasse.size() > 1 ? infoClasse[1] : "");

    for (const auto& linha : quadroAtributos) {
        infoClasse.push_back(linha);
    }

    bool confirmou = TelaMenuIDE::exibirConfirmacaoDeEscolhaComArteLadoALado("CLASSE", classeNome, infoClasse, arteClasse);
    if (!confirmou) {
        return exibir(nomeJogador, nomeRaca);
    }

    TelaClasse::Resultado r;
    r.indice = indiceReal;
    return r;
}