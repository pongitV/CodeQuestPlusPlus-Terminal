#include "UI/Renderers/3D/RaycasterScreens/Menu/RaycasterNameScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Utils/MenuRaycasterLayout.h"
#include "UI/Renderers/3D/RaycasterScreens/Utils/MenuRaycasterUtils.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

#include "UI/Screens/Menu/BaseMenuScreen.h"
#include "Core/Utils/DialogFunctions.h"
#include "Core/Utils/InputControl.h"

NameScreen::Result ScreenNameRaycaster::display() {
    std::cout << "\033[?25l";
    MenuRaycasterUtils::curlBackground3D("Vila", nullptr);

    int width = Appearance::getTerminalWidth();
    std::string colorTitle = "\033[38;2;255;215;0m";
    std::string colorNarrative = "\033[38;2;180;180;255m";
    std::string name;
    std::string messageError = "";

    while (true) {
        MenuRaycasterUtils::increaseCycleDay();
        std::ostringstream buffer;
        MenuRaycasterUtils::displayBackground3D(buffer);

        int y = 3;
        int espTitle = std::max(0, (width - 17) / 2);
        int espNarrative = std::max(0, (width - 50) / 2);
        
        std::string promptStr = "> Digite o nome do seu personagem [0 para voltar]: ";
        int boxW = std::max(54, (int)promptStr.length() + (int)name.length() + 5);
        int boxX = ScreenBaseMenu::calculateOffsetCentral(boxW, width);
        ScreenBaseMenu::drawBoxBlack(buffer, 2, boxX, boxW, 8);

        MenuRaycasterUtils::superimposeTextAbsolute(buffer, colorTitle + "O NOME DO DESTINO\033[0m", y++, espTitle); y++;
        MenuRaycasterUtils::superimposeTextAbsolute(buffer, colorNarrative + "O mundo clama por um novo destino...\033[0m", y++, espNarrative);
        MenuRaycasterUtils::superimposeTextAbsolute(buffer, colorNarrative + "E todas as lendas possuem um nome.\033[0m", y++, espNarrative); y++;

        int yPrompt = y;
        int espPrompt = ScreenBaseMenu::calculateOffsetCentral(promptStr + name + "_", width);
        MenuRaycasterUtils::superimposeTextAbsolute(buffer, promptStr + "\033[38;2;100;255;100m" + name + "\033[38;2;150;255;150m_\033[0m", yPrompt, espPrompt);
        
        if (!messageError.empty()) {
            int espErr = ScreenBaseMenu::calculateOffsetCentral(messageError, width);
            MenuRaycasterUtils::superimposeTextAbsolute(buffer, "\033[38;2;255;100;100m" + messageError + "\033[0m", yPrompt + 2, espErr);
        }

        MenuRaycasterUtils::flushFrameForConsole(buffer.str());

        if (!InputControl::pressedKey()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            continue;
        }

        unsigned char key = static_cast<unsigned char>(InputControl::readKey());
        
        if (key != 0) {
            messageError = ""; 
        }

        if (key == '\r' || key == '\n') {
            if (name == "0") {
                NameScreen::Result r;
                r.returned = true;
                return r;
            }
            if (name.empty() || name.length() > 20) {
                messageError = name.empty() ? "Nome invalido! Nao pode ser vazio." : "Nome muito longo! Maximo 20 caracteres.";
                continue;
            }
            NameScreen::Result r;
            r.name = name;
            return r;
        }
        if ((key == 8 || key == 127) && !name.empty()) name.pop_back();
        if (key >= 32 && key <= 126 && name.length() < 20) name += static_cast<char>(key);
    }
}
