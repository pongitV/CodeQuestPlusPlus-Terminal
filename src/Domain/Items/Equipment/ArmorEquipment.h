#pragma once

#include <string>
#include <memory>

#include "Domain/Items/Item.h"
#include "Domain/Characters/Character.h"

class EquipmentArmor : public Item 
{
private:
    std::string name;
    int reductionFixed;
    int reqResistance;
    int reqConstitution;
    int penaltyDexterity;

public:
    EquipmentArmor(const std::string& name, int reductionFixed, int reqResistance, int reqConstitution, int price = 3);
    
    int getReqResistance() const;
    int getReqConstitution() const;

    std::string getItemName() const override;
    EquipmentType getType() const override;

    int getReductionFixed() const override;
    void setPenaltyDexterity(int pen) { penaltyDexterity = pen; }

    std::string getInfoStatus() const override;

    bool canBeEquippedBy(Character* character) const override;
    bool isEquipable() const override { return true; }
    std::vector<std::string> getDetailsInspection(Character* character = nullptr) const override;

    std::unique_ptr<Item> generateCopyImproved() const override;
};

std::unique_ptr<Item> manufactureEquipmentArmor(ItemID id);
