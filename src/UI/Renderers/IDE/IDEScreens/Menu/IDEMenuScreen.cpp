#include "UI/Renderers/IDE/IDEScreens/Menu/IDEMenuScreen.h"
#include "Core/Utils/InputControl.h"
#include "Core/Utils/Appearance.h"
#include "UI/Screens/Menu/BaseMenuScreen.h"
#include <iostream>
#include <thread>
#include <chrono>

void IDEMenuScreen::displayGameLogoPanel(const std::string& screenTitle, bool /*animarFadeIn*/) {
    // [PT-BR] Na visao IDE nao utilizamos animacao de fadeIn; renderizamos como codigo estatico
    // [EN-US] In IDE view we do not use fadeIn animation; rendered as static code
    Appearance::clearScreen();
    
    std::string colorType = "\033[38;2;78;201;176m"; // Cyan
    std::string colorString = "\033[38;2;214;157;133m"; // Orange/Greenish
    std::string colorPunct = "\033[38;2;212;212;212m"; // White/Gray
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string reset = "\033[0m";

    std::cout << "\n\n  " << colorType << "GameEngine" << colorPunct << "::" 
              << colorType << "Display" << colorPunct << "::" 
              << colorKeyword << "renderTitle" << colorPunct << "("
              << colorString << "\"" << screenTitle << "\"" << colorPunct << ");\n\n" << reset;
}

int IDEMenuScreen::displayMainMenuOptions() {
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorType = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorString = "\033[38;2;214;157;133m"; // Orange
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string colorHighlight = "\033[48;2;38;79;120m\033[38;2;255;255;255m"; // Azul escuro bg
    std::string reset = "\033[0m";

    std::vector<std::string> options = {"Novo Jogo", "Sair do Jogo"};
    
    std::string colorEnum = "\033[38;2;78;201;176m"; // Cyan

    int selectionCurrent = 0;
    std::cout << "\033[?25l";
    InputControl::clearBuffer();

    while (true) {
        std::vector<std::string> blockCentral;
        blockCentral.push_back("  " + colorType + "GameEngine" + colorPunct + "::" 
              + colorType + "Display" + colorPunct + "::" 
              + colorKeyword + "renderTitle" + colorPunct + "("
              + colorString + "\"MENU PRINCIPAL\"" + colorPunct + ");");
        blockCentral.push_back("");
        blockCentral.push_back(colorComment + "// Selecione uma opcao para continuar" + reset);
        blockCentral.push_back(colorKeyword + "enum class " + colorEnum + "MenuOption " + colorPunct + "{");
        
        for (int i = 0; i < (int)options.size(); ++i) {
            std::string line = "    ";
            std::string nameOption = options[i] == "Novo Jogo" ? "NEW_GAME" : "EXIT_GAME";
            
            if (i == selectionCurrent) {
                line += colorHighlight + nameOption + reset;
            } else {
                line += colorPunct + nameOption + reset;
            }
            
            if (i < (int)options.size() - 1) {
                line += colorPunct + ",";
            }
            line += " " + colorComment + "// " + options[i] + reset;
            
            blockCentral.push_back(line);
        }
        
        blockCentral.push_back(colorPunct + "};");
        blockCentral.push_back("");
        blockCentral.push_back(colorKeyword + "return " + colorPunct + "await_selection();");

        int spacesY = calculateSpaceY(blockCentral.size());
        for (int i = 0; i < spacesY; ++i) std::cout << "\n";

        printCentralizedBlockIDE(blockCentral);

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
            if (selectionCurrent == 1) {
                if (displayConfirmationExit()) {
                    return 1;
                }
            } else {
                return selectionCurrent;
            }
        }
    }
}

