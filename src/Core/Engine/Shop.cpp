/*
 * Arquivo: Shop.cpp
 * Proposito: Implementacao das transacoes e telas de loja com NPCs.
 */

#include "Core/Engine/Shop.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "Domain/Items/ItemFactory.h"
#include "Core/Utils/InputControl.h"
#include "Systems/Progression/Diary.h"
#include "UI/Screens/BaseScreen.h"

void Shop::processPurchase(Character* currentPlayer, const std::string& shopTitle, Color shopColor, 
                                      std::map<int, ShopProduct>& currentStock, 
                                      const std::function<void(const std::string&)>& displayNPCDialog, 
                                      const std::function<std::string(ItemID)>& extraNameFormatter,
                                      const std::vector<std::string>& asciiArt) {
    std::vector<std::map<int, ShopProduct>::iterator> sortedItems;
    for (auto it = currentStock.begin(); it != currentStock.end(); ++it) {
        sortedItems.push_back(it);
    }

    Appearance::sortAlphabetically(sortedItems, [&extraNameFormatter](auto it) {
        std::string itemName = ItemFactory::getNameFromID(it->second.itemId);
        if (extraNameFormatter) itemName += extraNameFormatter(it->second.itemId);
        return itemName;
    });

    Appearance::startPopupInteraction();
    Appearance::updateMinPopupSize(100, 24); // Padroniza o tamanho da tela de Loja

    bool animateEntrance = true;
    while (true) {
        std::vector<std::string> text = { 
            "Seu Ouro: " + std::to_string(currentPlayer->getInventory()->getGold()) + "G" 
        };
        
            std::vector<std::string> options;
            for (auto it : sortedItems) {
                auto& product = it->second;
                std::string itemName = ItemFactory::getNameFromID(product.itemId);
                if (extraNameFormatter) itemName += extraNameFormatter(product.itemId);
                
                std::string price = std::to_string(product.price) + "G";
                std::string stockInfo = (product.quantity == -1) ? "" : (product.quantity == 0 ? " (Esgotado)" : " (Estoque: " + std::to_string(product.quantity) + ")");
                
                options.push_back(itemName + " - " + price + stockInfo);
            }
            options.push_back("VOLTAR");
            
            int choice = InputControl::readMenuSelectionInPopup(shopTitle, text, options, shopColor, asciiArt, animateEntrance);
            animateEntrance = false;
            
            if (choice == -1 || choice == static_cast<int>(sortedItems.size())) {
                break;
            }
            
            auto it = sortedItems[choice];
            auto& product = it->second;

            if (product.quantity == 0) {
                displayNPCDialog("Este item esta esgotado!");
            } else {
                int maxBuyer = currentPlayer->getInventory()->getGold() / product.price;
                if (maxBuyer == 0) {
                    displayNPCDialog("Voce nao tem ouro suficiente para isso!");
                } else {
                    int maxPossible = (product.quantity == -1) ? maxBuyer : std::min(maxBuyer, product.quantity);
                int qtyToBuy = 1;
                
                if (maxPossible > 1) {
                    std::string itemName = ItemFactory::getNameFromID(product.itemId);
                    if (extraNameFormatter) itemName += extraNameFormatter(product.itemId);
                    
                    std::vector<std::string> qtyOptions = {
                        "Comprar 1 unidade",
                        "Comprar Maximo (" + std::to_string(maxPossible) + " unidades)",
                        "Digitar quantidade...",
                        "Cancelar"
                    };
                    
                    int qtyChoice = InputControl::readMenuSelectionInPopup(
                        "QUANTIDADE: " + itemName,
                        {"Preco Unitario: " + std::to_string(product.price) + "G", "Voce pode comprar ate " + std::to_string(maxPossible) + " unidades."},
                        qtyOptions, 
                        shopColor,
                        asciiArt,
                        false // Nunca animar submenus da loja
                    );
                    
                    if (qtyChoice == 0) {
                        qtyToBuy = 1;
                    } else if (qtyChoice == 1) {
                        qtyToBuy = maxPossible;
                    } else if (qtyChoice == 2) {
                        std::string qtyMsg = "Quantidade (1 a " + std::to_string(maxPossible) + ", 0 cancelar): ";
                        qtyToBuy = Appearance::readIntegerInFloatingPopup(qtyMsg, 0, maxPossible, shopColor);
                    } else {
                        qtyToBuy = 0; // Cancelar
                    }
                }

                    if (qtyToBuy > 0) {
                        currentPlayer->getInventory()->addGold(-(product.price * qtyToBuy));
                        if (product.quantity != -1) product.quantity -= qtyToBuy;
                        std::string newName = ItemFactory::getNameFromID(product.itemId);
                        for (int i = 0; i < qtyToBuy; ++i) currentPlayer->getInventory()->addItem(ItemFactory::createItem(product.itemId));
                        Diary::instance().registerItem(Appearance::removeANSIColors(newName));
                        displayNPCDialog(std::to_string(qtyToBuy) + "x " + newName + " comprado(s)!");
                    }
                }
            }
    }
}
