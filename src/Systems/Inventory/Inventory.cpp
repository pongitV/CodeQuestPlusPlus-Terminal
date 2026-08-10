#include <algorithm>
#include <unordered_map>

#include "Systems/Inventory/Inventory.h"
#include "Domain/Items/Item.h"
#include "Domain/Items/Items/ConsumableItem.h"
#include "Core/Engine/GameMenu.h"
#include "Core/Utils/Appearance.h"

Inventory::Inventory() : goldAmount(0) {}

bool Inventory::isEmpty() const { return itemList.empty(); }

int Inventory::getGold() const { return goldAmount; }

int Inventory::countItem(const std::string& itemName) const 
{
    auto it = countItems_.find(itemName);
    return it != countItems_.end() ? it->second : 0;
}


void Inventory::addGold(int additionalGold) 
{ 
    goldAmount = std::max(0, goldAmount + additionalGold); 
}

void Inventory::addItem(std::unique_ptr<Item> newItem) 
{ 
    if (newItem) {
        countItems_[newItem->getItemName()]++;
        itemList.push_back(std::move(newItem));
    }
}

void Inventory::removeItem(const std::string& itemName) 
{
    auto it = std::find_if(itemList.begin(), itemList.end(), [&](const std::unique_ptr<Item>& item) 
    {
        return item->getItemName() == itemName;
    });
    
    if (it != itemList.end()) 
    {
        countItems_[itemName]--;
        if (countItems_[itemName] <= 0) countItems_.erase(itemName);
        itemList.erase(it);
    }
}

void Inventory::removeItem(Item* exactItem) 
{
    if (!exactItem) return;
    auto it = std::find_if(itemList.begin(), itemList.end(), [&](const std::unique_ptr<Item>& item) 
    {
        return item.get() == exactItem;
    });
    
    if (it != itemList.end()) {
        std::string name = exactItem->getItemName();
        countItems_[name]--;
        if (countItems_[name] <= 0) countItems_.erase(name);
        itemList.erase(it);
    }
}

Item* Inventory::searchItemByCode(const std::string& codeTyped, Item* weaponEquipped, Item* shieldEquipped, Item* armorEquipped)
{
    if (codeTyped.length() < 2) return nullptr;

    char categoryLetter = std::toupper(codeTyped.back());
    std::string numericalPart = codeTyped.substr(0, codeTyped.length() - 1);
    
    if (!std::all_of(numericalPart.begin(), numericalPart.end(), ::isdigit)) return nullptr;
    
    int itemIndex = std::stoi(numericalPart);
    if (itemIndex <= 0) return nullptr;

    if (categoryLetter == 'E')
    {
        if (itemIndex == 1) return weaponEquipped;
        if (itemIndex == 2) return shieldEquipped;
        if (itemIndex == 3) return armorEquipped;
        return nullptr;
    }

    auto searchByTypeGrouped = [&](auto condition) -> Item* {
        std::unordered_map<std::string, size_t> indexCache;
        std::vector<std::string> displayedItemNames;

        for (size_t i = 0; i < itemList.size(); ++i) {
            Item* itemCurrent = itemList[i].get();
            if (condition(itemCurrent)) {
                if (indexCache.find(itemCurrent->getItemName()) == indexCache.end()) {
                    indexCache[itemCurrent->getItemName()] = i;
                    displayedItemNames.push_back(itemCurrent->getItemName());
                }
            }
        }
        
        Appearance::sortAlphabetically(displayedItemNames);

        if (itemIndex > 0 && itemIndex <= static_cast<int>(displayedItemNames.size())) {
            size_t originalInventoryIndex = indexCache[displayedItemNames[itemIndex - 1]];
            return itemList[originalInventoryIndex].get();
        }
        return nullptr;
    };

    switch (categoryLetter) {
        case 'A':
            return searchByTypeGrouped([&](Item* itemEvaluated) { 
                return itemEvaluated->isEquipable() && itemEvaluated != weaponEquipped && itemEvaluated != shieldEquipped && itemEvaluated != armorEquipped; 
            });
        case 'C': return searchByTypeGrouped([](Item* itemEvaluated) { return itemEvaluated->getType() == EquipmentType::CONSUMABLE; });
        case 'S': return searchByTypeGrouped([](Item* itemEvaluated) { return itemEvaluated->getType() == EquipmentType::MATERIAL; });
        case 'M': return searchByTypeGrouped([](Item* itemEvaluated) { return itemEvaluated->getType() == EquipmentType::MISSION; });
        default:  return nullptr;
    }
}
