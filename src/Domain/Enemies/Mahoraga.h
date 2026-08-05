#pragma once

#include "Domain/Characters/Races/BaseRace.h"

class Mahoraga : public BaseRace {
private:
    int parrysSuffered = 0;
    int defensesWithShieldSuffered = 0;
public:
    std::string getRaceName() const override;
    TypeRace getTypeRace() const override { return TypeRace::Mahoraga; }
    Attributes getAttributesRace() const override;
    std::vector<std::unique_ptr<Item>> getEquipmentRace() const override;
    const std::vector<std::string>& getAppearanceRace() const override;

    std::string getNameSkillRace() const override;
    std::string getDescriptionSkillRace() const override;
    
    void aoCauseDamage(Character* attacker, Character* target, int damageCaused) override;
    void aoSufferParryPerfect() override;
    void aoHaveAttackBlockedByShield();
    bool ignoreParry() const override;
    bool ignoreShield() const override;

    InfoBestiary getInfoBestiary() const override;
    void performDrops(Character* enemy, Character* currentPlayer, std::vector<std::string>& obtainedItems, int& totalGold, int& totalXp) override;
};
