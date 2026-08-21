#include "UI/Renderers/IDE/IDEScreens/Defeat/IDEDefeatScreen.h"
#include <iostream>
#include <vector>
#include <fstream>
#include "UI/Renderers/IDE/IDETheme.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"

void IDEDefeatScreen::display(Character* currentPlayer, int, int, int totalDamageCaused, int, int, int combatTurns) {
    Appearance::clearScreen();
    
    std::string titleStr = IDETheme::keyword("class") + " " + IDETheme::type("DefeatScreen") + " {";
    
    // [PT-BR] Animacao de fade-in do titulo
    // [EN-US] Title fade-in animation
    {
        int targetY = 2;
        std::string lineTitle = Appearance::spacesToCenter(Appearance::getVisualLength(titleStr)) + titleStr;
        
        Appearance::animateFadeIn(30, 40, [&](int /*frame*/, int intensity) {
            float pct = intensity / 255.0f;
            int c = static_cast<int>(255 * pct);
            std::string colorFade = "\033[38;2;" + std::to_string(c) + ";" + std::to_string(c) + ";" + std::to_string(c) + "m";
            std::cout << "\033[" << (targetY + 1) << ";1H" << colorFade
                      << Appearance::removeANSIColors(lineTitle) << "\033[0m\033[J" << std::flush;
        });
    }
    
    Appearance::printCentralized(titleStr);
    
    std::vector<std::string> code = {
        IDETheme::keyword("public:"),
        "    " + IDETheme::type("void") + " " + IDETheme::function("exibirResultado") + "() {",
        "        " + IDETheme::type("bool") + " " + IDETheme::variable("sobreviveu") + " = " + IDETheme::keyword("false") + ";",
        "        " + IDETheme::type("if") + " (!sobreviveu) {",
        "            " + IDETheme::variable("std::cout") + " << " + IDETheme::stringLiteral(currentPlayer->getName() + " pereceu em batalha...") + " << " + IDETheme::variable("std::endl") + ";",
        "            " + IDETheme::variable("estatisticas") + "->" + IDETheme::function("imprimir") + "(" + IDETheme::number(std::to_string(combatTurns)) + ", " + IDETheme::number(std::to_string(totalDamageCaused)) + ");",
        "            " + IDETheme::keyword("throw") + " " + IDETheme::type("std::runtime_error") + "(" + IDETheme::stringLiteral("GAME OVER") + ");",
        "        }",
        "    }",
        "};"
    };

    std::cout << "\n";
    Appearance::printBlockCentralizedTyping(code, 15);
    
    std::cout << "\n\n";
    std::string errorMsg = "\033[41;37m FATAL ERROR \033[0m: Unhandled exception em tempo de execucao: GAME OVER";
    Appearance::printCentralized(errorMsg);
    
    std::cout << "\n\n";
    Appearance::printCentralized(IDETheme::comment("Pressione [ENTER] para reiniciar a sessao..."));

    InputControl::waitForEnter();
}
