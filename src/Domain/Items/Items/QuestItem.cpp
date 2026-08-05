#include "Domain/Items/Items/QuestItem.h"
#include <functional>
#include <unordered_map>
#include "Domain/Items/ItemFactory.h"

ItemMission::ItemMission(const std::string& name, int price) : Item(price), name(name) {}

std::string ItemMission::getItemName() const { return name; }
EquipmentType ItemMission::getType() const { return EquipmentType::MISSION; }

std::vector<std::string> ItemMission::getDetailsInspection(Character* /*personagem*/) const {
    std::vector<std::string> details;
    details.push_back(" > Tipo: Item de Missao");
    if (!descriptionInspection.empty()) {
        for (const auto& desc : descriptionInspection) details.push_back(desc);
    } else {
        details.push_back(" > Lore: Um item misterioso e importante para sua jornada.");
    }
    return details;
}

std::unique_ptr<Item> manufactureItemMission(ItemID id) {
    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> builders = {
        {ItemID::DeviceLanguage, []() { 
            auto i = std::make_unique<ItemMission>(ItemFactory::getNameFromID(ItemID::DeviceLanguage), 500); 
            i->setDescriptionInspection({" > Lore: Um estranho artefato de plastico com teclas.", "   Nao parece pertencer a este mundo, mas emana", "   uma energia peculiar..."});
            return i;
        }}
    };
    auto it = builders.find(id);
    if (it != builders.end()) return it->second();
    return nullptr;
}
