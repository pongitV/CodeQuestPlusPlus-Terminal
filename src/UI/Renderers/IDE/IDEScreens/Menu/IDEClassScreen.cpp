#include "UI/Renderers/IDE/IDEScreens/Menu/IDEClassScreen.h"
#include "UI/Renderers/IDE/IDEScreens/Menu/IDEMenuScreen.h"
#include "UI/Renderers/IDE/IDETheme.h"
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
    std::string colorHighlight = "\033[1;38;2;78;201;176m"; // Ciano brilhante bold, sem fundo
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

        std::vector<std::string> tabs = {
            "ClassRegistry.hpp",
            "SkillTrees.sys"
        };
        int width = Appearance::getTerminalWidth();
        int height = Appearance::getTerminalHeight();
        auto editorView = IDETheme::renderEditorView(tabs, 0, "// src/Domain/Characters/Classes/ClassRegistry.hpp > enum class ClassRole", blockCentral, width, height, "[W/S] Selecionar Classe | [ENTER] Confirmar | [ESC] Voltar");

        Appearance::clearScreen();
        for (const auto& l : editorView) std::cout << l << "\n";
        std::cout << "\033[J" << std::flush;

        unsigned char key = static_cast<unsigned char>(InputControl::readNavKey());

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
