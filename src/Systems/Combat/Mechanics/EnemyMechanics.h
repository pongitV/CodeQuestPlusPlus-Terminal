#pragma once
#include <vector>

class Character;

class EnemyMechanics {
public:
    // Determina o alvo de um ataque do inimigo baseado na prioridade (minions > aliados normais > player)
    static Character* chooseTarget(const std::vector<Character*>& alliesAlive, Character* currentPlayer);
};
