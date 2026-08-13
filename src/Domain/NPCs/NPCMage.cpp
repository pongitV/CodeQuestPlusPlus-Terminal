#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <map>

#include "Domain/NPCs/NPCMage.h"
#include "UI/Screens/Menu/MenuScreen.h"
#include "Domain/Items/Item.h"
#include "Domain/Items/ItemFactory.h"
#include "UI/Screens/Inventory/InventoryScreen.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Domain/Items/Equipment/WeaponEquipment.h"
#include "Core/Engine/Shop.h"
#include "Core/Utils/DialogFunctions.h"
#include "Systems/Progression/Diary.h"
#include "Systems/Progression/Progression.h"
#include "Systems/Progression/ProgressionFlags.h"
#include "UI/Screens/BaseScreen.h"
#include "Domain/NPCs/NPCMageLayout.h"

namespace {
    std::map<int, ShopProduct> buffPotionsStock = {
        {1, {ItemID::FuryPotion, 25, -1}},
        {2, {ItemID::ElixirArcane, 25, -1}}
    };

    std::map<int, ShopProduct> debuffPotionsStock = {
        {1, {ItemID::BottleSlime, 30, -1}},
        {2, {ItemID::BottleWeakness, 30, -1}}
    };

    struct CharmOperation {
        std::string nameMenu;
        ItemID materialId;
        int qty;
        ItemID weaponRestrictedId; 
        std::function<bool(EquipmentWeapon*)> checkConflict;
        std::string msgConflict;
        std::function<std::string(Character*, EquipmentWeapon*)> apply;
    };

    const std::vector<CharmOperation> enchantmentOperations = {
        { "Sangramento (40x Dente de Goblin)", ItemID::ToothGoblin, 40, ItemID::None, 
          [](EquipmentWeapon* a){ return a->ownsEffectBleeding(); }, "Esta arma ja esta encantada com Sangramento!",
          [](Character*, EquipmentWeapon* a){ a->applyEffectBleeding(); a->changeName(a->getItemName() + " (Sangrenta)"); return a->getItemName(); } },
          
        { "Lentidao (5x Nucleo pegajoso)", ItemID::NucleusSticky, 5, ItemID::None,
          [](EquipmentWeapon* a){ return a->ownsEffectSlow(); }, "Esta arma ja esta encantada com Lentidao!",
          [](Character*, EquipmentWeapon* a){ a->applyEffectSlow(); a->changeName(a->getItemName() + " (Viscosa)"); return a->getItemName(); } },
          
        { "Quebra de Resistencia (25x Po magico)", ItemID::MagicPowder, 25, ItemID::None,
          [](EquipmentWeapon* a){ return a->hasProperty(Property::Penetrating); }, "Esta arma ja esta encantada com Reducao de Resistencia!",
          [](Character*, EquipmentWeapon* a){ a->changeName(a->getItemName() + " (Quebra-Defesas)"); a->addProperty(Property::Penetrating); return a->getItemName(); } },
          
        { "Arco recurvo de madeira: Magia (1x Madeira enfeiticada)", ItemID::WoodBewitched, 1, ItemID::BowWood,
          [](EquipmentWeapon* a){ return a->hasProperty(Property::Magic); }, "Esta arma ja esta encantada com Magia!",
          [](Character* currentPlayer, EquipmentWeapon* weaponChosen) {
              std::string nameBow = ItemFactory::getNameFromID(ItemID::BowWood);
              std::string name = weaponChosen->getItemName();
              size_t post = name.find(nameBow);
              if (post != std::string::npos) name.replace(post, 23, "Arco recurvo de madeira enfeiticada");
              int newDamageMagician = weaponChosen->getMagicalDamage() + (weaponChosen->getPhysicsDamage() / 2);
              auto newBowObj = std::make_unique<EquipmentWeapon>(name, weaponChosen->getPhysicsDamage(), newDamageMagician, weaponChosen->getReqStrength(), weaponChosen->getReqDexterity(), weaponChosen->getReqIntelligence(), weaponChosen->getReqWisdom(), 0);
              EquipmentWeapon* newBow = newBowObj.get();
              if (weaponChosen->ownsEffectBleeding()) newBow->applyEffectBleeding();
              if (weaponChosen->ownsEffectSlow()) newBow->applyEffectSlow();
              if (weaponChosen->hasProperty(Property::Penetrating)) newBow->addProperty(Property::Penetrating);
              newBow->addProperty(Property::Magic);

              bool wasEquipped = (currentPlayer->getWeapons() == weaponChosen);
              if (wasEquipped) currentPlayer->unequipWeapon();
              currentPlayer->getInventory()->removeItem(weaponChosen);
              currentPlayer->getInventory()->addItem(std::move(newBowObj));
              if (wasEquipped) currentPlayer->equipItem(newBow);
              return newBow->getItemName();
          } },
          
        { "Cajado de cristal magico: Cipos (1x Coracao da floresta)", ItemID::HeartForest, 1, ItemID::StaffCrystal,
          [](EquipmentWeapon* a){ return a->hasProperty(Property::VinePrison); }, "Esta arma ja esta encantada com Cipos!",
          [](Character*, EquipmentWeapon* a){
              std::string nameStaff = ItemFactory::getNameFromID(ItemID::StaffCrystal);
              std::string name = a->getItemName();
              size_t post = name.find(nameStaff);
              if (post != std::string::npos) name.replace(post, 24, "Cajado de cipos");
              a->changeName(name);
              a->addProperty(Property::VinePrison);
              return a->getItemName();
          } },
          
        { "Violao encantado: Raizes (1x Madeira enfeiticada)", ItemID::WoodBewitched, 1, ItemID::ViolaEnchanted,
          [](EquipmentWeapon* a){ return a->hasProperty(Property::ViolaMagician); }, "Esta arma ja esta encantada com Raizes!",
          [](Character*, EquipmentWeapon* a){
              std::string nameViola = ItemFactory::getNameFromID(ItemID::ViolaEnchanted);
              std::string name = a->getItemName();
              size_t post = name.find(nameViola);
              if (post != std::string::npos) name.replace(post, 16, "Violao enfeiticado");
              else name += " enfeiticado";
              a->changeName(name);
              a->addProperty(Property::ViolaMagician);
              return a->getItemName();
          } }
    };

