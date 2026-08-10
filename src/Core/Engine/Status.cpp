/*
 * Arquivo: Status.cpp
 * Proposito: Implementacao das logicas de aplicacao e remocao de efeitos de status nos personagens.
 */

#include "Core/Engine/Status.h"

#include <iostream>

#include "Domain/Characters/Classes/BaseClass.h"
#include "Domain/Characters/Character.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/DialogFunctions.h"
#include "UI/Screens/Combat/CombatScreen.h"

void BloodSuckEffect::applyTurnStart(Character* target) {
    if (!Character::isValid(attacker) || attacker->getHealth() <= 0) return;
    int rootsDamage = target->getHealth() / 5;
    if (rootsDamage > 0) 
    {
        target->modifyHealth(-rootsDamage);
        attacker->modifyHealth(rootsDamage);
        Appearance::registerBattleLog(DialogueFunctions::formatStatusMsg("Drenou " + std::to_string(rootsDamage) + " de HP de " + target->getName() + " e curou " + attacker->getName() + "!", Color::GREEN));
    }
}

void NecrosisEffect::applyTurnStart(Character* target) {
    if (target->getHealth() <= 0) return;
    target->modifyHealth(-damagePerTurn);
    Color necrosisColor = (target->getClassName() != "Monstro") ? Color::LIGHT_RED : Color::RED;
    Appearance::registerBattleLog(DialogueFunctions::formatStatusMsg(target->getName() + " sofreu " + std::to_string(damagePerTurn) + " de dano por necrose!", necrosisColor));
}

void SlownessEffect::onEnterMap(Character* target) {
    if (target->getClass()) target->getFinalAttributes().dexterity = target->getClass()->applyArcherPassiveSlownessPenalty(target->getFinalAttributes().dexterity);
    else target->getFinalAttributes().dexterity /= 2;
}

void SlownessEffect::onExitMap(Character* target) {
    if (target->getClass()) target->getFinalAttributes().dexterity = target->getClass()->revertArcherPassiveSlownessPenalty(target->getFinalAttributes().dexterity);
    else target->getFinalAttributes().dexterity *= 2;
    Appearance::registerBattleLog(DialogueFunctions::formatStatusMsg(target->getName() + " se livrou da gosma e recuperou sua agilidade.", Color::MAGENTA));
}

void WeaknessEffect::onEnterMap(Character* target) {
    lostStrength = target->getFinalAttributes().strength / 4;
    target->getFinalAttributes().strength -= lostStrength;
}

void WeaknessEffect::onExitMap(Character* target) {
    target->getFinalAttributes().strength += lostStrength;
    Appearance::registerBattleLog(DialogueFunctions::formatStatusMsg(target->getName() + " recuperou sua forca original.", Color::RED));
}

void ResistanceBreakEffect::onEnterMap(Character* target) {
    lostResistance = static_cast<int>(target->getFinalAttributes().resistance * 0.20);
    lostConstitution = static_cast<int>(target->getFinalAttributes().constitution * 0.10);
    target->getFinalAttributes().resistance -= lostResistance;
    target->getFinalAttributes().constitution -= lostConstitution;
}

void ResistanceBreakEffect::onExitMap(Character* target) {
    target->getFinalAttributes().resistance += lostResistance;
    target->getFinalAttributes().constitution += lostConstitution;
}

void ResistanceBreakEffect::applyTurnStart(Character* target) {
    Appearance::registerBattleLog(DialogueFunctions::formatStatusMsg(target->getName() + " continua enfraquecido pelo po magico! (-" + std::to_string(lostResistance) + " Res, -" + std::to_string(lostConstitution) + " Con)", Color::CYAN));
}

void BleedingEffect::applyTurnStart(Character* target) {
    if (target->getHealth() <= 0) return;
    target->modifyHealth(-damagePerTurn);
    Color bleedingColor = (target->getClassName() != "Monstro") ? Color::LIGHT_RED : Color::RED;
    Appearance::registerBattleLog(DialogueFunctions::formatStatusMsg(target->getName() + " sofreu " + std::to_string(damagePerTurn) + " de dano por sangramento!", bleedingColor));
}

int HalfDamageEffect::processReceivedDamage(int damage) {
    int reducedDamage = damage / 2;
    Appearance::registerBattleLog(DialogueFunctions::formatStatusMsg("O dano foi reduzido pela metade! (Through the wire)", Color::CYAN));
    return reducedDamage;
}

void WarCryEffect::onEnterMap(Character* target) {
    target->getFinalAttributes().strength += strengthBonus;
    target->getFinalAttributes().dexterity += dexterityBonus;
}

