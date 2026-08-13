#include "Domain/Items/Equipment/WeaponEquipment.h"

#include <iostream>
#include <vector>

#include "Domain/Characters/Character.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/RandomGenerator.h"
#include "UI/Screens/Combat/CombatScreen.h"
#include <functional>
#include <unordered_map>
#include "Domain/Items/ItemFactory.h"

EquipmentWeapon::EquipmentWeapon(const std::string& name, int damagePhysical, int damageMagical, int reqStrength, int reqDexterity, int reqIntelligence, int reqWisdom, int price)
    : Item(price), name(name), damagePhysical(damagePhysical), damageMagical(damageMagical), reqStrength(reqStrength), reqDexterity(reqDexterity), reqIntelligence(reqIntelligence), reqWisdom(reqWisdom), effectBleeding(false), effectSlow(false)
{
}

std::string EquipmentWeapon::getItemName() const { return name; }
void EquipmentWeapon::changeName(const std::string& n) { name = n; }
EquipmentType EquipmentWeapon::getType() const { return EquipmentType::WEAPONS; }

int EquipmentWeapon::getPhysicsDamage() const { return damagePhysical; }
int EquipmentWeapon::getMagicalDamage() const { return damageMagical; }

int EquipmentWeapon::getReqStrength() const { return reqStrength; }
int EquipmentWeapon::getReqDexterity() const { return reqDexterity; }
int EquipmentWeapon::getReqIntelligence() const { return reqIntelligence; }
int EquipmentWeapon::getReqWisdom() const { return reqWisdom; }

bool EquipmentWeapon::ownsEffectBleeding() const { return effectBleeding; }
bool EquipmentWeapon::ownsEffectSlow() const { return effectSlow; }

bool EquipmentWeapon::canBeEquippedBy(Character* character) const {
    if (!character) return false;
    return character->getStrength() >= reqStrength &&
           character->getDexterity() >= reqDexterity &&
           character->getIntelligence() >= reqIntelligence &&
           character->getWisdom() >= reqWisdom;
}

std::vector<std::string> EquipmentWeapon::getDetailsInspection(Character* character) const {
    std::vector<std::string> lines;
    lines.push_back(" > Tipo: Arma");

    std::string physStr = std::to_string(damagePhysical);
    std::string magicStr = std::to_string(damageMagical);

    if (character) {
        int strength = character->getStrength();
        int dexterity = character->getDexterity();
        int intelli = character->getIntelligence();
        int wisdom = character->getWisdom();
        
        if (damagePhysical == 0 && damageMagical > 0) { strength /= 10; dexterity /= 10; }
        else if (damagePhysical > 0 && damageMagical == 0) { intelli /= 10; wisdom /= 10; }
        
        int damagePhysIs = std::max(0, static_cast<int>((damagePhysical + strength) * (1.0 + (dexterity / 100.0)) * character->getMultiplier()));
        int damageMagicIs = std::max(0, static_cast<int>((damageMagical + intelli) * (1.0 + (wisdom / 100.0)) * character->getMultiplier()));
        
        physStr += " " + Appearance::color(Color::GRAY) + "-> " + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m" + "C/ Seus Atributos: " + Appearance::color(Color::LIGHT_RED) + std::to_string(damagePhysIs) + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m";
        magicStr += " " + Appearance::color(Color::GRAY) + "-> " + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m" + "C/ Seus Atributos: " + Appearance::color(Color::BLUE) + std::to_string(damageMagicIs) + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m";
    }

    lines.push_back(" > Dano Fisico: " + physStr);
    lines.push_back(" > Dano Magico: " + magicStr);
    lines.push_back(" > Requisitos:");
    bool hasReq = false;
    if (reqStrength > 0) { lines.push_back("   - Forca: " + std::to_string(reqStrength)); hasReq = true; }
    if (reqDexterity > 0) { lines.push_back("   - Destreza: " + std::to_string(reqDexterity)); hasReq = true; }
    if (reqIntelligence > 0) { lines.push_back("   - Inteligencia: " + std::to_string(reqIntelligence)); hasReq = true; }
    if (reqWisdom > 0) { lines.push_back("   - Sabedoria: " + std::to_string(reqWisdom)); hasReq = true; }
    if (!hasReq) lines.push_back("   - Nenhum requisito.");
    
    lines.push_back(" > Efeitos e Propriedades:");
    bool hasEffect = false;
    if (effectBleeding) { lines.push_back("   - Sangramento (Dano continuo no alvo)"); hasEffect = true; }
    if (effectSlow) { lines.push_back("   - Lentidao (Reduz destreza do alvo)"); hasEffect = true; }
    if (hasProperty(Property::Penetrating)) { lines.push_back("   - Penetrante (Reduz resistencia do alvo)"); hasEffect = true; }
    if (hasProperty(Property::Magic)) { lines.push_back("   - Magica (Parte do dano ignora defesa)"); hasEffect = true; }
    if (hasProperty(Property::IgnoreDefense)) { lines.push_back("   - Exterminio (Ignora 100% da Resistencia e Constituicao do alvo)"); hasEffect = true; }
    if (hasProperty(Property::ViolaMagician)) { lines.push_back("   - Raizes Drenantes (Causa dano e cura o usuario)"); hasEffect = true; }
    if (hasProperty(Property::VinePrison)) { lines.push_back("   - Prisao de Cipos (Chance de atordoar alvo)"); hasEffect = true; }
    if (!hasEffect) lines.push_back("   - Nenhuma propriedade extra.");
    return lines;
}

