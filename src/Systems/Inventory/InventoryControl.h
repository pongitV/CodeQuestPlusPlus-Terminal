/*
 * Arquivo: InventoryControl.h
 * Propósito: Regras de uso e equipagem de itens no inventário (processamento de resultados e erros).
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
    Error_ShiftJaUsed,
    Error_ShieldBroken,
    Error_Requirements,
    Error_NoCanUse,
    Nothing
};

struct UseItemInfo {
    ResultItem result;
    std::string itemName;
    std::string messageExtra;
    bool consumiuShift = false;
};

class ControlInventory {
public:
    static UseItemInfo useOuEquip(Character* player, Item* item, bool shiftJaWasConsumed);
    static std::string getMessageError(Item* item, bool emCombat);
};
