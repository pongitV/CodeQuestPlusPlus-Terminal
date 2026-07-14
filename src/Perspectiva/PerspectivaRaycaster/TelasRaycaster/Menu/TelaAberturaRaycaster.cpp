#include "TelaAberturaRaycaster.h"
#include "../Utils/MenuRaycasterLayout.h"
#include "../Utils/MenuRaycasterUtils.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

#include "../../../TelasBase/Menu/TelaBaseMenu.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"

void TelaAberturaRaycaster::exibir() {
    std::cout << "\033[?25l";
    MenuRaycasterUtils::cachearFundo3D("Vila", nullptr);

    int larguraConsole = Aparencia::obterLarguraTerminal();
    int alturaConsole = Aparencia::obterAlturaTerminal();

    int larguraLogoMax = 0;
    int larguraTotal = 0;
    for (size_t i = 0; i < ArtesRaycaster::logoTexto.size(); ++i) {
        int wLogo = Aparencia::obterComprimentoVisual(ArtesRaycaster::logoTexto[i]);
        int wPlus = (i < ArtesRaycaster::logoPlus.size())
            ? Aparencia::obterComprimentoVisual(ArtesRaycaster::logoPlus[i]) : 0;
        larguraLogoMax = std::max(larguraLogoMax, wLogo);
        larguraTotal    = std::max(larguraTotal, wLogo + wPlus);
    }

    int margemLogo = TelaBaseMenu::calcularOffsetCentral(larguraTotal, larguraConsole);
    int yLogo = 3;
    int yPrompt = std::max(yLogo + (int)ArtesRaycaster::logoTexto.size() + 2, alturaConsole - 3);

    std::string versao = "Versao 0.1";
    int colVersao = std::max(1, larguraConsole - (int)versao.length() - 1);

    ControleDeInput::limparBuffer();
    while (!ControleDeInput::teclaPressionada()) {
        MenuRaycasterUtils::incrementarCicloDia();
        MenuRaycasterUtils::aplicarCicloDiaNoite(MenuRaycasterUtils::s_fundo3DMenu);
        MenuRaycasterUtils::desenharCastelo();
        MenuRaycasterUtils::desenharCenaBatalha();

        for (size_t i = 1; i < ArtesRaycaster::logoTexto.size(); ++i) {
            int row = yLogo + (int)i - 1;
            int plusIdx = (int)i - 1;
            MenuRaycasterUtils::sobreporNoFrame(row, margemLogo, ArtesRaycaster::logoTexto[i], 255, 255, 255);
            if (plusIdx < (int)ArtesRaycaster::logoPlus.size()) {
                MenuRaycasterUtils::sobreporNoFrame(row, margemLogo + larguraLogoMax, ArtesRaycaster::logoPlus[plusIdx], 255, 165, 0);
            }
        }

        std::string prompt = "[PRESSIONE ENTER PARA CONTINUAR]";
        int espPrompt = TelaBaseMenu::calcularOffsetCentral(static_cast<int>(prompt.size()), larguraConsole);
        MenuRaycasterUtils::sobreporNoFrame(yPrompt, espPrompt, prompt, 255, 255, 255);

        MenuRaycasterUtils::sobreporNoFrame(alturaConsole - 1, colVersao, versao, 100, 100, 100);

        // Saída única via console API (sem flickering)
        std::ostringstream abrBuf;
        abrBuf << "\033[H";
        for (size_t y = 0; y < MenuRaycasterUtils::s_fundo3DMenu.size(); ++y) {
            abrBuf << MenuRaycasterUtils::s_fundo3DMenu[y];
            if (y < MenuRaycasterUtils::s_fundo3DMenu.size() - 1) abrBuf << "\n";
        }
        MenuRaycasterUtils::flushFrameParaConsole(abrBuf.str());

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    ControleDeInput::lerTecla();
    ControleDeInput::limparBuffer();
}