bool IDEMenuScreen::displayConfirmationExit() {
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorFunc = "\033[38;2;220;220;170m"; // Yellow
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorString = "\033[38;2;214;157;133m"; // Orange/Greenish
    std::string colorHighlight = "\033[48;2;38;79;120m\033[38;2;255;255;255m"; // Azul escuro bg
    std::string reset = "\033[0m";

    int selectionCurrent = 1;
    InputControl::clearBuffer();

    while (true) {
        Appearance::clearScreen();
        std::vector<std::string> blockCentral;
        blockCentral.push_back(colorKeyword + "bool " + colorFunc + "confirmExit" + colorPunct + "() {");
        blockCentral.push_back("    " + colorFunc + "print" + colorPunct + "(" + colorString + "\"Deseja realmente sair do jogo?\"" + colorPunct + ");");
        
        std::string opNo = (selectionCurrent == 0) ? (colorHighlight + "false" + reset) : (colorKeyword + "false" + reset);
        std::string opYes = (selectionCurrent == 1) ? (colorHighlight + "true" + reset) : (colorKeyword + "true" + reset);
        
        blockCentral.push_back("    " + colorKeyword + "return " + colorPunct + (selectionCurrent == 0 ? opNo : opYes) + colorPunct + "; // [0] false (NAO), [1] true (SIM)");
        blockCentral.push_back(colorPunct + "}");

        int spacesY = calculateSpaceY(blockCentral.size());
        for (int i = 0; i < spacesY; ++i) std::cout << "\n";

        printCentralizedBlockIDE(blockCentral);

        unsigned char key = static_cast<unsigned char>(InputControl::readKey());
        if (key == 224 || key == 0 || key == '\033') {
            unsigned char nextKey = static_cast<unsigned char>(InputControl::readKey());
            if (nextKey == '[') nextKey = static_cast<unsigned char>(InputControl::readKey());
            if (nextKey == 72 || nextKey == 'A') key = 'w';
            else if (nextKey == 80 || nextKey == 'B') key = 's';
        }

        if (key == 'w' || key == 'W' || key == 'a' || key == 'A') {
            selectionCurrent = 0;
        } else if (key == 's' || key == 'S' || key == 'd' || key == 'D') {
            selectionCurrent = 1;
        } else if (key == '\r' || key == '\n') {
            return selectionCurrent == 1;
        }
    }
}

bool IDEMenuScreen::displayChooseConfirmationWithArtSideBySide(const std::string& chooseType, const std::string& chooseName, const std::vector<std::string>& informationForDisplay, const std::vector<std::string>& asciiArtForDisplay) {
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorFunc = "\033[38;2;220;220;170m"; // Yellow
    std::string colorType = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorString = "\033[38;2;214;157;133m"; // Orange/Greenish
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string colorHighlight = "\033[48;2;38;79;120m\033[38;2;255;255;255m"; // Azul escuro bg
    std::string reset = "\033[0m";

    int selectionCurrent = 1;
    InputControl::clearBuffer();

    while (true) {
        Appearance::clearScreen();
        
        std::vector<std::string> blockCentral;
        blockCentral.push_back(colorComment + "// Previa da " + chooseType + ": " + chooseName + reset);
        blockCentral.push_back(colorType + "PreviewData " + colorPunct + "data = {");
        
        for (const auto& info : informationForDisplay) {
            blockCentral.push_back("    " + colorString + "\"" + info + "\"" + colorPunct + ",");
        }
        
        blockCentral.push_back("    " + colorKeyword + "struct " + colorPunct + "{");
        for (const auto& art : asciiArtForDisplay) {
            blockCentral.push_back("        " + colorComment + "// " + art + reset);
        }
        blockCentral.push_back("    " + colorPunct + "} art");
        blockCentral.push_back(colorPunct + "};");
        blockCentral.push_back("");
        
        std::string opReturn = (selectionCurrent == 0) ? (colorHighlight + "false" + reset) : (colorKeyword + "false" + reset);
        std::string opConfirm = (selectionCurrent == 1) ? (colorHighlight + "true" + reset) : (colorKeyword + "true" + reset);
        
        blockCentral.push_back(colorKeyword + "return " + colorFunc + "confirm" + colorPunct + "(" + (selectionCurrent == 0 ? opReturn : opConfirm) + colorPunct + ");");
        
        int spacesY = calculateSpaceY(blockCentral.size());
        for (int i = 0; i < spacesY; ++i) std::cout << "\n";
        
        printCentralizedBlockIDE(blockCentral);

        unsigned char key = static_cast<unsigned char>(InputControl::readKey());
        if (key == 224 || key == 0 || key == '\033') {
            unsigned char nextKey = static_cast<unsigned char>(InputControl::readKey());
            if (nextKey == '[') nextKey = static_cast<unsigned char>(InputControl::readKey());
            if (nextKey == 72 || nextKey == 'A') key = 'w';
            else if (nextKey == 80 || nextKey == 'B') key = 's';
        }

        if (key == 'w' || key == 'W' || key == 'a' || key == 'A') {
            selectionCurrent = 0;
        } else if (key == 's' || key == 'S' || key == 'd' || key == 'D') {
            selectionCurrent = 1;
        } else if (key == '\r' || key == '\n') {
            return selectionCurrent == 1;
        }
    }
}

