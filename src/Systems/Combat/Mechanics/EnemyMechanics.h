#pragma once
#include <vector>

class Character;

class MechanicsEnemy {
public:
    // Determina o alvo de um ataque do inimigo baseado na prioridade (minions > aliados normais > jogador)
    static Character* chooseTarget(const std::vector<Character*>& alliesAlive, Character* currentPlayer);
};
