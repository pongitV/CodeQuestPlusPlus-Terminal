#pragma once

#include "Domain/Characters/Character.h"
#include "Systems/Inventory/Inventory.h"
#include "Domain/Items/Item.h"
#include "Domain/Items/ItemFactory.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "UI/Screens/Combat/CombatScreen.h"
#include <memory>
#include <functional>
#include <iostream>
#include <algorithm>

enum class TypeClass 
{
    None,
    Archer,
    Bard,
    Warrior,
    Wizard,
    NECROMANCER
};

enum class SkillID
{
    None,
    Determination,
    PlumbingArcane,
    FlashingLights,
    OnSight,
    ThroughTheWire,
    WithdrawalWithAim,
    SpectralInvocation
};

class Character;
class Item;
class Combat;

class BaseClass 
{
public:
    virtual ~BaseClass() = default;

    // INFORMACOES DA CLASSE
    virtual std::string getClassName() const = 0;
    virtual TypeClass getTypeClass() const = 0;
    virtual const std::vector<std::string>& getAppearanceClassMenu() const = 0;
    virtual Attributes getAttributesClass() const = 0;
    virtual std::vector<std::unique_ptr<Item>> getEquipmentClass() const = 0;
 
    virtual std::string getNamePassiveClass() const = 0;
    virtual std::string getDescriptionPassiveClass() const = 0;
    virtual std::string getRechargeSkillClass() const = 0;

    // HABILIDADE DA CLASSE
    virtual std::string getNameSkillClass() const = 0;
    virtual std::string getDescriptionSkillClass() const = 0;
    virtual void useSkillClass(Combat* combat, Character* characterUser, std::vector<Character*>& enemies) = 0;
    virtual TypeAttack getTypeAttack() const { return TypeAttack::UNIQUE; }
    virtual bool skillConsumeShift() const { return true; }

protected:
    void notifyMessageCombat(const std::string& msgWithColor, const std::string& /*msgSemCor*/) const {
        Appearance::registerBattleLog(msgWithColor);
    }

    bool checkEReportRecharge(Character* characterUser, int remainingTurns, const std::string& nameSkill) const {
        if (remainingTurns > 0) {
            std::cout << "\n" << CombatScreen::combatMargin() << Appearance::color(Color::RED) << "[SISTEMA]: A habilidade " << nameSkill << " esta em recarga (" << remainingTurns << " turnos)!" << Appearance::color(Color::RESET) << "\n";
            Appearance::registerBattleLog("[SISTEMA]: A habilidade " + nameSkill + " esta em recarga (" + std::to_string(remainingTurns) + " turnos)!");
            InputControl::waitForEnter();
            characterUser->setSkillCanceled(true);
            return true;
        }
        return false;
    }

public:
    // PASSIVAS DE CLASSE
    virtual int processCurePassiveBard(int cureBase) const { return cureBase; }
    virtual double processMultiplierBuffPassiveBard(double multBase) const { return multBase; }
    virtual int processPenaltyArmorPassiveArcher(int penaltyBase) const { return penaltyBase; }
    virtual int applyArcherPassiveSlownessPenalty(int dexterityCurrent) const { return dexterityCurrent / 2; }
    virtual int revertArcherPassiveSlownessPenalty(int dexterityCurrent) const { return dexterityCurrent * 2; }

    // PROCESSAMENTO DE DANO
    virtual void executeAttackWithClassPassive(Character* attacker, Character* defender, int damageBase, int damagePiercing, std::vector<std::unique_ptr<Character>>& enemies, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool isAttackerPlayer) {

        damageBase = processDamagePreAttack(attacker, defender, damageBase, isAttackerPlayer, enemies.size());

        bool isArea = attacker->getTypeAttack() == TypeAttack::AREA && isAttackerPlayer && !enemies.empty();

        if (isArea) {
            executeAttackArea(attacker, defender, damageBase, damagePiercing, enemies, applyDamage, isAttackerPlayer);
        } else {
            executeAttackUnique(attacker, defender, damageBase, damagePiercing, enemies, applyDamage, isAttackerPlayer);
        }
    }

protected:
    virtual void executeAttackArea(Character* attacker, Character* defender, int damageBase, int damagePiercing, std::vector<std::unique_ptr<Character>>& enemies, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool isAttackerPlayer) {
        std::string msgInfo = attacker->getName() + " desfere um ataque em area!";
        Appearance::registerBattleLog(msgInfo);
        int damageDivided = std::max(1, damageBase / static_cast<int>(enemies.size()));
        int piercingDivided = damagePiercing / static_cast<int>(enemies.size());

        bool activatedPassive = false;
        for (auto& enemyCurrent : enemies) {
            applyDamage(attacker, enemyCurrent.get(), damageDivided, piercingDivided);
            processDamagePostAttack(attacker, enemyCurrent.get(), defender, damageBase, damagePiercing, applyDamage, isAttackerPlayer, true, activatedPassive);
        }
    }

    virtual void executeAttackUnique(Character* attacker, Character* defender, int damageBase, int damagePiercing, std::vector<std::unique_ptr<Character>>& enemies, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool isAttackerPlayer) {
        if (defender != nullptr) {
            std::string msgInfo = attacker->getName() + " ataca " + defender->getName() + "!";
            Appearance::registerBattleLog(msgInfo);
            applyDamage(attacker, defender, damageBase, damagePiercing);
        }

        bool activatedPassive = false;
        for (auto& enemyCurrent : enemies) {
            processDamagePostAttack(attacker, enemyCurrent.get(), defender, damageBase, damagePiercing, applyDamage, isAttackerPlayer, false, activatedPassive);
        }
    }

    virtual int processDamagePreAttack(Character* /*atacante*/, Character* /*defensor*/, int damageBase, bool /*isAtacanteJogador*/, size_t /*qtdInimigos*/) { return damageBase; }
    virtual void processDamagePostAttack(Character* /*atacante*/, Character* /*alvoAtual*/, Character* /*defensorPrincipal*/, int /*danoBase*/, int /*danoPerfurante*/, const std::function<void(Character*, Character*, int, int)>& /*aplicarDano*/, bool /*isAtacanteJogador*/, bool /*isArea*/, bool& /*ativouPassiva*/) {}
};
