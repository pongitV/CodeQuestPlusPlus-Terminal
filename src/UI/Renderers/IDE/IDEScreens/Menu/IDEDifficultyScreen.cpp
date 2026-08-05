#include "UI/Renderers/IDE/IDEScreens/Menu/IDEDifficultyScreen.h"
#include "UI/Renderers/IDE/IDEScreens/Menu/IDEMenuScreen.h"
#include <iostream>
#include <vector>
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"

DifficultyScreen::Result ScreenDifficultyGO::display(const std::string& namePlayer, const std::string& raceName, const std::string& className) {
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorEnum = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string colorHighlight = "\033[48;2;38;79;120m\033[38;2;255;255;255m"; // Azul escuro bg
    std::string reset = "\033[0m";

    std::vector<std::string> options = {"FACIL", "NORMAL", "DIFICIL", "Voltar (Classe)"};
    
    int selectionCurrent = 0;
    InputControl::clearBuffer();

    while (true) {
        std::cout << "\033[?25l";
        Appearance::clearScreen();
        
        std::vector<std::string> blockCentral;
        blockCentral.push_back(colorComment + "// JOGADOR: " + namePlayer + " | RACA: " + raceName + " | CLASSE: " + className + reset);
        blockCentral.push_back(colorComment + "// Selecione o nivel de desafio da sua jornada" + reset);
        blockCentral.push_back(colorKeyword + "enum class " + colorEnum + "DifficultyLevel " + colorPunct + "{");
        
        for (int i = 0; i < (int)options.size(); ++i) {
            std::string line = "    ";
            std::string nameOption = options[i];
            
            if (i == selectionCurrent) {
                line += colorHighlight + nameOption + reset;
            } else {
                line += colorPunct + nameOption + reset;
            }
            
            if (i < (int)options.size() - 1) {
                line += colorPunct + ",";
            }
            blockCentral.push_back(line);
        }
        
        blockCentral.push_back(colorPunct + "};");
        blockCentral.push_back("");

        if (selectionCurrent == 0) {
            blockCentral.push_back(colorComment + "// FACIL: Inimigos com 1x Atributos, sem habilidades adicionais" + reset);
        } else if (selectionCurrent == 1) {
            blockCentral.push_back(colorComment + "// NORMAL: Inimigos com 1.5x Atributos, com habilidades de raca" + reset);
        } else if (selectionCurrent == 2) {
            blockCentral.push_back(colorComment + "// DIFICIL: Inimigos com 2x Atributos, com habilidades de raca e classe" + reset);
        }

        int spacesY = ScreenMenuGO::calculateSpaceY(blockCentral.size());
        for (int i = 0; i < spacesY; ++i) std::cout << "\n";
        
        ScreenMenuGO::printBlockCentralizedGO(blockCentral);

        unsigned char key = static_cast<unsigned char>(InputControl::readKey());
        if (key == 224 || key == 0 || key == '\033') {
            unsigned char nextKey = static_cast<unsigned char>(InputControl::readKey());
            if (nextKey == '[') nextKey = static_cast<unsigned char>(InputControl::readKey());
            if (nextKey == 72 || nextKey == 'A') key = 'w';
            else if (nextKey == 80 || nextKey == 'B') key = 's';
        }

        if (key == 'w' || key == 'W') {
            selectionCurrent = (selectionCurrent - 1 + (int)options.size()) % (int)options.size();
        } else if (key == 's' || key == 'S') {
            selectionCurrent = (selectionCurrent + 1) % (int)options.size();
        } else if (key == '\r' || key == '\n') {
            break;
        }
    }

    if (selectionCurrent == 3) {
        DifficultyScreen::Result r;
        r.returned = true;
        return r;
    }

    DifficultyScreen::Result r;
    r.index = selectionCurrent;
    return r;
}