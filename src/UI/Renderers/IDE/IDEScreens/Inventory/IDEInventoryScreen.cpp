#include "UI/Renderers/IDE/IDEScreens/Inventory/IDEInventoryScreen.h"
#include <iostream>
#include <vector>
#include <fstream>
#include "UI/Renderers/IDE/IDETheme.h"
#include "UI/Screens/BaseScreen.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "UI/Screens/Inventory/InventoryScreenLayout.h"
#include "Domain/Items/Item.h"

void ScreenInventoryGO::displayHeaderInventory(bool animate) {
    Appearance::displayArtPanel(ArtsInventory::soonInventory, 121, Color::YELLOW, "", animate);
}

void ScreenInventoryGO::displayBoxEquipped(Character* currentPlayer) {
    Item* weaponEquipped = currentPlayer->getWeapons();
    Item* shieldEquipped = currentPlayer->getShield();
    Item* armorEquipped = currentPlayer->getArmor();
    Item* consumableQuickly = currentPlayer->getConsumableQuickly();

    std::vector<std::string> lines;
    lines.push_back("Arma     : " + (weaponEquipped ? weaponEquipped->getItemName() + weaponEquipped->getInfoStatus() : "Nenhuma"));
    lines.push_back("Escudo   : " + (shieldEquipped ? shieldEquipped->getItemName() + shieldEquipped->getInfoStatus() : "Nenhum"));
    lines.push_back("Armadura : " + (armorEquipped ? armorEquipped->getItemName() + armorEquipped->getInfoStatus() : "Nenhuma"));
    lines.push_back("Cura Rap.: " + (consumableQuickly ? consumableQuickly->getItemName() + " (" + std::to_string(currentPlayer->getInventory()->countItem(consumableQuickly->getItemName())) + "x)" : "Vazio"));

    Appearance::printBlockCentralized(BaseScreen::createBox(lines, "EQUIPAMENTOS ATUAIS", 40, Color::YELLOW));
}

void ScreenInventoryGO::displayInspectionItem(Item* item, Character* currentPlayer) {
    if (!item) return;

    std::vector<std::string> lines;
    lines.push_back(Appearance::color(Color::YELLOW) + " ═══ " + item->getItemName() + " ═══" + Appearance::color(Color::RESET));

    std::vector<std::string> details = item->getDetailsInspection(currentPlayer);
    lines.insert(lines.end(), details.begin(), details.end());

    lines.push_back(" > Preco de Venda: " + std::to_string(item->getPriceSale()) + "G");

    std::cout << "\n";
    Appearance::printCentralized(lines[0]);
    std::cout << "\n";

    std::vector<std::string> rest(lines.begin() + 1, lines.end());
    Appearance::printBlockCentralized(rest);
}
