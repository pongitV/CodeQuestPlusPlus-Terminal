#include "UI/Screens/Menu/TutorialScreen.h"
#include "UI/Screens/Menu/MenuScreen.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>

#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Core/Utils/RandomGenerator.h"
#include "Systems/Combat/Parry.h"

namespace {
    void displayInfoBox(const std::string& infoBox) {
        if (!infoBox.empty()) {
            std::string cleanInfoBox = infoBox;
            size_t post = 0;
            while ((post = cleanInfoBox.find('|', post)) != std::string::npos) {
                cleanInfoBox.replace(post, 1, "\xe2\x95\x91");
                post += 3;
            }

            int infoLength = Appearance::getVisualLength(infoBox);
            std::string traces = "";
            for (int i = 0; i < infoLength - 2; ++i) traces += "\xe2\x95\x90";

            std::string margin = Appearance::spacesToCenter(infoLength);
            std::cout << margin << "\xe2\x95\x94" << traces << "\xe2\x95\x97\n";
            std::cout << margin << cleanInfoBox << "\n";
            std::cout << margin << "\xe2\x95\x9a" << traces << "\xe2\x95\x9d\n\n";
        }
    }

    void rotateTutorialTyping(const std::string& infoBox) {
        struct LevelTutorial {
            std::string nameEnemy;
            int digits;
            int timeLimitMs;
        };

        std::vector<LevelTutorial> levels = {
            {"(Nivel 1)", 3, 3000},
            {"(Nivel 2)", 4, 2500},
            {"(Nivel 3)", 5, 2000},
            {"(Nivel Extra)", 6, 1500}
        };

        Appearance::clearScreen();
        MenuScreen::displayGameLogoPanel("TUTORIAL DE PARRY DIGITACAO", false);
        if (infoBox.empty()) std::cout << "\n";
        else displayInfoBox(infoBox);

        std::vector<std::string> explanation = {
            "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90 COMO FUNCIONA O PARRY DIGITACAO \xe2\x95\x90\xe2\x95\x90\xe2\x95\x90",
            "Uma sequencia de numeros aparecera na tela com um limite de tempo.",
            "Digite os numeros rapidamente na sequencia correta e pressione ENTER.",
            "Se for rapido o suficiente e nao errar, o dano sera reduzido/anulado!"
        };

        Appearance::printBlockCentralized(explanation);
        std::cout << "\n";
        InputControl::waitForEnter();

        for (size_t i = 0; i < levels.size(); ++i) {
            Appearance::clearScreen();
            MenuScreen::displayGameLogoPanel("TUTORIAL DE PARRY DIGITACAO - " + levels[i].nameEnemy);

            if (infoBox.empty()) std::cout << "\n";
            else displayInfoBox(infoBox);

            Appearance::printCentralized("Inimigo: " + levels[i].nameEnemy);
            Appearance::printCentralized("Sequencia: " + std::to_string(levels[i].digits) + " digitos | Tempo limite: " + std::to_string(levels[i].timeLimitMs / 1000) + "s");
            InputControl::waitForEnter();

            int hits = 0;
            for (int test = 1; test <= 5; ++test) {
                std::cout << "\n";
                Appearance::printCentralized("[ Teste " + std::to_string(test) + "/5 ] Prepare-se...");
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                std::string sequence = "";
                for(int d = 0; d < levels[i].digits; d++){
                    sequence += std::to_string(RandomGenerator::getInteger(0, 9));
                }

                Appearance::printCentralized("O " + levels[i].nameEnemy + " ataca! DIGITE: " + Appearance::color(Color::CYAN) + sequence + Appearance::color(Color::RESET));

                auto home = std::chrono::steady_clock::now();
                std::string entry = InputControl::readEntryProtected("> ");
                auto end = std::chrono::steady_clock::now();

                int timeSpentMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(end - home).count());

                if (entry == sequence) {
                    if (timeSpentMs <= levels[i].timeLimitMs) {
                        Appearance::printCentralized(Appearance::color(Color::GREEN) + "Parry Perfeito! (" + std::to_string(timeSpentMs) + "ms)" + Appearance::color(Color::RESET));
                        hits++;
                    } else {
                        Appearance::printCentralized(Appearance::color(Color::YELLOW) + "Muito lento! (" + std::to_string(timeSpentMs) + "ms / " + std::to_string(levels[i].timeLimitMs) + "ms)" + Appearance::color(Color::RESET));
                    }
                } else {
                    Appearance::printCentralized(Appearance::color(Color::RED) + "Errou a sequencia! (Voce digitou: " + entry + ")" + Appearance::color(Color::RESET));
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
            }

            std::cout << "\n";
            Appearance::printCentralized("Resultado do nivel: " + std::to_string(hits) + "/5 acertos.");
            InputControl::waitForEnter();
        }
    }

