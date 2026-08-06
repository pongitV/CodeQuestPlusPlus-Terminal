/*
 * Arquivo: Inventory.h
 * Proposito: Armazenamento e gerenciamento dos itens e moedas de ouro do personagem.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iterator>

#include "Domain/Items/Item.h"

/*
 * Classe responsavel pela colecao de itens e contagem de ouro.
 */
class Inventory
{
private:
    std::vector<std::unique_ptr<Item>> listDeItems;
    std::unordered_map<std::string, int> countItems_;
    int quantityDeGold;

public:
    std::vector<Item*> getAllItems() const { 
        std::vector<Item*> itemsRaw;
        itemsRaw.reserve(listDeItems.size());
        std::transform(listDeItems.begin(), listDeItems.end(), std::back_inserter(itemsRaw), [](const auto& item) { return item.get(); });
        return itemsRaw;
    }
    
    Inventory();
    ~Inventory() = default;

    // Funcoes Basicas e Estado
    bool thisEmpty() const;
    int getGold() const;
    int countItem(const std::string& itemName) const;

    // Manipulacao do Inventario
    void addGold(int quantityAdditional);
    void addItem(std::unique_ptr<Item> newItem);
    void removeItem(const std::string& itemName);
    void removeItem(Item* itemExact);

    // Buscas e Interacoes
    Item* searchItemByCode(const std::string& codeTyped, Item* weaponEquipped, Item* shieldEquipped, Item* armorEquipped);
};
