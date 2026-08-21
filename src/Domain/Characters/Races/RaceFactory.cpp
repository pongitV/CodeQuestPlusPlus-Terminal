#include "Domain/Characters/Races/RaceFactory.h"
#include "Domain/Characters/Races/Dwarf.h"
#include "Domain/Characters/Races/Elf.h"
#include "Domain/Characters/Races/Human.h"
#include "Domain/Characters/Races/Orc.h"
// [PT-BR] Inclusao de novos tipos de racas conforme expansoes futuras
// [EN-US] Inclusion of new race types as needed for future expansions

std::unique_ptr<BaseRace> RaceFactory::createRace(TypeRace type) {
    switch (type) {
        case TypeRace::Dwarf: return std::make_unique<Dwarf>();
        case TypeRace::Elf: return std::make_unique<Elf>();
        case TypeRace::Human: return std::make_unique<Human>();
        case TypeRace::Ork: return std::make_unique<Ork>();
        default: return nullptr;
    }
}

std::vector<TypeRace> RaceFactory::getPlayableBreeds() {
    return {TypeRace::Dwarf, TypeRace::Elf, TypeRace::Human, TypeRace::Ork};
}
