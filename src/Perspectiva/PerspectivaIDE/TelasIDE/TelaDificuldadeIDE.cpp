#include "TelaDificuldadeIDE.h"
#include "TelaMenuIDE.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/FuncoesDialogo.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../TelasBase/Menu/TelaMenuLayout.h"

namespace {
    void exibirInfoBox(const std::string& infoBox) {
        if (infoBox.empty()) return;
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
}

TelaDificuldade::Resultado TelaDificuldadeIDE::exibir(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse) {
    int selecaoAtual = 0;
    int totalOpcoes = 4;
    std::string infoBox = "| JOGADOR: " + nomeJogador + " | RACA: " + nomeRaca + " | CLASSE: " + nomeClasse + " |";
    std::vector<std::string> narracao = {FuncoesDialogo::formatarMsgSistema("Escolha o nivel de desafio da sua jornada:")};

    std::vector<std::string> opcoes = {
        Aparencia::cor(Cor::VERDE) + "FACIL" + Aparencia::cor(Cor::RESET) + "   (Inimigos com 1x Atributos, sem habilidades adicionais)      ",
        Aparencia::cor(Cor::AMARELO) + "NORMAL" + Aparencia::cor(Cor::RESET) + "  (Inimigos com 1.5x Atributos, com habilidades de raca)       ",
        Aparencia::cor(Cor::VERMELHO) + "DIFICIL" + Aparencia::cor(Cor::RESET) + " (Inimigos com 2x Atributos, com habilidades de raca e classe)",
        "VOLTAR (selecao de classe)                                           "
    };

    std::cout << "\033[?25l";
    Aparencia::limparTela();
    TelaMenuIDE::exibirPainelLogoJogo("DIFICULDADE DO MUNDO", false);
    if (!infoBox.empty()) exibirInfoBox(infoBox);
    else std::cout << "\n";
    Aparencia::imprimirBlocoCentralizadoDigitando(narracao);
    std::cout << "\n";

    ControleDeInput::limparBuffer();
    while (true) {
        auto agora = std::chrono::steady_clock::now();
        int tempoMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(agora.time_since_epoch()).count());
        std::string cursorIcon = ((tempoMs / 400) % 2 == 0) ? ">  " : " > ";

        std::ostringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
        buffer << "\033[H";
        TelaMenuIDE::exibirPainelLogoJogo("DIFICULDADE DO MUNDO", false);
        if (!infoBox.empty()) exibirInfoBox(infoBox);
        else buffer << "\n";
        Aparencia::imprimirBlocoCentralizado(narracao);
        buffer << "\n";

        int larguraTerminal = Aparencia::obterLarguraTerminal();
        std::vector<std::string> linhasOpcoes;
        for (int i = 0; i < totalOpcoes; ++i) {
            if (i == selecaoAtual) {
                linhasOpcoes.push_back(Aparencia::cor(Cor::VERDE) + cursorIcon + Aparencia::cor(Cor::RESET) + opcoes[i]);
            } else {
                linhasOpcoes.push_back("   " + opcoes[i]);
            }
        }

        std::vector<std::string> arte;
        if (selecaoAtual == 0) {
            for (const auto& linha : ArtesMenu::arteDificuldadeFacil) arte.push_back(Aparencia::cor(Cor::VERDE) + linha + Aparencia::cor(Cor::RESET));
        } else if (selecaoAtual == 1) {
            for (const auto& linha : ArtesMenu::arteDificuldadeMedia) arte.push_back(Aparencia::cor(Cor::AMARELO) + linha + Aparencia::cor(Cor::RESET));
        } else if (selecaoAtual == 2) {
            for (const auto& linha : ArtesMenu::arteDificuldadeDificil) arte.push_back(Aparencia::cor(Cor::VERMELHO) + linha + Aparencia::cor(Cor::RESET));
        }

        for (const auto& linha : linhasOpcoes) {
            int comp = Aparencia::obterComprimentoVisual(linha);
            int espacos = std::max(0, (larguraTerminal - comp) / 2);
            buffer << std::string(espacos, ' ') << linha << "\033[K\n";
        }
        buffer << "\033[K\n";
        for (const auto& linha : arte) {
            int comp = Aparencia::obterComprimentoVisual(linha);
            int espacos = std::max(0, (larguraTerminal - comp) / 2);
            buffer << std::string(espacos, ' ') << linha << "\033[K\n";
        }
        buffer << "\033[J";
        std::cout.rdbuf(oldCout);
        std::cout << buffer.str() << std::flush;

        if (!ControleDeInput::teclaPressionada()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            continue;
        }

        unsigned char tecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
        if (tecla == 224 || tecla == 0 || tecla == '\033') {
            unsigned char proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == '[') proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == 72 || proxTecla == 'A') tecla = 'w';
            else if (proxTecla == 80 || proxTecla == 'B') tecla = 's';
            else if (proxTecla == 27) {
                TelaDificuldade::Resultado r;
                r.voltou = true;
                return r;
            }
        }

        if (tecla == 'w' || tecla == 'W') {
            selecaoAtual--;
            if (selecaoAtual < 0) selecaoAtual = totalOpcoes - 1;
        } else if (tecla == 's' || tecla == 'S') {
            selecaoAtual++;
            if (selecaoAtual >= totalOpcoes) selecaoAtual = 0;
        } else if (tecla == '\r' || tecla == '\n') {
            if (selecaoAtual == 3) {
                TelaDificuldade::Resultado r;
                r.voltou = true;
                return r;
            }
            TelaDificuldade::Resultado r;
            r.indice = selecaoAtual;
            return r;
        }
    }
}