#include "UI/Renderers/3D/RaycasterScreens/Menu/RaycasterMenuScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Utils/MenuRaycasterLayout.h"
#include "UI/Renderers/3D/RaycasterScreens/Utils/MenuRaycasterUtils.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>
#include <cmath>
#include "UI/Screens/Menu/BaseMenuScreen.h"
#include "Core/Utils/DialogFunctions.h"
#include "Core/Utils/InputControl.h"

int ScreenMenuRaycaster::displayMainMenuOptions() {
    std::vector<std::string> options = {"Novo Jogo", "Sair do jogo"};

    std::vector<std::vector<std::string>> arts = {
        ArtsRaycaster::artNewAdventure,
        ArtsRaycaster::artLeave
    };

    int selectionCurrent = 0;
    bool confirmingExit = false;
    int widthConsole = Appearance::getTerminalWidth();

    std::cout << "\033[?25l";
    InputControl::clearBuffer();
    MenuRaycasterUtils::curlBackground3D("Vila", nullptr);

    while (true) {
        MenuRaycasterUtils::increaseCycleDay();
        std::ostringstream buffer;
        MenuRaycasterUtils::displayBackground3D(buffer);
        std::string title = "MENU PRINCIPAL";
        std::string colorTitle = "\033[38;2;255;215;0m";
        int offsetTitle = ScreenBaseMenu::calculateOffsetCentral(title, widthConsole);
        MenuRaycasterUtils::superimposeText3D(buffer, colorTitle + title + "\033[0m", 3, offsetTitle, widthConsole);
        int offsetNav = ScreenBaseMenu::calculateOffsetCentral(10, widthConsole);
        MenuRaycasterUtils::superimposeText3D(buffer, "\033[38;2;100;100;100m<  \u2191  |  \u2193  >\033[0m", 4, offsetNav, widthConsole);

        int lineHome = 6;
        int offsetOptions = std::max(0, (widthConsole - 30) / 2 - 10);

        int maxArtH = 0;
        int maxArtW = 0;
        if (selectionCurrent >= 0 && selectionCurrent < (int)arts.size()) {
            maxArtH = (int)arts[selectionCurrent].size();
            for (const auto& l : arts[selectionCurrent]) {
                maxArtW = std::max(maxArtW, Appearance::getVisualLength(l));
            }
        }
        int inaOptions = (int)options.size();
        int boxH = std::max(maxArtH, inaOptions) + 2;
        if (confirmingExit) {
            boxH += 3; 
        }
        int boxW = 32 + maxArtW + 2; 
        if (confirmingExit && boxW < 40) boxW = 40; 
        ScreenBaseMenu::drawBoxBlack(buffer, lineHome - 1, offsetOptions - 2, boxW, boxH);

        for (int i = 0; i < (int)options.size(); ++i) {
            std::string cursor = (i == selectionCurrent && !confirmingExit) ? "> " : "  ";
            std::string colorOption = (i == selectionCurrent && !confirmingExit)
                ? "\033[38;2;255;215;0m"
                : "\033[38;2;180;180;180m";
            MenuRaycasterUtils::superimposeTextAbsolute(buffer, colorOption + cursor + options[i] + "\033[0m", lineHome + i, offsetOptions);
        }

        if (confirmingExit) {
            std::string msgConf = "Confirmar saida? [S]im / [N]ao";
            MenuRaycasterUtils::superimposeTextAbsolute(buffer, "\033[38;2;255;100;100m" + msgConf + "\033[0m", lineHome + inaOptions + 1, offsetOptions);
        }

        if (selectionCurrent >= 0 && selectionCurrent < (int)arts.size()) {
            const auto& art = arts[selectionCurrent];
            int artX = offsetOptions + 32;
            int artY = lineHome;
            
            if (selectionCurrent == 0) {
                long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
                double pulsation = (std::sin(ms * 0.005) + 1.0) / 2.0; 
                int shineCall = 150 + (int)(pulsation * 105);

                std::vector<MenuRaycasterUtils::GroupColor> groups = {
                    {"@", 255, shineCall, 0}, 
                    {"*", 255, shineCall / 2, 0}, 
                    {"#\\/", 139, 69, 19},
                    {"O", 150, 150, 150}
                };
                MenuRaycasterUtils::printArtPixelated(buffer, art, groups, artX, artY);
            } else if (selectionCurrent == 1) {
                long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
                int frameIdx = (ms / 150) % 6; 
                std::vector<std::string> frameArt;
                if (frameIdx == 0 || frameIdx == 5) frameArt = ArtsRaycaster::tvFrame1;
                else if (frameIdx == 1) frameArt = ArtsRaycaster::tvFrame2;
                else if (frameIdx == 2) frameArt = ArtsRaycaster::tvFrame3;
                else if (frameIdx == 3) frameArt = ArtsRaycaster::tvFrame4;
                else frameArt = ArtsRaycaster::tvFrame5; 

                std::vector<MenuRaycasterUtils::GroupColor> groupsTV = {
                    {"#\\/O_o][-|", 80, 80, 80},     
                    {"%", 200, 255, 255}   
                };
                MenuRaycasterUtils::printArtPixelated(buffer, frameArt, groupsTV, artX, artY);
            } else {
                MenuRaycasterUtils::printArtPixelatedSimple(buffer, art, 200, 180, 255, artX, artY);
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
            else if (nextKey == 27) return -1;
        }

        if (confirmingExit) {
            if (key == 's' || key == 'S') {
                return 1;
            } else if (key == 'n' || key == 'N' || key == 27) {
                confirmingExit = false;
            }
            continue;
        }

        if (key == 'w' || key == 'W') {
            selectionCurrent = (selectionCurrent - 1 + (int)options.size()) % (int)options.size();
        } else if (key == 's' || key == 'S') {
            selectionCurrent = (selectionCurrent + 1) % (int)options.size();
        } else if (key == '\r' || key == '\n') {
            if (selectionCurrent == 1) {
                confirmingExit = true;
            } else {
                return selectionCurrent;
            }
        }
    }
}



void ScreenMenuRaycaster::displayPanelSoonGame(const std::string& titleDaScreen, bool animateFadeIn) {
    int widthConsole = Appearance::getTerminalWidth();
    MenuRaycasterUtils::curlBackground3D("Vila", nullptr);

    if (animateFadeIn) {
        for (int f = 0; f <= 10; ++f) {
            MenuRaycasterUtils::increaseCycleDay();
            std::ostringstream buffer;
            MenuRaycasterUtils::displayBackground3D(buffer);

            int shine = f * 255 / 10;
            if (shine > 255) shine = 255;
            std::string color = "\033[38;2;" + std::to_string(shine) + ";" +
                              std::to_string(shine) + ";" + std::to_string(shine) + "m";
            int offset = ScreenBaseMenu::calculateOffsetCentral(titleDaScreen, widthConsole);
            MenuRaycasterUtils::superimposeText3D(buffer, color + titleDaScreen + "\033[0m", 3, offset, widthConsole);
            MenuRaycasterUtils::flushFrameForConsole(buffer.str());
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    } else {
        std::ostringstream buffer;
        MenuRaycasterUtils::displayBackground3D(buffer);
        int offset = ScreenBaseMenu::calculateOffsetCentral(titleDaScreen, widthConsole);
        MenuRaycasterUtils::superimposeText3D(buffer, "\033[38;2;255;215;0m" + titleDaScreen + "\033[0m", 3, offset, widthConsole);
        MenuRaycasterUtils::flushFrameForConsole(buffer.str());
    }
}

bool ScreenMenuRaycaster::displayConfirmationDeChooseWithArtSideASide(
    const std::string& typeDeChoose, const std::string& nameDaChoose,
    const std::vector<std::string>& informationForDisplay,
    const std::vector<std::string>& artAsciiForDisplay)
{
    int widthConsole = Appearance::getTerminalWidth();
    std::cout << "\033[?25l";
    InputControl::clearBuffer();

    MenuRaycasterUtils::curlBackground3D("Vila", nullptr);
    MenuRaycasterUtils::increaseCycleDay();

    std::ostringstream buffer;
    MenuRaycasterUtils::displayBackground3D(buffer);

    std::string title = "PREVIA DA " + typeDeChoose + ": " + nameDaChoose;
    int offset = ScreenBaseMenu::calculateOffsetCentral(title, widthConsole);
    MenuRaycasterUtils::superimposeText3D(buffer, "\033[38;2;255;215;0m" + title + "\033[0m", 3, offset, widthConsole);

    int infoY = 5;
    int artY = 5;
    int infoX = 5;
    int artX = widthConsole / 2 + 2;

    for (size_t i = 0; i < informationForDisplay.size(); ++i) {
        MenuRaycasterUtils::superimposeText3D(buffer, informationForDisplay[i], infoY + (int)i, infoX, widthConsole);
    }
    MenuRaycasterUtils::printArtPixelatedSimple(buffer, artAsciiForDisplay, 200, 180, 255, artX, artY);

    MenuRaycasterUtils::flushFrameForConsole(buffer.str());
    std::vector<std::string> options = {"VOLTAR", "CONFIRMAR"};
    std::string margin = std::to_string(ScreenBaseMenu::calculateOffsetCentral(20, widthConsole));
    int optionDeConfirmation = InputControl::readSelectionMenuWithArrows(options, false, margin);
    return optionDeConfirmation == 1;
}

std::vector<std::string> ScreenMenuRaycaster::composeFrameDeAttributes(
    const Attributes& stats, const std::string& titleDry,
    const std::string& titleSkill, const std::string& nameHab,
    const std::string& descHab,
    const std::string& titleSkill2, const std::string& nameHab2,
    const std::string& descHab2)
{
    auto formatAttribute = [](const std::string& attrName, int valueAtr) {
        std::string colorVal;
        if (attrName == "Resistencia") colorVal = "\033[38;2;100;100;255m";
        else if (attrName == "Constituicao") colorVal = "\033[38;2;0;255;255m";
        else if (attrName == "Vida") {
            if (valueAtr > 100) colorVal = "\033[38;2;100;255;100m";
            else if (valueAtr < 100) colorVal = "\033[38;2;255;80;80m";
            else colorVal = "\033[38;2;255;255;255m";
        } else {
            if (valueAtr > 10) colorVal = "\033[38;2;100;255;100m";
            else if (valueAtr < 10) colorVal = "\033[38;2;255;80;80m";
            else colorVal = "\033[38;2;255;255;255m";
        }
        std::string sign = (valueAtr >= 0 ? "+" : "");
        return " - " + attrName + ": " + colorVal + sign + std::to_string(valueAtr) + "\033[0m";
    };

    std::vector<std::string> result;
    result.push_back("\033[38;2;255;255;255m" + titleDry + "\033[0m");
    result.push_back(formatAttribute("Vida", stats.health));
    result.push_back(formatAttribute("Forca", stats.strength));
    result.push_back(formatAttribute("Destreza", stats.dexterity));
    result.push_back(formatAttribute("Resistencia", stats.resistance));
    result.push_back(formatAttribute("Constituicao", stats.constitution));
    result.push_back(formatAttribute("Inteligencia", stats.intelligence));
    result.push_back(formatAttribute("Sabedoria", stats.wisdom));
    result.push_back("");
    result.push_back("\033[38;2;255;255;255m" + titleSkill + "\033[0m");
    result.push_back(" \033[38;2;0;200;255m" + nameHab + "\033[0m");

    std::istringstream stream(descHab);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty())
            result.push_back(" - \033[38;2;180;180;180m" + line + "\033[0m");
    }

    if (!titleSkill2.empty()) {
        result.push_back("");
        result.push_back("\033[38;2;255;255;255m" + titleSkill2 + "\033[0m");
        result.push_back(" \033[38;2;0;200;255m" + nameHab2 + "\033[0m");
        std::istringstream stream2(descHab2);
        while (std::getline(stream2, line)) {
            if (!line.empty())
                result.push_back(" - \033[38;2;180;180;180m" + line + "\033[0m");
        }
    }

    return result;
}

