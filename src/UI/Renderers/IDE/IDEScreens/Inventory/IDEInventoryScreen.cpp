#include "UI/Renderers/IDE/IDEScreens/Inventory/IDEInventoryScreen.h"
#include "UI/Renderers/IDE/IDETheme.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Domain/Characters/Character.h"
#include "Domain/Items/Item.h"
#include <iostream>
#include <iomanip>
#include <vector>

void IDEInventoryScreen::displayHeader(bool /*animate*/, int /*startY*/) {
    int width = Appearance::getTerminalWidth();
    std::vector<std::string> tabs = {
        "InventoryHeap.hpp",
        "HeapLayout.sys"
    };
    std::cout << IDETheme::renderTabBar(tabs, 0, width) << "\n";
    std::cout << IDETheme::comment("// src/Systems/Inventory/InventoryHeap.hpp > std::vector<std::unique_ptr<Item>> heap") << "\n\n";
}

void IDEInventoryScreen::displayBoxEquipped(Character* player) {
    if (!player) return;
    int width = Appearance::getTerminalWidth();

    Item* weapon = player->getWeapons();
    Item* shield = player->getShield();
    Item* armor = player->getArmor();
    Item* quick = player->getConsumableQuickly();

    std::vector<std::string> lines;
    lines.push_back(IDETheme::comment("// Referências ativas de hardware alocadas nos slots:"));
    lines.push_back(IDETheme::keyword("struct ") + IDETheme::type("EquippedHardware") + IDETheme::punctuation(" {"));
    
    std::string wName = weapon ? weapon->getItemName() : "nullptr";
    lines.push_back("    " + IDETheme::type("Weapon*") + " " + IDETheme::variable("eqWeapon") + " = " 
                  + (weapon ? ("&heap[" + IDETheme::stringLiteral(wName) + "]") : IDETheme::keyword("nullptr")) + ";");

    std::string sName = shield ? shield->getItemName() : "nullptr";
    lines.push_back("    " + IDETheme::type("Shield*") + " " + IDETheme::variable("eqShield") + " = " 
                  + (shield ? ("&heap[" + IDETheme::stringLiteral(sName) + "]") : IDETheme::keyword("nullptr")) + ";");

    std::string aName = armor ? armor->getItemName() : "nullptr";
    lines.push_back("    " + IDETheme::type("Armor* ") + " " + IDETheme::variable("eqArmor")  + " = " 
                  + (armor ? ("&heap[" + IDETheme::stringLiteral(aName) + "]") : IDETheme::keyword("nullptr")) + ";");

    std::string qName = quick ? quick->getItemName() : "nullptr";
    lines.push_back("    " + IDETheme::type("Item*  ") + " " + IDETheme::variable("eqQuick")  + " = " 
                  + (quick ? ("&heap[" + IDETheme::stringLiteral(qName) + "]") : IDETheme::keyword("nullptr")) + ";");

    lines.push_back(IDETheme::punctuation("} slots;"));

    auto centered = IDETheme::centerBlock(lines, width);
    for (const auto& l : centered) std::cout << l << "\n";
    std::cout << "\n";
}

void IDEInventoryScreen::displayDetailItem(Item* item) {
    if (!item) return;
    int width = Appearance::getTerminalWidth();
    int height = Appearance::getTerminalHeight();

    std::vector<std::string> lines;
    lines.push_back(IDETheme::preprocessor("#pragma once"));
    lines.push_back(IDETheme::keyword("namespace ") + IDETheme::type("Domain::Items") + IDETheme::punctuation(" {"));
    lines.push_back("");
    lines.push_back(IDETheme::keyword("class ") + IDETheme::type("ItemInstance") + IDETheme::punctuation(" final : public ") + IDETheme::type("Item") + IDETheme::punctuation(" {"));
    lines.push_back(IDETheme::keyword("public:"));
    lines.push_back("    " + IDETheme::type("const char*") + " " + IDETheme::variable("name") + "     = " + IDETheme::stringLiteral(item->getItemName()) + ";");
    lines.push_back("    " + IDETheme::type("uint32_t") + "    " + IDETheme::variable("priceSale") + "= " + IDETheme::number(item->getPriceSale()) + ";");
    lines.push_back("    " + IDETheme::type("bool") + "        " + IDETheme::variable("equipable") + "= " + (item->isEquipable() ? "true" : "false") + ";");
    lines.push_back("");
    lines.push_back("    " + IDETheme::keyword("void ") + IDETheme::function("onUse") + IDETheme::punctuation("(") + IDETheme::type("Character*") + " " + IDETheme::variable("target") + IDETheme::punctuation(") override;"));
    lines.push_back(IDETheme::punctuation("};"));
    lines.push_back("");
    lines.push_back(IDETheme::punctuation("} // namespace Domain::Items"));

    std::vector<std::string> tabs = { "ItemInspector.hpp", "HeapAllocation.sys" };
    auto editorView = IDETheme::renderEditorView(tabs, 0, "// src/Domain/Items/" + item->getItemName() + ".hpp", lines, width, height, "[ENTER] Retornar");

    Appearance::clearScreen();
    for (const auto& l : editorView) std::cout << l << "\n";
    std::cout << "\033[J" << std::flush;
    InputControl::waitForEnter();
}

void IDEInventoryScreen::renderMenu(const std::vector<std::string>& lines, const std::string& title, int selectionCurrent, int& outW, int& outH) {
    int width = Appearance::getTerminalWidth();
    outH = static_cast<int>(lines.size());
    outW = 40;

    std::vector<std::string> menuLines;
    menuLines.push_back(IDETheme::comment("// Vector da heap: " + title));

    for (size_t i = 0; i < lines.size(); ++i) {
        std::string hexIndex = "[0x" + (i < 16 ? std::string("0") : "") + std::to_string(i) + "] ";
        if (static_cast<int>(i) == selectionCurrent) {
            menuLines.push_back(std::string(IDETheme::COLOR_ACTIVE_TAB) + "> heap" + hexIndex + " = " + lines[i] + " <" + std::string(IDETheme::COLOR_RESET));
        } else {
            menuLines.push_back("  " + IDETheme::punctuation("heap") + hexIndex + " = " + lines[i]);
        }
    }

    auto centered = IDETheme::centerBlock(menuLines, width);
    for (const auto& l : centered) std::cout << l << "\n";
    std::cout << "\033[J\n";
}
