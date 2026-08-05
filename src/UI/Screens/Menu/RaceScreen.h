#pragma once
#include <string>
#include "Domain/Characters/Races/BaseRace.h"

class RaceScreen {
public:
    struct Result {
        int index = 0;
        bool returned = false;
        std::string name;
        TypeRace selectedRace = TypeRace::None;
    };
    static Result display(const std::string& namePlayer);
};
