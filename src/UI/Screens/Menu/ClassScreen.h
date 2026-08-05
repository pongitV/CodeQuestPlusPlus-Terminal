#pragma once
#include <string>
#include "Domain/Characters/Classes/BaseClass.h"

class ClassScreen {
public:
    struct Result {
        int index = 0;
        bool returned = false;
        std::string name;
        TypeClass selectedClass = TypeClass::None;
    };
    static Result display(const std::string& namePlayer, const std::string& raceName);
};
