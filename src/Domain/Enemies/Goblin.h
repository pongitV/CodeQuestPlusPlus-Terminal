#pragma once

#include <string>
#include <vector>

#include "Domain/Characters/Races/BaseRace.h"

class Goblin : public BaseRace
{
public:
    std::string getRaceName() const override;
    TypeRace getTypeRace() const override { return TypeRace::Goblin; }
    Attributes getAttributesRace() const override;
    std::string getNameSkillRace() const override;
    std::string getDescriptionSkillRace() const override;
    const std::vector<std::string>& getAppearanceRace() const override;

    InfoBestiary getInfoBestiary() const override;

    std::vector<std::unique_ptr<Item>> getEquipmentRace() const override;
    void performDrops(Character* enemy, Character* currentPlayer, std::vector<std::string>& obtainedItems, int& totalGold, int& totalXp) override;
};
