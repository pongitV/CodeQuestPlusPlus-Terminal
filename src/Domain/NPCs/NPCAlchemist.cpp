#include "Domain/NPCs/NPCAlchemist.h"
#include "Domain/NPCs/NPCAlchemistLayout.h"
#include "Core/Engine/GameMenu.h"
#include "Core/Utils/InputControl.h"
#include "Domain/Items/ItemFactory.h"
#include <iostream>

void NPCAlchemist::interact(Character* player) {
    InputControl::executeLoopMenuPopup(
        [this, player]() { this->displayDialogue(player); },
        [this, player]() { return this->getOptionsMenu(player, Appearance::getTerminalWidth()); },
        [this, player](const std::string& op) { this->processOption(player, op, Appearance::getTerminalWidth()); return true; },
        getPlaceName(), getHeaderColor(), getArtASCII()
    );
}

std::string NPCAlchemist::getPlaceName() const {
    return "LABORATORIO DE ALQUIMIA";
}

Color NPCAlchemist::getHeaderColor() const {
    return Color::GREEN_DARK;
}

Color NPCAlchemist::getArtColor() const {
    return Color::GREEN_DARK;
}

const std::vector<std::string>& NPCAlchemist::getArtASCII() const {
    return NPCAlchemistLayouts::artAlchemist;
}

void NPCAlchemist::displayDialogue(Character* player) {
    std::vector<std::string> lines = {
        "Seja bem-vindo ao laboratorio de transmutacao!",
        "Eu sou Quintus, o Alquimista Real. Se voce me trouxer ingredientes de monstros",
        "e alimentos terrestres, posso transmutar elixires poderosos!",
        "Minhas criacoes podem curar sua alma ou devastar as defesas inimigas."
    };
    Appearance::displayPopup("QUINTUS - ALQUIMISTA REAL", lines, Color::GREEN_DARK, getArtASCII());
}

std::vector<std::string> NPCAlchemist::getOptionsMenu(Character* player, int /*larguraDoTerminal*/) {
    return {
        "Pocao de Cura Grande (50%VM) [1x Maca + 1x Po magico]",
        "Pocao de Forca Alquimica [1x Pao + 1x Dente de goblin]",
        "Pocao de Veneno Alquimica [1x Carne Seca + 1x Gosma acida]",
        "Pocao de Lentidao Alquimica [1x Queijo + 1x Nucleo pegajoso]",
        "Voltar"
    };
}

void NPCAlchemist::processOption(Character* player, const std::string& option, int /*larguraDoTerminal*/) {
    std::string foodReq = "";
    std::string requiredDrop = "";
    ItemID productId = ItemID::None;

    if (option.find("Cura Grande") != std::string::npos) {
        foodReq = "Maca";
        requiredDrop = "Po magico";
        productId = ItemID::LittleCureBig;
    }
    else if (option.find("Forca Alquimica") != std::string::npos) {
        foodReq = "Pao";
        requiredDrop = "Dente de goblin";
        productId = ItemID::LittleGallowsAlchemy;
    }
    else if (option.find("Veneno Alquimica") != std::string::npos) {
        foodReq = "Carne Seca";
        requiredDrop = "Gosma acida";
        productId = ItemID::LittlePoisonAlchemy;
    }
    else if (option.find("Lentidao Alquimica") != std::string::npos) {
        foodReq = "Queijo";
        requiredDrop = "Nucleo pegajoso";
        productId = ItemID::LittleSlowAlchemy;
    }

    if (productId != ItemID::None) {
        auto* backpack = player->getInventory();
        int foodQuantity = backpack->countItem(foodReq);
        int dropQuantity = backpack->countItem(requiredDrop);

        if (foodQuantity >= 1 && dropQuantity >= 1) {
            backpack->removeItem(foodReq);
            backpack->removeItem(requiredDrop);

            auto newItem = ItemFactory::createItem(productId);
            if (newItem) {
                std::string productName = newItem->getItemName();
                backpack->addItem(std::move(newItem));

                std::vector<std::string> msgSuccess = {
                    "Mistura fervilhando... Vapor borbulhando...",
                    "Sucesso! VocÃª obteve: " + productName
                };
                Appearance::displayPopup("TRANSMUTACAO COMPLETA", msgSuccess, Color::GREEN_DARK, getArtASCII());
            }
        } else {
            std::vector<std::string> msgError = {
                "Ingredientes insuficientes!",
                "VocÃª precisa de:",
                " -> 1x " + foodReq + " (Possui: " + std::to_string(foodQuantity) + ")",
                " -> 1x " + requiredDrop + " (Possui: " + std::to_string(dropQuantity) + ")"
            };
            Appearance::displayPopup("FALHA NA TRANSMUTACAO", msgError, Color::RED, getArtASCII());
        }
    }
}