void WarCryEffect::onExitMap(Character* target) {
    target->getFinalAttributes().strength -= strengthBonus;
    target->getFinalAttributes().dexterity -= dexterityBonus;
}

void AttributesBuffEffect::onExitMap(Character* target) {
    if (target->getMultiplier() != 1.0) {
        target->setMultiplier(1.0);
    }
    Appearance::registerBattleLog(DialogueFunctions::formatSystemMsg("O efeito da habilidade expirou!", Color::LIGHT_GREEN));
}

void AdaptationWheelEffect::applyTurnStart(Character* target) {
    if (target->getHealth() <= 0) return;
    if (!target->getArmor() || !target->getArmor()->hasProperty(Property::AdaptationArmor)) return;
    
    int healing = target->getMaxHealth() * 0.05;
    if (healing > 0) {
        target->modifyHealth(healing);
        CombatScreen::addFixedMessage(CombatScreen::combatMargin() + Appearance::color(Color::GREEN) + ">> A Roda gira... Regenerou " + std::to_string(healing) + " HP!" + Appearance::color(Color::RESET) + "\n");
        Appearance::registerBattleLog(DialogueFunctions::formatSkillMsg("A Roda gira... Regenerou " + std::to_string(healing) + " HP!", Color::GREEN));
    }
}

void AdaptationWheelEffect::onExitMap(Character* target) {
    target->getFinalAttributes().strength -= bStrength;
    target->getFinalAttributes().dexterity -= bDexterity;
    target->getFinalAttributes().resistance -= bResistance;
    target->getFinalAttributes().constitution -= bConstitution;
    target->getFinalAttributes().intelligence -= bIntelligence;
    target->getFinalAttributes().wisdom -= bWisdom;
    target->forceCacheRecalculation();
}

void AdaptationWheelEffect::adapt(Character* target, Character* enemy) {
    if (!enemy) return;
    
    // --- 1. Adaptacao Defensiva (Baseada no inimigo) ---
    int enemyPhysicalStrength = enemy->getStrength() + enemy->getDexterity();
    int enemyMagicalStrength = enemy->getIntelligence() + enemy->getWisdom();
    
    std::string defenseMsg;
    if (enemyPhysicalStrength >= enemyMagicalStrength) {
        target->changeStaticAttribute(AttributeType::Resistance, 2); 
        target->changeStaticAttribute(AttributeType::Constitution, 2);
        bResistance += 2; bConstitution += 2;
        defenseMsg = "defesa fisica";
    } else {
        target->changeStaticAttribute(AttributeType::Wisdom, 2); 
        target->changeStaticAttribute(AttributeType::Constitution, 2);
        bWisdom += 2; bConstitution += 2;
        defenseMsg = "defesa magica";
    }

    // --- 2. Adaptacao Ofensiva (Baseada na arma do jogador) ---
    int weaponPhysicalDamage = 1;
    int weaponMagicalDamage = 0;
    if (target->getWeapons()) {
        weaponPhysicalDamage = target->getWeapons()->getPhysicsDamage();
        weaponMagicalDamage = target->getWeapons()->getMagicalDamage();
    }

    std::string attackMsg;
    if (weaponMagicalDamage > weaponPhysicalDamage) {
        target->changeStaticAttribute(AttributeType::Intelligence, 2); 
        target->changeStaticAttribute(AttributeType::Wisdom, 2);
        bIntelligence += 2; bWisdom += 2;
        attackMsg = "poder magico";
    } else if (weaponPhysicalDamage > weaponMagicalDamage) {
        target->changeStaticAttribute(AttributeType::Strength, 2); 
        target->changeStaticAttribute(AttributeType::Dexterity, 2);
        bStrength += 2; bDexterity += 2;
        attackMsg = "poder fisico";
    } else {
        // Armas hibridas (ex: Espada de Exterminio)
        target->changeStaticAttribute(AttributeType::Strength, 2); 
        target->changeStaticAttribute(AttributeType::Dexterity, 2);
        target->changeStaticAttribute(AttributeType::Intelligence, 2);
        bStrength += 2; bDexterity += 2; bIntelligence += 2;
        attackMsg = "poder hibrido";
    }

    CombatScreen::addFixedMessage(CombatScreen::combatMargin() + "\033[5m" + Appearance::color(Color::YELLOW) + "* KLINK! *" + Appearance::color(Color::RESET) + " A Roda adapta " + defenseMsg + " e " + attackMsg + " (+2)!\n");
}
void InvincibleEffect::onExitMap(Character* target) { target->addEffect(std::make_unique<SureStrikeEffect>(99)); }
