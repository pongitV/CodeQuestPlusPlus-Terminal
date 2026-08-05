/*
 * Arquivo: Parry.h
 * Propósito: Sistema de minigame de bloqueio perfeito (Parry) no momento de um ataque inimigo.
 */

#pragma once

#include <functional>
#include <string>

class Character;

/*
 * Gerenciador da mecânica de parry/parada em combate.
 */
class Parry 
{
public:
    static std::function<void()> onUpdateScreen;
    static std::string minigameMessage;
    static std::string minigameBar;

    // Executa a tentativa de parry e retorna verdadeiro em caso de bloqueio bem-sucedido
    static bool tryParry(Character* attacker, Character* defender, int damageMitigated, int& quantityDeDamageReduced);

    static std::string getMessageFeedback(bool parrySuccess, int damageEnd);

    // Minigames de parry (Reação rápida de teclado/movimento)
    static bool executeMinigameMovement(int difficulty, int damageMitigated, int& quantityDeDamageReduced);
    static bool executeMinigameTyping(int difficulty, int damageMitigated, int& quantityDeDamageReduced);
};

