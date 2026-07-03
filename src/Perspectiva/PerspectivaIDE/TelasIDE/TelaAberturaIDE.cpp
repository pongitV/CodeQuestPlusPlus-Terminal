#include "TelaAberturaIDE.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../TelasBase/Menu/TelaMenuLayout.h"

void TelaAberturaIDE::exibir() {
    std::cout << "\033[?25l";
    Aparencia::limparTela();

    int larguraConsole = Aparencia::obterLarguraTerminal();
    int alturaConsole = Aparencia::obterAlturaTerminal();

    int larguraLinhaCompleta = 140;
    int recuo = std::max(0, (larguraConsole - larguraLinhaCompleta) / 2);
    std::string margem(recuo, ' ');

    Aparencia::animarFadeIn(32, 30, [&](int, int intensidade) {
        std::ostringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

        std::string corRGB = Aparencia::obterCorRGBFade(Cor::BRANCO, intensidade);
        int intensidadeLaranjaG = static_cast<int>(intensidade * 0.65);
        std::string corRGBPlus = "\033[38;2;" + std::to_string(intensidade) + ";" + std::to_string(intensidadeLaranjaG) + ";0m";

        buffer << "\n\n";
        for (size_t i = 0; i < ArtesMenu::logoTexto.size(); ++i) {
            buffer << margem << corRGB << ArtesMenu::logoTexto[i];
            if (i < ArtesMenu::logoPlus.size()) buffer << corRGBPlus << ArtesMenu::logoPlus[i];
            buffer << "\033[0m\n";
        }
        buffer << "\033[J";
        std::cout.rdbuf(oldCout);
        std::cout << "\033[H" << buffer.str() << std::flush;
    });

    int frame = 0;
    int estadoAnimacao = 0;
    int intensidadeCor = 0;
    int arteAtual = 0;
    int holdCounter = 0;

    const auto& todasAsArtes = ArtesMenu::animacaoRpg;
    if (todasAsArtes.empty()) return;

    ControleDeInput::limparBuffer();
    while (!ControleDeInput::teclaPressionada()) {
        std::ostringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

        std::string corBranco = "\033[38;2;255;255;255m";
        std::string corLaranja = "\033[38;2;255;165;0m";

        buffer << "\n\n";
        for (size_t i = 0; i < ArtesMenu::logoTexto.size(); ++i) {
            buffer << margem << corBranco << ArtesMenu::logoTexto[i];
            if (i < ArtesMenu::logoPlus.size()) buffer << corLaranja << ArtesMenu::logoPlus[i];
            buffer << "\033[0m\n";
        }

        buffer << "\n\n\n";
        std::string corOpcao = (frame % 8 < 4) ? "\033[38;2;255;255;255m" : "\033[38;2;120;120;120m";
        std::string textoOpcao = "-> PRESSIONE QUALQUER TECLA PARA INICIAR <-";
        int espacosMenu = std::max(0, (larguraConsole - static_cast<int>(textoOpcao.length())) / 2);
        buffer << std::string(espacosMenu, ' ') << corOpcao << textoOpcao << "\033[0m\n\n\n";

        if (estadoAnimacao == 0) {
            intensidadeCor += 15;
            if (intensidadeCor >= 255) { intensidadeCor = 255; estadoAnimacao = 1; holdCounter = 0; }
        } else if (estadoAnimacao == 1) {
            holdCounter++;
            if (holdCounter > 30) estadoAnimacao = 2;
        } else if (estadoAnimacao == 2) {
            intensidadeCor -= 15;
            if (intensidadeCor <= 0) { intensidadeCor = 0; estadoAnimacao = 0; arteAtual = (arteAtual + 3) % todasAsArtes.size(); }
        }

        if (intensidadeCor > 0) {
            std::string corFade = "\033[38;2;" + std::to_string(intensidadeCor) + ";" + std::to_string(intensidadeCor) + ";" + std::to_string(intensidadeCor) + "m";
            const auto& arteEsq = todasAsArtes[arteAtual % todasAsArtes.size()];
            const auto& arteCen = todasAsArtes[(arteAtual + 1) % todasAsArtes.size()];
            const auto& arteDir = todasAsArtes[(arteAtual + 2) % todasAsArtes.size()];

            int wEsq = 0; for (const auto& l : arteEsq) wEsq = std::max(wEsq, Aparencia::obterComprimentoVisual(l));
            int wCen = 0; for (const auto& l : arteCen) wCen = std::max(wCen, Aparencia::obterComprimentoVisual(l));
            int wDir = 0; for (const auto& l : arteDir) wDir = std::max(wDir, Aparencia::obterComprimentoVisual(l));

            int centerEsq = larguraConsole / 6;
            int centerCen = larguraConsole / 2;
            int centerDir = 5 * larguraConsole / 6;

            int startEsq = std::max(0, centerEsq - wEsq / 2);
            int startCen = std::max(0, centerCen - wCen / 2);
            int startDir = std::max(0, centerDir - wDir / 2);

            int maxL = std::max({ static_cast<int>(arteEsq.size()), static_cast<int>(arteCen.size()), static_cast<int>(arteDir.size()) });
            for (int i = 0; i < maxL; ++i) {
                std::string strEsq = (i < (int)arteEsq.size()) ? arteEsq[i] : "";
                std::string strCen = (i < (int)arteCen.size()) ? arteCen[i] : "";
                std::string strDir = (i < (int)arteDir.size()) ? arteDir[i] : "";
                std::string linhaFinal = "";
                int cursor = 0;
                auto printCol = [&](int start, const std::string& texto) {
                    if (texto.empty()) return;
                    int padding = start - cursor;
                    if (padding > 0) { linhaFinal += std::string(padding, ' '); cursor += padding; }
                    linhaFinal += corFade + texto + "\033[0m";
                    cursor += Aparencia::obterComprimentoVisual(texto);
                };
                printCol(startEsq, strEsq);
                printCol(startCen, strCen);
                printCol(startDir, strDir);
                buffer << linhaFinal << "\033[K\n";
            }
        }

        buffer << "\033[J";
        std::string versao = "Versao 0.1";
        int colVersao = std::max(1, larguraConsole - (int)versao.length() - 1);
        buffer << "\033[" << alturaConsole << ";" << colVersao << "H\033[38;2;100;100;100m" << versao << "\033[0m";

        std::cout.rdbuf(oldCout);
        std::cout << "\033[H" << buffer.str() << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        frame++;
    }

    ControleDeInput::lerTecla();
    ControleDeInput::limparBuffer();
}