#include "UI/Renderers/IDE/IDEScreens/Menu/IDERaceScreen.h"
#include "UI/Renderers/IDE/IDEScreens/Menu/IDEMenuScreen.h"
#include "UI/Renderers/IDE/IDETheme.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Domain/Characters/Races/RaceFactory.h"
#include "Domain/Characters/Character.h"

struct OptionRace { TypeRace type; std::string name; };

RaceScreen::Result IDERaceScreen::display(const std::string& namePlayer) {
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorEnum = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string colorHighlight = "\033[1;38;2;78;201;176m"; // Ciano brilhante bold, sem fundo
    std::string reset = "\033[0m";

    std::vector<OptionRace> optionsGeneral;
    for (auto t : RaceFactory::getPlayableBreeds()) {
        auto temp = RaceFactory::createRace(t);
        optionsGeneral.push_back({t, temp->getRaceName()});
    }
    std::sort(optionsGeneral.begin(), optionsGeneral.end(), [](const OptionRace& a, const OptionRace& b) { return a.name < b.name; });
    
    int totalOptions = (int)optionsGeneral.size() + 1;
    int selectionCurrent = 0;
    InputControl::clearBuffer();

    while (true) {
        std::cout << "\033[?25l";
        Appearance::clearScreen();
        
        std::vector<std::string> blockCentral;
        blockCentral.push_back(colorComment + "// JOGADOR: " + namePlayer + reset);
        blockCentral.push_back(colorComment + "// Selecione sua raca" + reset);
        blockCentral.push_back(colorKeyword + "enum class " + colorEnum + "Race " + colorPunct + "{");
        
        for (int i = 0; i < totalOptions; ++i) {
            std::string line = "    ";
            std::string nameOption = (i == (int)optionsGeneral.size()) ? "Voltar (Nome)" : optionsGeneral[i].name;
            
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
            "RaceFactory.hpp",
            "CharacterArchetypes.sys"
        };
        int width = Appearance::getTerminalWidth();
        int height = Appearance::getTerminalHeight();
        auto editorView = IDETheme::renderEditorView(tabs, 0, "// src/Domain/Characters/Races/RaceFactory.hpp > enum class Race", blockCentral, width, height, "[W/S] Selecionar Raca | [ENTER] Confirmar | [ESC] Voltar");

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
        RaceScreen::Result r;
        r.returned = true;
        return r;
    }

    std::string raceName = optionsGeneral[selectionCurrent].name;
    std::vector<std::string> artRace;
    std::vector<std::string> infoRace = { "Raca: " + raceName };

    auto raceInstance = RaceFactory::createRace(optionsGeneral[selectionCurrent].type);
    std::vector<std::string> artOriginal = raceInstance->getAppearanceRace();
    
    Attributes attributes = raceInstance->getAttributesRace();
    if (attributes.strength != 0) infoRace.push_back("Forca " + std::string(attributes.strength > 0 ? "+" : "") + std::to_string(attributes.strength));
    if (attributes.dexterity != 0) infoRace.push_back("Destreza " + std::string(attributes.dexterity > 0 ? "+" : "") + std::to_string(attributes.dexterity));
    if (attributes.constitution != 0) infoRace.push_back("Constituicao " + std::string(attributes.constitution > 0 ? "+" : "") + std::to_string(attributes.constitution));
    if (attributes.intelligence != 0) infoRace.push_back("Inteligencia " + std::string(attributes.intelligence > 0 ? "+" : "") + std::to_string(attributes.intelligence));
    if (attributes.wisdom != 0) infoRace.push_back("Sabedoria " + std::string(attributes.wisdom > 0 ? "+" : "") + std::to_string(attributes.wisdom));
    if (attributes.resistance != 0) infoRace.push_back("Resistencia " + std::string(attributes.resistance > 0 ? "+" : "") + std::to_string(attributes.resistance));
    
    artRace = IDEMenuScreen::compressArtASCII(artOriginal, 3, 3);

    bool confirmed = IDEMenuScreen::displayChooseConfirmationWithArtSideBySide("RACA", raceName, infoRace, artRace);
    if (!confirmed) {
        // Retorno recursivo ao cancelar
        return display(namePlayer);
    }

    RaceScreen::Result r;
    r.index = selectionCurrent;
    r.name = raceName;
    r.selectedRace = optionsGeneral[selectionCurrent].type;
    return r;
}
