#include "Domain/Items/Items/ConsumableItem.h"
#include "Core/Engine/Status.h"
#include "Domain/Characters/Character.h"
#include "Core/Utils/Appearance.h"
#include "UI/Screens/Combat/CombatScreen.h"
#include "UI/PerspectiveManager.h"
#include "Core/Terminal/InputOutputControl/InputControl.h"
#include <functional>
#include <unordered_map>
#include "Domain/Items/ItemFactory.h"

static void showWarningConsumable(const std::string& msg, Color colorMsg = Color::WHITE) {
    if (PerspectiveManager::getInstance().is3DViewActive()) {
        std::vector<std::string> msgBroken = { msg };
        InputControl::readMenuSelectionInPopup(" SISTEMA ", msgBroken, {"[<] VOLTAR"}, colorMsg);
    } else {
        std::cout << "\n" << CombatScreen::combatMargin() << Appearance::color(colorMsg) << "[SISTEMA]: " << msg << Appearance::color(Color::RESET) << "\n";
    }
}

ConsumableItem::ConsumableItem(const std::string& name, int price) : Item(price), name(name)
{
}

std::string ConsumableItem::getItemName() const { return name; }

EquipmentType ConsumableItem::getType() const { return EquipmentType::CONSUMABLE; }

std::vector<std::string> ConsumableItem::getDetailsInspection(Character* /*personagem*/) const {
    std::vector<std::string> details;
    details.push_back(" > Tipo: Consumivel");
    if (!descriptionInspection.empty()) {
        for (const auto& desc : descriptionInspection) details.push_back(" > Efeitos: " + desc);
    } else {
        details.push_back(" > Efeitos: Pode ser consumido para aplicar efeitos.");
    }
    return details;
}

