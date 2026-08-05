#include "UI/Renderers/3D/RaycasterScreens/Menu/RaycasterParryScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Utils/MenuRaycasterLayout.h"
#include "UI/Renderers/3D/RaycasterScreens/Utils/MenuRaycasterUtils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>

#include "UI/Screens/Menu/BaseMenuScreen.h"
#include "Core/Utils/DialogFunctions.h"
#include "Core/Utils/InputControl.h"
#include "Core/Utils/RandomGenerator.h"
#include "Systems/Combat/Parry.h"

namespace {

    int displayScreenWithText(int widthConsole, const std::string& title, const std::vector<std::string>& lines) {
        while (!InputControl::pressedKey()) {
            MenuRaycasterUtils::increaseCycleDay();
            std::ostringstream buff;
            MenuRaycasterUtils::displayBackground3D(buff);

            int visTit = Appearance::getVisualLength(title);
            int espTit = std::max(0, (widthConsole - visTit) / 2);

            int maxW = visTit;
            for (size_t i = 0; i < lines.size(); ++i) {
                maxW = std::max(maxW, Appearance::getVisualLength(lines[i]));
            }
            int boxW = maxW + 10;
            if (boxW > widthConsole - 4) boxW = widthConsole - 4;
            int boxX = ScreenBaseMenu::calculateOffsetCentral(boxW, widthConsole);
            int inaLines = (int)lines.size() + 4;
            ScreenBaseMenu::drawBoxBlack(buff, 2, boxX, boxW, inaLines);

            MenuRaycasterUtils::superimposeTextAbsolute(buff, "\033[38;2;255;215;0m" + title + "\033[0m", 3, espTit);

            for (size_t i = 0; i < lines.size(); ++i) {
                int visLen = Appearance::getVisualLength(lines[i]);
                int esp = ScreenBaseMenu::calculateOffsetCentral(visLen, widthConsole);
                MenuRaycasterUtils::superimposeTextAbsolute(buff, lines[i], 5 + (int)i, esp);
            }

            MenuRaycasterUtils::flushFrameForConsole(buff.str());
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
        int key = InputControl::readKey();
        InputControl::clearBuffer();
        return key;
    }

    void displayCountRegressive(int widthConsole, int home) {
        for (int i = home; i > 0; --i) {
            for (int f = 0; f < 15; ++f) {
                MenuRaycasterUtils::increaseCycleDay();
                std::ostringstream buff;
                MenuRaycasterUtils::displayBackground3D(buff);

                std::string ina = std::to_string(i);
                int esp = ScreenBaseMenu::calculateOffsetCentral(ina, widthConsole);
                int boxX = ScreenBaseMenu::calculateOffsetCentral(6, widthConsole);
                ScreenBaseMenu::drawBoxBlack(buff, 9, boxX, 6, 3);
                MenuRaycasterUtils::superimposeTextAbsolute(buff, "\033[38;2;255;255;100m" + ina + "\033[0m", 10, esp);

                MenuRaycasterUtils::flushFrameForConsole(buff.str());
                std::this_thread::sleep_for(std::chrono::milliseconds(65));
            }
        }
        for (int f = 0; f < 10; ++f) {
            MenuRaycasterUtils::increaseCycleDay();
            std::ostringstream buff;
            MenuRaycasterUtils::displayBackground3D(buff);

            std::string go = "VAI!";
            int esp = ScreenBaseMenu::calculateOffsetCentral(go, widthConsole);
            int boxX = ScreenBaseMenu::calculateOffsetCentral(8, widthConsole);
            ScreenBaseMenu::drawBoxBlack(buff, 9, boxX, 8, 3);
            MenuRaycasterUtils::superimposeTextAbsolute(buff, "\033[38;2;100;255;100m" + go + "\033[0m", 10, esp);

            MenuRaycasterUtils::flushFrameForConsole(buff.str());
            std::this_thread::sleep_for(std::chrono::milliseconds(80));
        }
    }

    void rotateTutorialMovement(int widthConsole) {
        struct Level { std::string name; int difficulty; };
        std::vector<Level> levels = {
            {"NIVEL 1 - Facil", 3},
            {"NIVEL 2 - Medio", 6},
            {"NIVEL 3 - Dificil", 10},
            {"NIVEL EXTRA - Desafio", 14}
        };

        for (size_t i = 0; i < levels.size(); ++i) {
            std::vector<std::string> intro = {
                "\033[38;2;255;215;0m" + levels[i].name + "\033[0m",
                "",
                "\033[38;2;180;180;255mPressione ESPACO quando o marcador estiver na zona verde!\033[0m",
                "",
                "\033[38;2;120;120;120mPressione ENTER para comecar...\033[0m"
            };
            displayScreenWithText(widthConsole, "TUTORIAL DE PARRY MOVIMENTO", intro);
            displayCountRegressive(widthConsole, 3);

            int hits = 0;
            for (int test = 1; test <= 5; ++test) {
                std::string msgResult = "";
                std::string colorResult = "";

                Parry::onUpdateScreen = [&]() {
                    MenuRaycasterUtils::increaseCycleDay();
                    std::ostringstream buff;
                    MenuRaycasterUtils::displayBackground3D(buff);

                    std::string title = "PARRY DE MOVIMENTO - DEMO";
                    std::string subtitle = "Teste " + std::to_string(test) + "/5 - " + levels[i].name;

                    int espTit = ScreenBaseMenu::calculateOffsetCentral(title, widthConsole);
                    int espSub = ScreenBaseMenu::calculateOffsetCentral(subtitle, widthConsole);

                    int boxW = std::max((int)title.length(), (int)subtitle.length()) + 50;
                    if (boxW > widthConsole - 4) boxW = widthConsole - 4;
                    int boxX = ScreenBaseMenu::calculateOffsetCentral(boxW, widthConsole);
                    ScreenBaseMenu::drawBoxBlack(buff, 1, boxX, boxW, 21);

                    MenuRaycasterUtils::superimposeTextAbsolute(buff, "\033[38;2;255;215;0m" + title + "\033[0m", 2, espTit);
                    MenuRaycasterUtils::superimposeTextAbsolute(buff, "\033[38;2;180;180;255m" + subtitle + "\033[0m", 3, espSub);

                    if (!Parry::minigameMessage.empty()) {
                        int compMsg = Appearance::getVisualLength(Parry::minigameMessage);
                        int espMsg = ScreenBaseMenu::calculateOffsetCentral(compMsg, widthConsole);
                        MenuRaycasterUtils::superimposeTextAbsolute(buff, Parry::minigameMessage, 15, espMsg);
                    }
                    if (!Parry::minigameBar.empty()) {
                        int compBar = Appearance::getVisualLength(Parry::minigameBar);
                        int espBar = ScreenBaseMenu::calculateOffsetCentral(compBar, widthConsole);
                        MenuRaycasterUtils::superimposeTextAbsolute(buff, Parry::minigameBar, 17, espBar);
                    }

                    if (!msgResult.empty()) {
                        int espRes = ScreenBaseMenu::calculateOffsetCentral(msgResult, widthConsole);
                        MenuRaycasterUtils::superimposeTextAbsolute(buff, colorResult + msgResult + "\033[0m", 20, espRes);
                    }

                    MenuRaycasterUtils::flushFrameForConsole(buff.str());
                };

                int dmgRed = 0;
                bool success = Parry::executeMinigameMovement(levels[i].difficulty, 100, dmgRed);

                if (success) {
                    if (dmgRed == 100) {
                        msgResult = "Parry Perfeito! (dano anulado)";
                        colorResult = "\033[38;2;100;255;100m";
                    } else {
                        msgResult = "Parry Efetivo! (dano reduzido)";
                        colorResult = "\033[38;2;255;255;100m";
                    }
                    hits++;
                } else {
                    msgResult = "Parry Falhou!";
                    colorResult = "\033[38;2;255;100;100m";
                }
                
                Parry::minigameMessage = "";
                Parry::minigameBar = "";
                
                auto endTest = std::chrono::steady_clock::now();
                while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - endTest).count() < 1200) {
                    Parry::onUpdateScreen();
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            }
            Parry::onUpdateScreen = nullptr;

            std::vector<std::string> result = {
                "\033[38;2;255;255;255mResultado: " + std::to_string(hits) + "/5 acertos.\033[0m",
                "",
                "\033[38;2;120;120;120mPressione ENTER para continuar...\033[0m"
            };
            displayScreenWithText(widthConsole, "TUTORIAL DE PARRY MOVIMENTO", result);
        }

        std::vector<std::string> completed = {
            "\033[38;2;100;255;100mTutorial de Parry Movimento concluido!\033[0m",
            "",
            "\033[38;2;120;120;120mPressione ENTER para continuar...\033[0m"
        };
        displayScreenWithText(widthConsole, "PARABENS!", completed);
    }

