#pragma once

#include "Domain/Items/Item.h"
#include <string>
#include <vector>

class MaterialItem : public Item {
private:
    std::string name;
public:
    MaterialItem(const std::string& name, int price = 3);

    std::string getItemName() const override;
    EquipmentType getType() const override;
    std::vector<std::string> getDetailsInspection(Character* character = nullptr) const override;
};

std::unique_ptr<Item> manufactureItemMaterial(ItemID id);
