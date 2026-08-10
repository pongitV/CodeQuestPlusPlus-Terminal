#include "UI/Renderers/3D/RaycasterScreens/Menu/RaycasterClassScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Utils/MenuRaycasterLayout.h"
#include "UI/Renderers/3D/RaycasterScreens/Utils/MenuRaycasterUtils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <memory>
#include <algorithm>

#include "UI/Screens/Menu/BaseMenuScreen.h"
#include "Core/Utils/DialogFunctions.h"
#include "Core/Utils/InputControl.h"
#include "Domain/Characters/Classes/ClassFactory.h"
#include "Domain/Characters/Character.h"

struct OptionClass { TypeClass type; std::string name; };

ClassScreen::Result ScreenClassRaycaster::display(const std::string& namePlayer, const std::string& raceName) {
    std::vector<OptionClass> optionsGeneral;
    for (auto t : ClassFactory::getClassesPlayable()) {
        auto temp = ClassFactory::createClass(t);
        optionsGeneral.push_back({t, temp->getClassName()});
    }
    std::sort(optionsGeneral.begin(), optionsGeneral.end(), [](const OptionClass& a, const OptionClass& b) { return a.name < b.name; });

    int selectionCurrent = 0;
    int widthConsole = Appearance::getTerminalWidth();

    std::cout << "\033[?25l";
    InputControl::clearBuffer();
    MenuRaycasterUtils::curlBackground3D("Vila", nullptr);

    while (true) {
        MenuRaycasterUtils::increaseCycleDay();
        std::ostringstream buffer;
        MenuRaycasterUtils::displayBackground3D(buffer);

        bool isReturn = (selectionCurrent >= (int)optionsGeneral.size());
        auto classObj = isReturn ? nullptr : ClassFactory::createClass(optionsGeneral[selectionCurrent].type);

        std::string infoBox = namePlayer + " | " + raceName;
        int boxW = (int)infoBox.length() + 4;
        int boxX = ScreenBaseMenu::calculateOffsetCentral(boxW, widthConsole);
        std::string edge;
        for (int i = 0; i < (int)infoBox.length() + 2; i++) edge += "\u2500";
        MenuRaycasterUtils::superimposeText3D(buffer, "\033[38;2;255;255;255m\u250C" + edge + "\u2510\033[0m", 2, boxX, widthConsole);
        MenuRaycasterUtils::superimposeText3D(buffer, "\033[38;2;255;255;255m\u2502 " + infoBox + " \u2502\033[0m", 3, boxX, widthConsole);
        MenuRaycasterUtils::superimposeText3D(buffer, "\033[38;2;255;255;255m\u2514" + edge + "\u2518\033[0m", 4, boxX, widthConsole);

        int yBase = 7;

        // Calcula dimensoes da arte para posicionamento
        int artWidth = 0;
        int colCenter = (widthConsole - 50) / 2;
        if (!isReturn) {
            const auto& art = classObj->getAppearanceClassMenu();
            for (const auto& l : art) artWidth = std::max(artWidth, (int)l.size());
            colCenter = std::max(20, (widthConsole - artWidth) / 2);
        }
        int colList = colCenter - 14;
        if (colList < 2) colList = 2;

        // Left: list of options
        int totalOptions = (int)optionsGeneral.size() + 1;
        for (int i = 0; i < totalOptions; ++i) {
            std::string nameOption = (i == (int)optionsGeneral.size()) ? "VOLTAR" : optionsGeneral[i].name;
            if (i == selectionCurrent) {
                MenuRaycasterUtils::superimposeText3D(buffer, "\033[38;2;0;255;0m> " + nameOption + "\033[0m", yBase + i, colList, widthConsole);
            } else {
                MenuRaycasterUtils::superimposeText3D(buffer, "  " + nameOption, yBase + i, colList, widthConsole);
            }
        }

        if (!isReturn) {
            Attributes attributes = classObj->getAttributesClass();

            // Center: Pixel art
            MenuRaycasterUtils::printArtPixelatedSimple(buffer, classObj->getAppearanceClassMenu(), 200, 180, 220, colCenter, yBase);

            // Direita: logo apos a arte
            int colSay = colCenter + artWidth + 3;
            if (colSay > widthConsole - 20) colSay = widthConsole - 20;

            // Right: Atributos (top), then skills
            int rightY = yBase;
            MenuRaycasterUtils::superimposeText3D(buffer, "\033[38;2;100;200;255m[ATRIBUTOS]\033[0m", rightY++, colSay, widthConsole);
            MenuRaycasterUtils::superimposeText3D(buffer, "  \033[38;2;180;180;180mHP " + std::to_string(attributes.health) + "\033[0m", rightY++, colSay, widthConsole);
            auto lineAttribute = [&](const std::string& label, int val, const std::string& color) {
                std::string sign = (val >= 0 ? "+" : "");
                return "  " + color + label + " " + sign + std::to_string(val) + "\033[0m";
            };
            MenuRaycasterUtils::superimposeText3D(buffer, lineAttribute("For", attributes.strength, "\033[38;2;255;150;150m"), rightY++, colSay, widthConsole);
            MenuRaycasterUtils::superimposeText3D(buffer, lineAttribute("Des", attributes.dexterity, "\033[38;2;150;255;150m"), rightY++, colSay, widthConsole);
            MenuRaycasterUtils::superimposeText3D(buffer, lineAttribute("Res", attributes.resistance, "\033[38;2;150;150;255m"), rightY++, colSay, widthConsole);
            MenuRaycasterUtils::superimposeText3D(buffer, lineAttribute("Con", attributes.constitution, "\033[38;2;0;255;255m"), rightY++, colSay, widthConsole);
            MenuRaycasterUtils::superimposeText3D(buffer, lineAttribute("Int", attributes.intelligence, "\033[38;2;100;200;255m"), rightY++, colSay, widthConsole);
            MenuRaycasterUtils::superimposeText3D(buffer, lineAttribute("Sab", attributes.wisdom, "\033[38;2;255;215;0m"), rightY++, colSay, widthConsole);

            rightY++;
            MenuRaycasterUtils::superimposeText3D(buffer, "\033[38;2;100;200;255m[ATIVA]\033[0m", rightY++, colSay, widthConsole);
            MenuRaycasterUtils::superimposeText3D(buffer, "  \033[38;2;255;255;255m" + classObj->getNameSkillClass() + "\033[0m", rightY++, colSay, widthConsole);
            std::istringstream descStream(classObj->getDescriptionSkillClass());
            std::string lineDesc;
            while (std::getline(descStream, lineDesc)) {
                if (!lineDesc.empty())
                    MenuRaycasterUtils::superimposeText3D(buffer, "  " + lineDesc, rightY++, colSay, widthConsole);
            }

            rightY++;
            std::string passName = classObj->getNamePassiveClass();
            std::string passDesc = classObj->getDescriptionPassiveClass();
            if (!passName.empty()) {
                MenuRaycasterUtils::superimposeText3D(buffer, "\033[38;2;100;200;255m[PASSIVA]\033[0m", rightY++, colSay, widthConsole);
                MenuRaycasterUtils::superimposeText3D(buffer, "  \033[38;2;255;255;255m" + passName + "\033[0m", rightY++, colSay, widthConsole);
                std::istringstream pStream(passDesc);
                while (std::getline(pStream, lineDesc)) {
                    if (!lineDesc.empty())
                        MenuRaycasterUtils::superimposeText3D(buffer, "  " + lineDesc, rightY++, colSay, widthConsole);
                }
            }
        }

        MenuRaycasterUtils::flushFrameForConsole(buffer.str());

        if (!InputControl::pressedKey()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            continue;
        }

        unsigned char key = static_cast<unsigned char>(InputControl::readKey());
        if (key == 224 || key == 0 || key == '\033') {
            unsigned char nextKey = static_cast<unsigned char>(InputControl::readKey());
            if (nextKey == '[') nextKey = static_cast<unsigned char>(InputControl::readKey());
            if (nextKey == 72 || nextKey == 'A') key = 'w';
            else if (nextKey == 80 || nextKey == 'B') key = 's';
            else if (nextKey == 27) {
                ClassScreen::Result r;
                r.returned = true;
                return r;
            }
        }

        if (key == 'w' || key == 'W') {
            selectionCurrent = (selectionCurrent - 1 + totalOptions) % totalOptions;
        } else if (key == 's' || key == 'S') {
            selectionCurrent = (selectionCurrent + 1) % totalOptions;
        } else if (key == '\r' || key == '\n') {
            if (isReturn) {
                ClassScreen::Result r;
                r.returned = true;
                return r;
            }
            ClassScreen::Result r;
            r.index = selectionCurrent;
            r.name = optionsGeneral[selectionCurrent].name;
            r.selectedClass = optionsGeneral[selectionCurrent].type;
            return r;
        }
    }
}
