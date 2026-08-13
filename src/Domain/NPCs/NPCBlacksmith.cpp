#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <algorithm>
#include <memory>

#include "Domain/NPCs/NPCBlacksmith.h"
#include "Domain/Items/Item.h"
#include "Domain/Items/ItemFactory.h"
#include "Domain/Items/Equipment/ArmorEquipment.h"
#include "Domain/Items/Equipment/ShieldEquipment.h"
#include "UI/Screens/Inventory/InventoryScreen.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Core/Engine/Shop.h"
#include "Core/Utils/DialogFunctions.h"
#include "Domain/NPCs/NPCBlacksmithLayout.h"

namespace {
    // --- DADOS DO ESTOQUE ---
    std::map<int, ShopProduct> stockWeapons = {
        {1, {ItemID::SwordIron, 40, -1}},
        {2, {ItemID::BowWood, 40, -1}},
        {3, {ItemID::StaffCrystal, 40, -1}},
        {4, {ItemID::ViolaEnchanted, 40, -1}}
    };
    
    std::map<int, ShopProduct> stockArmor = {
        {1, {ItemID::ArmorMesh, 40, -1}},
        {2, {ItemID::ArmorLeather, 40, -1}},
        {3, {ItemID::Tunic, 40, -1}},
        {4, {ItemID::CostumeNoble, 40, -1}}
    };
    
    void processEquipmentPurchase(Character* currentPlayer, bool buyingWeapons);
    void processAnvilImprovement(Character* currentPlayer);
    void processUpgradeByMaterial(Character* currentPlayer);
    void processShieldRepair(Character* currentPlayer);

    // --- APARENCIA E DIALOGOS ---
    void dialogueKiss(const std::vector<std::string>& lines) {
        Appearance::displayPopup("BJORN", lines, Color::CYAN, NPCBlacksmithLayouts::artBlacksmith);
    }
    
    void dialogueKissUnique(const std::string& msg) {
        dialogueKiss({msg});
    }
}

// --- INFORMACOES DO LUGAR ---
std::string NPCBlacksmith::getPlaceName() const {
    return "FORJA DO BJORN";
}

Color NPCBlacksmith::getHeaderColor() const {
    return Color::CYAN;
}

Color NPCBlacksmith::getArtColor() const {
    return Color::CYAN;
}

const std::vector<std::string>& NPCBlacksmith::getArtASCII() const {
    return NPCBlacksmithLayouts::artBlacksmith;
}

// --- INTERACAO E MENU ---
void NPCBlacksmith::interact(Character* player) {
    InputControl::executeLoopMenuPopup(
        [this, player]() { this->displayDialogue(player); },
        [this, player]() { return this->getOptionsMenu(player, Appearance::getTerminalWidth()); },
        [this, player](const std::string& op) { this->processOption(player, op, Appearance::getTerminalWidth()); return true; },
        getPlaceName(), getHeaderColor(), getArtASCII()
    );
}

void NPCBlacksmith::displayDialogue(Character* /*player*/) {
    dialogueKiss(std::vector<std::string>{
        "Bem-vindo a minha forja, salvador!",
        "O que vai ser hoje?"
    });
}

std::vector<std::string> NPCBlacksmith::getOptionsMenu(Character* /*player*/, int /*terminalWidth*/) {
    return {
        "COMPRAR Armas das Classes",
        "COMPRAR Armaduras das Classes",
        "MELHORAR POR FUSAO",
        "MELHORAR POR MATERIAL",
        "Missoes de Bjorn",
        "VOLTAR"
    };
}

void NPCBlacksmith::processOption(Character* player, const std::string& option, int /*terminalWidth*/) {
    if (option == "COMPRAR Armas das Classes" || option == "COMPRAR Armaduras das Classes") {
        processEquipmentPurchase(player, option == "COMPRAR Armas das Classes");
    } else if (option == "MELHORAR POR FUSAO") {
        processAnvilImprovement(player);
    } else if (option == "MELHORAR POR MATERIAL") {
        processUpgradeByMaterial(player);
    } else if (option == "CONSERTAR Escudo") {
        processShieldRepair(player);
    } else if (option == "Missoes de Bjorn") {
        InteractionNPC::processMenuMissionsEmpty(player, "MISSOES DE BJORN", Color::CYAN, "Bjorn", "Nao tenho nenhum trabalho especial para voce no momento.");
    }
}

