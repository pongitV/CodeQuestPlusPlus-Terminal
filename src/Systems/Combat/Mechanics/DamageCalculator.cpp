#include "Systems/Combat/Mechanics/DamageCalculator.h"
#include "Domain/Interfaces/IAttacker.h"
#include "Domain/Interfaces/IDamageable.h"

std::pair<int, int> DamageCalculator::calculateDamageOffensiveBase(IAttacker* attacker) {
    if (attacker) {
        return attacker->calculateDamageOffensiveBase();
    }
    return {0, 0};
}

int DamageCalculator::calculateMitigationDefensive(IDamageable* target, int damageGross, int damagePiercing) {
    if (target) {
        return target->calculateDefenseBase(damageGross, damagePiercing);
    }
    return damageGross + damagePiercing;
}
