#pragma once
#include "Domain/Characters/Character.h"

class IAttributesUI {
public:
    virtual ~IAttributesUI() = default;
    virtual void display(Character* player) = 0;
    virtual void displayDetailsAttributes(Character* currentPlayer) = 0;
    virtual void managePlayerCharacterSheet(Character* currentPlayer) = 0;
};
