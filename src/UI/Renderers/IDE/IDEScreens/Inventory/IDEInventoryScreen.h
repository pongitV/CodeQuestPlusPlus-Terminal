#pragma once

#include "UI/Interfaces/IInventoryUI.h"
#include <string>
#include <vector>

class Character;
class Item;

class IDEInventoryScreen : public IInventoryUI {
public:
    IDEInventoryScreen() = default;
    ~IDEInventoryScreen() override = default;

    void displayHeader(bool animate, int startY) override;
    void displayBoxEquipped(Character* player) override;
    void displayDetailItem(Item* item) override;
    void renderMenu(const std::vector<std::string>& lines, const std::string& title, int selectionCurrent, int& outW, int& outH) override;

    static IDEInventoryScreen& instance() {
        static IDEInventoryScreen s_instance;
        return s_instance;
    }
};
