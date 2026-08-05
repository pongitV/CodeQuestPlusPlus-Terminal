#pragma once

#include <string>
#include <vector>
#include "UI/IInventoryUI.h"

class Character;
class Item;

class ScreenInventoryRaycaster : public IInventoryUI {
public:
    void renderMenu(const std::vector<std::string>& lines, const std::string& title, int selectionCurrent, int& outW, int& outH) override;
    void displayHeader(bool ehGo, int startY) override;
    void displayBoxEquipped(Character* currentPlayer) override;
    void displayDetailItem(Item* item) override;
};
