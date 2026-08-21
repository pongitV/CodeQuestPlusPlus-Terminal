// [PT-BR] Arquivo: Map4Kingdom.h
// [PT-BR] Proposito: Definicao do mapa final e capital (Reino / Castelo Real) e suas sub-areas.
// [EN-US] File: Map4Kingdom.h
// [EN-US] Purpose: Definition of final capital map (Kingdom / Royal Castle) and its sub-areas.

#pragma once

#include <string>
#include <vector>

#include "Domain/Characters/Character.h"
#include "World/InteractionMap.h"

// [PT-BR] Implementacao do mapa do Reino.
// [EN-US] Implementation of the Kingdom map.
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

    // [PT-BR] Controle de Submapa (Igreja)
    // [EN-US] Submap Control (Church)
    bool playerIsInsideSubMap;
    std::vector<std::string> savedMainMapMatrix;
    int savedPositionXBeforeEnteringSubMap;
    int savedPositionYBeforeEnteringSubMap;
    std::vector<std::string> savedChurchMapMatrix;
    bool churchAlreadyVisited;

public:
    explicit Map4Kingdom(Character* playerCharacter);
    ~Map4Kingdom() override;

    std::string getTitle() const override { return currentMapTitle; }
    NextMapTransition startExplorationLoop() override;
};
