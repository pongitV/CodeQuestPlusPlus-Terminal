#include "UI/Renderers/3D/RaycasterScreens/Pause/RaycasterPauseScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Utils/MenuRaycasterLayout.h"
#include "UI/Renderers/3D/RaycasterScreens/Utils/MenuRaycasterUtils.h"
#include "UI/Screens/Menu/BaseMenuScreen.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Domain/Characters/Character.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <string>
#include <algorithm>

static int renderMenuSimple(const std::string& title, const std::vector<std::string>& options) {
    int widthConsole = Appearance::getTerminalWidth();
    int selectionCurrent = 0;
    InputControl::clearBuffer();
    
    while (true) {
        MenuRaycasterUtils::increaseCycleDay();
        std::ostringstream buffer;
        MenuRaycasterUtils::displayBackground3D(buffer);

        int espTit = ScreenBaseMenu::calculateOffsetCentral(title, widthConsole);
        
        int boxW = Appearance::getVisualLength(title) + 10;
        for (const auto& op : options) {
            boxW = std::max(boxW, Appearance::getVisualLength(op) + 10);
        }
        if (boxW > widthConsole - 4) boxW = widthConsole - 4;
        
        int boxX = ScreenBaseMenu::calculateOffsetCentral(boxW, widthConsole);
        int yBase = 8;
        
        ScreenBaseMenu::drawBoxBlack(buffer, yBase - 2, boxX, boxW, (int)options.size() + 4);
        MenuRaycasterUtils::superimposeTextAbsolute(buffer, "\033[38;2;255;215;0m" + title + "\033[0m", yBase - 1, espTit);

        for (int i = 0; i < (int)options.size(); ++i) {
            std::string icon = (i == selectionCurrent) ? "> " : "  ";
            std::string color = (i == selectionCurrent) ? "\033[38;2;0;255;0m" : "\033[38;2;120;120;120m";
            int espOption = ScreenBaseMenu::calculateOffsetCentral(options[i] + "  ", widthConsole);
            MenuRaycasterUtils::superimposeTextAbsolute(buffer, color + icon + options[i] + "\033[0m", yBase + 1 + i, espOption - 2);
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

        if (key == 'w' || key == 'W') {
            selectionCurrent = (selectionCurrent - 1 + (int)options.size()) % (int)options.size();
        } else if (key == 's' || key == 'S') {
            selectionCurrent = (selectionCurrent + 1) % (int)options.size();
        } else if (key == '\r' || key == '\n') {
            return selectionCurrent;
        }
    }
}

int ScreenPauseRaycaster::renderMenuPause() {
    std::vector<std::string> options = {
        "VOLTAR AO JOGO",
        "CONFIGURACOES",
        "SAIR DO JOGO"
    };
    int res = renderMenuSimple("[ PAUSE ]", options);
    if (res == -1) return 0; 
    return res;
}

int ScreenPauseRaycaster::renderMenuConfiguracoes(Character* player) {
    std::string difStr;
    switch (player->getDifficulty()) {
        case GameDifficulty::Easy: difStr = "\033[38;2;100;255;100mFACIL\033[0m"; break;
        case GameDifficulty::Normal: difStr = "\033[38;2;255;255;100mNORMAL\033[0m"; break;
        case GameDifficulty::Difficult: difStr = "\033[38;2;255;100;100mDIFICIL\033[0m"; break;
    }

    std::string parryState = "\033[38;2;255;100;100mDESLIGADO\033[0m"; // Vermelho
    if (player->getParryActivated()) {
        if (player->getParryModern()) {
            parryState = "\033[38;2;100;255;100mMOVIMENTO\033[0m"; // Verde
        } else {
            parryState = "\033[38;2;100;100;255mDIGITACAO\033[0m"; // Azul
        }
    }
    std::vector<std::string> options = {
        "ALTERAR DIFICULDADE: " + difStr,
        "ALTERNAR PARRY: " + parryState,
        "APARENCIA / SKIN",
        "FUNDO DO TERMINAL",
        "SENSIBILIDADE DA CAMERA",
        "VOLTAR"
    };
    int res = renderMenuSimple("[ CONFIGURACOES ]", options);
    
    if (res == -1) return 6;
    if (res == 0) return 0;
    if (res == 1) {
        if (!player->getParryActivated()) {
            // DESLIGADO -> MOVIMENTO
            player->setModernParry(true);
            return 1; // TelaPause toggles Ativado to true
        } else if (player->getParryModern()) {
            // MOVIMENTO -> DIGITACAO
            return 2; // TelaPause toggles Moderno to false
        } else {
            // DIGITACAO -> DESLIGADO
            return 1; // TelaPause toggles Ativado to false
        }
    }
    if (res == 2) return 3;
    if (res == 3) return 4;
    if (res == 4) return 5;
    if (res == 5) return 6;
    
    return 6;
}

int ScreenPauseRaycaster::renderMenuAppearance(Character*) { return 2; }
int ScreenPauseRaycaster::renderMenuBackground(int) { return 6; }
int ScreenPauseRaycaster::renderMenuSensitivity(int percX, int percY) {
    std::vector<std::string> options = {
        "EIXO X (Yaw): " + std::to_string(percX) + "%",
        "EIXO Y (Pitch): " + std::to_string(percY) + "%",
        "VOLTAR"
    };
    int res = renderMenuSimple("[ SENSIBILIDADE ]", options);
    if (res == -1) return 2;
    return res;
}