std::string EquipmentWeapon::getInfoStatus() const {
    std::string ef = "";
    if (ownsEffectBleeding()) ef += " | +Sangramento";
    if (ownsEffectSlow()) ef += " | +Lentidao";
    if (hasProperty(Property::Penetrating)) ef += " | +Penetracao";
    
    std::string reqs = "";
    bool hasReq = false;
    if (reqStrength > 0) { reqs += std::to_string(reqStrength) + " For "; hasReq = true; }
    if (reqDexterity > 0) { reqs += std::to_string(reqDexterity) + " Des "; hasReq = true; }
    if (reqIntelligence > 0) { reqs += std::to_string(reqIntelligence) + " Int "; hasReq = true; }
    if (reqWisdom > 0) { reqs += std::to_string(reqWisdom) + " Sab "; hasReq = true; }
    if (hasReq) reqs = " | Req: " + reqs;

    return " (Dano: " + std::to_string(damagePhysical) + "F/" + std::to_string(damageMagical) + "M" + ef + reqs + ")";
}

void EquipmentWeapon::applyEffectBleeding() { effectBleeding = true; }
void EquipmentWeapon::applyEffectSlow() { effectSlow = true; }

void EquipmentWeapon::beforeCausingDamage(Character* attacker, Character* target) {
    if (hasProperty(Property::Penetrating) && !target->ownsEffect(EffectID::ResistanceBreak)) {
        target->addEffect(std::make_unique<ResistanceBreakEffect>());
        CombatScreen::addFixedMessage(CombatScreen::combatMargin() + Appearance::color(Color::CYAN) + ">> A arma de " + attacker->getName() + " ativou o po magico! O ataque enfraqueceu " + target->getName() + " ate o fim do combate!" + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m" + "\n");
        Appearance::registerBattleLog(Appearance::color(Color::CYAN) + ">> A arma de " + attacker->getName() + " ativou o po magico! O ataque enfraqueceu " + target->getName() + " ate o fim do combate!" + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m");
    }
}

void EquipmentWeapon::onCausingDamage(Character* attacker, Character* target, int damageCaused) {
    if (damageCaused <= 0) return;

    if (hasProperty(Property::ViolaMagician) && !target->ownsEffect(EffectID::BloodSuck)) {
        target->addEffect(std::make_unique<BloodSuckEffect>(2, attacker));
    }

    if (hasProperty(Property::VinePrison) && RandomGenerator::rollChance(30) && !target->ownsEffect(EffectID::Stun)) {
        target->addEffect(std::make_unique<StunEffect>(1));
    }

    if (ownsEffectBleeding() && !target->ownsEffect(EffectID::Bleeding)) {
        int damageBleeding = std::max(1, target->getMaxHealth() / 10);
        target->addEffect(std::make_unique<BleedingEffect>(3, damageBleeding));
        Color bleedingColor = (target->getClassName() != "Monstro") ? Color::LIGHT_RED : Color::RED;
        CombatScreen::addFixedMessage(CombatScreen::combatMargin() + Appearance::color(bleedingColor) + ">> " + target->getName() + " comecou a sangrar profundamente! (3 turnos)" + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m" + "\n");
        Appearance::registerBattleLog(Appearance::color(bleedingColor) + ">> " + target->getName() + " comecou a sangrar profundamente! (3 turnos)" + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m");
    }

    if (ownsEffectSlow() && !target->ownsEffect(EffectID::Slowness)) {
        target->addEffect(std::make_unique<SlownessEffect>(3));
        CombatScreen::addFixedMessage(CombatScreen::combatMargin() + Appearance::color(Color::MAGENTA) + ">> " + target->getName() + " foi coberto por gosma e sua destreza caiu pela metade! (3 turnos)" + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m" + "\n");
        Appearance::registerBattleLog(Appearance::color(Color::MAGENTA) + ">> " + target->getName() + " foi coberto por gosma e sua destreza caiu pela metade! (3 turnos)" + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m");
    }
}

int EquipmentWeapon::ensureDamageMinimum(int finalDamage) {
    int minimum = 1;
    if (hasProperty(Property::ViolaBase)) {
        minimum = std::max(minimum, damageMagical);
    }
    return std::max(finalDamage, minimum);
}

std::unique_ptr<Item> EquipmentWeapon::generateCopyImproved() const {
    auto newWeapon = std::make_unique<EquipmentWeapon>(name + "+", static_cast<int>(damagePhysical * 1.5), static_cast<int>(damageMagical * 1.5), reqStrength, reqDexterity, reqIntelligence, reqWisdom, priceSale * 2);
    
    for (Property prop : properties) newWeapon->addProperty(prop);
    newWeapon->addProperty(Property::Improved);
    
    if (effectBleeding) newWeapon->applyEffectBleeding();
    if (effectSlow) newWeapon->applyEffectSlow();
    
    return newWeapon;
}

std::unique_ptr<Item> manufactureEquipmentWeapon(ItemID id) {
    auto createWeapon = [](ItemID id, int dPhys, int dMagic, int rFor, int rDes, int rInt, int rSat, int price) {
        return std::make_unique<EquipmentWeapon>(ItemFactory::getNameFromID(id), dPhys, dMagic, rFor, rDes, rInt, rSat, price);
    };

    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> builders = {
        {ItemID::DaggerStone, [createWeapon]() { return createWeapon(ItemID::DaggerStone, 5, 0, 0, 0, 0, 0, 3); }},
        {ItemID::BowWood, [createWeapon]() { return createWeapon(ItemID::BowWood, 10, 0, 0, 0, 0, 0, 3); }},
        {ItemID::StaffCrystal, [createWeapon]() { return createWeapon(ItemID::StaffCrystal, 0, 30, 0, 0, 0, 0, 3); }},
        {ItemID::WandCorroded, [createWeapon]() { return createWeapon(ItemID::WandCorroded, 0, 25, 0, 0, 0, 0, 3); }},
        {ItemID::ViolaEnchanted, [createWeapon]() { 
            auto viola = createWeapon(ItemID::ViolaEnchanted, 0, 10, 0, 0, 0, 0, 3); 
            viola->addProperty(Property::ViolaBase);
            return viola; 
        }},
        {ItemID::StaffBone, [createWeapon]() { 
            auto weapon = createWeapon(ItemID::StaffBone, 2, 8, 0, 0, 5, 10, 10); 
            weapon->addProperty(Property::Magic);
            return weapon; 
        }},
        {ItemID::SwordIron, [createWeapon]() { return createWeapon(ItemID::SwordIron, 10, 0, 0, 0, 0, 0, 3); }},
        {ItemID::AxWar, [createWeapon]() { return createWeapon(ItemID::AxWar, 15, 0, 10, 0, 0, 0, 3); }},
        {ItemID::SlimeAcidWeapon, [createWeapon]() { return createWeapon(ItemID::SlimeAcidWeapon, 2, 7, 0, 0, 0, 0, 3); }},
        {ItemID::TrunkRumpled, [createWeapon]() { return createWeapon(ItemID::TrunkRumpled, 40, 0, 25, 0, 0, 0, 30); }},
        {ItemID::SwordKnight, [createWeapon]() { return createWeapon(ItemID::SwordKnight, 12, 0, 0, 0, 0, 0, 0); }},
        {ItemID::SwordExtermination, []() { 
            std::string name = "Espada de Exterminio";
            std::string degrade = "";
            for (size_t i = 0; i < name.length(); ++i) {
                int r = 255 * (name.length() - 1 - i) / (name.length() - 1);
                int g = r;
                int b = 255 * i / (name.length() - 1);
                degrade += "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m" + std::string(1, name[i]);
            }
            degrade += "\033[0m";
            auto weapon = std::make_unique<EquipmentWeapon>(degrade, 65, 65, 40, 40, 0, 0, 5000);
            weapon->addProperty(Property::Magic);
            weapon->addProperty(Property::IgnoreDefense);
            return weapon;
        }}
    };
    auto it = builders.find(id);
    if (it != builders.end()) return it->second();
    return nullptr;
}
