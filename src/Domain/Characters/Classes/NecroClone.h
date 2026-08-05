#pragma once

#include "Domain/Characters/Classes/BaseClass.h"
#include "Domain/Characters/Races/BaseRace.h"
#include <string>
#include <vector>
#include <memory>

class Item;
class Combat;
class Character;

class RaceClone : public BaseRace {
private:
    std::string nameOriginal;
    std::vector<std::string> appearanceOriginal;
public:
    RaceClone(const std::string& n, const std::vector<std::string>& a);
    std::string getRaceName() const override;
    TypeRace getTypeRace() const override;
    const std::vector<std::string>& getAppearanceRace() const override;
    Attributes getAttributesRace() const override;
    std::string getNameSkillRace() const override;
    std::string getDescriptionSkillRace() const override;
};

class ClassClone : public BaseClass {
public:
    std::string getClassName() const override;
    TypeClass getTypeClass() const override;
    const std::vector<std::string>& getAppearanceClassMenu() const override;
    Attributes getAttributesClass() const override;
    std::vector<std::unique_ptr<Item>> getEquipmentClass() const override;
    std::string getNamePassiveClass() const override;
    std::string getDescriptionPassiveClass() const override;
    std::string getRechargeSkillClass() const override;
    std::string getNameSkillClass() const override;
    std::string getDescriptionSkillClass() const override;
    void useSkillClass(Combat*, Character*, std::vector<Character*>&) override;
};