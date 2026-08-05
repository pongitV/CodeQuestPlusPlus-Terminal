#include "UI/Screens/Attributes/AttributesScreen.h"
#include "UI/PerspectiveManager.h"
#include "UI/Screens/BaseScreen.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Core/Utils/DialogFunctions.h"
#include "Domain/Items/Item.h"

PowerCombat AttributesScreen::calculatePowerCombat(Character* currentPlayer, double multiplier) {
    int damageFis = 1, damageMag = 0;
    if (currentPlayer->getWeapons()) {
        damageFis = currentPlayer->getWeapons()->getPhysicsDamage();
        damageMag = currentPlayer->getWeapons()->getMagicalDamage();
    }
    int strength = currentPlayer->getStrength();
    int dexterity = currentPlayer->getDexterity();
    int intelli = currentPlayer->getIntelligence();
    int wisdom = currentPlayer->getWisdom();

    if (damageFis == 0 && damageMag > 0) { strength /= 10; dexterity /= 10; }
    else if (damageFis > 0 && damageMag == 0) { intelli /= 10; wisdom /= 10; }

    PowerCombat p;
    p.damageFisIs = std::max(0, static_cast<int>((damageFis + strength) * (1.0 + (dexterity / 100.0)) * multiplier));
    p.damageMagIs = std::max(0, static_cast<int>((damageMag + intelli) * (1.0 + (wisdom / 100.0)) * multiplier));
    p.defFixed = currentPlayer->getResistance();
    p.mitigation = std::min(50.0, currentPlayer->getConstitution() / 2.0);
    return p;
}

DebuffInfo AttributesScreen::calculateDebuff(Character* currentPlayer) {
    DebuffInfo d;
    d.hasBuff = (currentPlayer->getShiftsEffect(EffectID::AttributesBuff) > 0 &&
                 currentPlayer->getMultiplier() > 1.0);
    d.lostStrength    = currentPlayer->ownsEffect(EffectID::Weakness)     ? (currentPlayer->getStrength() / 3)      : 0;
    d.dexterityLost = currentPlayer->ownsEffect(EffectID::Slowness)     ? currentPlayer->getDexterity()       : 0;
    d.resLost      = currentPlayer->ownsEffect(EffectID::ResistanceBreak) ? currentPlayer->getResistance() : 0;
    d.constLost    = currentPlayer->ownsEffect(EffectID::ResistanceBreak) ? (currentPlayer->getConstitution() / 2) : 0;
    return d;
}

void AttributesScreen::display(Character* currentPlayer)
{
    PerspectiveManager::getAttributesUI().display(currentPlayer);
}

void AttributesScreen::managePlayerCharacterSheet(Character* currentPlayer)
{
    if (currentPlayer == nullptr) return;

    PerspectiveManager::getAttributesUI().managePlayerCharacterSheet(currentPlayer);
}
