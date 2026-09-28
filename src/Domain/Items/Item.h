#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>
#include <algorithm>
#include "Core/Utils/Appearance.h"

// Tipos de equipamentos e categorias de itens no jogo
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
    // Armas
    DaggerStone, BowWood, StaffCrystal, WandCorroded, ViolaEnchanted, SwordIron, AxWar, SlimeAcidWeapon, TrunkRumpled, SwordKnight, SwordExtermination, StaffBone,
    
    // Escudos
    ShieldMetal, BarrierMagic, CoverMagic, ArmbandsSilver,
    
    // Armaduras
    ArmorMesh, ArmorLeather, Tunic, CostumeNoble, ArmorRags, ArmorKnight, ArmorChest, AdaptationWheel, ClothesRitualist,
    
    // Consumiveis
    HealingPotion30, FuryPotion, ElixirArcane, BottleSlime, BottleWeakness, OrganRegenerator,
    TalismanBear, TalismanCrow, TalismanLeopard, TalismanOwl,
    Apple, Bread, Cheese, DriedMeat,
    GreatHealingPotion, AlchemicalStrengthPotion, AlchemicalPoisonPotion, AlchemicalSlownessPotion,
    
    // Materiais
    SlimeAcid, ToothGoblin, NucleusSticky, MagicPowder, WoodBewitched, HeartForest, StoneUpgrade, RoyalInvitation,
    
    // Missoes
    DeviceLanguage
};

class Character;

class Item 
{
protected:
    std::vector<Property> properties;
    int sellPrice;
    int& priceSale = sellPrice; // Compatibilidade retroativa
    std::function<void(Character*, Character*)> useAction;
    std::function<void(Character*, Character*)>& actionUse = useAction;
    std::function<bool(Item*, Character*, bool*)> inventoryAction;
    std::function<bool(Item*, Character*, bool*)>& actionInventory = inventoryAction;
    std::vector<std::string> inspectionDescription;
    std::vector<std::string>& descriptionInspection = inspectionDescription;

public:
    Item(int price = 3) : sellPrice(price) {}
    virtual ~Item() = default;
    virtual std::string getItemName() const = 0;
    virtual EquipmentType getType() const { return EquipmentType::NONE; }
    
    virtual int getPhysicalDamage() const { return 0; }
    virtual inline int getPhysicsDamage() const { return getPhysicalDamage(); }
    
    virtual int getMagicalDamage() const { return 0; }
    virtual double getReductionPercentage() const { return 0.0; }
    virtual int getReductionFixed() const { return 0; }
    
    virtual int getShieldFixedDamageReduction() const { return 0; }
    virtual inline int getReductionDamageFixedShield() const { return getShieldFixedDamageReduction(); }
    
    virtual int getShieldCurrentDurability() const { return 0; }
    virtual inline int getDurabilityCurrentShield() const { return getShieldCurrentDurability(); }
    
    virtual void setInspectionDescription(const std::vector<std::string>& desc) { inspectionDescription = desc; }
    virtual void setInspectionDescription(const std::string& desc) { inspectionDescription = {desc}; }
    virtual inline void setDescriptionInspection(const std::vector<std::string>& desc) { setInspectionDescription(desc); }
    virtual inline void setDescriptionInspection(const std::string& desc) { setInspectionDescription(desc); }

    virtual bool canBeEquippedBy(Character* /*personagem*/) const { return true; }
    virtual bool isEquipable() const { return false; }
    virtual std::string getMessageRequirement() const { return "\n[SISTEMA]: Atributos insuficientes para equipar " + getItemName() + "!\n"; }
    
    virtual std::vector<std::string> getInspectionDetails(Character* /*personagem*/ = nullptr) const {
        std::vector<std::string> details;
        details.push_back(" > Tipo: Desconhecido");
        details.push_back(" > Descricao: Nenhuma informacao disponivel.");
        return details;
    }
    virtual inline std::vector<std::string> getDetailsInspection(Character* p = nullptr) const { return getInspectionDetails(p); }

    virtual void changeName(const std::string& /*n*/) {}
    
    virtual bool hasBleedingEffect() const { return false; }
    virtual inline bool ownsEffectBleeding() const { return hasBleedingEffect(); }
    
    virtual bool hasSlowEffect() const { return false; }
    virtual inline bool ownsEffectSlow() const { return hasSlowEffect(); }
    
    virtual void applyEffectBleeding() {}
    virtual void applyEffectSlow() {}

    virtual void reduceDurability(int /*qtd*/) {}
    virtual void increaseDurability(int /*qtd*/) {}
    
    virtual void beforeCausingDamage(Character* /*atacante*/, Character* /*alvo*/) {}
    virtual void onCausingDamage(Character* /*atacante*/, Character* /*alvo*/, int /*danoCausado*/) {}
    virtual int ensureDamageMinimum(int finalDamage) { return std::max(finalDamage, 1); }

    virtual int getSellPrice() const { return sellPrice; }
    virtual inline int getPriceSale() const { return getSellPrice(); }
    
    // Retorna status adicional formatado do item (vazio por padrao)
    virtual std::string getInfoStatus() const { return ""; }
    
    virtual void use(Character* user, Character* target) {
        if (user == nullptr || target == nullptr) return;
        if (useAction) useAction(user, target);
    }
    virtual void setUseAction(std::function<void(Character*, Character*)> action) { useAction = action; }
    virtual inline void setActionUse(std::function<void(Character*, Character*)> action) { setUseAction(action); }
    
    virtual void setInventoryAction(std::function<bool(Item*, Character*, bool*)> action) { inventoryAction = action; }
    virtual inline void setActionInventory(std::function<bool(Item*, Character*, bool*)> action) { setInventoryAction(action); }
    
    virtual bool useFromInventory(Character* user, bool* shiftWasConsumed) {
        if (inventoryAction) return inventoryAction(this, user, shiftWasConsumed);
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
    
    virtual std::unique_ptr<Item> generateUpgradedCopy() const { return nullptr; }
    virtual inline std::unique_ptr<Item> generateCopyImproved() const { return generateUpgradedCopy(); }
};