bool ScreenMenuRaycaster::displayConfirmationExit() {
    int widthConsole = Appearance::getTerminalWidth();
    std::cout << "\033[?25l";
    InputControl::clearBuffer();
    MenuRaycasterUtils::curlBackground3D("Vila", nullptr);
    int selectionCurrent = 1;
    
    std::string question = "Deseja realmente sair do jogo?";
    std::vector<std::string> options = {"NAO", "SIM"};
    
    while (true) {
        MenuRaycasterUtils::increaseCycleDay();
        std::ostringstream buffer;
        MenuRaycasterUtils::displayBackground3D(buffer);
        
        int boxW = Appearance::getVisualLength(question) + 10;
        int boxH = 7;
        int boxX = ScreenBaseMenu::calculateOffsetCentral(boxW, widthConsole);
        int boxY = 14;
        
        ScreenBaseMenu::drawBoxBlack(buffer, boxY, boxX, boxW, boxH);
        
        int offsetQuestion = ScreenBaseMenu::calculateOffsetCentral(question, widthConsole);
        MenuRaycasterUtils::superimposeTextAbsolute(buffer, "\033[38;2;255;255;255m" + question + "\033[0m", boxY + 2, offsetQuestion);

        for (int i = 0; i < (int)options.size(); ++i) {
            std::string cursor = (i == selectionCurrent) ? "> " : "  ";
            std::string colorOption = (i == selectionCurrent) ? "\033[38;2;255;215;0m" : "\033[38;2;180;180;180m";
            int offsetOptions = ScreenBaseMenu::calculateOffsetCentral(options[i] + cursor, widthConsole);
            MenuRaycasterUtils::superimposeTextAbsolute(buffer, colorOption + cursor + options[i] + "\033[0m", boxY + 4 + i, offsetOptions);
        }
        
        int artY = std::max(2, boxY - 11);

        long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        int frameIdx = (ms / 150) % 6; 
        std::vector<std::string> frameArt;
        if (frameIdx == 0 || frameIdx == 5) frameArt = ArtsRaycaster::tvFrame1;
        else if (frameIdx == 1) frameArt = ArtsRaycaster::tvFrame2;
        else if (frameIdx == 2) frameArt = ArtsRaycaster::tvFrame3;
        else if (frameIdx == 3) frameArt = ArtsRaycaster::tvFrame4;
        else frameArt = ArtsRaycaster::tvFrame5; 

        int artX = ScreenBaseMenu::calculateOffsetCentral((int)frameArt[0].length(), widthConsole);

        std::vector<MenuRaycasterUtils::GroupColor> groupsTV = {
            {"#\\/O_o][-|", 80, 80, 80},     
            {"%", 200, 255, 255}   
        };
        MenuRaycasterUtils::printArtPixelated(buffer, frameArt, groupsTV, artX, artY);
        
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
            else if (nextKey == 27) return false;
        }

        if (key == 'w' || key == 'W') {
            selectionCurrent = (selectionCurrent - 1 + (int)options.size()) % (int)options.size();
        } else if (key == 's' || key == 'S') {
            selectionCurrent = (selectionCurrent + 1) % (int)options.size();
        } else if (key == '\r' || key == '\n') {
            return selectionCurrent == 1;
        }
    }
}
