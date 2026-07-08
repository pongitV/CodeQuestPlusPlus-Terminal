#include "TelaIntroducaoIDE.h"
#include <iostream>
#include <string>
#include <vector>
#include "../../../../Core/Utilidades/Aparencia.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"
#include "TelaMenuIDE.h"

void TelaIntroducaoIDE::exibir() {
    std::cout << "\033[?25l";
    Aparencia::limparTela();

    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string reset = "\033[0m";

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

    std::vector<std::string> blocoCentral;
    blocoCentral.push_back(colorComment + "/* ==========================================" + reset);
    blocoCentral.push_back(colorComment + " *               LORE DE AETHERION           " + reset);
    blocoCentral.push_back(colorComment + " * ========================================== */" + reset);
    blocoCentral.push_back("");

    for (const auto& linha : dialogo) {
        if (!linha.empty()) {
            blocoCentral.push_back(colorComment + "// " + linha + reset);
        } else {
            blocoCentral.push_back("");
        }
    }

    blocoCentral.push_back("");
    blocoCentral.push_back(colorKeyword + "system" + colorPunct + "(\"pause\");");

    int espacosY = TelaMenuIDE::calcularEspacoY(blocoCentral.size());
    for (int i = 0; i < espacosY; ++i) std::cout << "\n";
    
    TelaMenuIDE::imprimirBlocoCentralizadoIDE(blocoCentral);

    ControleDeInput::limparBuffer();
    ControleDeInput::lerTecla(); // Sincrono
    ControleDeInput::limparBuffer();
}