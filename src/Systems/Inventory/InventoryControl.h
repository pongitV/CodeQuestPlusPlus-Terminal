#pragma once
#include <string>

class Character;
class Item;

enum class ResultItem {
    Equipped,
    Unequipped,
    Used_Turn,
    Used_WithoutTurn,
    Used_Shift = Used_Turn,
    Used_WithoutShift = Used_WithoutTurn,
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

class InventoryController {
public:
    static UseItemInfo useOrEquip(Character* player, Item* item, bool turnAlreadyConsumed);
    static std::string getMessageError(Item* item, bool inCombat);
};

// Apelido para compatibilidade retroativa
using ControlInventory = InventoryController;
