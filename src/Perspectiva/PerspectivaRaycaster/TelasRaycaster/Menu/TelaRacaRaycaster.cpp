#include "TelaRacaRaycaster.h"
#include "../Utils/MenuRaycasterLayout.h"
#include "../Utils/MenuRaycasterUtils.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <memory>
#include <algorithm>

#include "../../../TelasBase/Menu/TelaBaseMenu.h"
#include "../../../../Core/Utilidades/FuncoesDialogo.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"
#include "../../../../Entidades/Racas/RacaBase.h"
#include "../../../../Entidades/Racas/Anao.h"
#include "../../../../Entidades/Racas/Elfo.h"
#include "../../../../Entidades/Racas/Humano.h"
#include "../../../../Entidades/Racas/Orc.h"
#include "../../../../Entidades/Personagem.h"

static std::unique_ptr<RacaBase> criarRaca(const std::string& nome) {
    if (nome == "Anao" || nome == "Dwarf") return std::make_unique<Dwarf>();
    if (nome == "Elfo") return std::make_unique<Elfo>();
    if (nome == "Humano") return std::make_unique<Humano>();
    if (nome == "Ork" || nome == "Orc") return std::make_unique<Ork>();
    return nullptr;
}

TelaRaca::Resultado TelaRacaRaycaster::exibir(const std::string& nomeJogador) {
    std::vector<std::string> racas = {"Dwarf", "Elfo", "Humano", "Ork"};
    Aparencia::ordenarAlfabeticamente(racas);
    racas.push_back("VOLTAR");

    int selecaoAtual = 0;
    int larguraConsole = Aparencia::obterLarguraTerminal();

    std::cout << "\033[?25l";
    ControleDeInput::limparBuffer();
    MenuRaycasterUtils::cachearFundo3D("Vila", nullptr);

    while (true) {
        MenuRaycasterUtils::incrementarCicloDia();
        std::ostringstream buffer;
        MenuRaycasterUtils::exibirFundo3D(buffer);

        bool isVoltar = (selecaoAtual >= (int)racas.size() - 1);
        std::string racaNome = isVoltar ? "" : racas[selecaoAtual];
        auto raca = isVoltar ? nullptr : criarRaca(racaNome);

        std::string titulo = "SELECIONE SUA RACA - " + nomeJogador;
        int boxW = (int)titulo.length() + 4;
        int boxX = TelaBaseMenu::calcularOffsetCentral(boxW, larguraConsole);
        std::string borda;
        for (int i = 0; i < (int)titulo.length() + 2; i++) borda += "\u2500";
        MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;255;255;255m\u250C" + borda + "\u2510\033[0m", 2, boxX, larguraConsole);
        MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;255;255;255m\u2502 " + titulo + " \u2502\033[0m", 3, boxX, larguraConsole);
        MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;255;255;255m\u2514" + borda + "\u2518\033[0m", 4, boxX, larguraConsole);

        int yBase = 7;

        // Calculate art dimensions for positioning
        int larguraArte = 0;
        int colCentro = (larguraConsole - 50) / 2;
        if (!isVoltar) {
            const auto& arte = raca->obterAparenciaRaca();
            for (const auto& l : arte) larguraArte = std::max(larguraArte, (int)l.size());
            colCentro = std::max(20, (larguraConsole - larguraArte) / 2);
        }
        int colLista = colCentro - 14;
        if (colLista < 2) colLista = 2;

        // Left: list of options
        for (int i = 0; i < (int)racas.size(); ++i) {
            if (i == selecaoAtual) {
                MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;0;255;0m> " + racas[i] + "\033[0m", yBase + i, colLista, larguraConsole);
            } else {
                MenuRaycasterUtils::sobreporTexto3D(buffer, "  " + racas[i], yBase + i, colLista, larguraConsole);
            }
        }

        if (!isVoltar) {
            Atributos atr = raca->obterAtributosRaca();

            // Center: Pixel art
            MenuRaycasterUtils::imprimirArtePixeladaSimples(buffer, raca->obterAparenciaRaca(), 200, 180, 220, colCentro, yBase);

            // Right: just after the art
            int colDir = colCentro + larguraArte + 3;
            if (colDir > larguraConsole - 20) colDir = larguraConsole - 20;

            // Right: Atributos (top), then skills
            int rightY = yBase;
            MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;100;200;255m[ATRIBUTOS]\033[0m", rightY++, colDir, larguraConsole);
            MenuRaycasterUtils::sobreporTexto3D(buffer, "  \033[38;2;180;180;180mHP " + std::to_string(atr.vida) + "\033[0m", rightY++, colDir, larguraConsole);
            auto linhaAtributo = [&](const std::string& label, int val, const std::string& cor) {
                std::string sinal = (val >= 0 ? "+" : "");
                return "  " + cor + label + " " + sinal + std::to_string(val) + "\033[0m";
            };
            MenuRaycasterUtils::sobreporTexto3D(buffer, linhaAtributo("For", atr.forca, "\033[38;2;255;150;150m"), rightY++, colDir, larguraConsole);
            MenuRaycasterUtils::sobreporTexto3D(buffer, linhaAtributo("Des", atr.destreza, "\033[38;2;150;255;150m"), rightY++, colDir, larguraConsole);
            MenuRaycasterUtils::sobreporTexto3D(buffer, linhaAtributo("Res", atr.resistencia, "\033[38;2;150;150;255m"), rightY++, colDir, larguraConsole);
            MenuRaycasterUtils::sobreporTexto3D(buffer, linhaAtributo("Con", atr.constituicao, "\033[38;2;0;255;255m"), rightY++, colDir, larguraConsole);
            MenuRaycasterUtils::sobreporTexto3D(buffer, linhaAtributo("Int", atr.inteligencia, "\033[38;2;100;200;255m"), rightY++, colDir, larguraConsole);
            MenuRaycasterUtils::sobreporTexto3D(buffer, linhaAtributo("Sab", atr.sabedoria, "\033[38;2;255;215;0m"), rightY++, colDir, larguraConsole);

            rightY++;
            MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;100;200;255m[PASSIVA]\033[0m", rightY++, colDir, larguraConsole);
            MenuRaycasterUtils::sobreporTexto3D(buffer, "  \033[38;2;255;255;255m" + raca->obterNomeHabilidadeRaca() + "\033[0m", rightY++, colDir, larguraConsole);
            std::istringstream passStream(raca->obterDescricaoHabilidadeRaca());
            std::string linhaP;
            while (std::getline(passStream, linhaP)) {
                if (!linhaP.empty())
                    MenuRaycasterUtils::sobreporTexto3D(buffer, "  " + linhaP, rightY++, colDir, larguraConsole);
            }
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
                TelaRaca::Resultado r;
                r.voltou = true;
                return r;
            }
        }

        if (tecla == 'w' || tecla == 'W') {
            selecaoAtual = (selecaoAtual - 1 + (int)racas.size()) % (int)racas.size();
        } else if (tecla == 's' || tecla == 'S') {
            selecaoAtual = (selecaoAtual + 1) % (int)racas.size();
        } else if (tecla == '\r' || tecla == '\n') {
            if (isVoltar) {
                TelaRaca::Resultado r;
                r.voltou = true;
                return r;
            }
            TelaRaca::Resultado r;
            r.indice = selecaoAtual;
            return r;
        }
    }
}
