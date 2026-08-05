#include "UI/Renderers/3D/RaycasterScreens/Menu/RaycasterDifficultyScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Utils/MenuRaycasterLayout.h"
#include "UI/Renderers/3D/RaycasterScreens/Utils/MenuRaycasterUtils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>

#include "UI/Screens/Menu/BaseMenuScreen.h"
#include "Core/Utils/DialogFunctions.h"
#include "Core/Utils/InputControl.h"

static const std::vector<std::string> artSkull = {
    "⠀⠀⠀⠀⣠⣤⣶⣶⣶⣤⣄⡀⠀",
    "⠀⠀⣴⣾⣿⣿⣿⣿⣿⣧⡀⠈⠢",
    "⠀⣼⣿⣿⣿⣿⣿⣿⣿⡿⠁⠀⠀",
    "⢰⡿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀",
    "⠘⣽⡿⠿⠿⣿⣿⣿⣿⣿⣦⣤⡀",
    "⠀⣟⠀⠀⠀⣸⣿⡏⠀⠀⠀⢹⠗",
    "⠀⣿⣷⣶⣾⡿⠁⠙⣄⣀⣀⣠⡀",
    "⠀⠙⠙⢿⡿⣷⣶⣤⣿⣿⡿⠿⠃",
    "⠀⠀⠀⠺⡏⡏⡏⡏⡏⠉⠁⠀⠀",
    "⠀⠀⠀⠀⠀⠀⠁⠁⠀⠀⠀⠀⠀",
};

DifficultyScreen::Result ScreenDifficultyRaycaster::display(const std::string& namePlayer, const std::string& raceName, const std::string& className) {
    std::vector<std::string> options = {
        "FACIL   (1.0x Atributos inimigos)",
        "MEDIO   (1.5x Atributos inimigos)",
        "DIFICIL (2.0x Atributos inimigos)",
        "VOLTAR"
    };

    struct ColorDifficulty { int r, g, b; };
    std::vector<ColorDifficulty> colors = {
        {100, 255, 100},
        {255, 215, 0},
        {255, 80, 80},
        {180, 180, 180}
    };

    int selectionCurrent = 0;
    int widthConsole = Appearance::getTerminalWidth();

    std::cout << "\033[?25l";
    InputControl::clearBuffer();
    MenuRaycasterUtils::curlBackground3D("Vila", nullptr);

    int artW = 0;
    for (const auto& l : artSkull) artW = std::max(artW, Appearance::getVisualLength(l));

    while (true) {
        MenuRaycasterUtils::increaseCycleDay();
        std::ostringstream buffer;
        MenuRaycasterUtils::displayBackground3D(buffer);

        std::string infoBox = namePlayer + " | " + raceName + " | " + className;
        int boxW = (int)infoBox.length() + 4;
        int boxX = ScreenBaseMenu::calculateOffsetCentral(boxW, widthConsole);
        std::string edge;
        for (int i = 0; i < (int)infoBox.length() + 2; i++) edge += "\u2500";
        MenuRaycasterUtils::superimposeText3D(buffer, "\033[38;2;255;255;255m\u250C" + edge + "\u2510\033[0m", 2, boxX, widthConsole);
        MenuRaycasterUtils::superimposeText3D(buffer, "\033[38;2;255;255;255m\u2502 " + infoBox + " \u2502\033[0m", 3, boxX, widthConsole);
        MenuRaycasterUtils::superimposeText3D(buffer, "\033[38;2;255;255;255m\u2514" + edge + "\u2518\033[0m", 4, boxX, widthConsole);

        int yBase = 7;
        int maxOpLen = 0;
        for (const auto& op : options) maxOpLen = std::max(maxOpLen, (int)op.length());
        int colList = std::max(2, (widthConsole - maxOpLen - artW - 6) / 2);
        int colArt = colList + maxOpLen + 4;

        int boxWidth = maxOpLen + artW + 8;
        int boxHeight = std::max((int)options.size(), (int)artSkull.size()) + 2;
        ScreenBaseMenu::drawBoxBlack(buffer, yBase - 1, colList - 2, boxWidth, boxHeight);

        for (int i = 0; i < (int)options.size(); ++i) {
            if (i == selectionCurrent) {
                MenuRaycasterUtils::superimposeTextAbsolute(buffer, "\033[38;2;" + std::to_string(colors[i].r) + ";" + std::to_string(colors[i].g) + ";" + std::to_string(colors[i].b) + "m> " + options[i] + "\033[0m", yBase + i, colList);
            } else {
                MenuRaycasterUtils::superimposeTextAbsolute(buffer, "\033[38;2;120;120;120m  " + options[i] + "\033[0m", yBase + i, colList);
            }
        }

        auto& c = colors[selectionCurrent];
        std::string colorArt = "\033[38;2;" + std::to_string(c.r) + ";" + std::to_string(c.g) + ";" + std::to_string(c.b) + "m";
        for (size_t i = 0; i < artSkull.size(); ++i) {
            MenuRaycasterUtils::superimposeTextAbsolute(buffer, colorArt + artSkull[i] + "\033[0m", yBase + (int)i, colArt);
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
                DifficultyScreen::Result r;
                r.returned = true;
                return r;
            }
        }

        if (key == 'w' || key == 'W') {
            selectionCurrent = (selectionCurrent - 1 + (int)options.size()) % (int)options.size();
        } else if (key == 's' || key == 'S') {
            selectionCurrent = (selectionCurrent + 1) % (int)options.size();
        } else if (key == '\r' || key == '\n') {
            if (selectionCurrent == 3) {
                DifficultyScreen::Result r;
                r.returned = true;
                return r;
            }
            DifficultyScreen::Result r;
            r.index = selectionCurrent;
            return r;
        }
    }
}