    void rotateTutorialTyping(int widthConsole) {
        struct Level { std::string name; int digits; int timeMs; };
        std::vector<Level> levels = {
            {"NIVEL 1 - Facil", 3, 3000},
            {"NIVEL 2 - Medio", 4, 2500},
            {"NIVEL 3 - Dificil", 5, 2000},
            {"NIVEL EXTRA - Desafio", 6, 1500}
        };

        for (size_t i = 0; i < levels.size(); ++i) {
            std::vector<std::string> intro = {
                "\033[38;2;255;215;0m" + levels[i].name + "\033[0m",
                "",
                "\033[38;2;180;180;255mDigite a sequencia de numeros que aparecer!\033[0m",
                "\033[38;2;180;180;255m" + std::to_string(levels[i].digits) + " digitos | " + std::to_string(levels[i].timeMs / 1000) + "s limite\033[0m",
                "",
                "\033[38;2;120;120;120mPressione ENTER para comecar...\033[0m"
            };
            displayScreenWithText(widthConsole, "TUTORIAL DE PARRY DIGITACAO", intro);
            displayCountRegressive(widthConsole, 3);

            int hits = 0;
            for (int test = 1; test <= 5; ++test) {
                std::string msgResult = "";
                std::string colorResult = "";

                Parry::onUpdateScreen = [&]() {
                    MenuRaycasterUtils::increaseCycleDay();
                    std::ostringstream buff;
                    MenuRaycasterUtils::displayBackground3D(buff);

                    std::string title = "PARRY DE DIGITACAO - DEMO";
                    std::string subtitle = "Teste " + std::to_string(test) + "/5 - " + levels[i].name;

                    int espTit = ScreenBaseMenu::calculateOffsetCentral(title, widthConsole);
                    int espSub = ScreenBaseMenu::calculateOffsetCentral(subtitle, widthConsole);

                    int boxW = std::max((int)title.length(), (int)subtitle.length()) + 50;
                    if (boxW > widthConsole - 4) boxW = widthConsole - 4;
                    int boxX = ScreenBaseMenu::calculateOffsetCentral(boxW, widthConsole);
                    ScreenBaseMenu::drawBoxBlack(buff, 1, boxX, boxW, 21);

                    MenuRaycasterUtils::superimposeTextAbsolute(buff, "\033[38;2;255;215;0m" + title + "\033[0m", 2, espTit);
                    MenuRaycasterUtils::superimposeTextAbsolute(buff, "\033[38;2;180;180;255m" + subtitle + "\033[0m", 3, espSub);

                    if (!Parry::minigameMessage.empty()) {
                        int compMsg = Appearance::getVisualLength(Parry::minigameMessage);
                        int espMsg = ScreenBaseMenu::calculateOffsetCentral(compMsg, widthConsole);
                        MenuRaycasterUtils::superimposeTextAbsolute(buff, Parry::minigameMessage, 15, espMsg);
                    }
                    if (!Parry::minigameBar.empty()) {
                        int compBar = Appearance::getVisualLength(Parry::minigameBar);
                        int espBar = ScreenBaseMenu::calculateOffsetCentral(compBar, widthConsole);
                        MenuRaycasterUtils::superimposeTextAbsolute(buff, Parry::minigameBar, 17, espBar);
                    }

                    if (!msgResult.empty()) {
                        int espRes = ScreenBaseMenu::calculateOffsetCentral(msgResult, widthConsole);
                        MenuRaycasterUtils::superimposeTextAbsolute(buff, colorResult + msgResult + "\033[0m", 20, espRes);
                    }

                    MenuRaycasterUtils::flushFrameForConsole(buff.str());
                };

                int dmgRed = 0;
                bool success = Parry::executeMinigameTyping(levels[i].digits * 2, 100, dmgRed);

                if (success) {
                    if (dmgRed == 100) {
                        msgResult = "Parry Perfeito! (dano anulado)";
                        colorResult = "\033[38;2;100;255;100m";
                    } else {
                        msgResult = "Parry Efetivo! (dano reduzido)";
                        colorResult = "\033[38;2;255;255;100m";
                    }
                    hits++;
                } else {
                    msgResult = "Parry Falhou!";
                    colorResult = "\033[38;2;255;100;100m";
                }

                Parry::minigameMessage = "";
                Parry::minigameBar = "";
                
                auto endTest = std::chrono::steady_clock::now();
                while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - endTest).count() < 1200) {
                    Parry::onUpdateScreen();
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            }
            Parry::onUpdateScreen = nullptr;

            std::vector<std::string> result = {
                "\033[38;2;255;255;255mResultado: " + std::to_string(hits) + "/5 acertos.\033[0m",
                "",
                "\033[38;2;120;120;120mPressione ENTER para continuar...\033[0m"
            };
            displayScreenWithText(widthConsole, "TUTORIAL DE PARRY DIGITACAO", result);
        }

