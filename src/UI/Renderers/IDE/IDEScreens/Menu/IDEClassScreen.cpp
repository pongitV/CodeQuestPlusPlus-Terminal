#include "UI/Renderers/IDE/IDEScreens/Menu/IDEClassScreen.h"
#include "UI/Renderers/IDE/IDEScreens/Menu/IDEMenuScreen.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Domain/Characters/Classes/ClassFactory.h"
#include "Domain/Characters/Character.h"

struct OptionClass { TypeClass type; std::string name; };

ClassScreen::Result IDEClassScreen::display(const std::string& namePlayer, const std::string& raceName) {
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorEnum = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string colorHighlight = "\033[48;2;38;79;120m\033[38;2;255;255;255m"; // Azul escuro bg
    std::string reset = "\033[0m";

    std::vector<OptionClass> optionsGeneral;
    for (auto t : ClassFactory::getClassesPlayable()) {
        auto temp = ClassFactory::createClass(t);
        optionsGeneral.push_back({t, temp->getClassName()});
    }
    std::sort(optionsGeneral.begin(), optionsGeneral.end(), [](const OptionClass& a, const OptionClass& b) { return a.name < b.name; });
    
    int totalOptions = (int)optionsGeneral.size() + 1;
    int selectionCurrent = 0;
    InputControl::clearBuffer();

    while (true) {
        std::cout << "\033[?25l";
        Appearance::clearScreen();
        
        std::vector<std::string> blockCentral;
        blockCentral.push_back(colorComment + "// JOGADOR: " + namePlayer + " | RACA: " + raceName + reset);
        blockCentral.push_back(colorComment + "// Selecione sua classe" + reset);
        blockCentral.push_back(colorKeyword + "enum class " + colorEnum + "ClassRole " + colorPunct + "{");
        
        for (int i = 0; i < totalOptions; ++i) {
            std::string line = "    ";
            std::string nameOption = (i == (int)optionsGeneral.size()) ? "Voltar (Raca)" : optionsGeneral[i].name;
            
            if (i == selectionCurrent) {
                line += colorHighlight + nameOption + reset;
            } else {
                line += colorPunct + nameOption + reset;
            }
            
            if (i < totalOptions - 1) {
                line += colorPunct + ",";
            }
            blockCentral.push_back(line);
        }
        
        blockCentral.push_back(colorPunct + "};");

        int spacesY = IDEMenuScreen::calculateSpaceY(blockCentral.size());
        for (int i = 0; i < spacesY; ++i) std::cout << "\n";
        
        IDEMenuScreen::printCentralizedBlockIDE(blockCentral);

        unsigned char key = static_cast<unsigned char>(InputControl::readKey());
        if (key == 224 || key == 0 || key == '\033') {
            unsigned char nextKey = static_cast<unsigned char>(InputControl::readKey());
            if (nextKey == '[') nextKey = static_cast<unsigned char>(InputControl::readKey());
            if (nextKey == 72 || nextKey == 'A') key = 'w';
            else if (nextKey == 80 || nextKey == 'B') key = 's';
        }

        if (key == 'w' || key == 'W') {
            selectionCurrent = (selectionCurrent - 1 + totalOptions) % totalOptions;
        } else if (key == 's' || key == 'S') {
            selectionCurrent = (selectionCurrent + 1) % totalOptions;
        } else if (key == '\r' || key == '\n') {
            break;
        }
    }

    if (selectionCurrent == (int)optionsGeneral.size()) {
        ClassScreen::Result r;
        r.returned = true;
        return r;
    }

    std::string className = optionsGeneral[selectionCurrent].name;
    std::vector<std::string> artClass;
    std::vector<std::string> infoClass = { "Classe: " + className };

    auto classInstance = ClassFactory::createClass(optionsGeneral[selectionCurrent].type);
    std::vector<std::string> artOriginal = classInstance->getAppearanceClassMenu();
    Attributes statsBase = classInstance->getAttributesClass();
    
    std::string skillString = "Habilidade: " + classInstance->getNameSkillClass();
    infoClass.push_back(skillString);
    
    artClass = IDEMenuScreen::compressArtASCII(artOriginal, 2, 2);

    std::vector<std::string> frameAttributes = IDEMenuScreen::composeAttributesFrame(
        statsBase, "ATRIBUTOS BASE", "HABILIDADE UNICA", className,
        skillString);

    for (const auto& line : frameAttributes) {
        infoClass.push_back(line);
    }

    bool confirmed = IDEMenuScreen::displayChooseConfirmationWithArtSideBySide("CLASSE", className, infoClass, artClass);
    if (!confirmed) {
        return display(namePlayer, raceName);
    }

    ClassScreen::Result r;
    r.index = selectionCurrent;
    r.name = className;
    r.selectedClass = optionsGeneral[selectionCurrent].type;
    return r;
}