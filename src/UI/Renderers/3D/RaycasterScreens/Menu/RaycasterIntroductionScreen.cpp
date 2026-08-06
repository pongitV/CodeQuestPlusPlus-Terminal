#include "UI/Renderers/3D/RaycasterScreens/Menu/RaycasterIntroductionScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Utils/MenuRaycasterLayout.h"
#include "UI/Renderers/3D/RaycasterScreens/Utils/MenuRaycasterUtils.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

#include "UI/Screens/Menu/BaseMenuScreen.h"
#include "Core/Utils/DialogFunctions.h"
#include "Core/Utils/InputControl.h"

void ScreenIntroductionRaycaster::display() {
    int widthConsole = Appearance::getTerminalWidth();

    std::cout << "\033[?25l";
    InputControl::clearBuffer();
    MenuRaycasterUtils::curlBackground3D("Vila", nullptr);

    std::string title = "Bem-vindo, Jovem Aventureiro!";
    std::string subtitle = "A Jornada comeca agora...";
    const auto& scene = ArtsRaycaster::sceneIntroduction;

    std::vector<std::string> messages = {
        "Voce desperta nos arredores de um lugar desconhecido...",
        "Na sua vista, uma pequena vila sendo atacada por monstros.",
        "Empunhando seu equipamento, voce sente que seu destino o aguarda.",
        "Um novo capitulo se inicia agora.",
        "",
        "Pressione Enter para iniciar sua jornada."
    };

    int widthTotal = 0;
    for (size_t i = 0; i < scene.size(); ++i) {
        widthTotal = std::max(widthTotal, Appearance::getVisualLength(scene[i]));
    }
    int marginSoon = ScreenBaseMenu::calculateOffsetCentral(widthTotal, widthConsole);
    int yTitle = 3;
    int charactersRevealed = 0;

    while (!InputControl::pressedKey()) {
        MenuRaycasterUtils::increaseCycleDay();

        std::ostringstream aprBuff;
        MenuRaycasterUtils::displayBackground3D(aprBuff);

        for (size_t i = 0; i < scene.size(); ++i) {
            MenuRaycasterUtils::superimposeText3D(aprBuff, "\033[38;2;255;255;255m" + scene[i] + "\033[0m", yTitle + (int)i, marginSoon, widthConsole);
        }

        int charCount = 0;
        int yMsg = yTitle + (int)scene.size() + 2;

        int maxMsgLen = 0;
        for (const auto& msg : messages) maxMsgLen = std::max(maxMsgLen, (int)msg.length());
        int boxW = maxMsgLen + 6;
        int boxX = ScreenBaseMenu::calculateOffsetCentral(boxW, widthConsole);
        ScreenBaseMenu::drawBoxBlack(aprBuff, yMsg - 1, boxX, boxW, (int)messages.size() + 2);

        for (const auto& msg : messages) {
            if (msg.empty()) {
                yMsg++;
                charCount += 5;
                continue;
            }
            if (charCount >= charactersRevealed) break;
            std::string textActive = "";
            for (char c : msg) {
                if (charCount < charactersRevealed) {
                    textActive += c;
                    charCount++;
                } else {
                    break;
                }
            }
            int esp = ScreenBaseMenu::calculateOffsetCentral(msg, widthConsole);
            MenuRaycasterUtils::superimposeTextAbsolute(aprBuff, "\033[38;2;200;200;200m" + textActive + "\033[0m", yMsg, esp);
            yMsg++;
            if (textActive == msg) {
                 charCount += 15;
            }
        }
        charactersRevealed += 3; // Tipo 3x mais rapido

        MenuRaycasterUtils::flushFrameForConsole(aprBuff.str());
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    
    InputControl::clearBuffer();
    std::cout << "\033[?25h";
}
