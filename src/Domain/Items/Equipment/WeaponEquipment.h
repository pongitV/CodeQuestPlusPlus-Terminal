#pragma once

#include "Domain/Items/Item.h"
#include <string>
#include <set>
#include <memory>

class EquipmentWeapon : public Item 
{
private:
    std::string name;
    int damagePhysical;
    int damageMagician;
    int reqGallows;
    int reqDexterity;
    int reqIntelligence;
    int reqWisdom;
    bool effectBleeding;
    bool effectSlow;

public:
    EquipmentWeapon(const std::string& name, int damagePhysical, int damageMagician, int reqGallows, int reqDexterity, int reqIntelligence, int reqWisdom, int price = 3);
    
    int getReqGallows() const;
    int getReqDexterity() const;
    int getReqIntelligence() const;
    int getReqWisdom() const;

    std::string getItemName() const override;
    void changeName(const std::string& n) override;
    EquipmentType getType() const override;

    int getPhysicsDamage() const override;
    int getMagicalDamage() const override;
    
    bool ownsEffectBleeding() const override;
    bool ownsEffectSlow() const override;

    std::string getInfoStatus() const override;

    bool canBeEquippedBy(Character* character) const override;
    bool isEquipable() const override { return true; }
    std::vector<std::string> getDetailsInspection(Character* character = nullptr) const override;

    void applyEffectBleeding() override;
    void applyEffectSlow() override;
    
    void beforeDeCauseDamage(Character* attacker, Character* target) override;
    void aoCauseDamage(Character* attacker, Character* target, int damageCaused) override;
    int ensureDamageMinimum(int damageEnd) override;

    std::unique_ptr<Item> generateCopyImproved() const override;
};

std::unique_ptr<Item> manufactureEquipmentWeapon(ItemID id);
