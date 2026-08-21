// [PT-BR] Arquivo: Map2Forest.h
// [PT-BR] Proposito: Definicao do segundo mapa jogavel (Floresta Sombria) e suas sub-areas.
// [EN-US] File: Map2Forest.h
// [EN-US] Purpose: Definition of the second playable map (Dark Forest) and its sub-areas.

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "Domain/Characters/Character.h"
#include "World/InteractionMap.h"

// [PT-BR] Implementacao do mapa da Floresta Sombria.
// [EN-US] Implementation of the Dark Forest map.
class Map2Forest final : public IMap 
{
public:
    std::vector<std::string> currentMapMatrix;
    int playerPositionX;
    int playerPositionY;
    Character* currentPlayer;
    
    std::vector<std::string> savedMainMapMatrix;
    int savedPositionXBeforeEnteringSubMap;
    int savedPositionYBeforeEnteringSubMap;
    bool playerIsInsideSubMap;
 
    std::vector<std::string> savedTreeHeartMapMatrix;
    std::vector<std::string> savedMazeMapMatrix;
    std::vector<std::string> savedBossRoomMapMatrix;
    bool heartOfTreeAlreadyVisited;
    bool mazeAlreadyVisited;
    bool bossRoomAlreadyVisited;
    bool isExplorationActive;
    std::string currentMapTitle;
    NextMapTransition nextMap;

    std::unordered_map<char, std::unique_ptr<ForestInteraction>> interactions;

public:
    explicit Map2Forest(Character* playerCharacter);
    ~Map2Forest() override;
    
    std::string getTitle() const override { return currentMapTitle; }
    NextMapTransition startExplorationLoop() override;

private:
    void initializeInteractions();
};
