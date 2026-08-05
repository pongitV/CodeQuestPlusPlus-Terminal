#include "Domain/Items/Equipment/ShieldEquipment.h"
#include <memory>
#include "Domain/Characters/Character.h"
#include "Core/Utils/Appearance.h"
#include <vector>
#include <functional>
#include <unordered_map>
#include "UI/Screens/Combat/CombatScreen.h"
#include "Domain/Items/ItemFactory.h"

EquipmentShield::EquipmentShield(const std::string& name, int reductionFixed, int durability, int reqResistance, int reqSecondary, AttributeType typeSecondary, int price)
    : Item(price), name(name), reductionFixed(reductionFixed), durability(durability), durabilityMaximum(durability), reqResistance(reqResistance), reqSecondary(reqSecondary), typeSecondary(typeSecondary)
{
}

std::string EquipmentShield::getItemName() const { return name; }
EquipmentType EquipmentShield::getType() const { return EquipmentType::SHIELD; }

int EquipmentShield::getDurabilityMaximum() const { return durabilityMaximum; }
void EquipmentShield::setDurability(int newDurability) { durability = newDurability; }
int EquipmentShield::getDurabilityCurrentShield() const { return durability; }
int EquipmentShield::getReductionDamageFixedShield() const { return (durability > 0) ? reductionFixed : 0; }

int EquipmentShield::getReqResistance() const { return reqResistance; }
int EquipmentShield::getReqSecondary() const { return reqSecondary; }
AttributeType EquipmentShield::getTypeSecondary() const { return typeSecondary; }

void EquipmentShield::reduceDurability(int qty) { 
    if (durability <= 0) return; // Ja estava quebrado
    
    durability -= qty; 
    if (durability <= 0) {
        durability = 0;
        CombatScreen::addFixedMessage(CombatScreen::combatMargin() + Appearance::color(Color::RED) + ">> O escudo [" + name + "] quebrou e perdeu seu poder de bloqueio!" + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m" + "\n");
        Appearance::registerBattleLog(Appearance::color(Color::RED) + ">> O escudo [" + name + "] quebrou e perdeu seu poder de bloqueio!" + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m");
    }
}
void EquipmentShield::increaseDurability(int qty) { durability += qty; }

bool EquipmentShield::canBeEquippedBy(Character* character) const {
    if (!character) return false;
    if (character->getResistance() < reqResistance) return false;
    int secVal = 0;
    switch(typeSecondary) {
        case AttributeType::Strength: secVal = character->getStrength(); break;
        case AttributeType::Dexterity: secVal = character->getDexterity(); break;
        case AttributeType::Intelligence: secVal = character->getIntelligence(); break;
        case AttributeType::Wisdom: secVal = character->getWisdom(); break;
        default: secVal = 9999;
    }
    return secVal >= reqSecondary;
}

std::vector<std::string> EquipmentShield::getDetailsInspection(Character* character) const {
    std::vector<std::string> lines;
    lines.push_back(" > Tipo: Escudo");

    std::string lockStr = std::to_string(reductionFixed) + " (Dano bloqueado na acao 'Defender')";
    if (character) {
        int defTotal = reductionFixed + character->getResistance();
        lockStr += " " + Appearance::color(Color::GRAY) + "-> " + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m" + "C/ Seus Atributos: " + Appearance::color(Color::YELLOW) + std::to_string(defTotal) + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m";
    }
    lines.push_back(" > Poder de Bloqueio: " + lockStr);
    lines.push_back(" > Durabilidade Maxima: " + std::to_string(durability) + " usos");
    lines.push_back(" > Requisitos:");
    bool hasReq = false;
    if (reqResistance > 0) { lines.push_back("   - Resistencia Base: " + std::to_string(reqResistance)); hasReq = true; }
    if (reqSecondary > 0) {
        std::string atrSec = "";
        if (typeSecondary == AttributeType::Strength) atrSec = "Forca";
        else if (typeSecondary == AttributeType::Dexterity) atrSec = "Destreza";
        else if (typeSecondary == AttributeType::Intelligence) atrSec = "Inteligencia";
        else if (typeSecondary == AttributeType::Wisdom) atrSec = "Sabedoria";
        lines.push_back("   - Atributo Secundario (" + atrSec + "): " + std::to_string(reqSecondary));
        hasReq = true;
    }
    if (!hasReq) lines.push_back("   - Nenhum requisito.");
    return lines;
}

std::string EquipmentShield::getInfoStatus() const {
    std::string info = " (Def: " + std::to_string(reductionFixed) + " | Dur: " + std::to_string(durability) + "/" + std::to_string(durabilityMaximum);
    std::string reqs = "";
    if (reqResistance > 0 || reqSecondary > 0) {
        reqs += " | Req: ";
        if (reqResistance > 0) reqs += std::to_string(reqResistance) + " Res ";
        if (reqSecondary > 0) {
            reqs += std::to_string(reqSecondary) + " ";
            if (typeSecondary == AttributeType::Strength) reqs += "For ";
            else if (typeSecondary == AttributeType::Dexterity) reqs += "Des ";
            else if (typeSecondary == AttributeType::Intelligence) reqs += "Int ";
            else if (typeSecondary == AttributeType::Wisdom) reqs += "Sab ";
        }
    }
    
    std::string tag = "";
    if (durability <= 0) {
        tag = " " + Appearance::color(Color::RED) + "[QUEBRADO]" + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m";
    } else if (durability < durabilityMaximum) {
        tag = " " + Appearance::color(Color::RED) + "[D]" + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m";
    }
    return info + reqs + ")" + tag;
}

std::unique_ptr<Item> EquipmentShield::generateCopyImproved() const {
    auto newShield = std::make_unique<EquipmentShield>(name + "+", static_cast<int>(reductionFixed * 1.5), static_cast<int>(durabilityMaximum * 1.5), reqResistance, reqSecondary, typeSecondary, priceSale * 2);
    for (Property prop : properties) newShield->addProperty(prop);
    newShield->addProperty(Property::Improved);
    return newShield;
}

std::unique_ptr<Item> manufactureEquipmentShield(ItemID id) {
    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> builders = {
        {ItemID::ShieldMetal, []() { return std::make_unique<EquipmentShield>(ItemFactory::getNameFromID(ItemID::ShieldMetal), 15, 5, 0, 0, AttributeType::Strength, 9); }},
        {ItemID::BarrierMagic, []() { return std::make_unique<EquipmentShield>(ItemFactory::getNameFromID(ItemID::BarrierMagic), 50, 2, 0, 0, AttributeType::Intelligence, 3); }},
        {ItemID::CoverMagic, []() { return std::make_unique<EquipmentShield>(ItemFactory::getNameFromID(ItemID::CoverMagic), 6, 10, 0, 0, AttributeType::Wisdom, 9); }},
        {ItemID::ArmbandsSilver, []() { return std::make_unique<EquipmentShield>(ItemFactory::getNameFromID(ItemID::ArmbandsSilver), 5, 3, 0, 0, AttributeType::Dexterity, 3); }}
    };
    auto it = builders.find(id);
    if (it != builders.end()) return it->second();
    return nullptr;
}
