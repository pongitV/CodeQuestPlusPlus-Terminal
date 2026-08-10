#pragma once

#include "Domain/Characters/Races/BaseRace.h"

class Dwarf : public BaseRace
{
public:
    std::string getRaceName() const override;
    TypeRace getTypeRace() const override { return TypeRace::Dwarf; }
    const std::vector<std::string>& getAppearanceRace() const override;
    Attributes getAttributesRace() const override;

    std::string getNameSkillRace() const override;
    std::string getDescriptionSkillRace() const override;
    int processDamageDefensive(int finalDamage, Character* defender) override;
};