namespace {
    // --- PROCESSAMENTO DE OPCOES ---
    void processEquipmentPurchase(Character* currentPlayer, bool buyingWeapons) {
        auto& currentStock = buyingWeapons ? stockWeapons : stockArmor;
        std::string shopTitle = buyingWeapons ? "FORJA - ARMAS" : "FORJA - ARMADURAS";

        Shop::processPurchase(currentPlayer, shopTitle, Color::CYAN, currentStock, 
            [](const std::string& msg) { dialogueKissUnique(msg); }, InteractionNPC::getFormatterStatusItem, NPCBlacksmithLayouts::artBlacksmith);
    }

    void processAnvilImprovement(Character* currentPlayer) {
        do {
            std::vector<Item*> itemsValid;
            std::vector<std::string> optionsItem;
            for (auto* item : currentPlayer->getInventory()->getAllItems()) {
                EquipmentType type = item->getType();
                if ((type == EquipmentType::WEAPONS || type == EquipmentType::SHIELD || type == EquipmentType::ARMOR) && !item->hasProperty(Property::Improved)) {
                    itemsValid.push_back(item);
                    optionsItem.push_back(item->getItemName());
                }
            }
            if (optionsItem.empty()) { dialogueKissUnique("Voce nao tem nenhum equipamento que eu possa melhorar!"); break; }
            optionsItem.push_back("VOLTAR");
            
            int choice = InputControl::readMenuSelectionInPopup("FUSAO DE EQUIPAMENTO", {"Qual item deseja fundir? (Requer copia no inventario)"}, optionsItem, Color::CYAN, NPCBlacksmithLayouts::artAnvil);
            if (choice == -1 || choice == static_cast<int>(optionsItem.size()) - 1) break;
            
            Item* itemBase = itemsValid[choice];
            if (!InteractionNPC::checkItemNoEquipped(currentPlayer, itemBase, "Bjorn", Color::CYAN, "Voce precisa DESEQUIPAR o item antes de usa-lo na bigorna!")) continue;

            if (!InteractionNPC::checkMaterialNoInventory(currentPlayer, itemBase->getItemName(), 2, "Bjorn", Color::CYAN)) continue;

            if ((currentPlayer->getWeapons() && currentPlayer->getWeapons()->getItemName() == itemBase->getItemName()) ||
                (currentPlayer->getShield() && currentPlayer->getShield()->getItemName() == itemBase->getItemName()) ||
                (currentPlayer->getArmor() && currentPlayer->getArmor()->getItemName() == itemBase->getItemName())) {
                dialogueKissUnique("Voce possui uma copia deste item equipada! DESEQUIPE antes de fundir."); continue;
            }

            std::unique_ptr<Item> newItem = itemBase->generateCopyImproved();

             if (newItem) {
                std::string nameAncient = itemBase->getItemName();
                std::string newName = newItem->getItemName();
                currentPlayer->getInventory()->removeItem(itemBase);
                currentPlayer->getInventory()->removeItem(nameAncient);
                currentPlayer->getInventory()->addItem(std::move(newItem));

                std::string equation = "[" + nameAncient + "] + [" + nameAncient + "] = [" + newName + "]";
            Appearance::displayPopup("FORJA - SUCESSO", {equation, "", "Ha! Trabalho feito! Seu equipamento esta mais forte do que nunca!"}, Color::CYAN, NPCBlacksmithLayouts::artAnvil);
            }
        } while (true);
    }

