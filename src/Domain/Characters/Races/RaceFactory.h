#pragma once

#include <memory>
#include "Domain/Characters/Races/BaseRace.h"

class RaceFactory {
public:
    static std::unique_ptr<BaseRace> createRace(TypeRace type);
    static std::vector<TypeRace> getPlayableBreeds();
};
