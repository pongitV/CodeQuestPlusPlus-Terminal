/*
 * Arquivo: Item.h
 * Propósito: Classe base abstrata para representação de itens (Equipamentos, Consumíveis, Materiais e Missões).
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>
#include <algorithm>
#include "Core/Utils/Appearance.h"

// Tipos de equipamentos e itens no jogo
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
    CipePrison,
    Improved,
    ImprovedMaterial,
    HealingConsumable,
    BuffConsumable,
    ConsumableDebuffSlow,
    ConsumableDebuffWeakness,
    TalismanGallows,
    TalismanIntelligence,
    TalismanDexterity,
    TalismanWisdom,
    ConsumablePowerTroll,
    AdaptationArmor
};

enum class ItemID {
    None = 0,
    // Armas
    DaggerStone, BowWood, StaffCrystal, WandCorroded, ViolaEnchanted, SwordIron, AxWar, SlimeAcidWeapon, TrunkRumpled, SwordKnight, SwordExtermination, StaffBone,
    
    // Escudos
    ShieldMetal, BarrierMagic, CoverMagic, ArmbandsSilver,
    
    // Armaduras
    ArmorMesh, ArmorLeather, Tunic, CostumeNoble, ArmorRags, ArmorKnight, ArmorBao, AdaptationWheel, ClothesRitualist,
    
    // Consumiveis
    LittleCure30, LittleFury, ElixirArcane, BottleSlime, BottleWeakness, OrganRegenerator,
    TalismanBear, TalismanCrow, TalismanLeopard, TalismanOwl,
    Litter, Bread, Cheese, MeatDrought,
    LittleCureBig, LittleGallowsAlchemy, LittlePoisonAlchemy, LittleSlowAlchemy,
    
    // Materiais
    SlimeAcid, ToothGoblin, NucleusSticky, PoMagician, WoodBewitched, HeartForest, StoneUpgrade, InvitationReal,
    
    // Missoes
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
    
    virtual void beforeDeCauseDamage(Character* /*atacante*/, Character* /*alvo*/) {}
    virtual void aoCauseDamage(Character* /*atacante*/, Character* /*alvo*/, int /*danoCausado*/) {}
    virtual int ensureDamageMinimum(int damageEnd) { return std::max(damageEnd, 1); }

    virtual int getPriceSale() const { return priceSale; }
    virtual std::string getInfoStatus() const { return ""; } // Vazio por padrão para itens sem status extra
    
    virtual void use(Character* user, Character* target) {
        if (user == nullptr || target == nullptr) return;
        if (actionUse) actionUse(user, target);
    }
    virtual void setActionUse(std::function<void(Character*, Character*)> action) { actionUse = action; }
    
    virtual void setActionInventory(std::function<bool(Item*, Character*, bool*)> action) { actionInventory = action; }
    virtual bool useDoInventory(Character* user, bool* shiftWasConsumed) {
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
