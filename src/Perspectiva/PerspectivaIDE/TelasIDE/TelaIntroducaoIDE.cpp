#include "TelaIntroducaoIDE.h"
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/FuncoesDialogo.h"
#include "../../../Core/Utilidades/ControleDeInput.h"

void TelaIntroducaoIDE::exibir() {
    std::cout << "\033[?25l";
    Aparencia::limparTela();

    int altura = Aparencia::obterAlturaTerminal();

    std::vector<std::string> dialogo = {
        "O mundo de Aetherion ja foi palco de grandes batalhas...",
        "",
        "Reinos surgiram e caıram, heroıs se ergueram e desapareceram nas brumas do tempo.",
        "Agora, uma nova sombra se estende sobre a terra.",
        "",
        "Dizem as lendas que um antigo mal, selado ha milenios, comeca a despertar.",
        "As fronteiras entre os mundos enfraquecem, e criaturas das trevas",
        "emergem das profundezas para semear o caos.",
        "",
        "Neste mundo de magia e metal, onde o destino e escrito com sangue e aco,",
        "uma nova historia esta prestes a comecar.",
        "",
        "A sua historia."
    };

    for (const auto& linha : dialogo) {
        Aparencia::limparTela();
        std::cout << "\033[48;2;0;0;0m";

        int centroY = altura / 2 - static_cast<int>(dialogo.size()) / 2;
        for (int i = 0; i < centroY; ++i) {
            std::cout << "\n";
        }

        if (!linha.empty()) {
            Aparencia::imprimirCentralizadoDigitando(linha, 30);
        }
        std::cout << "\n";

        if (linha.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(400));
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }

    std::cout << "\033[0m";
    ControleDeInput::aguardarEnter();
}