#pragma once

#include "Domain/Items/Item.h"
#include <string>

class ItemMission : public Item {
private:
    std::string name;
public:
    ItemMission(const std::string& name, int price = 500);
    std::string getItemName() const override;
    EquipmentType getType() const override;
    std::vector<std::string> getDetailsInspection(Character* character = nullptr) const override;
};

std::unique_ptr<Item> manufactureItemMission(ItemID id);
