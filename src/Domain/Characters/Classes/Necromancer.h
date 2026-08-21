#pragma once

#include "Domain/Characters/Classes/BaseClass.h"

class Necromancer : public BaseClass {
public:
    // [PT-BR] --- INFORMACOES DA CLASSE ---
    // [EN-US] --- CLASS INFORMATION ---
    std::string getClassName() const override;
    TypeClass getTypeClass() const override { return TypeClass::NECROMANCER; }
    const std::vector<std::string>& getAppearanceClassMenu() const override;
    Attributes getAttributesClass() const override;
    std::vector<std::unique_ptr<Item>> getEquipmentClass() const override;

    // [PT-BR] --- HABILIDADE DA CLASSE ---
    // [EN-US] --- CLASS SKILL ---
    std::string getNameSkillClass() const override;
    std::string getDescriptionSkillClass() const override;
    std::string getRechargeSkillClass() const override;
    void useSkillClass(Combat* combat, Character* characterUser, std::vector<Character*>& enemies) override;

    // [PT-BR] --- PASSIVA DA CLASSE ---
    // [EN-US] --- CLASS PASSIVE ---
    std::string getNamePassiveClass() const override;
    std::string getDescriptionPassiveClass() const override;
    void executeAttackWithClassPassive(Character* attacker, Character* defender, int damageBase, int damagePiercing, std::vector<std::unique_ptr<Character>>& enemies, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool applyPassive) override;
};