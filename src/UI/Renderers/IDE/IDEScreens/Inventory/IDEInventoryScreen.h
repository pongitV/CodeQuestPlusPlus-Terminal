#pragma once

#include <string>
#include <vector>
#include <utility>

#include "Domain/Characters/Character.h"

class Item;

class ScreenInventoryGO {
public:
    static void displayHeaderInventory(bool animate = false);
    static void displayBoxEquipped(Character* currentPlayer);
    static void displayInspectionItem(Item* item, Character* currentPlayer = nullptr);
};