    // --- APARENCIA E DIALOGOS ---
    void processEnchantments(Character* currentPlayer, bool isUniversal);
    void processPotions(Character* currentPlayer, bool isBuff);
    void processMissionLabyrinth(Character* currentPlayer);
    void processMenuMissions(Character* currentPlayer);

    void dialogueMorgana(const std::vector<std::string>& lines) {
        Appearance::displayPopup("MORGANA", lines, Color::MAGENTA, NPCMageLayouts::artMage);
    }
    
    void dialogueMorganaUnique(const std::string& msg) {
        dialogueMorgana({msg});
    }
}

// --- INFORMACOES DO LUGAR ---
std::string NPCMage::getPlaceName() const {
    return "CABANA DA BRUXA";
}

Color NPCMage::getHeaderColor() const {
    return Color::MAGENTA;
}

Color NPCMage::getArtColor() const {
    return Color::MAGENTA;
}

const std::vector<std::string>& NPCMage::getArtASCII() const {
    return NPCMageLayouts::artMage;
}

// --- INTERACAO E MENU ---
void NPCMage::interact(Character* player) {
    InputControl::executeLoopMenuPopup(
        [this, player]() { this->displayDialogue(player); },
        [this, player]() { return this->getOptionsMenu(player, Appearance::getTerminalWidth()); },
        [this, player](const std::string& op) { this->processOption(player, op, Appearance::getTerminalWidth()); return true; },
        getPlaceName(), getHeaderColor(), getArtASCII()
    );
}

void NPCMage::displayDialogue(Character* /*player*/) {
    if (Progression::instance().getFlag(Flags::Forest_MorganaQuest)) {
        dialogueMorgana(std::vector<std::string>{
            "O Labirinto o aguarda..."
        });
    } else {
        dialogueMorgana(std::vector<std::string>{
            "Hmmm... sinto cheiro de poder no ar.",
            "O que voce busca, viajante?"
        });
    }
}

