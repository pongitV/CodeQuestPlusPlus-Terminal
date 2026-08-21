#include "Domain/Enemies/BaseEnemyClass.h"

// [PT-BR] --- INFORMACOES DA CLASSE ---
// [EN-US] --- CLASS INFORMATION ---
std::string BaseEnemyClass::getClassName() const 
{ 
    return "Monstro"; 
}

Attributes BaseEnemyClass::getAttributesClass() const 
{ 
    return { 0, 0, 0, 0, 0, 0, 0 }; 
}

const std::vector<std::string>& BaseEnemyClass::getAppearanceClassMenu() const 
{ 
    static const std::vector<std::string> appearanceEmpty = {};
    return appearanceEmpty;
}

std::vector<std::unique_ptr<Item>> BaseEnemyClass::getEquipmentClass() const 
{ 
    return {}; 
}

// [PT-BR] --- PASSIVA DA CLASSE ---
// [EN-US] --- CLASS PASSIVE ---
std::string BaseEnemyClass::getNamePassiveClass() const { return "Nenhuma"; }
std::string BaseEnemyClass::getDescriptionPassiveClass() const { return "Inimigos nao possuem passivas de classe."; }

// [PT-BR] --- HABILIDADE DA CLASSE ---
// [EN-US] --- CLASS SKILL ---
std::string BaseEnemyClass::getRechargeSkillClass() const { return ""; }
std::string BaseEnemyClass::getNameSkillClass() const { return "Nenhuma"; }
std::string BaseEnemyClass::getDescriptionSkillClass() const { return "Inimigos basicos nao possuem habilidades ativas."; }
void BaseEnemyClass::useSkillClass(Combat* /*combate*/, Character* /*userCharacter*/, std::vector<Character*>& /*enemyList*/) 
{
}

TypeAttack BaseEnemyClass::getTypeAttack() const { return TypeAttack::UNIQUE; }
bool BaseEnemyClass::skillConsumeShift() const { return true; }
