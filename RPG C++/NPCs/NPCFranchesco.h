#pragma once

#include "../Sistema/Personagem.h"
#include <string>
#include <vector>

class NPCFranchesco
{
public:
    static void interagir(Personagem* jogadorAtual);
    static std::vector<std::string> obterMapaLoja();
};