    void rotateTutorialMovement(const std::string& infoBox) {
        struct LevelTutorialMovement {
            std::string nameEnemy;
            int difficulty;
        };

        std::vector<LevelTutorialMovement> levelsMove = {
            {"(Nivel 1)", 3},
            {"(Nivel 2)", 6},
            {"(Nivel 3)", 10},
            {"(Nivel Extra)", 14}
        };

        Appearance::clearScreen();
        MenuScreen::displayGameLogoPanel("TUTORIAL DE PARRY MOVIMENTO", false);
        if (infoBox.empty()) std::cout << "\n";
        else displayInfoBox(infoBox);

        std::vector<std::string> explanation = {
            "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90 COMO FUNCIONA O PARRY MOVIMENTO \xe2\x95\x90\xe2\x95\x90\xe2\x95\x90",
            "Uma barra horizontal com uma zona verde central surgira na tela.",
            "Um cursor '>' percorrera a barra da esquerda para a direita.",
            "Pressione [ESPACO] no momento exato em que o cursor estiver na zona verde!",
            "Se acertar bem no centro, sera um Parry Perfeito (anula o dano!)."
        };

        Appearance::printBlockCentralized(explanation);
        std::cout << "\n";
        InputControl::waitForEnter();

        for (size_t i = 0; i < levelsMove.size(); ++i) {
            Appearance::clearScreen();
            MenuScreen::displayGameLogoPanel("TUTORIAL DE PARRY MOVIMENTO - " + levelsMove[i].nameEnemy);

            if (infoBox.empty()) std::cout << "\n";
            else displayInfoBox(infoBox);

            Appearance::printCentralized("Inimigo: " + levelsMove[i].nameEnemy);
            Appearance::printCentralized("Pressione [ESPACO] no momento exato na area verde!");
            InputControl::waitForEnter();

            int hits = 0;
            for (int test = 1; test <= 5; ++test) {
                std::cout << "\n";
                Appearance::printCentralized("[ Teste " + std::to_string(test) + "/5 ] Prepare-se...");
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                int dmgRed = 0;
                bool success = Parry::executeMinigameMovement(levelsMove[i].difficulty, 100, dmgRed);

                std::cout << "\n";
                if (success) {
                    if (dmgRed == 100) {
                        Appearance::printCentralized(Appearance::color(Color::GREEN) + "Parry Perfeito!" + Appearance::color(Color::RESET));
                    } else {
                        Appearance::printCentralized(Appearance::color(Color::YELLOW) + "Parry Efetivo!" + Appearance::color(Color::RESET));
                    }
                    hits++;
                } else {
                    Appearance::printCentralized(Appearance::color(Color::RED) + "Parry Falhou!" + Appearance::color(Color::RESET));
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }

            std::cout << "\n";
            Appearance::printCentralized("Resultado do nivel: " + std::to_string(hits) + "/5 acertos.");
            InputControl::waitForEnter();
        }
    }
}

void TutorialScreen::displayParryTutorial(const std::string& infoBox) {
    std::cout << "\033[?25l";

    while (true) {
        Appearance::clearScreen();
        MenuScreen::displayGameLogoPanel("TUTORIAL DE PARRY", false);
        if (infoBox.empty()) std::cout << "\n";
        else displayInfoBox(infoBox);

        std::vector<std::string> explanation = {
            "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90 COMO FUNCIONA O PARRY \xe2\x95\x90\xe2\x95\x90\xe2\x95\x90",
            "Quando um inimigo atacar, voce deve estar pronto para reagir.",
            "Existem dois modos de Parry no jogo:",
            "  1. MOVIMENTO (Minigame de Barra de Tempo/Espaco)",
            "  2. DIGITACAO (Minigame de Sequencia Numerica)",
            "",
            "Escolha qual tutorial deseja realizar:"
        };

        Appearance::printBlockCentralized(explanation);
        std::cout << "\n";

        std::vector<std::string> optionsTutorial = {
            "TUTORIAL DE PARRY MOVIMENTO (Barra/Espaco)",
            "TUTORIAL DE PARRY DIGITACAO (Numeros/Enter)",
            "REALIZAR AMBOS OS TUTORIAIS",
            "PULAR TUTORIAL"
        };

        int choice = InputControl::readSelectionMenuWithArrows(optionsTutorial, true);

        if (choice == 0) {
            rotateTutorialMovement(infoBox);
            break;
        } else if (choice == 1) {
            rotateTutorialTyping(infoBox);
            break;
        } else if (choice == 2) {
            rotateTutorialMovement(infoBox);
            rotateTutorialTyping(infoBox);
            break;
        } else {
            return;
        }
    }

    std::cout << "\033[?25l";
    Appearance::clearScreen();
    MenuScreen::displayGameLogoPanel("TUTORIAL CONCLUIDO");
    if (infoBox.empty()) std::cout << "\n";
    else displayInfoBox(infoBox);

    Appearance::printCentralized("Voce concluiu as etapas selecionadas do tutorial de Parry!");

    InputControl::waitForEnter();
}
