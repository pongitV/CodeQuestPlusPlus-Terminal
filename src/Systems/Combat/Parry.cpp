#include "Systems/Combat/Parry.h"
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "Core/Utils/InputControl.h"
#include "Domain/Characters/Character.h"
#include "Core/Utils/RandomGenerator.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/DialogFunctions.h"
#include "UI/PerspectiveManager.h"
#include "UI/Screens/Combat/CombatScreen.h"

std::function<void()> Parry::onUpdateScreen = nullptr;
std::string Parry::minigameMessage = "";
std::string Parry::minigameBar = "";

bool Parry::tryParry(Character* attacker, Character* defender, int damageMitigated, int& damageReduced) 
{
    int attackerDexterity = attacker ? std::max(1, attacker->getDexterity()) : 1;
    int defenderDexterity = defender ? std::max(1, defender->getDexterity()) : 1;

    if (attackerDexterity > defenderDexterity * 2) 
    {
        std::string agileMessage = "O inimigo e agil demais para voce efetivar o parry!";
        if (Parry::onUpdateScreen) {
            Parry::minigameMessage = "\033[48;2;120;0;0m " + agileMessage + " \033[0m";
            Parry::onUpdateScreen();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            Parry::minigameMessage = "";
            Parry::onUpdateScreen();
        } else {
            std::cout << "\n" << CombatScreen::combatMargin() << DialogueFunctions::formatCombatMsg(agileMessage, Color::BG_RED) << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        damageReduced = 0;
        return false;
    }

    int difficulty = std::clamp(damageMitigated / 5 + (attackerDexterity / 10), 1, 20);

    bool success = false;
    // [PT-BR] Verifica se estamos na visao terminal (IDE)
    // [EN-US] Checks if terminal view (IDE) is currently active
    bool isTerminal = !PerspectiveManager::getInstance().is3DViewActive();
    if (isTerminal) {
        // [PT-BR] Na visao terminal, forca o minigame de digitacao
        // [EN-US] In terminal view, forces typing minigame
        success = executeMinigameTyping(difficulty, damageMitigated, damageReduced);
    } else {
        // [PT-BR] Fora da visao terminal, permite alternancia entre movimento e digitacao
        // [EN-US] Outside terminal view, allows switching between movement and typing
        if (defender && defender->getParryModern()) {
            success = executeMinigameMovement(difficulty, damageMitigated, damageReduced);
        } else {
            success = executeMinigameTyping(difficulty, damageMitigated, damageReduced);
        }
    }
    return success;
}

std::string Parry::getMessageFeedback(bool parrySuccess, int finalDamage) {
    if (parrySuccess) {
        if (finalDamage <= 0) return "Parry Perfeito! Ataque anulado.";
        else return "Parry efetivo! -" + std::to_string(finalDamage) + " HP.";
    }
    return "Parry falhou! -" + std::to_string(finalDamage) + " HP.";
}

bool Parry::executeMinigameMovement(int difficulty, int damageMitigated, int& damageReduced) 
{
    std::string instructions = "Aperte [ESPACO] no alvo!";
    Parry::minigameMessage = instructions;
    
    int sizeBar = 40;
    int sweetSpotCenter = 32;
    int sizeSweetSpot = std::clamp(6 - (difficulty / 3), 1, 4); 
    
    int positionCurrent = 0;
    bool spacePressed = false;
    int positionPressed = -1;
    
    InputControl::clearBuffer();
    
    int delayMs = std::clamp(20 - difficulty, 5, 25);
    auto minigameStart = std::chrono::steady_clock::now();

    while (positionCurrent <= sizeBar) {
        std::string bar = "[";
        for (int i = 0; i < sizeBar; i++) {
            bool noSweetSpot = (i >= sweetSpotCenter - sizeSweetSpot/2 && i <= sweetSpotCenter + sizeSweetSpot/2);
            
            if (i == positionCurrent) {
                // [PT-BR] Cursor branco
                // [EN-US] White cursor
                bar += "\033[48;2;255;255;255m>\033[0m";
            } else if (noSweetSpot) {
                // [PT-BR] Zona Verde de acerto
                // [EN-US] Green hit zone
                bar += "\033[38;2;50;255;50m█\033[0m";
            } else if (i < positionCurrent) {
                // [PT-BR] Rastro visual
                // [EN-US] Visual trail
                bar += "░";
            } else {
                bar += " ";
            }
        }
        bar += "]";

        Parry::minigameBar = "Parry: " + bar;
        
        if (Parry::onUpdateScreen) {
            Parry::onUpdateScreen();
        } else {
            std::cout << "\r" << CombatScreen::combatMargin() << Parry::minigameBar << std::flush;
        }

        if (InputControl::pressedKey()) {
            char key = InputControl::readKey();
            auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - minigameStart).count();
            if (elapsedMs > 150) {
                if (key == ' ') {
                    spacePressed = true;
                    positionPressed = positionCurrent;
                    break;
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        positionCurrent++;
    }

    Parry::minigameMessage = "";
    Parry::minigameBar = "";
    if (Parry::onUpdateScreen) {
        Parry::onUpdateScreen();
    } else {
        std::cout << "\n";
    }
    
    if (spacePressed) {
        bool noSweetSpot = (positionPressed >= sweetSpotCenter - sizeSweetSpot/2 && positionPressed <= sweetSpotCenter + sizeSweetSpot/2);
        if (noSweetSpot) {
            int distance = std::abs(positionPressed - sweetSpotCenter);
            if (distance <= 1) {
                // [PT-BR] Parry Perfeito! (100% de absorcao)
                // [EN-US] Perfect Parry! (100% absorption)
                damageReduced = damageMitigated; 
            } else {
                // [PT-BR] Parry Efetivo (50% de absorcao)
                // [EN-US] Effective Parry (50% absorption)
                damageReduced = std::max(1, damageMitigated / 2); 
            }
            return true;
        }
    }
    
    damageReduced = 0;
    return false;
}

bool Parry::executeMinigameTyping(int difficulty, int damageMitigated, int& damageReduced) 
{
    int sizeSequence = std::clamp(4 + difficulty / 5, 4, 8);
    std::string sequence = "";
    for (int i = 0; i < sizeSequence; ++i) {
        sequence += std::to_string(RandomGenerator::getInteger(0, 9));
    }

    double timeLimit = std::max(1.5, 4.0 - (difficulty / 6.0));

    std::string instructions = "DIGITE: " + sequence;
    Parry::minigameMessage = instructions;

    std::string answer = "";
    auto home = std::chrono::steady_clock::now();
    bool timeSoldout = false;
    bool completed = false;

    InputControl::clearBuffer();

    while (true) {
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - home).count();

        if (elapsed >= timeLimit) {
            timeSoldout = true;
            break;
        }

        std::ostringstream ss;
        ss << "Parry [Tempo: " << std::fixed << std::setprecision(1) << std::max(0.0, timeLimit - elapsed) << "s]: " << answer;
        Parry::minigameBar = ss.str();

        if (Parry::onUpdateScreen) {
            Parry::onUpdateScreen();
        } else {
            std::cout << "\r" << CombatScreen::combatMargin() << Parry::minigameBar << "\033[K" << std::flush;
        }

        if (InputControl::pressedKey()) {
            char c = InputControl::readKey();
            auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - home).count();
            if (c == '\r' || c == '\n') {
                if (elapsedMs > 150 && !answer.empty()) {
                    completed = true;
                    break;
                }
            } else if (c == '\b' || c == 127) {
                // [PT-BR] Trata tecla Backspace
                // [EN-US] Handles Backspace key
                if (!answer.empty()) {
                    answer.pop_back();
                }
            } else if (std::isdigit(static_cast<unsigned char>(c))) {
                answer += c;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    Parry::minigameMessage = "";
    Parry::minigameBar = "";

    if (timeSoldout) {
        if (Parry::onUpdateScreen) {
            Parry::minigameMessage = "\033[1;38;2;255;50;50mTEMPO ESGOTADO!\033[0m";
            Parry::onUpdateScreen();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            Parry::minigameMessage = "";
            Parry::onUpdateScreen();
        } else {
            std::cout << CombatScreen::combatMargin() << "\033[1;38;2;255;50;50mTEMPO ESGOTADO!\033[0m\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        damageReduced = 0;
        return false;
    }

    if (Parry::onUpdateScreen) {
        Parry::onUpdateScreen();
    } else {
        std::cout << "\n";
    }

    auto end = std::chrono::steady_clock::now();
    double timeTotal = std::chrono::duration<double>(end - home).count();

    if (completed && answer == sequence) {
        if (timeTotal <= timeLimit * 0.5) {
            // [PT-BR] Parry Perfeito por digitacao!
            // [EN-US] Perfect Parry by typing!
            damageReduced = damageMitigated;
        } else {
            // [PT-BR] Parry Efetivo por digitacao!
            // [EN-US] Effective Parry by typing!
            damageReduced = std::max(1, damageMitigated / 2);
        }
        return true;
    }

    damageReduced = 0;
    return false;
}
