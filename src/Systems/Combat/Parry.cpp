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

bool Parry::tryParry(Character* attacker, Character* defender, int damageMitigated, int& quantityDeDamageReduced) 
{
    int dexterityDoAttacker = attacker ? std::max(1, attacker->getDexterity()) : 1;
    int dexterityDoDefender = defender ? std::max(1, defender->getDexterity()) : 1;

    if (dexterityDoAttacker > dexterityDoDefender * 2) 
    {
        std::string msgAgile = "O inimigo e agil demais para voce efetivar o parry!";
        if (Parry::onUpdateScreen) {
            Parry::minigameMessage = "\033[48;2;120;0;0m " + msgAgile + " \033[0m";
            Parry::onUpdateScreen();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            Parry::minigameMessage = "";
            Parry::onUpdateScreen();
        } else {
            std::cout << "\n" << CombatScreen::combatMargin() << DialogueFunctions::formatCombatMsg(msgAgile, Color::BG_RED) << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        quantityDeDamageReduced = 0;
        return false;
    }

    int difficulty = std::clamp(damageMitigated / 5 + (dexterityDoAttacker / 10), 1, 20);

    bool success = false;
    // Verifica se estamos na visao terminal (IDE)
    bool isTerminal = !PerspectiveManager::getInstance().is3DViewActive();
    if (isTerminal) {
        // Na visao terminal, FORCA o minigame de digitacao
        success = executeMinigameTyping(difficulty, damageMitigated, quantityDeDamageReduced);
    } else {
        // Fora da visao terminal, permite alternancia entre movimento e digitacao
        if (defender && defender->getParryModern()) {
            success = executeMinigameMovement(difficulty, damageMitigated, quantityDeDamageReduced);
        } else {
            success = executeMinigameTyping(difficulty, damageMitigated, quantityDeDamageReduced);
        }
    }
    return success;
}

std::string Parry::getMessageFeedback(bool parrySuccess, int damageEnd) {
    if (parrySuccess) {
        if (damageEnd <= 0) return "Parry Perfeito! Ataque anulado.";
        else return "Parry efetivo! -" + std::to_string(damageEnd) + " HP.";
    }
    return "Parry falhou! -" + std::to_string(damageEnd) + " HP.";
}

bool Parry::executeMinigameMovement(int difficulty, int damageMitigated, int& quantityDeDamageReduced) 
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
                bar += "\033[48;2;255;255;255m>\033[0m"; // Cursor branco
            } else if (noSweetSpot) {
                bar += "\033[38;2;50;255;50m█\033[0m"; // Zona Verde
            } else if (i < positionCurrent) {
                bar += "░"; // Rastro
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
                // Parry Perfeito!
                quantityDeDamageReduced = damageMitigated; 
            } else {
                // Parry Efetivo
                quantityDeDamageReduced = std::max(1, damageMitigated / 2); 
            }
            return true;
        }
    }
    
    quantityDeDamageReduced = 0;
    return false;
}

bool Parry::executeMinigameTyping(int difficulty, int damageMitigated, int& quantityDeDamageReduced) 
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
            } else if (c == '\b' || c == 127) { // Backspace
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
        quantityDeDamageReduced = 0;
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
            // Parry Perfeito!
            quantityDeDamageReduced = damageMitigated;
        } else {
            // Parry Efetivo!
            quantityDeDamageReduced = std::max(1, damageMitigated / 2);
        }
        return true;
    }

    quantityDeDamageReduced = 0;
    return false;
}
