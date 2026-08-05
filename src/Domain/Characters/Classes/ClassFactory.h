#pragma once

#include <memory>
#include <vector>
#include "Domain/Characters/Classes/BaseClass.h"

class ClassFactory {
public:
    static std::unique_ptr<BaseClass> createClass(TypeClass type);
    static std::vector<TypeClass> getClassesPlayable();
};
