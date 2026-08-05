/*
 * Arquivo: EnemyCreator.h
 * Propósito: Fábrica responsável pela instanciação de grupos de inimigos para combates.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Domain/Characters/Character.h"

/*
 * Classe estática utilitária para fábrica de inimigos.
 */
class EnemyCreator
{
public:
    // Métodos de criação para cada tipo de inimigo pré-configurado
    static std::vector<std::unique_ptr<Character>> createGoblinEnemy(int quantity = 3);
    static std::vector<std::unique_ptr<Character>> createSlimeEnemy(int quantity = 3);
    static std::vector<std::unique_ptr<Character>> createFairyEnemy(int quantity = 5);
    static std::vector<std::unique_ptr<Character>> createExiledOrcEnemy(int quantity = 1);
    static std::vector<std::unique_ptr<Character>> createForestAbominationEnemy(int quantity = 1);
    static std::vector<std::unique_ptr<Character>> createTrollEnemy(int quantity = 1);
    static std::vector<std::unique_ptr<Character>> createMimicEnemy(int quantity = 1);
    static std::vector<std::unique_ptr<Character>> createMahoragaEnemy(int quantity = 1);

private:
    /*
     * Função utilitária genérica para instanciação de lotes de inimigos com variação aleatória de atributos.
     */
    template<typename RaceType, typename ClasseType>
    static std::vector<std::unique_ptr<Character>> createGenericEnemies(int quantity, int maxVariation = 10);
};

