#pragma once
#include <vector>
#include <memory>

class Character;

class ShiftManager {
public:
    // [PT-BR] Retorna a maior destreza entre todos os inimigos
    // [EN-US] Returns highest dexterity among all enemies
    static int calculateMaxDexterityEnemies(const std::vector<std::unique_ptr<Character>>& enemies);
    
    // [PT-BR] Verifica se os inimigos atacam antes do jogador (destreza inimiga > destreza player)
    // [EN-US] Checks if enemies attack before player (enemy dexterity > player dexterity)
    static bool enemiesActFirst(Character* player, int maxEnemyDexterity);
    
    // [PT-BR] Verifica se os inimigos possuem o dobro da destreza do jogador (atacam 2x seguidas no inicio)
    // [EN-US] Checks if enemies have double player dexterity (attack twice in a row initially)
    static bool enemiesHaveDoubleAgility(Character* player, int maxEnemyDexterity);
    
    // [PT-BR] Verifica se o jogador possui o dobro de destreza (ganha turno extra no inicio)
    // [EN-US] Checks if player has double agility (gains extra turn initially)
    static bool playerHasExtraTurnAtStart(Character* player, int maxEnemyDexterity);
};
