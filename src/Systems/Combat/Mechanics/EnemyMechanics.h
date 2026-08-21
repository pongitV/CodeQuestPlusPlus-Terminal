#pragma once
#include <vector>

class Character;

class EnemyMechanics {
public:
    // [PT-BR] Determina o alvo de um ataque do inimigo baseado na prioridade (minions > aliados normais > player)
    // [EN-US] Determines enemy attack target based on priority (minions > normal allies > player)
    static Character* chooseTarget(const std::vector<Character*>& alliesAlive, Character* currentPlayer);
};
