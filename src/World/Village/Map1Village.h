/*
 * Arquivo: Map1Village.h
 * Propósito: Definição do mapa inicial (Vila Inicial / Vila 1) e suas sub-áreas.
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

#include "Domain/Characters/Character.h"
#include "World/InteractionMap.h"

/*
 * Implementação do primeiro mapa jogável (Vila).
 */
class Map1Village final : public IMap 
{
public:
    std::vector<std::string> currentMapMatrix;
    int playerPositionX;
    int playerPositionY;
    Character* currentPlayer;
    bool isExplorationActive;
    std::string currentMapTitle;

    std::unordered_map<char, std::unique_ptr<VillageInteraction>> interactions;

    std::vector<std::string> savedMainMapMatrix;
    int savedPositionXBeforeEnteringSubMap;
    int savedPositionYBeforeEnteringSubMap;
    bool playerIsInsideSubMap;

    std::vector<std::string> savedCaveMapMatrix;
    std::vector<std::string> savedSpawnMapMatrix;
    std::vector<std::string> villageBaseMap;

    // State moved from loop function
    bool bjornRescued;
    bool caveAlreadyVisited;
    bool spawnAlreadyVisited;

    NextMapTransition nextMap;
    bool cameFromForest;

public:
    explicit Map1Village(Character* playerCharacter);
    ~Map1Village() override;

    std::string getTitle() const override { return currentMapTitle; }
    NextMapTransition startExplorationLoop() override;

private:
    void initializeInteractions();
};
