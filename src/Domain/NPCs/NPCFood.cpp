#include "Domain/NPCs/NPCFood.h"
#include "Domain/NPCs/NPCFoodLayout.h"
#include "Core/Engine/GameMenu.h"
#include "Core/Utils/InputControl.h"
#include "Domain/Items/ItemFactory.h"
#include <iostream>

void NPCFood::interact(Character* player) {
    InteractionNPC::interact(player);
}

std::string NPCFood::getPlaceName() const {
    return "FEIRA DO REINO";
}

Color NPCFood::getHeaderColor() const {
    return Color::LIGHT_GREEN;
}

Color NPCFood::getArtColor() const {
    return Color::LIGHT_GREEN;
}

const std::vector<std::string>& NPCFood::getArtASCII() const {
    return NPCFoodLayouts::artFood;
}

void NPCFood::displayDialogue(Character* player) {
    std::vector<std::string> lines = {
        "Olá, combatente! Sente fome? A jornada deve ser cansativa.",
        "Tenho as melhores e mais frescas provisões do reino!",
        "Nossos alimentos curam sua vida instantaneamente ao serem consumidos na mochila."
    };
    Appearance::displayPopup("VENDEDOR DE COMIDAS", lines, Color::LIGHT_GREEN, getArtASCII());
}

std::vector<std::string> NPCFood::getOptionsMenu(Character* player, int /*larguraDoTerminal*/) {
    return {
        "Maca (Cura 15 HP) - 5G",
        "Pao (Cura 25 HP) - 10G",
        "Queijo (Cura 40 HP) - 18G",
        "Carne Seca (Cura 60 HP) - 30G",
        "Voltar"
    };
}

void NPCFood::processOption(Character* player, const std::string& option, int /*larguraDoTerminal*/) {
    ItemID idPurchase = ItemID::None;
    int cost = 0;

    if (option.find("Maca") != std::string::npos) {
        idPurchase = ItemID::Litter;
        cost = 5;
    }
    else if (option.find("Pao") != std::string::npos) {
        idPurchase = ItemID::Bread;
        cost = 10;
    }
    else if (option.find("Queijo") != std::string::npos) {
        idPurchase = ItemID::Cheese;
        cost = 18;
    }
    else if (option.find("Carne Seca") != std::string::npos) {
        idPurchase = ItemID::MeatDrought;
        cost = 30;
    }

    if (idPurchase != ItemID::None) {
        if (player->getInventory()->getGold() >= cost) {
            auto item = ItemFactory::createItem(idPurchase);
            if (item) {
                player->getInventory()->addGold(-cost);
                std::string itemName = item->getItemName();
                player->getInventory()->addItem(std::move(item));
                Appearance::displayPopup("COMPRA REALIZADA", {"Voce comprou: " + itemName}, Color::LIGHT_GREEN, getArtASCII());
            }
        } else {
            Appearance::displayPopup("OURO INSUFICIENTE", {"Voce nao tem ouro suficiente para comprar este alimento!"}, Color::RED, getArtASCII());
        }
    }
}