        std::vector<std::string> completed = {
            "\033[38;2;100;255;100mTutorial de Parry Digitacao concluido!\033[0m",
            "",
            "\033[38;2;120;120;120mPressione ENTER para continuar...\033[0m"
        };
        displayScreenWithText(widthConsole, "PARABENS!", completed);
        InputControl::readKey();
        InputControl::clearBuffer();
    }

}

ParryScreen::Result ScreenParryRaycaster::display(const std::string& namePlayer, const std::string& raceName, const std::string& className) {
    int widthConsole = Appearance::getTerminalWidth();
    std::vector<std::string> options = {
        "PARRY DESLIGADO",
        "PARRY MOVIMENTO (Barra deslizante)",
        "PARRY DIGITACAO (Digitar por tempo)",
        "VOLTAR"
    };

    int selectionCurrent = 0;

    std::cout << "\033[?25l";
    InputControl::clearBuffer();
    MenuRaycasterUtils::curlBackground3D("Vila", nullptr);

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
        int espOption = ScreenBaseMenu::calculateOffsetCentral(50, widthConsole);
        
        ScreenBaseMenu::drawBoxBlack(buffer, yBase - 1, espOption - 2, 54, (int)options.size() + 2);

        for (int i = 0; i < (int)options.size(); ++i) {
            std::string icon = (i == selectionCurrent) ? "> " : "  ";
            std::string color = (i == selectionCurrent) ? "\033[38;2;0;255;0m" : "\033[38;2;120;120;120m";
            MenuRaycasterUtils::superimposeTextAbsolute(buffer, color + icon + options[i] + "\033[0m", yBase + i, espOption);
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
                ParryScreen::Result r;
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
                ParryScreen::Result r;
                r.returned = true;
                return r;
            }

            ParryScreen::Result r;
            if (selectionCurrent == 1) {
                r.mode = ParryScreen::Result::Mode::Movement;
                std::vector<std::string> explanation = {
                    "\033[38;2;180;180;255mUma barra horizontal com uma zona verde central surgira na tela.\033[0m",
                    "\033[38;2;180;180;255mUm cursor percorrera a barra da esquerda para a direita.\033[0m",
                    "\033[38;2;180;180;255mPressione ESPACO no momento exato em que o cursor estiver na zona verde!\033[0m",
                    "",
                    "\033[38;2;120;120;120mPressione ENTER para iniciar o tutorial...\033[0m",
                    "\033[38;2;120;120;120mPressione ESC para pular o tutorial...\033[0m"
                };
                int key = displayScreenWithText(widthConsole, "PARRY MOVIMENTO - TUTORIAL", explanation);
                if (key != 27) {
                    rotateTutorialMovement(widthConsole);
                }
            } else if (selectionCurrent == 2) {
                r.mode = ParryScreen::Result::Mode::Typing;
                std::vector<std::string> explanation = {
                    "\033[38;2;180;180;255mUma sequencia de numeros aparecera na tela com um limite de tempo.\033[0m",
                    "\033[38;2;180;180;255mDigite os numeros rapidamente na sequencia correta e pressione ENTER.\033[0m",
                    "\033[38;2;180;180;255mSe for rapido o suficiente, o dano sera reduzido ou anulado!\033[0m",
                    "",
                    "\033[38;2;120;120;120mPressione ENTER para iniciar o tutorial...\033[0m",
                    "\033[38;2;120;120;120mPressione ESC para pular o tutorial...\033[0m"
                };
                int key = displayScreenWithText(widthConsole, "PARRY DIGITACAO - TUTORIAL", explanation);
                if (key != 27) {
                    rotateTutorialTyping(widthConsole);
                }
            } else {
                r.mode = ParryScreen::Result::Mode::Off;
            }
            return r;
        }
    }
}
