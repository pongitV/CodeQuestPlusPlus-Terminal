#pragma once

#include "Domain/Characters/Classes/BaseClass.h"

class Item;
class Combat;

class Wizard : public BaseClass
{
private:
    TypeAttack typeAttackCurrent = TypeAttack::UNIQUE;

public:
    // INFORMACOES DA CLASSE
    std::string getClassName() const override;
    TypeClass getTypeClass() const override { return TypeClass::Wizard; }
    const std::vector<std::string>& getAppearanceClassMenu() const override;
    Attributes getAttributesClass() const override;
    std::vector<std::unique_ptr<Item>> getEquipmentClass() const override;

    // PASSIVA DA CLASSE
    std::string getNamePassiveClass() const override;
    std::string getDescriptionPassiveClass() const override;

    // HABILIDADE DA CLASSE
    std::string getRechargeSkillClass() const override;
    std::string getNameSkillClass() const override;
    std::string getDescriptionSkillClass() const override;
    void useSkillClass(Combat* combat, Character* characterUser, std::vector<Character*>& enemies) override;

protected:
    // PROCESSAMENTO DE DANO 
    int processDamagePreAttack(Character* attacker, Character* defender, int damageBase, bool isAttackerPlayer, size_t enemyCount) override;
    void processDamagePostAttack(Character* attacker, Character* targetCurrent, Character* defenderMain, int damageBase, int damagePiercing, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool isAttackerPlayer, bool isArea, bool& activatedPassive) override;
};
