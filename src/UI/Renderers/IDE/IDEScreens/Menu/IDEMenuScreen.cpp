#include "UI/Renderers/IDE/IDEScreens/Menu/IDEMenuScreen.h"
#include "Core/Utils/InputControl.h"
#include "Core/Utils/Appearance.h"
#include "UI/Screens/Menu/BaseMenuScreen.h"
#include <iostream>
#include <thread>
#include <chrono>

void ScreenMenuGO::displayPanelSoonGame(const std::string& titleDaScreen, bool /*animarFadeIn*/) {
    // Na IDE, nao usamos animacao de fadeIn, printamos como codigo estatico
    Appearance::clearScreen();
    
    std::string colorType = "\033[38;2;78;201;176m"; // Cyan
    std::string colorString = "\033[38;2;214;157;133m"; // Orange/Greenish
    std::string colorPunct = "\033[38;2;212;212;212m"; // White/Gray
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string reset = "\033[0m";

    std::cout << "\n\n  " << colorType << "GameEngine" << colorPunct << "::" 
              << colorType << "Display" << colorPunct << "::" 
              << colorKeyword << "renderTitle" << colorPunct << "("
              << colorString << "\"" << titleDaScreen << "\"" << colorPunct << ");\n\n" << reset;
}

int ScreenMenuGO::displayMainMenuOptions() {
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

        printBlockCentralizedGO(blockCentral);

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

bool ScreenMenuGO::displayConfirmationExit() {
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

        printBlockCentralizedGO(blockCentral);

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

bool ScreenMenuGO::displayConfirmationDeChooseWithArtSideASide(const std::string& typeDeChoose, const std::string& nameDaChoose, const std::vector<std::string>& informationForDisplay, const std::vector<std::string>& artAsciiForDisplay) {
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
        blockCentral.push_back(colorComment + "// Previa da " + typeDeChoose + ": " + nameDaChoose + reset);
        blockCentral.push_back(colorType + "PreviewData " + colorPunct + "data = {");
        
        for (const auto& info : informationForDisplay) {
            blockCentral.push_back("    " + colorString + "\"" + info + "\"" + colorPunct + ",");
        }
        
        blockCentral.push_back("    " + colorKeyword + "struct " + colorPunct + "{");
        for (const auto& art : artAsciiForDisplay) {
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
        
        printBlockCentralizedGO(blockCentral);

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

std::vector<std::string> ScreenMenuGO::composeFrameDeAttributes(const Attributes& stats, const std::string& titleDry, const std::string& titleSkill, const std::string& nameHab, const std::string& descHab, const std::string& titleSkill2, const std::string& nameHab2, const std::string& descHab2) {
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorType = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorString = "\033[38;2;214;157;133m"; // Orange/Greenish
    std::string colorIna = "\033[38;2;181;206;168m"; // Light Green (numbers)
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string reset = "\033[0m";

    std::vector<std::string> res;
    res.push_back(colorComment + "// " + titleDry + reset);
    res.push_back(colorKeyword + "struct " + colorType + "Stats " + colorPunct + "{");
    res.push_back("    " + colorType + "int " + colorPunct + "Vida = " + colorIna + std::to_string(stats.health) + colorPunct + ";");
    res.push_back("    " + colorType + "int " + colorPunct + "Forca = " + colorIna + std::to_string(stats.strength) + colorPunct + ";");
    res.push_back("    " + colorType + "int " + colorPunct + "Destreza = " + colorIna + std::to_string(stats.dexterity) + colorPunct + ";");
    res.push_back("    " + colorType + "int " + colorPunct + "Resistencia = " + colorIna + std::to_string(stats.resistance) + colorPunct + ";");
    res.push_back("    " + colorType + "int " + colorPunct + "Constituicao = " + colorIna + std::to_string(stats.constitution) + colorPunct + ";");
    res.push_back("    " + colorType + "int " + colorPunct + "Inteligencia = " + colorIna + std::to_string(stats.intelligence) + colorPunct + ";");
    res.push_back("    " + colorType + "int " + colorPunct + "Sabedoria = " + colorIna + std::to_string(stats.wisdom) + colorPunct + ";");
    res.push_back(colorPunct + "};" + reset);
    res.push_back("");
    
    res.push_back(colorComment + "// " + titleSkill + reset);
    res.push_back(colorType + "Ability " + colorPunct + "skill1 = " + colorString + "\"" + nameHab + "\"" + colorPunct + ";");
    res.push_back(colorComment + "/* " + descHab + " */" + reset);
    
    if (!titleSkill2.empty()) {
        res.push_back("");
        res.push_back(colorComment + "// " + titleSkill2 + reset);
        res.push_back(colorType + "Ability " + colorPunct + "skill2 = " + colorString + "\"" + nameHab2 + "\"" + colorPunct + ";");
        res.push_back(colorComment + "/* " + descHab2 + " */" + reset);
    }
    
    return res;
}

std::vector<std::string> ScreenMenuGO::compressArtASCII(const std::vector<std::string>& artOriginal, int factorY, int factorX) {
    std::vector<std::string> compressed;
    if (artOriginal.empty() || factorY <= 0 || factorX <= 0) return compressed;

    for (size_t y = 0; y < artOriginal.size(); y += factorY) {
        std::string newLine = "";
        for (size_t x = 0; x < artOriginal[y].length(); x += factorX) {
            char c = ' ';
            // Tenta pegar o caractere mais denso no bloco (fatorY x fatorX)
            for (size_t dy = 0; dy < (size_t)factorY && y + dy < artOriginal.size(); dy++) {
                for (size_t dx = 0; dx < (size_t)factorX && x + dx < artOriginal[y + dy].length(); dx++) {
                    char cur = artOriginal[y + dy][x + dx];
                    if (cur != ' ' && cur != '\n' && cur != '\r' && c == ' ') {
                        c = cur;
                    } else if (cur == '#' || cur == '@' || cur == '%' || cur == '*') { // Prioriza caracteres mais "visíveis"
                        c = cur;
                    }
                }
            }
            newLine += c;
        }
        // Trim right spaces to keep it clean
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

int ScreenMenuGO::calculateSpaceY(int linesDoContent) {
    int height = Appearance::getTerminalHeight();
    return std::max(0, (height - linesDoContent) / 2);
}

void ScreenMenuGO::printBlockCentralizedGO(const std::vector<std::string>& block, int recoilAdditionalX) {
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
