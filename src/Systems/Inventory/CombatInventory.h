#pragma once

class Character;
class Item;

class CombatInventory
{
public:
    static void manageInventory(Character* currentPlayer, bool* shiftWasConsumed = nullptr);
};
