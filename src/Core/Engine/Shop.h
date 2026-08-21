// [PT-BR] Arquivo: Shop.h
// [PT-BR] Proposito: Sistema generico de loja para compra de itens de NPCs.
// [EN-US] File: Shop.h
// [EN-US] Purpose: Generic shop system for purchasing items from NPCs.

#pragma once
#include <map>
#include <string>
#include <functional>
#include <vector>
#include "Domain/Characters/Character.h"
#include "Domain/Items/Item.h"
#include "Core/Utils/Appearance.h"

// [PT-BR] Representa um produto disponivel em uma loja.
// [EN-US] Represents a product available in a shop.
struct ShopProduct {
    ItemID itemId;
    int price;
    // [PT-BR] Quantidade em estoque (-1 indica estoque infinito)
    // [EN-US] Stock quantity (-1 indicates infinite stock)
    int quantity;
};

// [PT-BR] Gerenciador da interface e transacoes de loja.
// [EN-US] Manager for shop interface and transactions.
class Shop {
public:
    // [PT-BR] Processa a interface interativa de compra com um NPC negociante
    // [EN-US] Processes the interactive purchase interface with a merchant NPC
    static void processPurchase(Character* currentPlayer, const std::string& shopTitle, Color shopColor, 
                                std::map<int, ShopProduct>& currentStock, 
                                const std::function<void(const std::string&)>& displayNPCDialog, 
                                const std::function<std::string(ItemID)>& extraNameFormatter = nullptr,
                                const std::vector<std::string>& asciiArt = {});
};
