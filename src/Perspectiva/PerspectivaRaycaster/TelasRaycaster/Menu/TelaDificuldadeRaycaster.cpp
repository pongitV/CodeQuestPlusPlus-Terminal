#include "TelaDificuldadeRaycaster.h"
#include "../Utils/MenuRaycasterLayout.h"
#include "../Utils/MenuRaycasterUtils.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>

#include "../../../TelasBase/Menu/TelaBaseMenu.h"
#include "../../../../Core/Utilidades/FuncoesDialogo.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"

static const std::vector<std::string> arteCaveira = {
    "⠀⠀⠀⠀⣠⣤⣶⣶⣶⣤⣄⡀⠀",
    "⠀⠀⣴⣾⣿⣿⣿⣿⣿⣧⡀⠈⠢",
    "⠀⣼⣿⣿⣿⣿⣿⣿⣿⡿⠁⠀⠀",
    "⢰⡿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀",
    "⠘⣽⡿⠿⠿⣿⣿⣿⣿⣿⣦⣤⡀",
    "⠀⣟⠀⠀⠀⣸⣿⡏⠀⠀⠀⢹⠗",
    "⠀⣿⣷⣶⣾⡿⠁⠙⣄⣀⣀⣠⡀",
    "⠀⠙⠙⢿⡿⣷⣶⣤⣿⣿⡿⠿⠃",
    "⠀⠀⠀⠺⡏⡏⡏⡏⡏⠉⠁⠀⠀",
    "⠀⠀⠀⠀⠀⠀⠁⠁⠀⠀⠀⠀⠀",
};

TelaDificuldade::Resultado TelaDificuldadeRaycaster::exibir(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse) {
    std::vector<std::string> opcoes = {
        "FACIL   (1.0x Atributos inimigos)",
        "MEDIO   (1.5x Atributos inimigos)",
        "DIFICIL (2.0x Atributos inimigos)",
        "VOLTAR"
    };

    struct CorDificuldade { int r, g, b; };
    std::vector<CorDificuldade> cores = {
        {100, 255, 100},
        {255, 215, 0},
        {255, 80, 80},
        {180, 180, 180}
    };

    int selecaoAtual = 0;
    int larguraConsole = Aparencia::obterLarguraTerminal();

    std::cout << "\033[?25l";
    ControleDeInput::limparBuffer();
    MenuRaycasterUtils::cachearFundo3D("Vila", nullptr);

    int arteW = 0;
    for (const auto& l : arteCaveira) arteW = std::max(arteW, Aparencia::obterComprimentoVisual(l));

    while (true) {
        MenuRaycasterUtils::incrementarCicloDia();
        std::ostringstream buffer;
        MenuRaycasterUtils::exibirFundo3D(buffer);

        std::string infoBox = nomeJogador + " | " + nomeRaca + " | " + nomeClasse;
        int boxW = (int)infoBox.length() + 4;
        int boxX = TelaBaseMenu::calcularOffsetCentral(boxW, larguraConsole);
        std::string borda;
        for (int i = 0; i < (int)infoBox.length() + 2; i++) borda += "\u2500";
        MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;255;255;255m\u250C" + borda + "\u2510\033[0m", 2, boxX, larguraConsole);
        MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;255;255;255m\u2502 " + infoBox + " \u2502\033[0m", 3, boxX, larguraConsole);
        MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;255;255;255m\u2514" + borda + "\u2518\033[0m", 4, boxX, larguraConsole);

        int yBase = 7;
        int maxOpLen = 0;
        for (const auto& op : opcoes) maxOpLen = std::max(maxOpLen, (int)op.length());
        int colLista = std::max(2, (larguraConsole - maxOpLen - arteW - 6) / 2);
        int colArte = colLista + maxOpLen + 4;

        int boxLargura = maxOpLen + arteW + 8;
        int boxAltura = std::max((int)opcoes.size(), (int)arteCaveira.size()) + 2;
        TelaBaseMenu::desenharCaixaPreta(buffer, yBase - 1, colLista - 2, boxLargura, boxAltura);

        for (int i = 0; i < (int)opcoes.size(); ++i) {
            if (i == selecaoAtual) {
                MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, "\033[38;2;" + std::to_string(cores[i].r) + ";" + std::to_string(cores[i].g) + ";" + std::to_string(cores[i].b) + "m> " + opcoes[i] + "\033[0m", yBase + i, colLista);
            } else {
                MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, "\033[38;2;120;120;120m  " + opcoes[i] + "\033[0m", yBase + i, colLista);
            }
        }

        auto& c = cores[selecaoAtual];
        std::string corArte = "\033[38;2;" + std::to_string(c.r) + ";" + std::to_string(c.g) + ";" + std::to_string(c.b) + "m";
        for (size_t i = 0; i < arteCaveira.size(); ++i) {
            MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, corArte + arteCaveira[i] + "\033[0m", yBase + (int)i, colArte);
        }

        MenuRaycasterUtils::flushFrameParaConsole(buffer.str());

        if (!ControleDeInput::teclaPressionada()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
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
            selecaoAtual = (selecaoAtual - 1 + (int)opcoes.size()) % (int)opcoes.size();
        } else if (tecla == 's' || tecla == 'S') {
            selecaoAtual = (selecaoAtual + 1) % (int)opcoes.size();
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
