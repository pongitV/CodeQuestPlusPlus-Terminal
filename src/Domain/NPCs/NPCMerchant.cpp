#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <map>

#include "Domain/NPCs/NPCMerchant.h"
#include "UI/Screens/Menu/MenuScreen.h"
#include "Domain/Items/Item.h"
#include "Domain/Items/ItemFactory.h"
#include "UI/Screens/Inventory/InventoryScreen.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Core/Engine/Shop.h"
#include "Core/Utils/DialogFunctions.h"
#include "Domain/NPCs/NPCMerchantLayout.h"

namespace {
    std::map<int, ShopProduct> stockPocoes = {
        {1, {ItemID::LittleCure30, 10, -1}}
    };

    std::map<int, ShopProduct> stockTalismans = {
        {1, {ItemID::TalismanBear, 200, 1}},
        {2, {ItemID::TalismanCrow, 200, 1}},
        {3, {ItemID::TalismanLeopard, 200, 1}},
        {4, {ItemID::TalismanOwl, 200, 1}}
    };

    std::map<int, ShopProduct> stockDelicacies = {
        {1, {ItemID::DeviceLanguage, 1000, 1}}
    };

    // --- APARENCIA E DIALOGOS ---
    void processPurchasePocoes(Character* currentPlayer);
    void processPurchaseTalismans(Character* currentPlayer);
    void processPurchaseDelicacies(Character* currentPlayer);
    void processSaleDeItems(Character* currentPlayer);

    void dialogueFrancesco(const std::vector<std::string>& lines) {
        Appearance::displayPopup("FRANCHESCO", lines, Color::YELLOW, NPCMerchantLayouts::artMerchant);
    }
    
    void dialogueFrancescoUnique(const std::string& msg) {
        dialogueFrancesco({msg});
    }
}

// --- INFORMACOES DO LUGAR ---
std::string NPCMerchant::getPlaceName() const {
    return "MERCADOR AMBULANTE";
}

Color NPCMerchant::getHeaderColor() const {
    return Color::YELLOW;
}

Color NPCMerchant::getArtColor() const {
    return Color::YELLOW;
}

const std::vector<std::string>& NPCMerchant::getArtASCII() const {
    return NPCMerchantLayouts::artMerchant;
}

// --- INTERACAO E MENU ---
void NPCMerchant::interact(Character* player) {
    InputControl::executeLoopMenuPopup(
        [this, player]() { this->displayDialogue(player); },
        [this, player]() { return this->getOptionsMenu(player, Appearance::getTerminalWidth()); },
        [this, player](const std::string& op) { this->processOption(player, op, Appearance::getTerminalWidth()); return true; },
        getPlaceName(), getHeaderColor(), getArtASCII()
    );
}

void NPCMerchant::displayDialogue(Character* player) {
    dialogueFrancesco(std::vector<std::string>{
        "Bem-vindo! De uma olhada nas",
        "minhas mercadorias."
    });
}

std::vector<std::string> NPCMerchant::getOptionsMenu(Character* player, int terminalWidth) {
    return {
        "COMPRAR Pocoes",
        "COMPRAR Talismas",
        "COMPRAR Iguarias",
        "VENDER Itens do Inventario",
        "Missoes de Franchesco",
        "VOLTAR"
    };
}

void NPCMerchant::processOption(Character* player, const std::string& option, int terminalWidth) {
    if (option == "COMPRAR Pocoes") {
        processPurchasePocoes(player);
    }
    else if (option == "COMPRAR Talismas") {
        processPurchaseTalismans(player);
    }
    else if (option == "COMPRAR Iguarias") {
        processPurchaseDelicacies(player);
    }
    else if (option == "VENDER Itens do Inventario") {
        processSaleDeItems(player);
    }
    else if (option == "Missoes de Franchesco") {
        InteractionNPC::processMenuMissionsEmpty(player, "MISSOES DE FRANCHESCO", Color::YELLOW, "Franchesco", "Ah, meu amigo! Nao tenho nenhum pedido especial para voce agora.");
    }
}

namespace {
    // --- PROCESSAMENTO DE OPCOES ---
    void processPurchasePocoes(Character* currentPlayer) {
        Shop::processPurchase(currentPlayer, "LOJA - POCOES", Color::YELLOW, stockPocoes, 
            [](const std::string& msg) { dialogueFrancescoUnique(msg); }, InteractionNPC::getFormatterStatusItem, NPCMerchantLayouts::artMerchant);
    }

    void processPurchaseTalismans(Character* currentPlayer) {
        Shop::processPurchase(currentPlayer, "LOJA - TALISMAS", Color::YELLOW, stockTalismans, 
            [](const std::string& msg) { dialogueFrancescoUnique(msg); }, InteractionNPC::getFormatterStatusItem, NPCMerchantLayouts::artMerchant);
    }

