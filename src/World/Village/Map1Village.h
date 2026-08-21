// [PT-BR] Arquivo: Map1Village.h
// [PT-BR] Proposito: Definicao do mapa inicial (Vila Inicial / Vila 1) e suas sub-areas.
// [EN-US] File: Map1Village.h
// [EN-US] Purpose: Definition of the starting map (Initial Village / Village 1) and its sub-areas.

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

#include "Domain/Characters/Character.h"
#include "World/InteractionMap.h"

// [PT-BR] Implementacao do primeiro mapa jogavel (Vila).
// [EN-US] Implementation of the first playable map (Village).
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

    // [PT-BR] Estado persistido do mapa durante exploracao
    // [EN-US] Persistent map state during exploration
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
