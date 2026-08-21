#pragma once

#include "Domain/Characters/Classes/BaseClass.h"

class Item;
class Combat;

class Wizard : public BaseClass
{
private:
    TypeAttack typeAttackCurrent = TypeAttack::UNIQUE;

public:
    // [PT-BR] --- INFORMACOES DA CLASSE ---
    // [EN-US] --- CLASS INFORMATION ---
    std::string getClassName() const override;
    TypeClass getTypeClass() const override { return TypeClass::Wizard; }
    const std::vector<std::string>& getAppearanceClassMenu() const override;
    Attributes getAttributesClass() const override;
    std::vector<std::unique_ptr<Item>> getEquipmentClass() const override;

    // [PT-BR] --- PASSIVA DA CLASSE ---
    // [EN-US] --- CLASS PASSIVE ---
    std::string getNamePassiveClass() const override;
    std::string getDescriptionPassiveClass() const override;

    // [PT-BR] --- HABILIDADE DA CLASSE ---
    // [EN-US] --- CLASS SKILL ---
    std::string getRechargeSkillClass() const override;
    std::string getNameSkillClass() const override;
    std::string getDescriptionSkillClass() const override;
    void useSkillClass(Combat* combat, Character* characterUser, std::vector<Character*>& enemies) override;

protected:
    // [PT-BR] --- PROCESSAMENTO DE DANO ---
    // [EN-US] --- DAMAGE PROCESSING ---
    int processDamagePreAttack(Character* attacker, Character* defender, int damageBase, bool isAttackerPlayer, size_t enemyCount) override;
    void processDamagePostAttack(Character* attacker, Character* targetCurrent, Character* defenderMain, int damageBase, int damagePiercing, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool isAttackerPlayer, bool isArea, bool& activatedPassive) override;
};
