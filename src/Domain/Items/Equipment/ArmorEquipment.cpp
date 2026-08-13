#include "Domain/Items/Equipment/ArmorEquipment.h"
#include "Domain/Characters/Character.h"
#include "Core/Utils/Appearance.h"
#include <vector>
#include <functional>
#include <unordered_map>
#include "Domain/Items/ItemFactory.h"

EquipmentArmor::EquipmentArmor(const std::string& name, int reductionFixed, int reqResistance, int reqConstitution, int price) 
    : Item(price), name(name), reductionFixed(reductionFixed), reqResistance(reqResistance), reqConstitution(reqConstitution), penaltyDexterity(reductionFixed / 3)
{
}

std::string EquipmentArmor::getItemName() const { return name; }
EquipmentType EquipmentArmor::getType() const { return EquipmentType::ARMOR; }

int EquipmentArmor::getReductionFixed() const { return reductionFixed; }
int EquipmentArmor::getReqResistance() const { return reqResistance; }
int EquipmentArmor::getReqConstitution() const { return reqConstitution; }

bool EquipmentArmor::canBeEquippedBy(Character* character) const {
    if (!character) return false;
    return character->getResistance() >= reqResistance &&
           character->getConstitution() >= reqConstitution;
}

std::vector<std::string> EquipmentArmor::getDetailsInspection(Character* character) const {
    std::vector<std::string> lines;
    lines.push_back(" > Tipo: Armadura");
    
    std::string defFixedStr = std::to_string(reductionFixed) + " (Reduz dano recebido permanentemente)";
    if (character) {
        int defTotal = reductionFixed + character->getResistance();
        defFixedStr += " " + Appearance::color(Color::GRAY) + "-> " + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m" + "C/ Seus Atributos: " + Appearance::color(Color::YELLOW) + std::to_string(defTotal) + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m";
    }
    lines.push_back(" > Defesa Fixa: " + defFixedStr);
    lines.push_back(" > Requisitos:");
    bool hasReq = false;
    if (reqResistance > 0) { lines.push_back("   - Resistencia: " + std::to_string(reqResistance)); hasReq = true; }
    if (reqConstitution > 0) { lines.push_back("   - Constituicao: " + std::to_string(reqConstitution)); hasReq = true; }
    if (!hasReq) lines.push_back("   - Nenhum requisito.");
    
    if (penaltyDexterity > 0) {
        lines.push_back(" > Penalidade: -" + std::to_string(penaltyDexterity) + " Destreza");
    } else {
        lines.push_back(" > Penalidade: Nenhuma");
    }
    
    if (hasProperty(Property::AdaptationArmor)) {
        lines.push_back(" > Efeitos Ocultos: A Roda gira a cada turno regenerando 5% do HP e adapta a");
        lines.push_back("                    sua defesa ao inimigo e seu ataque a sua arma (+2 status)!");
    }
    return lines;
}

std::string EquipmentArmor::getInfoStatus() const {
    std::string info = " (Def: " + std::to_string(reductionFixed);
    if (penaltyDexterity > 0) {
        info += " | -" + std::to_string(penaltyDexterity) + " Dest";
    }
    
    std::string reqs = "";
    if (reqResistance > 0 || reqConstitution > 0) {
        reqs += " | Req: ";
        if (reqResistance > 0) reqs += std::to_string(reqResistance) + " Res ";
        if (reqConstitution > 0) reqs += std::to_string(reqConstitution) + " Con ";
    }

    return info + reqs + ")";
}

std::unique_ptr<Item> EquipmentArmor::generateCopyImproved() const {
    auto newArmor = std::make_unique<EquipmentArmor>(name + "+", static_cast<int>(reductionFixed * 1.5), reqResistance, reqConstitution, priceSale * 2);
    for (Property prop : properties) newArmor->addProperty(prop);
    newArmor->addProperty(Property::Improved);
    return newArmor;
}

std::unique_ptr<Item> manufactureEquipmentArmor(ItemID id) {
    auto createArmor = [](ItemID id, int def, int rRes, int rCon, int price) {
        return std::make_unique<EquipmentArmor>(ItemFactory::getNameFromID(id), def, rRes, rCon, price);
    };

    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> builders = {
        {ItemID::ArmorMesh, [createArmor]() { return createArmor(ItemID::ArmorMesh, 7, 0, 0, 3); }},
        {ItemID::ArmorLeather, [createArmor]() { return createArmor(ItemID::ArmorLeather, 5, 0, 0, 3); }},
        {ItemID::Tunic, [createArmor]() { return createArmor(ItemID::Tunic, 2, 0, 0, 3); }},
        {ItemID::CostumeNoble, [createArmor]() { return createArmor(ItemID::CostumeNoble, 4, 0, 0, 3); }},
        {ItemID::ArmorRags, [createArmor]() { return createArmor(ItemID::ArmorRags, 3, 0, 0, 3); }},
        {ItemID::ArmorKnight, [createArmor]() { return createArmor(ItemID::ArmorKnight, 12, 0, 0, 0); }},
        {ItemID::ClothesRitualist, [createArmor]() { return createArmor(ItemID::ClothesRitualist, 3, 0, 0, 15); }},
        {ItemID::ArmorChest, [createArmor]() { 
            auto armor = createArmor(ItemID::ArmorChest, 20, 0, 0, 150); 
            armor->setPenaltyDexterity(10);
            return armor; 
        }},
        {ItemID::AdaptationWheel, []() { 
            std::string name = "Roda da Adaptacao";
            std::string degrade = "";
            for (size_t i = 0; i < name.length(); ++i) {
                int r = 255;
                int g = 255 * i / (name.length() - 1);
                int b = 255 * (name.length() - 1 - i) / (name.length() - 1);
                degrade += "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m" + std::string(1, name[i]);
            }
            degrade += "\033[0m";
            auto armor = std::make_unique<EquipmentArmor>(degrade, 40, 0, 0, 5000); 
            armor->setPenaltyDexterity(0);
            armor->addProperty(Property::AdaptationArmor);
            return armor; 
        }}
    };
    auto it = builders.find(id);
    if (it != builders.end()) return it->second();
    return nullptr;
}
