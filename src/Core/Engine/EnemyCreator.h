/*
 * Arquivo: EnemyCreator.h
 * Proposito: Fabrica responsavel pela instanciacao de grupos de inimigos para combates.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Domain/Characters/Character.h"

/*
 * Classe estatica utilitaria para fabrica de inimigos.
 */
class EnemyCreator
{
public:
    // Metodos de criacao para cada tipo de inimigo pre-configurado
    static std::vector<std::unique_ptr<Character>> createGoblinEnemy(int quantity = 3);
    static std::vector<std::unique_ptr<Character>> createSlimeEnemy(int quantity = 3);
    static std::vector<std::unique_ptr<Character>> createFairyEnemy(int quantity = 5);
    static std::vector<std::unique_ptr<Character>> createExiledOrcEnemy(int quantity = 1);
    static std::vector<std::unique_ptr<Character>> createForestAbominationEnemy(int quantity = 1);
    static std::vector<std::unique_ptr<Character>> createTrollEnemy(int quantity = 1);
    static std::vector<std::unique_ptr<Character>> createMimicEnemy(int quantity = 1);
    static std::vector<std::unique_ptr<Character>> createMahoragaEnemy(int quantity = 1);

private:
    // Funcao utilitaria generica para instanciacao de lotes de inimigos com variacao aleatoria de atributos
    template<typename RaceType, typename ClasseType>
    static std::vector<std::unique_ptr<Character>> createGenericEnemies(int quantity, int maxVariation = 10);
};

