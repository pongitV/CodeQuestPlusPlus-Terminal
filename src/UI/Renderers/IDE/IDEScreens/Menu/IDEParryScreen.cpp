#include "UI/Renderers/IDE/IDEScreens/Menu/IDEParryScreen.h"
#include "UI/Renderers/IDE/IDEScreens/Menu/IDEMenuScreen.h"
#include <iostream>
#include <vector>
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Systems/Combat/Parry.h"
#include <chrono>
#include <thread>

namespace {
    void rotateTutorialMovementIDE() {
        struct Level { std::string name; int difficulty; };
        std::vector<Level> levels = {
            {"NIVEL 1 - Facil", 3},
            {"NIVEL 2 - Medio", 6},
            {"NIVEL 3 - Dificil", 10},
            {"NIVEL EXTRA - Desafio", 14}
        };

        for (size_t i = 0; i < levels.size(); ++i) {
            Appearance::clearScreen();
            std::vector<std::string> intro = {
                "\033[38;2;255;215;0m" + levels[i].name + "\033[0m",
                "",
                "Pressione ESPACO quando o marcador estiver na zona verde!",
                "",
                "Pressione ENTER para comecar..."
            };
            int spaces = IDEMenuScreen::calculateSpaceY(intro.size());
            for (int e = 0; e < spaces; e++) std::cout << "\n";
            IDEMenuScreen::printCentralizedBlockIDE(intro);
            
            while(true) {
                unsigned char key = InputControl::readKey();
                if (key == '\r' || key == '\n') break;
            }

            int hits = 0;
            for (int test = 1; test <= 5; ++test) {
                std::string msgResult = "";
                std::string colorResult = "";

                Parry::onUpdateScreen = [&]() {
                    Appearance::clearScreen();
                    std::vector<std::string> screen = {
                        "\033[38;2;255;215;0mPARRY DE MOVIMENTO - DEMO\033[0m",
                        "\033[38;2;180;180;255mTeste " + std::to_string(test) + "/5 - " + levels[i].name + "\033[0m",
                        "",
                        ""
                    };

                    if (!Parry::minigameMessage.empty()) {
                        screen.push_back(Parry::minigameMessage);
                    }
                    if (!Parry::minigameBar.empty()) {
                        screen.push_back(Parry::minigameBar);
                    }
                    screen.push_back("");
                    if (!msgResult.empty()) {
                        screen.push_back(colorResult + msgResult + "\033[0m");
                    }

                    int offsetY = IDEMenuScreen::calculateSpaceY(screen.size());
                    for (int e = 0; e < offsetY; e++) std::cout << "\n";
                    IDEMenuScreen::printCentralizedBlockIDE(screen);
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
        }
    }

    void rotateTutorialTypingIDE() {
        struct Level { std::string name; int digits; int timeMs; };
        std::vector<Level> levels = {
            {"NIVEL 1 - Facil", 3, 3000},
            {"NIVEL 2 - Medio", 4, 2500},
            {"NIVEL 3 - Dificil", 5, 2000},
            {"NIVEL EXTRA - Desafio", 6, 1500}
        };

        for (size_t i = 0; i < levels.size(); ++i) {
            Appearance::clearScreen();
            std::vector<std::string> intro = {
                "\033[38;2;255;215;0m" + levels[i].name + "\033[0m",
                "",
                "Digite a sequencia de numeros que aparecer!",
                std::to_string(levels[i].digits) + " digitos | " + std::to_string(levels[i].timeMs / 1000) + "s limite",
                "",
                "Pressione ENTER para comecar..."
            };
            int spaces = IDEMenuScreen::calculateSpaceY(intro.size());
            for (int e = 0; e < spaces; e++) std::cout << "\n";
            IDEMenuScreen::printCentralizedBlockIDE(intro);
            
            while(true) {
                unsigned char key = InputControl::readKey();
                if (key == '\r' || key == '\n') break;
            }

            int hits = 0;
            for (int test = 1; test <= 5; ++test) {
                std::string msgResult = "";
                std::string colorResult = "";

                Parry::onUpdateScreen = [&]() {
                    Appearance::clearScreen();
                    std::vector<std::string> screen = {
                        "\033[38;2;255;215;0mPARRY DE DIGITACAO - DEMO\033[0m",
                        "\033[38;2;180;180;255mTeste " + std::to_string(test) + "/5 - " + levels[i].name + "\033[0m",
                        "",
                        ""
                    };

                    if (!Parry::minigameMessage.empty()) {
                        screen.push_back(Parry::minigameMessage);
                    }
                    if (!Parry::minigameBar.empty()) {
                        screen.push_back(Parry::minigameBar);
                    }
                    screen.push_back("");
                    if (!msgResult.empty()) {
                        screen.push_back(colorResult + msgResult + "\033[0m");
                    }

                    int offsetY = IDEMenuScreen::calculateSpaceY(screen.size());
                    for (int e = 0; e < offsetY; e++) std::cout << "\n";
                    IDEMenuScreen::printCentralizedBlockIDE(screen);
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
        }
    }
}

ParryScreen::Result IDEParryScreen::display(const std::string& namePlayer, const std::string& raceName, const std::string& className) {
    (void)namePlayer;
    (void)raceName;
    (void)className;

    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorType = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string colorHighlight = "\033[48;2;38;79;120m\033[38;2;255;255;255m"; // Azul escuro bg
    std::string reset = "\033[0m";

    std::vector<std::string> options = {
        "PARRY DESLIGADO",
        "PARRY MOVIMENTO",
        "PARRY DIGITACAO",
        "VOLTAR"
    };
    
    int selectionCurrent = 0;
    InputControl::clearBuffer();

    while (true) {
        std::cout << "\033[?25l";
        Appearance::clearScreen();
        
        std::vector<std::string> blockCentral;
        blockCentral.push_back(colorComment + "// SISTEMA DE COMBATE" + reset);
        blockCentral.push_back(colorComment + "// O sistema de defesas depende do Parry, que eh um minigame de reacao." + reset);
        blockCentral.push_back(colorComment + "// Escolha o modo de Parry" + reset);
        blockCentral.push_back("");
        
        blockCentral.push_back(colorType + "void " + colorKeyword + "configurarParry" + colorPunct + "() {");
        
        for (int i = 0; i < (int)options.size(); ++i) {
            std::string line = "    ";
            std::string nameOption = options[i];
            
            if (i == selectionCurrent) {
                line += colorHighlight + nameOption + reset;
            } else {
                line += colorPunct + nameOption + reset;
            }
            
            blockCentral.push_back(line);
        }
        
        blockCentral.push_back(colorPunct + "}");

        int spacesY = IDEMenuScreen::calculateSpaceY(blockCentral.size());
        for (int i = 0; i < spacesY; ++i) std::cout << "\n";
        
        IDEMenuScreen::printCentralizedBlockIDE(blockCentral);

        unsigned char key = static_cast<unsigned char>(InputControl::readKey());
        if (key == 224 || key == 0 || key == '\033') {
            unsigned char nextKey = static_cast<unsigned char>(InputControl::readKey());
            if (nextKey == '[') nextKey = static_cast<unsigned char>(InputControl::readKey());
            if (nextKey == 72 || nextKey == 'A') key = 'w';
            else if (nextKey == 80 || nextKey == 'B') key = 's';
        }

        if (key == 'w' || key == 'W') {
            selectionCurrent = (selectionCurrent - 1 + (int)options.size()) % (int)options.size();
        } else if (key == 's' || key == 'S') {
            selectionCurrent = (selectionCurrent + 1) % (int)options.size();
        } else if (key == '\r' || key == '\n') {
            break;
        }
    }

    if (selectionCurrent == 3) {
        ParryScreen::Result r;
        r.returned = true;
        return r;
    }

    ParryScreen::Result r;
    if (selectionCurrent == 1) {
        r.mode = ParryScreen::Result::Mode::Movement;
        rotateTutorialMovementIDE();
    } else if (selectionCurrent == 2) {
        r.mode = ParryScreen::Result::Mode::Typing;
        rotateTutorialTypingIDE();
    } else {
        r.mode = ParryScreen::Result::Mode::Off;
    }
    
    return r;
}