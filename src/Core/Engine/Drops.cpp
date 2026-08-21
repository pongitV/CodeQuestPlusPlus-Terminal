// [PT-BR] Arquivo: Drops.cpp
// [PT-BR] Proposito: Implementacao das funcoes de recompensa e distribuicao de itens/XP/ouro.
// [EN-US] File: Drops.cpp
// [EN-US] Purpose: Implementation of reward functions and distribution of items/XP/gold.

#include "Core/Engine/Drops.h"
#include "Domain/Characters/Character.h"
#include "Domain/Items/ItemFactory.h"
#include "Core/Utils/Appearance.h"
#include "UI/Screens/Combat/CombatScreen.h"
#include "Core/Utils/RandomGenerator.h"
#include "Systems/Progression/Diary.h"
#include "Core/Utils/DialogFunctions.h"

void Drops::reportAndProcessXpGold(Character* player, int xpDrop, int goldDrop, int& totalGold, int& totalXp) 
{
    player->gainXp(xpDrop);
    player->gainGold(goldDrop);
    totalXp += xpDrop;
    totalGold += goldDrop;

    if (goldDrop > 0) Appearance::registerBattleLog(DialogueFunctions::formatDropMsg("+" + std::to_string(goldDrop) + "G", Color::YELLOW));
    if (xpDrop > 0) Appearance::registerBattleLog(DialogueFunctions::formatDropMsg("+" + std::to_string(xpDrop) + " XP", Color::CYAN));
}

void Drops::reportItemDrop(const std::string& itemName, int quantity) 
{
    Appearance::registerBattleLog(DialogueFunctions::formatDropMsg("+" + std::to_string(quantity) + "x " + itemName, Color::WHITE));
}

void Drops::giveAndProcessItem(Character* player, ItemID itemId, int quantity, std::vector<std::string>& obtainedItems, int dropChance)
{
    if (quantity <= 0) return;
    if (dropChance < 100 && !RandomGenerator::rollChance(dropChance)) return;

    std::string itemName = ItemFactory::getNameFromID(itemId);
    if (itemName.empty() || itemName == "Desconhecido") {
        auto temp = ItemFactory::createItem(itemId);
        if (temp) itemName = temp->getItemName();
    }
    for (int i = 0; i < quantity; ++i) {
        auto createdItem = ItemFactory::createItem(itemId);
        // [PT-BR] Obtem o nome formatado (com cores/degrade se aplicavel) no primeiro item criado
        // [EN-US] Gets the formatted name (with colors/gradients if applicable) on the first created item
        if (createdItem && i == 0) itemName = createdItem->getItemName();
        player->getInventory()->addItem(std::move(createdItem));
        obtainedItems.push_back(itemName);
    }
    Diary::instance().registerItem(Appearance::removeANSIColors(itemName));
    reportItemDrop(itemName, quantity);
}
