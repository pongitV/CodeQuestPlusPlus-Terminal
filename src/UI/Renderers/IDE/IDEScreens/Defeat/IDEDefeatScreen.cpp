#include "UI/Renderers/IDE/IDEScreens/Defeat/IDEDefeatScreen.h"
#include <iostream>
#include <vector>
#include <fstream>
#include "UI/Renderers/IDE/IDETheme.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"

void ScreenDefeatGO::display(Character* currentPlayer, int, int, int totalDeDamageCaused, int, int, int shiftsCombat) {
    Appearance::clearScreen();
    
    std::string titleStr = ThemeGO::keyword("class") + " " + ThemeGO::type("TelaDerrota") + " {";
    
    // Fade in padronizado do titulo em Y=2
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
        ThemeGO::keyword("public:"),
        "    " + ThemeGO::type("void") + " " + ThemeGO::function("exibirResultado") + "() {",
        "        " + ThemeGO::type("bool") + " " + ThemeGO::variable("sobreviveu") + " = " + ThemeGO::keyword("false") + ";",
        "        " + ThemeGO::type("if") + " (!sobreviveu) {",
        "            " + ThemeGO::variable("std::cout") + " << " + ThemeGO::stringLiteral(currentPlayer->getName() + " pereceu em batalha...") + " << " + ThemeGO::variable("std::endl") + ";",
        "            " + ThemeGO::variable("estatisticas") + "->" + ThemeGO::function("imprimir") + "(" + ThemeGO::number(std::to_string(shiftsCombat)) + ", " + ThemeGO::number(std::to_string(totalDeDamageCaused)) + ");",
        "            " + ThemeGO::keyword("throw") + " " + ThemeGO::type("std::runtime_error") + "(" + ThemeGO::stringLiteral("GAME OVER") + ");",
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
    Appearance::printCentralized(ThemeGO::comment("Pressione [ENTER] para reiniciar a sessao..."));

    InputControl::waitForEnter();
}
