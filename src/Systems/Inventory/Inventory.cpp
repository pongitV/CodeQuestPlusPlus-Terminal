#include <algorithm>
#include <unordered_map>

#include "Systems/Inventory/Inventory.h"
#include "Domain/Items/Item.h"
#include "Domain/Items/Items/ConsumableItem.h"
#include "Core/Engine/GameMenu.h"
#include "Core/Utils/Appearance.h"

Inventory::Inventory() : quantityDeGold(0) {}

bool Inventory::thisEmpty() const { return listDeItems.empty(); }

int Inventory::getGold() const { return quantityDeGold; }

int Inventory::countItem(const std::string& itemName) const 
{
    auto it = countItems_.find(itemName);
    return it != countItems_.end() ? it->second : 0;
}


void Inventory::addGold(int quantityAdditional) 
{ 
    quantityDeGold = std::max(0, quantityDeGold + quantityAdditional); 
}

void Inventory::addItem(std::unique_ptr<Item> newItem) 
{ 
    if (newItem) {
        countItems_[newItem->getItemName()]++;
        listDeItems.push_back(std::move(newItem));
    }
}

void Inventory::removeItem(const std::string& itemName) 
{
    auto it = std::find_if(listDeItems.begin(), listDeItems.end(), [&](const std::unique_ptr<Item>& item) 
    {
        return item->getItemName() == itemName;
    });
    
    if (it != listDeItems.end()) 
    {
        countItems_[itemName]--;
        if (countItems_[itemName] <= 0) countItems_.erase(itemName);
        listDeItems.erase(it);
    }
}

void Inventory::removeItem(Item* itemExact) 
{
    if (!itemExact) return;
    auto it = std::find_if(listDeItems.begin(), listDeItems.end(), [&](const std::unique_ptr<Item>& item) 
    {
        return item.get() == itemExact;
    });
    
    if (it != listDeItems.end()) {
        std::string name = itemExact->getItemName();
        countItems_[name]--;
        if (countItems_[name] <= 0) countItems_.erase(name);
        listDeItems.erase(it);
    }
}

Item* Inventory::searchItemByCode(const std::string& codeTyped, Item* weaponEquipped, Item* shieldEquipped, Item* armorEquipped)
{
    if (codeTyped.length() < 2) return nullptr;

    char lyricsDaCategory = std::toupper(codeTyped.back());
    std::string partNumerical = codeTyped.substr(0, codeTyped.length() - 1);
    
    if (!std::all_of(partNumerical.begin(), partNumerical.end(), ::isdigit)) return nullptr;
    
    int indexDoItem = std::stoi(partNumerical);
    if (indexDoItem <= 0) return nullptr;

    if (lyricsDaCategory == 'E')
    {
        if (indexDoItem == 1) return weaponEquipped;
        if (indexDoItem == 2) return shieldEquipped;
        if (indexDoItem == 3) return armorEquipped;
        return nullptr;
    }

    auto searchByTypeGrouped = [&](auto condition) -> Item* {
        std::unordered_map<std::string, size_t> cacheDeIndex;
        std::vector<std::string> namesOfItemsDisplayed;

        for (size_t i = 0; i < listDeItems.size(); ++i) {
            Item* itemCurrent = listDeItems[i].get();
            if (condition(itemCurrent)) {
                if (cacheDeIndex.find(itemCurrent->getItemName()) == cacheDeIndex.end()) {
                    cacheDeIndex[itemCurrent->getItemName()] = i;
                    namesOfItemsDisplayed.push_back(itemCurrent->getItemName());
                }
            }
        }
        
        Appearance::sortAlphabetically(namesOfItemsDisplayed);

        if (indexDoItem > 0 && indexDoItem <= static_cast<int>(namesOfItemsDisplayed.size())) {
            size_t indexOriginalNoInventory = cacheDeIndex[namesOfItemsDisplayed[indexDoItem - 1]];
            return listDeItems[indexOriginalNoInventory].get();
        }
        return nullptr;
    };

    switch (lyricsDaCategory) {
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
