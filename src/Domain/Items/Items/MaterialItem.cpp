#include "Domain/Items/Items/MaterialItem.h"
#include "Core/Utils/Appearance.h"
#include <vector>
#include <functional>
#include <unordered_map>
#include "Domain/Items/ItemFactory.h"

MaterialItem::MaterialItem(const std::string& name, int price) : Item(price), name(name)
{
}

std::string MaterialItem::getItemName() const { return name; }
EquipmentType MaterialItem::getType() const { return EquipmentType::MATERIAL; }

std::vector<std::string> MaterialItem::getDetailsInspection(Character* /*personagem*/) const {
    std::vector<std::string> lines;
    lines.push_back(" > Tipo: Material");
    
    if (!descriptionInspection.empty()) {
        for (const auto& desc : descriptionInspection) lines.push_back(" > Descricao: " + desc);
    } else {
        lines.push_back(" > Descricao: Pode ser util para construcoes ou rituais.");
    }
    return lines;
}

std::unique_ptr<Item> manufactureItemMaterial(ItemID id) {
    auto createMaterial = [](ItemID id, int price, const std::string& desc = "") {
        auto m = std::make_unique<MaterialItem>(ItemFactory::getNameFromID(id), price);
        if (!desc.empty()) m->setDescriptionInspection(desc);
        return m;
    };

    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> builders = {
        {ItemID::SlimeAcid, [createMaterial]() { return createMaterial(ItemID::SlimeAcid, 5); }},
        {ItemID::ToothGoblin, [createMaterial]() { return createMaterial(ItemID::ToothGoblin, 1, "Pode ser usado na Cabana da Bruxa para encantar armas com Sangramento (Requer 40x)."); }},
        {ItemID::NucleusSticky, [createMaterial]() { return createMaterial(ItemID::NucleusSticky, 30, "Pode ser usado na Cabana da Bruxa para encantar armas com Lentidao (Requer 5x)."); }},
        {ItemID::PoMagician, [createMaterial]() { return createMaterial(ItemID::PoMagician, 15, "Pode ser usado na Cabana da Bruxa para encantar armas com Quebra de Resistencia Permanente (Requer 25x)."); }},
        {ItemID::WoodBewitched, [createMaterial]() { return createMaterial(ItemID::WoodBewitched, 3, "Pode ser usada na Cabana da Bruxa para encantar o Arco ou o Violao (Requer 1x)."); }},
        {ItemID::HeartForest, [createMaterial]() { return createMaterial(ItemID::HeartForest, 3, "Usado na Cabana da Bruxa para encantar o Cajado ou para desbloquear a passagem do labirinto (Requer 3x)."); }},
        {ItemID::StoneUpgrade, [createMaterial]() { return createMaterial(ItemID::StoneUpgrade, 3, "Uma pedra extremamente rara. Pode ser usada na Forja de Bjorn para conceder +3 de Defesa (Resistencia) a uma armadura."); }},
        {ItemID::InvitationReal, [createMaterial]() { return createMaterial(ItemID::InvitationReal, 1, "Permite o acesso livre aos portoes do Reino Real."); }}
    };
    auto it = builders.find(id);
    if (it != builders.end()) return it->second();
    return nullptr;
}
