// [PT-BR] Arquivo: Item.h
// [PT-BR] Proposito: Classe base abstrata para representacao de itens (Equipamentos, Consumiveis, Materiais e Missoes).
// [EN-US] File: Item.h
// [EN-US] Purpose: Abstract base class representing items (Equipment, Consumables, Materials, and Quests).

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>
#include <algorithm>
#include "Core/Utils/Appearance.h"

// [PT-BR] Tipos de equipamentos e categorias de itens no jogo
// [EN-US] Equipment types and item categories in the game
enum class EquipmentType 
{
    NONE,     
    WEAPONS,       
    SHIELD,     
    ARMOR,   
    CONSUMABLE, 
    MISSION,
    MATERIAL
};

enum class Property 
{
    None,
    Magic,
    Penetrating,
    IgnoreDefense,
    ViolaBase,
    ViolaMagician,
    VinePrison,
    Improved,
    ImprovedMaterial,
    HealingConsumable,
    BuffConsumable,
    ConsumableDebuffSlow,
    ConsumableDebuffWeakness,
    StrengthTalisman,
    TalismanIntelligence,
    TalismanDexterity,
    TalismanWisdom,
    ConsumablePowerTroll,
    AdaptationArmor
};

enum class ItemID {
    None = 0,
    // [PT-BR] Armas
    // [EN-US] Weapons
    DaggerStone, BowWood, StaffCrystal, WandCorroded, ViolaEnchanted, SwordIron, AxWar, SlimeAcidWeapon, TrunkRumpled, SwordKnight, SwordExtermination, StaffBone,
    
    // [PT-BR] Escudos
    // [EN-US] Shields
    ShieldMetal, BarrierMagic, CoverMagic, ArmbandsSilver,
    
    // [PT-BR] Armaduras
    // [EN-US] Armors
    ArmorMesh, ArmorLeather, Tunic, CostumeNoble, ArmorRags, ArmorKnight, ArmorChest, AdaptationWheel, ClothesRitualist,
    
    // [PT-BR] Consumiveis
    // [EN-US] Consumables
    HealingPotion30, FuryPotion, ElixirArcane, BottleSlime, BottleWeakness, OrganRegenerator,
    TalismanBear, TalismanCrow, TalismanLeopard, TalismanOwl,
    Apple, Bread, Cheese, DriedMeat,
    GreatHealingPotion, AlchemicalStrengthPotion, AlchemicalPoisonPotion, AlchemicalSlownessPotion,
    
    // [PT-BR] Materiais
    // [EN-US] Materials
    SlimeAcid, ToothGoblin, NucleusSticky, MagicPowder, WoodBewitched, HeartForest, StoneUpgrade, RoyalInvitation,
    
    // [PT-BR] Missoes
    // [EN-US] Quests
    DeviceLanguage
};

class Character;

class Item 
{
protected:
    std::vector<Property> properties;
    int priceSale;
    std::function<void(Character*, Character*)> actionUse;
    std::function<bool(Item*, Character*, bool*)> actionInventory;
    std::vector<std::string> descriptionInspection;
public:
    Item(int price = 3) : priceSale(price) {}
    virtual ~Item() = default;
    virtual std::string getItemName() const = 0;
    virtual EquipmentType getType() const { return EquipmentType::NONE; }
    virtual int getPhysicsDamage() const { return 0; }
    virtual int getMagicalDamage() const { return 0; }
    virtual double getReductionPercentage() const { return 0.0; }
    virtual int getReductionFixed() const { return 0; }
    virtual int getReductionDamageFixedShield() const { return 0; }
    virtual int getDurabilityCurrentShield() const { return 0; }
    
    virtual void setDescriptionInspection(const std::vector<std::string>& desc) { descriptionInspection = desc; }
    virtual void setDescriptionInspection(const std::string& desc) { descriptionInspection = {desc}; }

    virtual bool canBeEquippedBy(Character* /*personagem*/) const { return true; }
    virtual bool isEquipable() const { return false; }
    virtual std::string getMessageRequirement() const { return "\n[SISTEMA]: Atributos insuficientes para equipar " + getItemName() + "!\n"; }
    
    virtual std::vector<std::string> getDetailsInspection(Character* /*personagem*/ = nullptr) const {
        std::vector<std::string> details;
        details.push_back(" > Tipo: Desconhecido");
        details.push_back(" > Descricao: Nenhuma informacao disponivel.");
        return details;
    }

    virtual void changeName(const std::string& /*n*/) {}
    virtual bool ownsEffectBleeding() const { return false; }
    virtual bool ownsEffectSlow() const { return false; }
    virtual void applyEffectBleeding() {}
    virtual void applyEffectSlow() {}

    virtual void reduceDurability(int /*qtd*/) {}
    virtual void increaseDurability(int /*qtd*/) {}
    
    virtual void beforeCausingDamage(Character* /*atacante*/, Character* /*alvo*/) {}
    virtual void onCausingDamage(Character* /*atacante*/, Character* /*alvo*/, int /*danoCausado*/) {}
    virtual int ensureDamageMinimum(int finalDamage) { return std::max(finalDamage, 1); }

    virtual int getPriceSale() const { return priceSale; }
    // [PT-BR] Retorna status adicional formatado do item (vazio por padrao)
    // [EN-US] Returns formatted additional status of the item (empty by default)
    virtual std::string getInfoStatus() const { return ""; }
    
    virtual void use(Character* user, Character* target) {
        if (user == nullptr || target == nullptr) return;
        if (actionUse) actionUse(user, target);
    }
    virtual void setActionUse(std::function<void(Character*, Character*)> action) { actionUse = action; }
    
    virtual void setActionInventory(std::function<bool(Item*, Character*, bool*)> action) { actionInventory = action; }
    virtual bool useFromInventory(Character* user, bool* shiftWasConsumed) {
        if (actionInventory) return actionInventory(this, user, shiftWasConsumed);
        return false;
    }

    virtual bool hasProperty(Property prop) const { 
        return std::find(properties.begin(), properties.end(), prop) != properties.end(); 
    }
    virtual void addProperty(Property prop) { 
        if (!hasProperty(prop)) properties.push_back(prop); 
    }
    virtual void removeProperty(Property prop) { 
        auto it = std::find(properties.begin(), properties.end(), prop);
        if (it != properties.end()) properties.erase(it); 
    }
    virtual const std::vector<Property>& getProperties() const { return properties; }
    virtual std::unique_ptr<Item> generateCopyImproved() const { return nullptr; }
};
