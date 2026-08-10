#pragma once
#include <vector>
#include <memory>

class Character;

class ShiftManager {
public:
    // Retorna a maior destreza entre todos os inimigos
    static int calculateMaxDexterityEnemies(const std::vector<std::unique_ptr<Character>>& enemies);
    
    // Verifica se os inimigos atacam antes do jogador (destreza inimiga > destreza jogador)
    static bool enemiesActFirst(Character* player, int maxEnemyDexterity);
    
    // Verifica se os inimigos possuem o dobro da destreza do jogador (atacam 2x seguidas no inicio)
    static bool enemiesHaveDoubleAgility(Character* player, int maxEnemyDexterity);
    
    // Verifica se o jogador possui o dobro de agilidade (ganha turno extra no inicio)
    static bool playerHasExtraTurnAtStart(Character* player, int maxEnemyDexterity);
};
