#pragma once

#include "Domain/Items/Item.h"
#include <string>
#include <memory>
#include "Domain/Characters/Character.h"


class EquipmentShield : public Item 
{
private:
    std::string name;
    int reductionFixed;
    int durability;
    int durabilityMaximum;
    int reqResistance;
    int reqSecondary;
    AttributeType typeSecondary;

public:
    EquipmentShield(const std::string& name, int reductionFixed, int durability, int reqResistance, int reqSecondary, AttributeType typeSecondary, int price = 3);
    
    int getReqResistance() const;
    int getReqSecondary() const;
    AttributeType getTypeSecondary() const;

    std::string getItemName() const override;
    EquipmentType getType() const override;

    int getDurabilityCurrentShield() const override;
    int getDurabilityMaximum() const;
    int getReductionDamageFixedShield() const override;
    void setDurability(int newDurability);
    void reduceDurability(int qty) override;
    void increaseDurability(int qty) override;

    std::string getInfoStatus() const override;

    bool canBeEquippedBy(Character* character) const override;
    bool isEquipable() const override { return true; }
    std::vector<std::string> getDetailsInspection(Character* character = nullptr) const override;

    std::unique_ptr<Item> generateCopyImproved() const override;
};

std::unique_ptr<Item> manufactureEquipmentShield(ItemID id);
