#pragma once

#include "Domain/Characters/Classes/BaseClass.h"

class Item;
class Combat;

class Bard : public BaseClass
{
public:
    // [PT-BR] --- INFORMACOES DA CLASSE ---
    // [EN-US] --- CLASS INFORMATION ---
    std::string getClassName() const override; 
    TypeClass getTypeClass() const override { return TypeClass::Bard; } 
    const std::vector<std::string>& getAppearanceClassMenu() const override;
    Attributes getAttributesClass() const override;
    std::vector<std::unique_ptr<Item>> getEquipmentClass() const override;

    // [PT-BR] --- PASSIVA DA CLASSE ---
    // [EN-US] --- CLASS PASSIVE ---
    std::string getNamePassiveClass() const override;
    std::string getDescriptionPassiveClass() const override;
    int processCurePassiveBard(int cureBase) const override;
    double processMultiplierBuffPassiveBard(double multBase) const override;

    // [PT-BR] --- HABILIDADE DA CLASSE ---
    // [EN-US] --- CLASS SKILL ---
    std::string getRechargeSkillClass() const override;
    std::string getNameSkillClass() const override;
    std::string getDescriptionSkillClass() const override;
    void useSkillClass(Combat* combat, Character* characterUser, std::vector<Character*>& enemies) override;
};
