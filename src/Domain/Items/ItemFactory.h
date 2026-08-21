#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Domain/Items/Item.h"

class ItemFactory {
public:

    // [PT-BR] Cria um item de forma type-safe baseada num Enum ItemID
    // [EN-US] Creates an item in a type-safe manner based on ItemID Enum
    static std::unique_ptr<Item> createItem(ItemID id);

    // [PT-BR] Mantido para retrocompatibilidade com sistema de Saves e Encantamentos (+)
    // [EN-US] Maintained for backward compatibility with Save and Enchantment systems (+)
    static std::unique_ptr<Item> createItem(const std::string& name);
    
    static std::vector<std::unique_ptr<Item>> createSeveralItems(ItemID id, int quantity);
    static std::vector<std::unique_ptr<Item>> createPotionKit(int quantity = 3);

    static std::string getNameFromID(ItemID id);
    static ItemID getIDFromName(const std::string& name);
};
