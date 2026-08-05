/*
 * Arquivo: EnemyCreator.cpp
 * Propósito: Implementação da fábrica de instanciação de inimigos com variação de atributos.
 */

#include "Core/Engine/EnemyCreator.h"
#include <type_traits>
#include "Domain/Characters/Races/Orc.h"
#include "Domain/Characters/Classes/BaseClass.h"
#include "Domain/Items/Item.h"
#include "Domain/Items/Equipment/WeaponEquipment.h"
#include "Domain/Items/Equipment/ArmorEquipment.h"
#include "Domain/Enemies/BaseEnemyClass.h"
#include "Domain/Enemies/Goblin.h"
#include "Domain/Enemies/Slime.h"
#include "Domain/Enemies/Fairy.h"
#include "Domain/Enemies/ExiledOrc.h"
#include "Domain/Enemies/ForestAbomination.h"
#include "Domain/Enemies/Mimic.h"
#include "Domain/Enemies/Troll.h"
#include "Domain/Enemies/Mahoraga.h"
#include "Core/Utils/RandomGenerator.h"

template<typename RaceType, typename ClasseType>
std::vector<std::unique_ptr<Character>> EnemyCreator::createGenericEnemies(int quantity, int maxVariation)
{
    std::vector<std::unique_ptr<Character>> horde;
    horde.reserve(quantity); 

    for (auto i{0}; i < quantity; ++i) 
    {
        auto race{std::make_unique<RaceType>()};
        auto raceName{race->getRaceName()};
        auto enemy = std::make_unique<Character>(
            raceName,
            std::move(race),
            std::make_unique<ClasseType>()
        );

        // Aplica variação percentual aleatória nos atributos para individualizar cada inimigo da horda
        int healthVariation = RandomGenerator::getInteger(-maxVariation, maxVariation);
        enemy->getEndAttributes().health += (enemy->getEndAttributes().health * healthVariation) / 100;
        enemy->setHealth(enemy->getEndAttributes().health); // Sincroniza os pontos de vida atuais com a nova vida máxima
        
        int strengthVariation = RandomGenerator::getInteger(-maxVariation, maxVariation);
        enemy->getEndAttributes().strength += (enemy->getEndAttributes().strength * strengthVariation) / 100;
        
        int dexterityVariation = RandomGenerator::getInteger(-maxVariation, maxVariation);
        enemy->getEndAttributes().dexterity += (enemy->getEndAttributes().dexterity * dexterityVariation) / 100;

        horde.push_back(std::move(enemy));
    }
    return horde;
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createGoblinEnemy(int quantity)
{
    return createGenericEnemies<Goblin, BaseEnemyClass>(quantity, 10);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createSlimeEnemy(int quantity)
{
    return createGenericEnemies<Slime, BaseEnemyClass>(quantity, 10);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createFairyEnemy(int quantity)
{
    return createGenericEnemies<Fairy, BaseEnemyClass>(quantity, 10);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createExiledOrcEnemy(int quantity)
{
    return createGenericEnemies<ExiledOrc, BaseEnemyClass>(quantity, 5);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createForestAbominationEnemy(int quantity)
{
    return createGenericEnemies<ForestAbomination, BaseEnemyClass>(quantity, 5);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createTrollEnemy(int quantity)
{
    return createGenericEnemies<Troll, BaseEnemyClass>(quantity, 5);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createMimicEnemy(int quantity)
{
    return createGenericEnemies<Mimic, BaseEnemyClass>(quantity, 10);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createMahoragaEnemy(int quantity)
{
    return createGenericEnemies<Mahoraga, BaseEnemyClass>(quantity, 5);
}
