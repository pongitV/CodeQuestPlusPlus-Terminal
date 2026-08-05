#pragma once
#include <string>
#include <vector>
#include "Core/Utils/Appearance.h"

class Character;
class Item;

class IInventoryUI {
public:
    virtual ~IInventoryUI() = default;
    virtual void displayHeader(bool animate, int startY) = 0;
    virtual void displayBoxEquipped(Character* player) = 0;
    virtual void displayDetailItem(Item* item) = 0;
    virtual void renderMenu(const std::vector<std::string>& lines, const std::string& title, int selectionCurrent, int& outW, int& outH) = 0;
};
