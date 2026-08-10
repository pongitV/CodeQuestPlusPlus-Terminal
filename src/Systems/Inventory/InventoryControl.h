/*
 * Arquivo: InventoryControl.h
 * Proposito: Regras de uso e equipagem de itens no inventario (processamento de resultados e erros).
 */

#pragma once
#include <string>

class Character;
class Item;

enum class ResultItem {
    Equipped,
    Unequipped,
    Used_Shift,
    Used_WithoutShift,
    Error_TurnAlreadyUsed,
    Error_ShieldBroken,
    Error_Requirements,
    Error_CannotUse,
    Nothing
};

struct UseItemInfo {
    ResultItem result;
    std::string itemName;
    std::string messageExtra;
    bool consumedTurn = false;
};

class ControlInventory {
public:
    static UseItemInfo useOrEquip(Character* player, Item* item, bool turnAlreadyConsumed);
    static std::string getMessageError(Item* item, bool inCombat);
};
