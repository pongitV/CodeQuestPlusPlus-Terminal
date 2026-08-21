// [PT-BR] Arquivo: Map3KingdomBridge.h
// [PT-BR] Proposito: Definicao do mapa de transicao e confronto (Ponte do Reino).
// [EN-US] File: Map3KingdomBridge.h
// [EN-US] Purpose: Definition of transition and encounter map (Kingdom Bridge).

#pragma once

#include <string>
#include <vector>

#include "Domain/Characters/Character.h"
#include "World/InteractionMap.h"

// [PT-BR] Implementacao do mapa da Ponte do Reino.
// [EN-US] Implementation of the Kingdom Bridge map.
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
