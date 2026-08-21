// [PT-BR] Arquivo: EnemyCreator.h
// [PT-BR] Proposito: Fabrica responsavel pela instanciacao de grupos de inimigos para combates.
// [EN-US] File: EnemyCreator.h
// [EN-US] Purpose: Factory responsible for instantiating enemy groups for combat encounters.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Domain/Characters/Character.h"

// [PT-BR] Classe estatica utilitaria para fabrica de inimigos.
// [EN-US] Static utility class for enemy factory operations.
class EnemyCreator
{
public:
    // [PT-BR] Metodos de criacao para cada tipo de inimigo pre-configurado
    // [EN-US] Creation methods for each pre-configured enemy type
    static std::vector<std::unique_ptr<Character>> createGoblinEnemy(int quantity = 3);
    static std::vector<std::unique_ptr<Character>> createSlimeEnemy(int quantity = 3);
    static std::vector<std::unique_ptr<Character>> createFairyEnemy(int quantity = 5);
    static std::vector<std::unique_ptr<Character>> createExiledOrcEnemy(int quantity = 1);
    static std::vector<std::unique_ptr<Character>> createForestAbominationEnemy(int quantity = 1);
    static std::vector<std::unique_ptr<Character>> createTrollEnemy(int quantity = 1);
    static std::vector<std::unique_ptr<Character>> createMimicEnemy(int quantity = 1);
    static std::vector<std::unique_ptr<Character>> createMahoragaEnemy(int quantity = 1);

private:
    // [PT-BR] Funcao utilitaria generica para instanciacao de lotes de inimigos com variacao aleatoria de atributos
    // [EN-US] Generic utility function for instantiating batches of enemies with randomized attribute variation
    template<typename RaceType, typename ClassType>
    static std::vector<std::unique_ptr<Character>> createGenericEnemies(int quantity, int maxVariation = 10);
};
