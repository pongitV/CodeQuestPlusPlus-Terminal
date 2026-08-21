// [PT-BR] Arquivo: Parry.h
// [PT-BR] Proposito: Sistema de minigame de bloqueio perfeito (Parry) no momento de um ataque inimigo.
// [EN-US] File: Parry.h
// [EN-US] Purpose: Perfect block (Parry) minigame system during an enemy attack.

#pragma once

#include <functional>
#include <string>

class Character;

// [PT-BR] Gerenciador da mecanica de parry/parada em combate.
// [EN-US] Manager for parry/block mechanics in combat.
class Parry 
{
public:
    static std::function<void()> onUpdateScreen;
    static std::string minigameMessage;
    static std::string minigameBar;

    // [PT-BR] Executa a tentativa de parry e retorna verdadeiro em caso de bloqueio bem-sucedido
    // [EN-US] Executes parry attempt and returns true on successful block
    static bool tryParry(Character* attacker, Character* defender, int damageMitigated, int& damageReduced);

    static std::string getMessageFeedback(bool parrySuccess, int finalDamage);

    // [PT-BR] Minigames de parry (Reacao rapida de teclado e movimento)
    // [EN-US] Parry minigames (Fast keyboard and movement reaction)
    static bool executeMinigameMovement(int difficulty, int damageMitigated, int& damageReduced);
    static bool executeMinigameTyping(int difficulty, int damageMitigated, int& damageReduced);
};
