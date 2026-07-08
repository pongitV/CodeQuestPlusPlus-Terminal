#include "TelaClasseRaycaster.h"
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
#include "../../../../Entidades/Classes/ClasseBase.h"
#include "../../../../Entidades/Classes/Arqueiro.h"
#include "../../../../Entidades/Classes/Bardo.h"
#include "../../../../Entidades/Classes/Guerreiro.h"
#include "../../../../Entidades/Classes/Mago.h"
#include "../../../../Entidades/Classes/Necromante.h"
#include "../../../../Entidades/Personagem.h"

static std::unique_ptr<ClasseBase> criarClasse(const std::string& nome) {
    if (nome == "Arqueiro") return std::make_unique<Arqueiro>();
    if (nome == "Bardo") return std::make_unique<Bardo>();
    if (nome == "Guerreiro") return std::make_unique<Guerreiro>();
    if (nome == "Mago") return std::make_unique<Mago>();
    if (nome == "Necromante") return std::make_unique<Necromante>();
    return nullptr;
}

TelaClasse::Resultado TelaClasseRaycaster::exibir(const std::string& nomeJogador, const std::string& nomeRaca) {
    std::vector<std::string> classes = {"Arqueiro", "Bardo", "Guerreiro", "Mago", "Necromante"};
    Aparencia::ordenarAlfabeticamente(classes);
    classes.push_back("VOLTAR");

    int selecaoAtual = 0;
    int larguraConsole = Aparencia::obterLarguraTerminal();

    std::cout << "\033[?25l";
    ControleDeInput::limparBuffer();
    MenuRaycasterUtils::cachearFundo3D("Vila", nullptr);

    while (true) {
        MenuRaycasterUtils::incrementarCicloDia();
        std::ostringstream buffer;
        MenuRaycasterUtils::exibirFundo3D(buffer);

        bool isVoltar = (selecaoAtual >= (int)classes.size() - 1);
        std::string classeNome = isVoltar ? "" : classes[selecaoAtual];
        auto classe = isVoltar ? nullptr : criarClasse(classeNome);

        std::string infoBox = nomeJogador + " | " + nomeRaca;
        int boxW = (int)infoBox.length() + 4;
        int boxX = TelaBaseMenu::calcularOffsetCentral(boxW, larguraConsole);
        std::string borda;
        for (int i = 0; i < (int)infoBox.length() + 2; i++) borda += "\u2500";
        MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;255;255;255m\u250C" + borda + "\u2510\033[0m", 2, boxX, larguraConsole);
        MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;255;255;255m\u2502 " + infoBox + " \u2502\033[0m", 3, boxX, larguraConsole);
        MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;255;255;255m\u2514" + borda + "\u2518\033[0m", 4, boxX, larguraConsole);

        int yBase = 7;

        // Calculate art dimensions for positioning
        int larguraArte = 0;
        int colCentro = (larguraConsole - 50) / 2;
        if (!isVoltar) {
            const auto& arte = classe->obterAparenciaClasseMenu();
            for (const auto& l : arte) larguraArte = std::max(larguraArte, (int)l.size());
            colCentro = std::max(20, (larguraConsole - larguraArte) / 2);
        }
        int colLista = colCentro - 14;
        if (colLista < 2) colLista = 2;

        // Left: list of options
        for (int i = 0; i < (int)classes.size(); ++i) {
            if (i == selecaoAtual) {
                MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;0;255;0m> " + classes[i] + "\033[0m", yBase + i, colLista, larguraConsole);
            } else {
                MenuRaycasterUtils::sobreporTexto3D(buffer, "  " + classes[i], yBase + i, colLista, larguraConsole);
            }
        }

        if (!isVoltar) {
            Atributos atr = classe->obterAtributosClasse();

            // Center: Pixel art
            MenuRaycasterUtils::imprimirArtePixeladaSimples(buffer, classe->obterAparenciaClasseMenu(), 200, 180, 220, colCentro, yBase);

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
            MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;100;200;255m[ATIVA]\033[0m", rightY++, colDir, larguraConsole);
            MenuRaycasterUtils::sobreporTexto3D(buffer, "  \033[38;2;255;255;255m" + classe->obterNomeHabilidadeClasse() + "\033[0m", rightY++, colDir, larguraConsole);
            std::istringstream descStream(classe->obterDescricaoHabilidadeClasse());
            std::string linhaDesc;
            while (std::getline(descStream, linhaDesc)) {
                if (!linhaDesc.empty())
                    MenuRaycasterUtils::sobreporTexto3D(buffer, "  " + linhaDesc, rightY++, colDir, larguraConsole);
            }

            rightY++;
            std::string passNome = classe->obterNomePassivaClasse();
            std::string passDesc = classe->obterDescricaoPassivaClasse();
            if (!passNome.empty()) {
                MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;100;200;255m[PASSIVA]\033[0m", rightY++, colDir, larguraConsole);
                MenuRaycasterUtils::sobreporTexto3D(buffer, "  \033[38;2;255;255;255m" + passNome + "\033[0m", rightY++, colDir, larguraConsole);
                std::istringstream pStream(passDesc);
                while (std::getline(pStream, linhaDesc)) {
                    if (!linhaDesc.empty())
                        MenuRaycasterUtils::sobreporTexto3D(buffer, "  " + linhaDesc, rightY++, colDir, larguraConsole);
                }
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
                TelaClasse::Resultado r;
                r.voltou = true;
                return r;
            }
        }

        if (tecla == 'w' || tecla == 'W') {
            selecaoAtual = (selecaoAtual - 1 + (int)classes.size()) % (int)classes.size();
        } else if (tecla == 's' || tecla == 'S') {
            selecaoAtual = (selecaoAtual + 1) % (int)classes.size();
        } else if (tecla == '\r' || tecla == '\n') {
            if (isVoltar) {
                TelaClasse::Resultado r;
                r.voltou = true;
                return r;
            }
            TelaClasse::Resultado r;
            r.indice = selecaoAtual;
            return r;
        }
    }
}
