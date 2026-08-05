#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "Domain/Characters/Character.h"
#include "World/InteractionMap.h"

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
