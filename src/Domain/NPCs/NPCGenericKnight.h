#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "Domain/Characters/Character.h"

class NPCGenericKnight {
public:
    // [PT-BR] --- CRIACAO DO NPC ---
    // [EN-US] --- NPC CREATION ---
    static std::unique_ptr<Character> createKnight(const std::string& name);

    // [PT-BR] --- INTERACAO ---
    // [EN-US] --- INTERACTION ---
    static void interact(Character* currentPlayer, bool& trollDefeated, bool& invitationReceived, int terminalWidth, std::vector<std::string>& currentMapMatrix, bool isExplorationActive, const std::function<void()>& restoreScreen, char destinationCell, int nextPositionX, int nextPositionY);
};
