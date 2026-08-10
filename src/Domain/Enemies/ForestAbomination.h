#pragma once

#include <string>
#include <vector>

#include "Domain/Characters/Races/BaseRace.h"

class ForestAbomination : public BaseRace
{
private:
    bool healingActively = false;

public:
    std::string getRaceName() const override;
    TypeRace getTypeRace() const override { return TypeRace::ForestAbomination; }
    Attributes getAttributesRace() const override;
    std::string getNameSkillRace() const override;
    std::string getDescriptionSkillRace() const override;
    const std::vector<std::string>& getAppearanceRace() const override;

    InfoBestiary getInfoBestiary() const override;

    void onCausingDamage(Character* attacker, Character* target, int damageCaused) override;

    void performDrops(Character* enemy, Character* currentPlayer, std::vector<std::string>& obtainedItems, int& totalGold, int& totalXp) override;
};
