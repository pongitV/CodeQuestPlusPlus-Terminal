#pragma once

#include <memory>
#include <vector>
#include "ClasseBase.h"

class FabricaClasses {
public:
    static std::unique_ptr<ClasseBase> criarClasse(TipoClasse tipo);
    static std::vector<TipoClasse> obterClassesJogaveis();
};
