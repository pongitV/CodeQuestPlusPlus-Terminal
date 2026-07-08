#include "TelaIntroducaoRaycaster.h"
#include "../Utils/MenuRaycasterLayout.h"
#include "../Utils/MenuRaycasterUtils.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

#include "../../../TelasBase/Menu/TelaBaseMenu.h"
#include "../../../../Core/Utilidades/FuncoesDialogo.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"

void TelaIntroducaoRaycaster::exibir() {
    int larguraConsole = Aparencia::obterLarguraTerminal();

    std::cout << "\033[?25l";
    ControleDeInput::limparBuffer();
    MenuRaycasterUtils::cachearFundo3D("Vila", nullptr);

    std::string titulo = "Bem-vindo, Jovem Aventureiro!";
    std::string subtitulo = "A Jornada comeca agora...";
    const auto& cena = ArtesRaycaster::cenaIntroducao;

    std::vector<std::string> mensagens = {
        "Voce desperta nos arredores de um lugar desconhecido...",
        "Na sua vista, uma pequena vila sendo atacada por monstros.",
        "Empunhando seu equipamento, voce sente que seu destino o aguarda.",
        "Um novo capitulo se inicia agora.",
        "",
        "Pressione Enter para iniciar sua jornada."
    };

    int larguraTotal = 0;
    for (size_t i = 0; i < cena.size(); ++i) {
        larguraTotal = std::max(larguraTotal, Aparencia::obterComprimentoVisual(cena[i]));
    }
    int margemLogo = TelaBaseMenu::calcularOffsetCentral(larguraTotal, larguraConsole);
    int yTitulo = 3;
    int caracteresRevelados = 0;

    while (!ControleDeInput::teclaPressionada()) {
        MenuRaycasterUtils::incrementarCicloDia();

        std::ostringstream abrBuf;
        MenuRaycasterUtils::exibirFundo3D(abrBuf);

        for (size_t i = 0; i < cena.size(); ++i) {
            MenuRaycasterUtils::sobreporTexto3D(abrBuf, "\033[38;2;255;255;255m" + cena[i] + "\033[0m", yTitulo + (int)i, margemLogo, larguraConsole);
        }

        int charCount = 0;
        int yMsg = yTitulo + (int)cena.size() + 2;

        int maxMsgLen = 0;
        for (const auto& msg : mensagens) maxMsgLen = std::max(maxMsgLen, (int)msg.length());
        int boxW = maxMsgLen + 6;
        int boxX = TelaBaseMenu::calcularOffsetCentral(boxW, larguraConsole);
        TelaBaseMenu::desenharCaixaPreta(abrBuf, yMsg - 1, boxX, boxW, (int)mensagens.size() + 2);

        for (const auto& msg : mensagens) {
            if (msg.empty()) {
                yMsg++;
                charCount += 5;
                continue;
            }
            if (charCount >= caracteresRevelados) break;
            std::string textoAtivo = "";
            for (char c : msg) {
                if (charCount < caracteresRevelados) {
                    textoAtivo += c;
                    charCount++;
                } else {
                    break;
                }
            }
            int esp = TelaBaseMenu::calcularOffsetCentral(msg, larguraConsole);
            MenuRaycasterUtils::sobreporTextoAbsoluto(abrBuf, "\033[38;2;200;200;200m" + textoAtivo + "\033[0m", yMsg, esp);
            yMsg++;
            if (textoAtivo == msg) {
                 charCount += 15;
            }
        }
        caracteresRevelados += 3; // Tipo 3x mais rápido

        MenuRaycasterUtils::flushFrameParaConsole(abrBuf.str());
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    
    ControleDeInput::limparBuffer();
    std::cout << "\033[?25h";
}
