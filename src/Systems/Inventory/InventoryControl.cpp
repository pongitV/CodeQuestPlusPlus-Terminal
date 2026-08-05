#include "Systems/Inventory/InventoryControl.h"
#include "Domain/Items/Item.h"
#include "Domain/Items/Equipment/WeaponEquipment.h"
#include "Domain/Items/Equipment/ShieldEquipment.h"
#include "Domain/Items/Equipment/ArmorEquipment.h"
#include "Domain/Characters/Character.h"
#include <string>

UseItemInfo ControlInventory::useOuEquip(Character* player, Item* item, bool shiftJaWasConsumed) {
    if (shiftJaWasConsumed) {
        return {ResultItem::Error_ShiftJaUsed, "", "", false};
    }

    if (item->isEquipable()) {
        if (item->getType() == EquipmentType::SHIELD && item->getDurabilityCurrentShield() <= 0) {
            return {ResultItem::Error_ShieldBroken, item->getItemName(), "", false};
        }

        bool unequipped = false;
        if (item == player->getWeapons()) {
            player->unequipWeapon();
            unequipped = true;
        } else if (item == player->getShield()) {
            player->unequipShield();
            unequipped = true;
        } else if (item == player->getArmor()) {
            player->unequipArmor();
            unequipped = true;
        }

        if (unequipped) {
            return {ResultItem::Unequipped, item->getItemName(), "", true};
        }

        if (!item->canBeEquippedBy(player)) {
            return {ResultItem::Error_Requirements, item->getItemName(), item->getMessageRequirement(), false};
        }

        player->equipItem(item);
        return {ResultItem::Equipped, item->getItemName(), "", true};
    }

    bool consumiu = false;
    if (item->useDoInventory(player, &consumiu)) {
        return {ResultItem::Used_Shift, item->getItemName(), "", consumiu};
    }

    return {ResultItem::Error_NoCanUse, item->getItemName(), "", false};
}

std::string ControlInventory::getMessageError(Item* item, bool emCombat) {
    switch (item->getType()) {
        case EquipmentType::MATERIAL:
            return "Materiais sao utilizados para NPCs especializados.";
        case EquipmentType::MISSION:
            return "Itens de missao sao ativados automaticamente.";
        case EquipmentType::CONSUMABLE:
            return "Este consumivel nao pode ser usado " + std::string(emCombat ? "no combate!" : "fora de combate!");
        default:
            return "Este item nao possui uso direto no inventario.";
    }
}
