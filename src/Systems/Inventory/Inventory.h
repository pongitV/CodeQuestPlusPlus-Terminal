// [PT-BR] Arquivo: Inventory.h
// [PT-BR] Proposito: Armazenamento e gerenciamento dos itens e moedas de ouro do personagem.
// [EN-US] File: Inventory.h
// [EN-US] Purpose: Storage and management of character items and gold coins.

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iterator>

#include "Domain/Items/Item.h"

// [PT-BR] Classe responsavel pela colecao de itens e contagem de ouro.
// [EN-US] Class responsible for item collection and gold count.
class Inventory
{
private:
    std::vector<std::unique_ptr<Item>> itemList;
    std::unordered_map<std::string, int> countItems_;
    int goldAmount;

public:
    std::vector<Item*> getAllItems() const { 
        std::vector<Item*> itemsRaw;
        itemsRaw.reserve(itemList.size());
        std::transform(itemList.begin(), itemList.end(), std::back_inserter(itemsRaw), [](const auto& item) { return item.get(); });
        return itemsRaw;
    }
    
    Inventory();
    ~Inventory() = default;

    // [PT-BR] --- Funcoes Basicas e Estado ---
    // [EN-US] --- Basic Functions and State ---
    bool isEmpty() const;
    int getGold() const;
    int countItem(const std::string& itemName) const;

    // [PT-BR] --- Manipulacao do Inventario ---
    // [EN-US] --- Inventory Manipulation ---
    void addGold(int additionalGold);
    void addItem(std::unique_ptr<Item> newItem);
    void removeItem(const std::string& itemName);
    void removeItem(Item* exactItem);

    // [PT-BR] --- Buscas e Interacoes ---
    // [EN-US] --- Queries and Interactions ---
    Item* searchItemByCode(const std::string& codeTyped, Item* weaponEquipped, Item* shieldEquipped, Item* armorEquipped);
};
