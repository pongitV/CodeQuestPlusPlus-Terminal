#include "UI/Renderers/IDE/IDEScreens/Menu/IDEIntroductionScreen.h"
#include <iostream>
#include <string>
#include <vector>
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "UI/Renderers/IDE/IDEScreens/Menu/IDEMenuScreen.h"

void ScreenIntroductionGO::display() {
    std::cout << "\033[?25l";
    Appearance::clearScreen();

    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string reset = "\033[0m";

    std::vector<std::string> dialogue = {
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

    std::vector<std::string> blockCentral;
    blockCentral.push_back(colorComment + "/* ==========================================" + reset);
    blockCentral.push_back(colorComment + " *               LORE DE AETHERION           " + reset);
    blockCentral.push_back(colorComment + " * ========================================== */" + reset);
    blockCentral.push_back("");

    for (const auto& line : dialogue) {
        if (!line.empty()) {
            blockCentral.push_back(colorComment + "// " + line + reset);
        } else {
            blockCentral.push_back("");
        }
    }

    blockCentral.push_back("");
    blockCentral.push_back(colorKeyword + "system" + colorPunct + "(\"pause\");");

    int spacesY = ScreenMenuGO::calculateSpaceY(blockCentral.size());
    for (int i = 0; i < spacesY; ++i) std::cout << "\n";
    
    ScreenMenuGO::printBlockCentralizedGO(blockCentral);

    InputControl::clearBuffer();
    InputControl::readKey(); // Sincrono
    InputControl::clearBuffer();
}