    void processUpgradeByMaterial(Character* currentPlayer) {
        std::string nameStoneUpgrade = ItemFactory::getNameFromID(ItemID::StoneUpgrade);
        do {
            if (!InteractionNPC::checkMaterialNoInventory(currentPlayer, nameStoneUpgrade, 1, "Bjorn", Color::CYAN)) {
                return;
            }
            
            std::vector<Item*> itemsValid;
            std::vector<std::string> optionsItem;
            for (auto* item : currentPlayer->getInventory()->getAllItems()) {
                if (item->getType() == EquipmentType::ARMOR && !item->hasProperty(Property::ImprovedMaterial)) {
                    itemsValid.push_back(item);
                    optionsItem.push_back(item->getItemName());
                }
            }
            if (optionsItem.empty()) { dialogueKissUnique("Voce nao tem armaduras validas para imbuir!"); break; }
            optionsItem.push_back("VOLTAR");
            
            int choice = InputControl::readMenuSelectionInPopup("IMBUIR ARMADURA", {"Qual armadura imbuir com a Pedra? (+3 Defesa)"}, optionsItem, Color::CYAN, NPCBlacksmithLayouts::artAnvil);
            if (choice == -1 || choice == static_cast<int>(optionsItem.size()) - 1) break;

            Item* itemForUpgrade = itemsValid[choice];
            if (!InteractionNPC::checkItemNoEquipped(currentPlayer, itemForUpgrade, "Bjorn", Color::CYAN, "Voce precisa DESEQUIPAR o item antes de usa-lo na bigorna!")) continue;

            EquipmentArmor* armor = dynamic_cast<EquipmentArmor*>(itemForUpgrade);
            if (!armor) continue;

            if (armor->hasProperty(Property::ImprovedMaterial)) {
                dialogueKissUnique("Esta armadura ja foi imbuida com a pedra magica!");
                continue;
            }

            std::string nameAncient = armor->getItemName();
            std::string newName = nameAncient + " (Imbuida)";

            auto newArmor = std::make_unique<EquipmentArmor>(
                newName, 
                armor->getReductionFixed() + 3, 
                armor->getReqResistance(), 
                armor->getReqConstitution(), 
                armor->getPriceSale() + 200
            );

            for (Property prop : armor->getProperties()) newArmor->addProperty(prop);
            newArmor->addProperty(Property::ImprovedMaterial);

            currentPlayer->getInventory()->removeItem(nameStoneUpgrade);
            currentPlayer->getInventory()->removeItem(armor);
            currentPlayer->getInventory()->addItem(std::move(newArmor));

            std::string equation = "[" + nameAncient + "] + [Pedra magica] = [" + newName + "]";
            Appearance::displayPopup("FORJA - SUCESSO", {equation, "", "Impressionante! A armadura agora possui +3 de defesa!"}, Color::CYAN, NPCBlacksmithLayouts::artAnvil);
        } while (true);
    }

    void processShieldRepair(Character* currentPlayer) {
        do {
            std::vector<EquipmentShield*> shieldsDamaged;
            std::vector<std::string> optionsShield;

            for (auto* item : currentPlayer->getInventory()->getAllItems()) {
                EquipmentShield* shield = dynamic_cast<EquipmentShield*>(item);
                if (shield && shield->getDurabilityCurrentShield() < shield->getDurabilityMaximum()) {
                    shieldsDamaged.push_back(shield);
                    int cost = (shield->getDurabilityMaximum() - shield->getDurabilityCurrentShield()) * 5;
                    optionsShield.push_back(shield->getItemName() + " (" + std::to_string(shield->getDurabilityCurrentShield()) + "/" + std::to_string(shield->getDurabilityMaximum()) + ") - " + std::to_string(cost) + "g");
                }
            }

            if (shieldsDamaged.empty()) {
                dialogueKissUnique("Voce nao tem nenhum escudo danificado que eu possa consertar!");
                break;
            }
            
            optionsShield.push_back("VOLTAR");
            
            int choice = InputControl::readMenuSelectionInPopup("CONSERTAR ESCUDO", {"Qual escudo deseja consertar? (5g por ponto perdido)"}, optionsShield, Color::CYAN, NPCBlacksmithLayouts::artAnvil);
            if (choice == -1 || choice == static_cast<int>(optionsShield.size()) - 1) break;

            EquipmentShield* shieldForFix = shieldsDamaged[choice];
            if (!InteractionNPC::checkItemNoEquipped(currentPlayer, shieldForFix, "Bjorn", Color::CYAN, "Voce precisa DESEQUIPAR o escudo antes de conserta-lo!")) continue;

            int durabilityLost = shieldForFix->getDurabilityMaximum() - shieldForFix->getDurabilityCurrentShield();
            int costRepair = durabilityLost * 5; // Exemplo: 5 de ouro por ponto de durabilidade perdida

            if (currentPlayer->getInventory()->getGold() >= costRepair) {
                currentPlayer->getInventory()->addGold(-costRepair);
                shieldForFix->setDurability(shieldForFix->getDurabilityMaximum());
                dialogueKissUnique("Hmph! Seu escudo esta como novo! (-" + std::to_string(costRepair) + "g)");
            } else {
                dialogueKissUnique("Voce nao tem ouro suficiente para consertar este escudo. Eu preciso de " + std::to_string(costRepair) + "g.");
            }
        } while (true);
    }
}
