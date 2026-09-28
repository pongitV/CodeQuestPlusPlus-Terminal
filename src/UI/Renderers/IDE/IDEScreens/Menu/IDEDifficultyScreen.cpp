#include "UI/Renderers/IDE/IDEScreens/Menu/IDEDifficultyScreen.h"
#include "UI/Renderers/IDE/IDEScreens/Menu/IDEMenuScreen.h"
#include "UI/Renderers/IDE/IDETheme.h"
#include <iostream>
#include <vector>
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"

DifficultyScreen::Result IDEDifficultyScreen::display(const std::string& namePlayer, const std::string& raceName, const std::string& className) {
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorEnum = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string colorHighlight = "\033[1;38;2;78;201;176m"; // Ciano brilhante bold, sem fundo
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

        std::vector<std::string> tabs = {
            "DifficultyConfig.sys",
            "GameTuning.hpp"
        };
        int width = Appearance::getTerminalWidth();
        int height = Appearance::getTerminalHeight();
        auto editorView = IDETheme::renderEditorView(tabs, 0, "// config/DifficultyConfig.sys > enum class DifficultyLevel", blockCentral, width, height, "[W/S] Selecionar Dificuldade | [ENTER] Confirmar");

        Appearance::clearScreen();
        for (const auto& l : editorView) std::cout << l << "\n";
        std::cout << "\033[J" << std::flush;

        unsigned char key = static_cast<unsigned char>(InputControl::readNavKey());

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
