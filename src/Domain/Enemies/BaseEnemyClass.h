/*
 * Arquivo: BaseEnemyClass.h
 * Proposito: Classe base generica para definicao de comportamentos e estatisticas de inimigos.
 */

#pragma once

#include <string>
#include <vector>

#include "Domain/Characters/Classes/BaseClass.h"

/*
 * Implementacao generica da interface BaseClass voltada a monstros e inimigos.
 */
class BaseEnemyClass : public BaseClass
{
public:
    std::string getClassName() const override;
    TypeClass getTypeClass() const override { return TypeClass::None; }
    Attributes getAttributesClass() const override;
    const std::vector<std::string>& getAppearanceClassMenu() const override;
    std::vector<std::unique_ptr<Item>> getEquipmentClass() const override;

    std::string getNamePassiveClass() const override;
    std::string getDescriptionPassiveClass() const override;
    std::string getRechargeSkillClass() const override;

    void useSkillClass(Combat* combat, Character* characterUser, std::vector<Character*>& enemies) override;
    std::string getNameSkillClass() const override;
    std::string getDescriptionSkillClass() const override;
    
    TypeAttack getTypeAttack() const override;
    bool skillConsumeShift() const override;
};
