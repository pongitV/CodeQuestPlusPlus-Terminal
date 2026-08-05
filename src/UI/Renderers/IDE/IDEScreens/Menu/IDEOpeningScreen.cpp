#include "UI/Renderers/IDE/IDEScreens/Menu/IDEOpeningScreen.h"
#include <iostream>
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "UI/Renderers/IDE/IDEScreens/Menu/IDEMenuScreen.h"

void ScreenOpeningGO::display() {
    std::cout << "\033[?25l";
    Appearance::clearScreen();
    
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorType = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorString = "\033[38;2;214;157;133m"; // Orange/Greenish
    std::string colorIna = "\033[38;2;181;206;168m"; // Light Green
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string reset = "\033[0m";

    std::vector<std::string> blockCentral;
    blockCentral.push_back(colorComment + "// Iniciando boot da Engine..." + reset);
    blockCentral.push_back(colorType + "System" + colorPunct + "::" + colorKeyword + "out" + colorPunct + "." + colorKeyword + "println" + colorPunct + "(" + colorString + "\"Carregando modulos principais...\"" + colorPunct + ");");
    blockCentral.push_back(colorType + "System" + colorPunct + "::" + colorKeyword + "out" + colorPunct + "." + colorKeyword + "println" + colorPunct + "(" + colorString + "\"[OK] Modulo Grafico Inicializado\"" + colorPunct + ");");
    blockCentral.push_back(colorType + "System" + colorPunct + "::" + colorKeyword + "out" + colorPunct + "." + colorKeyword + "println" + colorPunct + "(" + colorString + "\"[OK] Sistema de Input Pronto\"" + colorPunct + ");");
    blockCentral.push_back(colorType + "System" + colorPunct + "::" + colorKeyword + "out" + colorPunct + "." + colorKeyword + "println" + colorPunct + "(" + colorString + "\"[OK] Motor de Perspectiva (IDE) Ativo\"" + colorPunct + ");");
    blockCentral.push_back("");
    blockCentral.push_back(colorType + "GameEngine" + colorPunct + "::" + colorKeyword + "run" + colorPunct + "();");
    blockCentral.push_back("");
    blockCentral.push_back(colorComment + "// -> PRESSIONE QUALQUER TECLA PARA INICIAR <-" + reset);

    int spacesY = ScreenMenuGO::calculateSpaceY(blockCentral.size());
    for (int i = 0; i < spacesY; ++i) std::cout << "\n";
    
    ScreenMenuGO::printBlockCentralizedGO(blockCentral);

    std::cout << std::flush;

    InputControl::clearBuffer();
    InputControl::readKey(); // Sincrono e imediato
    InputControl::clearBuffer();
}