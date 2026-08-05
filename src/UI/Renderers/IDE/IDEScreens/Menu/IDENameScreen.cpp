#include "UI/Renderers/IDE/IDEScreens/Menu/IDENameScreen.h"
#include "UI/Renderers/IDE/IDEScreens/Menu/IDEMenuScreen.h"
#include <iostream>
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"

NameScreen::Result ScreenNameGO::display() {
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorType = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorString = "\033[38;2;214;157;133m"; // Orange
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string reset = "\033[0m";

    std::string name;
    
    while (true) {
        std::cout << "\033[?25l";
        Appearance::clearScreen();
        
        std::vector<std::string> blockCentral;
        blockCentral.push_back(colorComment + "// O mundo clama por um novo destino..." + reset);
        blockCentral.push_back(colorComment + "// E todas as lendas possuem um nome." + reset);
        blockCentral.push_back(colorType + "std" + colorPunct + "::" + colorType + "string " + colorPunct + "playerName;");
        
        if (!name.empty() && name.length() > 20) {
            blockCentral.push_back(colorType + "System" + colorPunct + "::" + colorKeyword + "err" + colorPunct + "." + colorKeyword + "println" + colorPunct + "(" + colorString + "\"Erro: Nome muito longo (max 20 chars).\"" + colorPunct + ");");
        }
        
        blockCentral.push_back(colorType + "std" + colorPunct + "::" + colorKeyword + "cin " + colorPunct + ">> playerName; " + colorComment + "// (Digite o nome ou '0' para voltar)" + reset);
        
        int spacesY = ScreenMenuGO::calculateSpaceY(blockCentral.size() + 1); // +1 pro input
        for (int i = 0; i < spacesY; ++i) std::cout << "\n";
        
        ScreenMenuGO::printBlockCentralizedGO(blockCentral);
        
        std::string promptStr = colorPunct + "> " + reset;
        int spacesX = std::max(0, (Appearance::getTerminalWidth() - Appearance::getVisualLength(promptStr)) / 2);
        std::string pad(spacesX, ' ');
        std::cout << pad << promptStr;
        
        std::cout << "\033[?25h"; // Mostra cursor
        name = InputControl::readEntryProtected();
        
        if (name == "0") {
            NameScreen::Result r;
            r.returned = true;
            return r;
        }
        
        if (!name.empty() && name.length() <= 20) {
            break;
        }
    }

    std::cout << "\033[?25l"; // Esconde cursor
    
    std::vector<std::string> info = { "O nome " + name + " sera registrado." };
    bool confirmed = ScreenMenuGO::displayConfirmationDeChooseWithArtSideASide("NOME", name, info, {});
    
    if (!confirmed) {
        return display(); // Chama recursivo se voltar
    }

    NameScreen::Result r;
    r.name = name;
    return r;
}