#include "UI/Renderers/3D/RaycasterScreens/Menu/RaycasterOpeningScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Utils/MenuRaycasterLayout.h"
#include "UI/Renderers/3D/RaycasterScreens/Utils/MenuRaycasterUtils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <fstream>
#include <thread>
#include <chrono>

#include "UI/Screens/Menu/BaseMenuScreen.h"
#include "Core/Utils/InputControl.h"


void ScreenOpeningRaycaster::display() {
    std::cout << "\033[?25l";
    MenuRaycasterUtils::curlBackground3D("Vila", nullptr);

    int widthConsole = Appearance::getTerminalWidth();
    int heightConsole = Appearance::getTerminalHeight();

    int widthSoonMax = 0;
    int widthTotal = 0;
    for (size_t i = 0; i < ArtsRaycaster::soonText.size(); ++i) {
        int wSoon = Appearance::getVisualLength(ArtsRaycaster::soonText[i]);
        int wPlus = (i < ArtsRaycaster::soonPlus.size())
            ? Appearance::getVisualLength(ArtsRaycaster::soonPlus[i]) : 0;
        widthSoonMax = std::max(widthSoonMax, wSoon);
        widthTotal    = std::max(widthTotal, wSoon + wPlus);
    }

    int marginSoon = ScreenBaseMenu::calculateOffsetCentral(widthTotal, widthConsole);
    int ySoon = 3;
    int yPrompt = std::max(ySoon + (int)ArtsRaycaster::soonText.size() + 2, heightConsole - 3);

    std::string version = "Versao 0.1";
    int colVersion = std::max(1, widthConsole - (int)version.length() - 1);



    InputControl::clearBuffer();
    while (!InputControl::pressedKey()) {
        MenuRaycasterUtils::increaseCycleDay();
        MenuRaycasterUtils::applyCycleDayNight(MenuRaycasterUtils::s_background3DMenu);
        MenuRaycasterUtils::drawCastle();
        MenuRaycasterUtils::drawSceneBattle();

        for (size_t i = 1; i < ArtsRaycaster::soonText.size(); ++i) {
            int row = ySoon + (int)i - 1;
            int plusIdx = (int)i - 1;
            MenuRaycasterUtils::superimposeNoFrame(row, marginSoon, ArtsRaycaster::soonText[i], 255, 255, 255);
            if (plusIdx < (int)ArtsRaycaster::soonPlus.size()) {
                MenuRaycasterUtils::superimposeNoFrame(row, marginSoon + widthSoonMax, ArtsRaycaster::soonPlus[plusIdx], 255, 165, 0);
            }
        }

        std::string prompt = "[PRESSIONE ENTER PARA CONTINUAR]";
        int espPrompt = ScreenBaseMenu::calculateOffsetCentral(static_cast<int>(prompt.size()), widthConsole);
        MenuRaycasterUtils::superimposeNoFrame(yPrompt, espPrompt, prompt, 255, 255, 255);

        MenuRaycasterUtils::superimposeNoFrame(heightConsole - 1, colVersion, version, 100, 100, 100);

        // Saída única via console API (sem flickering)
        std::ostringstream aprBuff;
        aprBuff << "\033[H";
        for (size_t y = 0; y < MenuRaycasterUtils::s_background3DMenu.size(); ++y) {
            aprBuff << MenuRaycasterUtils::s_background3DMenu[y];
            if (y < MenuRaycasterUtils::s_background3DMenu.size() - 1) aprBuff << "\n";
        }
        

        MenuRaycasterUtils::flushFrameForConsole(aprBuff.str());

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    InputControl::readKey();
    InputControl::clearBuffer();
}
