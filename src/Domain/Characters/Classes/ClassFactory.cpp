#include "Domain/Characters/Classes/ClassFactory.h"
#include "Domain/Characters/Classes/Archer.h"
#include "Domain/Characters/Classes/Bard.h"
#include "Domain/Characters/Classes/Warrior.h"
#include "Domain/Characters/Classes/Mage.h"
#include "Domain/Characters/Classes/Necromancer.h"

std::unique_ptr<BaseClass> ClassFactory::createClass(TypeClass type) {
    switch (type) {
        case TypeClass::Archer: return std::make_unique<Archer>();
        case TypeClass::Bard: return std::make_unique<Bard>();
        case TypeClass::Warrior: return std::make_unique<Warrior>();
        case TypeClass::Wizard: return std::make_unique<Wizard>();
        case TypeClass::NECROMANCER: return std::make_unique<Necromancer>();
        default: return nullptr;
    }
}

std::vector<TypeClass> ClassFactory::getClassesPlayable() {
    return {TypeClass::Archer, TypeClass::Bard, TypeClass::Warrior, TypeClass::Wizard, TypeClass::NECROMANCER};
}
