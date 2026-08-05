#pragma once

#include "Domain/Characters/Character.h"
#include <utility>
#include <vector>
#include <string>

class ScreenInventory 
{
public:
    static void displayBoxEquipped(Character* currentPlayer);
    static std::vector<std::pair<std::string, Item*>> getListCategory(Character* currentPlayer, int category, bool showPrices = false);
    static void displayInspectionItem(Item* item, Character* currentPlayer = nullptr);
    static void displayHeaderInventory(bool animate = false, int startY = -1);
};
