#pragma once

#include "Domain/Characters/Races/BaseRace.h"
#include <string>
#include <vector>
#include <memory>

class Mimic : public BaseRace
{
private:
    int goldStolenTotal = 0;

public:
    std::string getRaceName() const override;
    TypeRace getTypeRace() const override { return TypeRace::Mimic; }
    const std::vector<std::string>& getAppearanceRace() const override;
    Attributes getAttributesRace() const override;
    std::vector<std::unique_ptr<Item>> getEquipmentRace() const override;

    std::string getNameSkillRace() const override;
    std::string getDescriptionSkillRace() const override;
    InfoBestiary getInfoBestiary() const override;
    void onCausingDamage(Character* attacker, Character* target, int damageCaused) override;
    void performDrops(Character* enemy, Character* currentPlayer, std::vector<std::string>& obtainedItems, int& totalGold, int& totalXp) override;
};