    void processPurchaseDelicacies(Character* currentPlayer) {
        Shop::processPurchase(currentPlayer, "LOJA - IGUARIAS", Color::YELLOW, stockDelicacies, 
            [](const std::string& msg) { dialogueFrancescoUnique(msg); }, InteractionNPC::getFormatterStatusItem, NPCMerchantLayouts::artMerchant);
    }

    void processSaleDeItems(Character* currentPlayer) {
        do {
            std::vector<std::pair<std::string, std::vector<Item*>>> groupsItems;
            std::map<std::string, int> indexMap;
            
            for (auto* item : currentPlayer->getInventory()->getAllItems()) {
                if (item->getType() != EquipmentType::MISSION) {
                    std::string itemName = item->getItemName();
                    bool equipped = currentPlayer->isItemEquipped(item);
                    std::string key = itemName;
                    if (equipped) {
                        key += " [Equipado]";
                    }
                    
                    if (indexMap.find(key) == indexMap.end()) {
                        indexMap[key] = groupsItems.size();
                        groupsItems.push_back({key, {item}});
                    } else {
                        groupsItems[indexMap[key]].second.push_back(item);
                    }
                }
            }

            std::vector<std::vector<Item*>> itemsValid;
            std::vector<std::string> optionsItem;
            
            for (const auto& group : groupsItems) {
                int qty = group.second.size();
                Item* itemExample = group.second.front();
                int priceSale = itemExample->getPriceSale();
                
                std::string textOption = group.first + " (" + std::to_string(priceSale) + "G)";
                if (qty > 1) {
                    textOption += " x" + std::to_string(qty);
                }
                
                itemsValid.push_back(group.second);
                optionsItem.push_back(textOption);
            }

            if (optionsItem.empty()) { dialogueFrancescoUnique("Voce nao tem nada que me interesse!"); break; }
            optionsItem.push_back("VOLTAR");
            
            int choice = InputControl::readMenuSelectionInPopup("VENDER ITENS", {"Seu Ouro: " + std::to_string(currentPlayer->getInventory()->getGold()) + "G", "Escolha um item para vender:"}, optionsItem, Color::YELLOW, NPCMerchantLayouts::artMerchant);
            if (choice == -1 || choice == static_cast<int>(optionsItem.size()) - 1) break;
            
            std::vector<Item*> itemsChosen = itemsValid[choice];
            Item* itemForSale = itemsChosen.front();

            if (currentPlayer->isItemEquipped(itemForSale)) {
                dialogueFrancescoUnique("Nao e possivel vender itens que estao equipados!");
                continue;
            }
            
            int qtyForSell = 1;
            if (itemsChosen.size() > 1) {
                std::vector<std::string> qtyOptions = {
                    "Vender 1 unidade",
                    "Vender Todos (" + std::to_string(itemsChosen.size()) + " unidades)",
                    "Digitar quantidade...",
                    "Cancelar"
                };
                
                int qtyChoice = InputControl::readMenuSelectionInPopup(
                    "QUANTIDADE: " + itemForSale->getItemName(),
                    {"Voce possui " + std::to_string(itemsChosen.size()) + " unidades deste item."},
                    qtyOptions, 
                    Color::YELLOW, 
                    NPCMerchantLayouts::artMerchant
                );
                
                if (qtyChoice == 0) {
                    qtyForSell = 1;
                } else if (qtyChoice == 1) {
                    qtyForSell = itemsChosen.size();
                } else if (qtyChoice == 2) {
                    std::string qtyMsg = "Quantidade (1 a " + std::to_string(itemsChosen.size()) + ", 0 cancelar): ";
                    qtyForSell = Appearance::readIntegerInFloatingPopup(qtyMsg, 0, itemsChosen.size(), Color::YELLOW);
                } else {
                    continue; // Cancelar
                }
            }
            
            if (qtyForSell == 0) continue;

            std::string nameItemSale = itemForSale->getItemName();
            int priceSaleUnitary = itemForSale->getPriceSale();
            int gainTotal = priceSaleUnitary * qtyForSell;
            
            currentPlayer->getInventory()->addGold(gainTotal);
            for (int i = 0; i < qtyForSell; ++i) {
                currentPlayer->getInventory()->removeItem(itemsChosen[i]);
            }
            
            if (qtyForSell > 1) {
                dialogueFrancescoUnique("Voce vendeu " + std::to_string(qtyForSell) + "x " + nameItemSale + " por " + std::to_string(gainTotal) + "G!");
            } else {
                dialogueFrancescoUnique("Voce vendeu " + nameItemSale + " por " + std::to_string(gainTotal) + "G!");
            }
        } while (true);
    }
}
