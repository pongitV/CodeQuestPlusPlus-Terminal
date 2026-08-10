#include "Domain/Items/ItemFactory.h"
#include "Domain/Items/Equipment/WeaponEquipment.h"
#include "Domain/Items/Equipment/ShieldEquipment.h"
#include "Domain/Items/Equipment/ArmorEquipment.h"
#include "Domain/Items/Items/ConsumableItem.h"
#include "Domain/Items/Items/MaterialItem.h"
#include "Domain/Items/Items/QuestItem.h"

#include "Domain/Characters/Character.h"
#include "Core/Utils/Appearance.h"

#include <functional>
#include <utility>
#include <unordered_map>

namespace {
    const std::pair<const char*, ItemID> globalNameToIDMap[] = {
        {"Adaga artesanal de pedra", ItemID::DaggerStone},
        {"Arco recurvo de madeira", ItemID::BowWood},
        {"Cajado de cristal magico", ItemID::StaffCrystal},
        {"Varinha corroida", ItemID::WandCorroded},
        {"Violao encantado", ItemID::ViolaEnchanted},
        {"Cajado de osso", ItemID::StaffBone},
        {"Espada longa de ferro", ItemID::SwordIron},
        {"Machado de guerra danificado", ItemID::AxWar},
        {"Gosma acida corrosiva", ItemID::SlimeAcidWeapon},
        {"Tronco de arvore amarrotado", ItemID::TrunkRumpled},
        {"Espada do Cavaleiro", ItemID::SwordKnight},
        
        {"Escudo medio de metal", ItemID::ShieldMetal},
        {"Barreira magica", ItemID::BarrierMagic},
        {"Capa magica", ItemID::CoverMagic},
        {"Bracedeiras de prata", ItemID::ArmbandsSilver},
        
        {"Armadura de malha e metal", ItemID::ArmorMesh},
        {"Armadura leve de couro com malha", ItemID::ArmorLeather},
        {"Tunica", ItemID::Tunic},
        {"Traje de Couro e tecido nobre", ItemID::CostumeNoble},
        {"Armadura de trapos e sucata", ItemID::ArmorRags},
        {"Armadura de Cavaleiro", ItemID::ArmorKnight},
        {"Roupas de Ritualista", ItemID::ClothesRitualist},
        {"Armadura de bau", ItemID::ArmorBao},
        {"Roda da Adaptacao", ItemID::AdaptationWheel},
        
        {"Pocao de Cura (30%VM)", ItemID::LittleCure30},
        {"Pocao de Furia (Buff)", ItemID::LittleFury},
        {"Elixir Arcano (Buff)", ItemID::ElixirArcane},
        {"Frasco de Gosma (Debuff)", ItemID::BottleSlime},
        {"Frasco de Fraqueza (Debuff)", ItemID::BottleWeakness},
        {"Orgao regenerador", ItemID::OrganRegenerator},
        {"Talisma do Urso", ItemID::TalismanBear},
        {"Talisma do Corvo", ItemID::TalismanCrow},
        {"Talisma do Leopardo", ItemID::TalismanLeopard},
        {"Talisma da Coruja", ItemID::TalismanOwl},
        {"Maca", ItemID::Litter},
        {"Pao", ItemID::Bread},
        {"Queijo", ItemID::Cheese},
        {"Carne Seca", ItemID::MeatDrought},
        {"Pocao de Cura Grande (50%VM)", ItemID::LittleCureBig},
        {"Pocao de Forca Alquimica", ItemID::LittleGallowsAlchemy},
        {"Pocao de Veneno Alquimica (Debuff)", ItemID::LittlePoisonAlchemy},
        {"Pocao de Lentidao Alquimica (Debuff)", ItemID::LittleSlowAlchemy},
        
        {"Gosma acida", ItemID::SlimeAcid},
        {"Dente de goblin", ItemID::ToothGoblin},
        {"Nucleo pegajoso", ItemID::NucleusSticky},
        {"Po magico", ItemID::PoMagician},
        {"Madeira enfeiticada", ItemID::WoodBewitched},
        {"Coracao da floresta", ItemID::HeartForest},
        {"Pedra magica de upgrade", ItemID::StoneUpgrade},
        {"Convite Real", ItemID::InvitationReal},
        
        {"Dispositivo de teclas de linguagem desconhecida", ItemID::DeviceLanguage}
    };

    struct MapsItems {
        std::unordered_map<ItemID, std::string> idForName;
        std::unordered_map<std::string, ItemID> nameForId;
        MapsItems() {
            for (const auto& pair : globalNameToIDMap) {
                idForName[pair.second] = pair.first;
                nameForId[pair.first] = pair.second;
            }
        }
    };

    const MapsItems& getMapsItems() {
        static MapsItems maps;
        return maps;
    }
}

std::string ItemFactory::getNameFromID(ItemID id) {
    const auto& map = getMapsItems().idForName;
    auto it = map.find(id);
    return it != map.end() ? it->second : "";
}

ItemID ItemFactory::getIDFromName(const std::string& name) {
    const auto& map = getMapsItems().nameForId;
    auto it = map.find(name);
    return it != map.end() ? it->second : ItemID::None;
}

std::vector<std::unique_ptr<Item>> ItemFactory::createSeveralItems(ItemID id, int quantity) {
    std::vector<std::unique_ptr<Item>> items;
    items.reserve(quantity);
    for (int i = 0; i < quantity; ++i) {
        items.push_back(createItem(id));
    }
    return items;
}

std::vector<std::unique_ptr<Item>> ItemFactory::createPotionKit(int quantity) {
    return createSeveralItems(ItemID::LittleCure30, quantity);
}


std::unique_ptr<Item> ItemFactory::createItem(const std::string& name) 
{
    if (!name.empty() && name.back() == '+') {
        std::string nameBase = name.substr(0, name.length() - 1);
        auto itemBase = createItem(nameBase);
        if (itemBase) return itemBase->generateCopyImproved();
        return nullptr;
    }

    ItemID id = getIDFromName(name);
    if (id != ItemID::None) return createItem(id);

    return nullptr;
}

std::unique_ptr<Item> ItemFactory::createItem(ItemID id) {
    static const std::vector<std::function<std::unique_ptr<Item>(ItemID)>> manufacturerChain = {
        manufactureEquipmentWeapon, manufactureEquipmentShield, manufactureEquipmentArmor,
        manufactureItemConsumable, manufactureItemMaterial, manufactureItemMission
    };
    for (const auto& manufacturer : manufacturerChain) {
        if (auto item = manufacturer(id)) return item;
    }
    return nullptr;
}