std::vector<std::string> NPCMage::getOptionsMenu(Character* /*player*/, int /*terminalWidth*/) {
    return {
        "ENCANTAR Armas (Universais)",
        "ENCANTAR Armas (Especificas)",
        "COMPRAR Pocoes de Buff",
        "COMPRAR Frascos de Debuff",
        "Missoes de Morgana",
        "VOLTAR"
    };
}

void NPCMage::processOption(Character* player, const std::string& option, int /*terminalWidth*/) {
    if (option == "ENCANTAR Armas (Universais)") {
        processEnchantments(player, true);
    }
    else if (option == "ENCANTAR Armas (Especificas)") {
        processEnchantments(player, false);
    }
    else if (option == "COMPRAR Pocoes de Buff" || option == "COMPRAR Frascos de Debuff") {
        processPotions(player, option == "COMPRAR Pocoes de Buff");
    }
    else if (option == "Missoes de Morgana") {
        processMenuMissions(player);
    }
}

namespace {
    // --- PROCESSAMENTO DE OPCOES ---
    void processEnchantments(Character* currentPlayer, bool isUniversal) {
        std::vector<const CharmOperation*> oopsCurrent;
        int home = isUniversal ? 0 : 3;
        int end = isUniversal ? 3 : 6;
        for (int i = home; i < end; ++i) {
            oopsCurrent.push_back(&enchantmentOperations[i]);
        }

        Appearance::sortAlphabetically(oopsCurrent, [](const CharmOperation* op) { return op->nameMenu; });

        while (true) {
            std::vector<std::string> lines;
            for (auto* op : oopsCurrent) lines.push_back(op->nameMenu);
            lines.push_back("VOLTAR");

            int id = InputControl::readMenuSelectionInPopup(
                isUniversal ? "CABANA - ENCANTOS UNIVERSAIS" : "CABANA - ENCANTOS ESPECIFICOS",
                {"Escolha um encantamento:"},
                lines,
                Color::MAGENTA,
                NPCMageLayouts::artMage
            );

            if (id == static_cast<int>(oopsCurrent.size()) || id == -1) {
                break;
            }

            const auto& op = *oopsCurrent[id];
            
            std::string itemNecessary = ItemFactory::getNameFromID(op.materialId);
            int qtyCurrent = currentPlayer->getInventory()->countItem(itemNecessary);
            if (qtyCurrent < op.qty) {
                dialogueMorganaUnique("Voce nao tem " + itemNecessary + " suficiente! (Possui: " + std::to_string(qtyCurrent) + "/" + std::to_string(op.qty) + ")");
                continue;
            }
            
            std::vector<Item*> itemsValid;
            std::vector<std::string> optionsItem;
            for (auto* item : currentPlayer->getInventory()->getAllItems()) {
                if (item->getType() == EquipmentType::WEAPONS) {
                    itemsValid.push_back(item);
                    optionsItem.push_back(item->getItemName());
                }
            }
            if (optionsItem.empty()) { dialogueMorganaUnique("Voce nao tem armas para encantar!"); continue; }
            optionsItem.push_back("VOLTAR");
            
            int chooseWeapon = InputControl::readMenuSelectionInPopup("ESCOLHA UMA ARMA", {"Qual arma deseja encantar?"}, optionsItem, Color::MAGENTA, NPCMageLayouts::artBoiler);
            if (chooseWeapon == -1 || chooseWeapon == static_cast<int>(optionsItem.size()) - 1) continue;
            
            EquipmentWeapon* weaponChosen = dynamic_cast<EquipmentWeapon*>(itemsValid[chooseWeapon]);
            
            if (op.weaponRestrictedId != ItemID::None) {
                std::string nameRestricted = ItemFactory::getNameFromID(op.weaponRestrictedId);
                if (weaponChosen->getItemName().find(nameRestricted) == std::string::npos) {
                    dialogueMorganaUnique("Este encantamento so funciona no " + nameRestricted + "!");
                    continue;
                }
            }
            
            if (op.checkConflict(weaponChosen)) {
                dialogueMorganaUnique(op.msgConflict);
                continue;
            }
            
            std::string nameAncientWeapon = weaponChosen->getItemName();
            for (int i = 0; i < op.qty; ++i) currentPlayer->getInventory()->removeItem(itemNecessary);
            
            std::string newName = op.apply(currentPlayer, weaponChosen);
            
            std::string equation = "[" + nameAncientWeapon + "] + " + std::to_string(op.qty) + "x [" + itemNecessary + "] = [" + newName + "]";
            Appearance::displayPopup("ENCANTAMENTO SUCESSO", {equation, "", "Feito! A magia flui pela sua arma..."}, Color::MAGENTA, NPCMageLayouts::artBoiler);
        }
    }