std::unique_ptr<Item> manufactureItemConsumable(ItemID id) {
    auto createLittleCure = []() {
        auto healing = std::make_unique<ConsumableItem>(ItemFactory::getNameFromID(ItemID::LittleCure30), 6);
        healing->addProperty(Property::HealingConsumable);
        healing->setDescriptionInspection("Restaura 30% da sua Vida Maxima.");
        healing->setActionInventory([](Item* item, Character* user, bool* shiftWasConsumed) {
            if (user->getHealth() >= user->getMaxHealth()) {
                showWarningConsumable("Sua vida ja esta cheia!");
                return true;
            }
            int lifeBefore = user->getHealth();
            int cureEstimated = static_cast<int>(user->getMaxHealth() * 0.30);
            user->modifyHealth(cureEstimated);
            int lifeAfter = user->getHealth();
            int cureReal = lifeAfter - lifeBefore;
            showWarningConsumable(item->getItemName() + " usada! +" + std::to_string(cureReal) + " HP. (Vida atual: " + std::to_string(lifeAfter) + "/" + std::to_string(user->getMaxHealth()) + ")", Color::GREEN);
            Appearance::registerBattleLog(Appearance::color(Color::GREEN) + "[SISTEMA]: " + item->getItemName() + " usada! +" + std::to_string(cureReal) + " HP." + Appearance::color(Color::RESET));
            
            if (user->getConsumableQuickly() == item) {
                user->unequipConsumable();
                std::string nameOfthisItem = item->getItemName();
                for (auto* otherItem : user->getInventory()->getAllItems()) {
                    if (otherItem != item && otherItem->getItemName() == nameOfthisItem) {
                        user->equipItem(otherItem);
                        break;
                    }
                }
            }

            user->getInventory()->removeItem(item);
            if (shiftWasConsumed) *shiftWasConsumed = true;
            return true;
        });
        return healing;
    };

    auto createTalisman = [](ItemID id, Property prop, AttributeType buffAtr, AttributeType debuffAtr, const std::string& desc) {
        auto t = std::make_unique<ConsumableItem>(ItemFactory::getNameFromID(id), 120);
        t->addProperty(prop);
        t->setDescriptionInspection(desc);
        t->setActionInventory([buffAtr, debuffAtr](Item* item, Character* user, bool* shiftWasConsumed) {
            user->changeStaticAttribute(buffAtr, 5);
            user->changeStaticAttribute(debuffAtr, -5);
            showWarningConsumable(item->getItemName() + " consumido!");
            
            if (user->getConsumableQuickly() == item) {
                user->unequipConsumable();
                std::string nameOfthisItem = item->getItemName();
                for (auto* otherItem : user->getInventory()->getAllItems()) {
                    if (otherItem != item && otherItem->getItemName() == nameOfthisItem) {
                        user->equipItem(otherItem);
                        break;
                    }
                }
            }

            user->getInventory()->removeItem(item);
            if (shiftWasConsumed) *shiftWasConsumed = true;
            return true;
        });
        return t;
    };

    auto createBuffAttributes = [](ItemID id) {
        auto buff = std::make_unique<ConsumableItem>(ItemFactory::getNameFromID(id), 3);
        buff->addProperty(Property::BuffConsumable);
        buff->setDescriptionInspection("Aumenta seus atributos em 1.5x por 2 turnos.");
        buff->setActionInventory([](Item* item, Character* user, bool* shiftWasConsumed) {
            if (!shiftWasConsumed) { showWarningConsumable("Pocoes de buff so podem ser usadas em combate!"); return true; }
            user->addEffect(std::make_unique<AttributesBuffEffect>(2));
            user->setMultiplier(1.5);
            showWarningConsumable(item->getItemName() + " consumida! Atributos ampliados em 1.5x por 2 turnos!", Color::LIGHT_GREEN);
            Appearance::registerBattleLog(Appearance::color(Color::LIGHT_GREEN) + "[SISTEMA]: " + item->getItemName() + " consumida! Atributos ampliados em 1.5x por 2 turnos!" + Appearance::color(Color::RESET));
            
            if (user->getConsumableQuickly() == item) {
                user->unequipConsumable();
                std::string nameOfthisItem = item->getItemName();
                for (auto* otherItem : user->getInventory()->getAllItems()) {
                    if (otherItem != item && otherItem->getItemName() == nameOfthisItem) {
                        user->equipItem(otherItem);
                        break;
                    }
                }
            }

            user->getInventory()->removeItem(item);
            *shiftWasConsumed = true;
            return true;
        });
        return buff;
    };

    auto createFood = [](ItemID id, int cureHP, int price, const std::string& desc) {
        auto food = std::make_unique<ConsumableItem>(ItemFactory::getNameFromID(id), price);
        food->addProperty(Property::HealingConsumable);
        food->setDescriptionInspection(desc);
        food->setActionInventory([cureHP](Item* item, Character* user, bool* shiftWasConsumed) {
            if (user->getHealth() >= user->getMaxHealth()) {
                showWarningConsumable("Sua vida ja esta cheia!");
                return true;
            }
            int lifeBefore = user->getHealth();
            user->modifyHealth(cureHP);
            int lifeAfter = user->getHealth();
            int cureReal = lifeAfter - lifeBefore;
            showWarningConsumable(item->getItemName() + " consumido(a)! +" + std::to_string(cureReal) + " HP. (Vida atual: " + std::to_string(lifeAfter) + "/" + std::to_string(user->getMaxHealth()) + ")", Color::GREEN);
            
            if (user->getConsumableQuickly() == item) {
                user->unequipConsumable();
            }
            user->getInventory()->removeItem(item);
            if (shiftWasConsumed) *shiftWasConsumed = true;
            return true;
        });
        return food;
    };

    auto createLittleCureBig = []() {
        auto healing = std::make_unique<ConsumableItem>(ItemFactory::getNameFromID(ItemID::LittleCureBig), 30);
        healing->addProperty(Property::HealingConsumable);
        healing->setDescriptionInspection("Restaura 50% da sua Vida Maxima.");
        healing->setActionInventory([](Item* item, Character* user, bool* shiftWasConsumed) {
            if (user->getHealth() >= user->getMaxHealth()) {
                showWarningConsumable("Sua vida ja esta cheia!");
                return true;
            }
            int lifeBefore = user->getHealth();
            int cureEstimated = static_cast<int>(user->getMaxHealth() * 0.50);
            user->modifyHealth(cureEstimated);
            int lifeAfter = user->getHealth();
            int cureReal = lifeAfter - lifeBefore;
            showWarningConsumable(item->getItemName() + " usada! +" + std::to_string(cureReal) + " HP. (Vida atual: " + std::to_string(lifeAfter) + "/" + std::to_string(user->getMaxHealth()) + ")", Color::GREEN);
            
            if (user->getConsumableQuickly() == item) {
                user->unequipConsumable();
            }
            user->getInventory()->removeItem(item);
            if (shiftWasConsumed) *shiftWasConsumed = true;
            return true;
        });
        return healing;
    };

    auto createLittleGallowsAlchemy = []() {
        auto buff = std::make_unique<ConsumableItem>(ItemFactory::getNameFromID(ItemID::LittleGallowsAlchemy), 40);
        buff->addProperty(Property::BuffConsumable);
        buff->setDescriptionInspection("Aumenta Forca em +5 e Destreza em +3 por 3 turnos em combate.");
        buff->setActionInventory([](Item* item, Character* user, bool* shiftWasConsumed) {
            if (!shiftWasConsumed) { showWarningConsumable("Pocoes de buff so podem ser usadas em combate!"); return true; }
            user->addEffect(std::make_unique<WarCryEffect>(3, 5, 3));
            showWarningConsumable(item->getItemName() + " consumida! +5 Forca e +3 Destreza por 3 turnos!", Color::LIGHT_GREEN);
            
            if (user->getConsumableQuickly() == item) {
                user->unequipConsumable();
            }
            user->getInventory()->removeItem(item);
            *shiftWasConsumed = true;
            return true;
        });
        return buff;
    };

    auto createLittlePoisonAlchemy = []() {
        auto debuff = std::make_unique<ConsumableItem>(ItemFactory::getNameFromID(ItemID::LittlePoisonAlchemy), 35);
        debuff->addProperty(Property::ConsumableDebuffWeakness);
        debuff->setDescriptionInspection("Aplica Necrose no alvo por 3 turnos (causa 12 de dano por turno).");
        debuff->setActionInventory([](Item* item, Character* user, bool* shiftWasConsumed) {
            if (!shiftWasConsumed) { showWarningConsumable("Pocoes de arremesso so podem ser usadas em combate!"); return true; }
            user->setItemSelectedForUse(item);
            return true;
        });
        debuff->setActionUse([](Character* /*usuario*/, Character* target) {
            if (!Character::isValid(target) || target->getHealth() <= 0) return;
            target->addEffect(std::make_unique<NecrosisEffect>(3, 12));
            CombatScreen::addFixedMessage("\n" + CombatScreen::combatMargin() + Appearance::color(Color::RED) + ">> Voce arremessou a pocao! " + target->getName() + " sofreu necrose (12 dano/turno) por 3 turnos!" + Appearance::color(Color::RESET) + "\n");
        });
        return debuff;
    };

    auto createLittleSlowAlchemy = []() {
        auto debuff = std::make_unique<ConsumableItem>(ItemFactory::getNameFromID(ItemID::LittleSlowAlchemy), 35);
        debuff->addProperty(Property::ConsumableDebuffSlow);
        debuff->setDescriptionInspection("Aplica Lentidao no alvo por 3 turnos (Reduz Destreza).");
        debuff->setActionInventory([](Item* item, Character* user, bool* shiftWasConsumed) {
            if (!shiftWasConsumed) { showWarningConsumable("Pocoes de arremesso so podem ser usadas em combate!"); return true; }
            user->setItemSelectedForUse(item);
            return true;
        });
        debuff->setActionUse([](Character* /*usuario*/, Character* target) {
            if (!Character::isValid(target) || target->getHealth() <= 0) return;
            target->addEffect(std::make_unique<SlownessEffect>(3));
            CombatScreen::addFixedMessage("\n" + CombatScreen::combatMargin() + Appearance::color(Color::MAGENTA) + ">> Voce arremessou a pocao! " + target->getName() + " esta sob efeito de Lentidao por 3 turnos!" + Appearance::color(Color::RESET) + "\n");
        });
        return debuff;
    };

    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> builders = {
        {ItemID::Litter, [createFood]() { return createFood(ItemID::Litter, 15, 5, "Restaura 15 HP fixo."); }},
        {ItemID::Bread, [createFood]() { return createFood(ItemID::Bread, 25, 10, "Restaura 25 HP fixo."); }},
        {ItemID::Cheese, [createFood]() { return createFood(ItemID::Cheese, 40, 18, "Restaura 40 HP fixo."); }},
        {ItemID::MeatDrought, [createFood]() { return createFood(ItemID::MeatDrought, 60, 30, "Restaura 60 HP fixo."); }},
        {ItemID::LittleCureBig, createLittleCureBig},
        {ItemID::LittleGallowsAlchemy, createLittleGallowsAlchemy},
        {ItemID::LittlePoisonAlchemy, createLittlePoisonAlchemy},
        {ItemID::LittleSlowAlchemy, createLittleSlowAlchemy},
        {ItemID::LittleCure30, createLittleCure},
        {ItemID::LittleFury, [createBuffAttributes]() { return createBuffAttributes(ItemID::LittleFury); }},
        {ItemID::ElixirArcane, [createBuffAttributes]() { return createBuffAttributes(ItemID::ElixirArcane); }},
        {ItemID::BottleSlime, []() {
            auto debuff = std::make_unique<ConsumableItem>(ItemFactory::getNameFromID(ItemID::BottleSlime));
            debuff->addProperty(Property::ConsumableDebuffSlow);
            debuff->setDescriptionInspection("Aplica Lentidao no alvo por 3 turnos (Reduz Destreza).");
            debuff->setActionInventory([](Item* item, Character* user, bool* shiftWasConsumed) {
                if (!shiftWasConsumed) { showWarningConsumable("Frascos de debuff so podem ser usados em combate!"); return true; }
                user->setItemSelectedForUse(item);
                return true;
            });
            debuff->setActionUse([](Character* /*usuario*/, Character* target) {
                if (!Character::isValid(target) || target->getHealth() <= 0) return;
                target->addEffect(std::make_unique<SlownessEffect>(3));
            CombatScreen::addFixedMessage("\n" + CombatScreen::combatMargin() + Appearance::color(Color::MAGENTA) + ">> Voce jogou o frasco! " + target->getName() + " esta com lentidao por 3 turnos!" + Appearance::color(Color::RESET) + "\n");
            });
            return debuff;
        }},
        {ItemID::BottleWeakness, []() {
            auto debuff = std::make_unique<ConsumableItem>(ItemFactory::getNameFromID(ItemID::BottleWeakness));
            debuff->addProperty(Property::ConsumableDebuffWeakness);
            debuff->setDescriptionInspection("Aplica Fraqueza no alvo por 3 turnos (-25% Forca).");
            debuff->setActionInventory([](Item* item, Character* user, bool* shiftWasConsumed) {
                if (!shiftWasConsumed) { showWarningConsumable("Frascos de debuff so podem ser usados em combate!"); return true; }
                user->setItemSelectedForUse(item);
                return true;
            });
            debuff->setActionUse([](Character* /*usuario*/, Character* target) {
                if (!Character::isValid(target) || target->getHealth() <= 0) return;
                target->addEffect(std::make_unique<WeaknessEffect>(3));
            CombatScreen::addFixedMessage("\n" + CombatScreen::combatMargin() + Appearance::color(Color::RED) + ">> Voce jogou o frasco! " + target->getName() + " teve sua forca reduzida em 25% por 3 turnos!" + Appearance::color(Color::RESET) + "\n");
            });
            return debuff;
        }},
        {ItemID::OrganRegenerator, []() { 
            auto buff = std::make_unique<ConsumableItem>(ItemFactory::getNameFromID(ItemID::OrganRegenerator), 500); 
            buff->addProperty(Property::ConsumablePowerTroll); 
            buff->setDescriptionInspection("Concede a regeneracao do Troll permanentemente (cura 100% HP apos batalhas).");
            buff->setActionInventory([](Item* item, Character* user, bool* shiftWasConsumed) {
                if (user->ownsRegenerationTroll()) {
                    showWarningConsumable("O poder regenerador do Troll ja corre em suas veias!");
                } else {
                    user->unlockRegenerationTroll();
                    user->modifyHealth(user->getMaxHealth());
                    showWarningConsumable(item->getItemName() + " consumido! Voce agora curara 100% do seu HP apos cada combate!", Color::GREEN);
                    
                    if (user->getConsumableQuickly() == item) user->unequipConsumable();
                    
                    user->getInventory()->removeItem(item);
                }
                if (shiftWasConsumed) *shiftWasConsumed = true;
                return true;
            });
            return buff; 
        }},
        {ItemID::TalismanBear, [createTalisman]() { return createTalisman(ItemID::TalismanBear, Property::TalismanGallows, AttributeType::Strength, AttributeType::Intelligence, "Concede +5 Forca e -5 Inteligencia permanentemente."); }},
        {ItemID::TalismanCrow, [createTalisman]() { return createTalisman(ItemID::TalismanCrow, Property::TalismanIntelligence, AttributeType::Intelligence, AttributeType::Strength, "Concede +5 Inteligencia e -5 Forca permanentemente."); }},
        {ItemID::TalismanLeopard, [createTalisman]() { return createTalisman(ItemID::TalismanLeopard, Property::TalismanDexterity, AttributeType::Dexterity, AttributeType::Wisdom, "Concede +5 Destreza e -5 Sabedoria permanentemente."); }},
        {ItemID::TalismanOwl, [createTalisman]() { return createTalisman(ItemID::TalismanOwl, Property::TalismanWisdom, AttributeType::Wisdom, AttributeType::Dexterity, "Concede +5 Sabedoria e -5 Destreza permanentemente."); }}
    };
    auto it = builders.find(id);
    if (it != builders.end()) return it->second();
    return nullptr;
}
