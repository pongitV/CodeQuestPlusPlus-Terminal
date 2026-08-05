#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Domain/Items/Item.h"

class ItemFactory {
public:

    // Cria um item de forma type-safe baseada num Enum.
    static std::unique_ptr<Item> createItem(ItemID id);

    // Mantido para retrocompatibilidade com sistema de Saves e Encantamentos (+).
    static std::unique_ptr<Item> createItem(const std::string& name);
    
    static std::vector<std::unique_ptr<Item>> createSeveralItems(ItemID id, int quantity);
    static std::vector<std::unique_ptr<Item>> createKitPocoes(int quantity = 3);

    static std::string getNameFromID(ItemID id);
    static ItemID getIDDeName(const std::string& name);
};
