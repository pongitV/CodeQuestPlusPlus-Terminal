#pragma once

#include <string>
#include <vector>

#include "Domain/Characters/Character.h"
#include "World/InteractionMap.h"

class Map4Kingdom final : public IMap 
{
public:
    std::vector<std::string> currentMapMatrix;
    int playerPositionX;
    int playerPositionY;
    Character* currentPlayer;
    
    bool isExplorationActive;
    std::string currentMapTitle;
    NextMapTransition nextMap;

    // Controle de Submapa (Igreja)
    bool playerIsInsideSubMap;
    std::vector<std::string> savedMainMapMatrix;
    int savedPositionXBeforeEnteringSubMap;
    int savedPositionYBeforeEnteringSubMap;
    std::vector<std::string> savedChurchMapMatrix;
    bool igrejaJaFoiVisitada;

public:
    explicit Map4Kingdom(Character* playerCharacter);
    ~Map4Kingdom() override;

    std::string getTitle() const override { return currentMapTitle; }
    NextMapTransition startExplorationLoop() override;
};
