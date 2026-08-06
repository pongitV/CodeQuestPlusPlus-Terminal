/*
 * Arquivo: Shop.h
 * Proposito: Sistema generico de loja para compra de itens de NPCs.
 */

#pragma once
#include <map>
#include <string>
#include <functional>
#include <vector>
#include "Domain/Characters/Character.h"
#include "Domain/Items/Item.h"
#include "Core/Utils/Appearance.h"

/*
 * Representa um produto disponivel em uma loja.
 */
struct ShopProduct {
    ItemID itemId;
    int price;
    int quantity; // Quantidade em estoque (-1 indica estoque infinito)
};

/*
 * Gerenciador da interface e transacoes de loja.
 */
class Shop {
public:
    // Processa a interface interativa de compra com um NPC negociante
    static void processPurchase(Character* currentPlayer, const std::string& shopTitle, Color shopColor, 
                                std::map<int, ShopProduct>& currentStock, 
                                const std::function<void(const std::string&)>& displayNPCDialog, 
                                const std::function<std::string(ItemID)>& extraNameFormatter = nullptr,
                                const std::vector<std::string>& asciiArt = {});
};