    void processPotions(Character* currentPlayer, bool isBuff) {
        std::string title = isBuff ? "CABANA - POCOES DE BUFF" : "CABANA - FRASCOS DE DEBUFF";
        auto& currentStock = isBuff ? buffPotionsStock : debuffPotionsStock;
        
        Shop::processPurchase(currentPlayer, title, Color::MAGENTA, currentStock, 
            [](const std::string& msg) { dialogueMorganaUnique(msg); }, InteractionNPC::getFormatterStatusItem);
    }

    void processMissionLabyrinth(Character* currentPlayer) {
        std::string nameHeart = ItemFactory::getNameFromID(ItemID::HeartForest);
        int qtyHearts = currentPlayer->getInventory()->countItem(nameHeart);

        if (qtyHearts < 3) {
            dialogueMorgana({
                "Voce ainda nao possui os 3 Coracoes da floresta que eu pedi. (Possui: " + std::to_string(qtyHearts) + "/3)",
                "Eles sao dropados por Abominacoes no Coracao da Arvore."
            });
            return;
        }

        for (int i = 0; i < 3; ++i) currentPlayer->getInventory()->removeItem(nameHeart);
        currentPlayer->unlockMaze();
        Diary::instance().registerMissionCompleted("morgana_coracoes");
        Progression::instance().setFlag(Flags::Forest_MorganaQuest, true);
        
        std::vector<std::string> dialogue = {
            "Ah, perfeitos! Estes coracoes pulsam com uma magia ancestral.",
            "Como recompensa, revelarei um segredo... Atras de mim, ha uma passagem secreta.",
            "Use a entrada [^L] para explorar o meu Labirinto Subterraneo.",
            "E um lugar perigoso, mergulhado em uma nevoa de cor roxa, mas guarda grandes tesouros."
        };
        Appearance::displayPopup("MISSAO CONCLUIDA", dialogue, Color::MAGENTA, NPCMageLayouts::artMage);
    }

    void processMenuMissions(Character* currentPlayer) {
        Diary::instance().registerMissionAccept("morgana_coracoes");
        while (true) {
            std::vector<std::string> missions;
            if (!currentPlayer->getUnlockedMaze()) {
                std::string nameHeart = ItemFactory::getNameFromID(ItemID::HeartForest);
                int qtyHearts = currentPlayer->getInventory()->countItem(nameHeart);
                if (qtyHearts >= 3) {
                    missions.push_back("[M] Entregar 3x Coracoes da floresta (Pronta)");
                } else {
                    missions.push_back("[M] Consiga 3x Coracoes da floresta");
                }
            } else {
                missions.push_back("(Nenhuma missao disponivel)");
            }
            missions.push_back("VOLTAR");

            int id = InputControl::readMenuSelectionInPopup(
                "MISSOES DE MORGANA",
                {"Escolha uma missao:"},
                missions,
                Color::MAGENTA,
                NPCMageLayouts::artMage
            );

            if (!currentPlayer->getUnlockedMaze() && id == 0) {
                processMissionLabyrinth(currentPlayer);
            } else if (currentPlayer->getUnlockedMaze() && id == 0) {
                dialogueMorganaUnique("Nao busco mais nada de voce no momento...");
            } else if (id == 1 || id == -1) {
                break;
            }
        }
    }
}