std::vector<std::string> IDEMenuScreen::composeAttributesFrame(const Attributes& stats, const std::string& dryTitle, const std::string& skillTitle, const std::string& skillName, const std::string& skillDesc, const std::string& skillTitle2, const std::string& skillName2, const std::string& skillDesc2) {
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorType = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorString = "\033[38;2;214;157;133m"; // Orange/Greenish
    std::string colorNumber = "\033[38;2;181;206;168m"; // Light Green (numbers)
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string reset = "\033[0m";

    std::vector<std::string> res;
    res.push_back(colorComment + "// " + dryTitle + reset);
    res.push_back(colorKeyword + "struct " + colorType + "Stats " + colorPunct + "{");
    res.push_back("    " + colorType + "int " + colorPunct + "Vida = " + colorNumber + std::to_string(stats.health) + colorPunct + ";");
    res.push_back("    " + colorType + "int " + colorPunct + "Forca = " + colorNumber + std::to_string(stats.strength) + colorPunct + ";");
    res.push_back("    " + colorType + "int " + colorPunct + "Destreza = " + colorNumber + std::to_string(stats.dexterity) + colorPunct + ";");
    res.push_back("    " + colorType + "int " + colorPunct + "Resistencia = " + colorNumber + std::to_string(stats.resistance) + colorPunct + ";");
    res.push_back("    " + colorType + "int " + colorPunct + "Constituicao = " + colorNumber + std::to_string(stats.constitution) + colorPunct + ";");
    res.push_back("    " + colorType + "int " + colorPunct + "Inteligencia = " + colorNumber + std::to_string(stats.intelligence) + colorPunct + ";");
    res.push_back("    " + colorType + "int " + colorPunct + "Sabedoria = " + colorNumber + std::to_string(stats.wisdom) + colorPunct + ";");
    res.push_back(colorPunct + "};" + reset);
    res.push_back("");
    
    res.push_back(colorComment + "// " + skillTitle + reset);
    res.push_back(colorType + "Ability " + colorPunct + "skill1 = " + colorString + "\"" + skillName + "\"" + colorPunct + ";");
    res.push_back(colorComment + "/* " + skillDesc + " */" + reset);
    
    if (!skillTitle2.empty()) {
        res.push_back("");
        res.push_back(colorComment + "// " + skillTitle2 + reset);
        res.push_back(colorType + "Ability " + colorPunct + "skill2 = " + colorString + "\"" + skillName2 + "\"" + colorPunct + ";");
        res.push_back(colorComment + "/* " + skillDesc2 + " */" + reset);
    }
    
    return res;
}

std::vector<std::string> IDEMenuScreen::compressArtASCII(const std::vector<std::string>& artOriginal, int factorY, int factorX) {
    std::vector<std::string> compressed;
    if (artOriginal.empty() || factorY <= 0 || factorX <= 0) return compressed;

    for (size_t y = 0; y < artOriginal.size(); y += factorY) {
        std::string newLine = "";
        for (size_t x = 0; x < artOriginal[y].length(); x += factorX) {
            char c = ' ';
            // [PT-BR] Amostra o caractere mais denso no bloco de escala (fatorY x fatorX)
    // [EN-US] Samples the densest character within scaling block (factorY x factorX)
            for (size_t dy = 0; dy < (size_t)factorY && y + dy < artOriginal.size(); dy++) {
                for (size_t dx = 0; dx < (size_t)factorX && x + dx < artOriginal[y + dy].length(); dx++) {
                    char cur = artOriginal[y + dy][x + dx];
                    if (cur != ' ' && cur != '\n' && cur != '\r' && c == ' ') {
                        c = cur;
                    } else if (cur == '#' || cur == '@' || cur == '%' || cur == '*') { // Prioriza caracteres mais "visiveis"
                        c = cur;
                    }
                }
            }
            newLine += c;
        }
        // [PT-BR] Remove espacos em branco a direita para otimizacao visual
    // [EN-US] Trims trailing whitespace on the right for visual cleanliness
        size_t end = newLine.find_last_not_of(" ");
        if (end != std::string::npos) {
            newLine = newLine.substr(0, end + 1);
            if (!newLine.empty()) {
                compressed.push_back(newLine);
            }
        }
    }
    return compressed;
}

int IDEMenuScreen::calculateSpaceY(int contentLines) {
    int height = Appearance::getTerminalHeight();
    return std::max(0, (height - contentLines) / 2);
}

void IDEMenuScreen::printCentralizedBlockIDE(const std::vector<std::string>& block, int recoilAdditionalX) {
    int width = Appearance::getTerminalWidth();
    int maxLen = 0;
    for (const auto& line : block) {
        int len = Appearance::getVisualLength(line);
        if (len > maxLen) maxLen = len;
    }
    
    int spaces = std::max(0, (width - maxLen) / 2) + recoilAdditionalX;
    std::string pad(spaces, ' ');
    
    for (const auto& line : block) {
        std::cout << pad << line << "\n";
    }
}
