#pragma once

#include <string>
#include <vector>

#include "Domain/Characters/Character.h"
#include "World/InteractionMap.h"

// Implementacao do mapa da Ponte do Reino.
class Map3KingdomBridge final : public IMap 
{
public:
    std::vector<std::string> currentMapMatrix;
    int playerPositionX;
    int playerPositionY;
    Character* currentPlayer;
    
    bool isExplorationActive;
    std::string currentMapTitle;
    NextMapTransition nextMap;

public:
    explicit Map3KingdomBridge(Character* playerCharacter);
    ~Map3KingdomBridge() override;

    std::string getTitle() const override { return currentMapTitle; }
    NextMapTransition startExplorationLoop() override;
};
