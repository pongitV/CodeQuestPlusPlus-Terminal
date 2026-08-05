#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "Domain/Characters/Character.h"

class NPCGenericKnight {
public:
    // CRIACAO DO NPC
    static std::unique_ptr<Character> createKnight(const std::string& name);

    // INTERACAO
    static void interact(Character* currentPlayer, bool& trollDefeated, bool& invitationReceived, int terminalWidth, std::vector<std::string>& currentMapMatrix, bool isExplorationActive, const std::function<void()>& restoreScreen, char destinationCell, int nextPositionX, int nextPositionY);
};
