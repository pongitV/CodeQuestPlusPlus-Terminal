#pragma once

#include "../Sistema/Personagem.h"

class NPCFranchesco 
{
public:
    static void interagir(Personagem* jogadorAtual);
    static std::vector<std::string> obterMapaLoja();
};
