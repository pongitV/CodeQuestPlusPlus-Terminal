#include <map>

#include "UI/Screens/Inventory/InventoryScreen.h"
#include "UI/PerspectiveManager.h"
#include "Core/Utils/Appearance.h"
#include "Domain/Items/Item.h"

void ScreenInventory::displayHeaderInventory(bool animate, int startY) {
    PerspectiveManager::getInventoryUI().displayHeader(animate, startY);
}

void ScreenInventory::displayBoxEquipped(Character* currentPlayer) {
    PerspectiveManager::getInventoryUI().displayBoxEquipped(currentPlayer);
}

std::vector<std::pair<std::string, Item*>> ScreenInventory::getListCategory(Character* currentPlayer, int category, bool showPrices)
{
    std::vector<std::pair<std::string, Item*>> list;
    if (!currentPlayer) return list;

    std::map<std::string, std::vector<Item*>> itemsGrouped;

    for (Item* item : currentPlayer->getInventory()->getAllItems()) {
        EquipmentType type = item->getType();
        if (category == 0 && (type == EquipmentType::WEAPONS || type == EquipmentType::SHIELD || type == EquipmentType::ARMOR)) {
            itemsGrouped[item->getItemName() + item->getInfoStatus()].push_back(item);
        } else if (category == 1 && type == EquipmentType::CONSUMABLE) {
            itemsGrouped[item->getItemName()].push_back(item);
        } else if (category == 2 && type == EquipmentType::MATERIAL) {
            itemsGrouped[item->getItemName()].push_back(item);
        } else if (category == 3 && type == EquipmentType::MISSION) {
            itemsGrouped[item->getItemName()].push_back(item);
        }
    }

    std::string suffix = (category == 1 || category == 2) ? "G / un" : "G";
    if (category == 3) suffix = "";

    for (auto const& [name, itemsGroup] : itemsGrouped) {
        Item* item = itemsGroup.front();
        std::string prefix = (itemsGroup.size() > 1) ? std::to_string(itemsGroup.size()) + "x " : "";
        std::string infoSale = showPrices ? Appearance::color(Color::YELLOW) + " (Venda: " + std::to_string(item->getPriceSale()) + suffix + ")" + Appearance::color(Color::RESET) : "";

        bool someEquipped = false;
        for (Item* it : itemsGroup) {
            if (currentPlayer->isItemEquipped(it)) {
                someEquipped = true;
                break;
            }
        }

        std::string nameEnd = name;
        if (someEquipped) {
            nameEnd = Appearance::color(Color::GREEN) + "[E] " + Appearance::color(Color::RESET) + name;
        }

        list.push_back({prefix + nameEnd + infoSale, item});
    }
    return list;
}

void ScreenInventory::displayInspectionItem(Item* item, [[maybe_unused]] Character* currentPlayer)
{
    PerspectiveManager::getInventoryUI().displayDetailItem(item);
}
