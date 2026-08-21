// [PT-BR] Arquivo: BaseEnemyClass.h
// [PT-BR] Proposito: Classe base generica para definicao de comportamentos e estatisticas de inimigos.
// [EN-US] File: BaseEnemyClass.h
// [EN-US] Purpose: Generic base class for defining enemy behaviors and statistics.

#pragma once

#include <string>
#include <vector>

#include "Domain/Characters/Classes/BaseClass.h"

// [PT-BR] Implementacao generica da interface BaseClass voltada a monstros e inimigos.
// [EN-US] Generic implementation of BaseClass interface targeted at monsters and enemies.
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
