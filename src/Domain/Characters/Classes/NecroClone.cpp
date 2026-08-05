#include "Domain/Characters/Classes/NecroClone.h"

RaceClone::RaceClone(const std::string& n, const std::vector<std::string>& a) : nameOriginal(n), appearanceOriginal(a) {}
std::string RaceClone::getRaceName() const { return nameOriginal; }
TypeRace RaceClone::getTypeRace() const { return TypeRace::None; }
const std::vector<std::string>& RaceClone::getAppearanceRace() const { return appearanceOriginal; }
Attributes RaceClone::getAttributesRace() const { return {0,0,0,0,0,0,0}; }
std::string RaceClone::getNameSkillRace() const { return "Ataque Basico"; }
std::string RaceClone::getDescriptionSkillRace() const { return "Ataca o inimigo."; }

std::string ClassClone::getClassName() const { return "Morto-Vivo"; }
TypeClass ClassClone::getTypeClass() const { return TypeClass::None; }
const std::vector<std::string>& ClassClone::getAppearanceClassMenu() const { static std::vector<std::string> empty; return empty; }
Attributes ClassClone::getAttributesClass() const { return {0,0,0,0,0,0,0}; }
std::vector<std::unique_ptr<Item>> ClassClone::getEquipmentClass() const { return {}; }
std::string ClassClone::getNamePassiveClass() const { return "Decomposicao"; }
std::string ClassClone::getDescriptionPassiveClass() const { return "O corpo reanimado se decompoe continuamente, perdendo 15% da Vida Maxima a cada turno do invocador."; }
std::string ClassClone::getRechargeSkillClass() const { return ""; }
std::string ClassClone::getNameSkillClass() const { return ""; }
std::string ClassClone::getDescriptionSkillClass() const { return ""; }
void ClassClone::useSkillClass(Combat*, Character*, std::vector<Character*>&) {}