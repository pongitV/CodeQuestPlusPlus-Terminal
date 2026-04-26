#pragma once

#include "../Sistema/Personagem.h"
#include <string>
#include <vector>

class NPCBjorn 
{
public:
    static void interagir(Personagem* jogadorAtual);
    static std::vector<std::string> obterMapaForja();
};
