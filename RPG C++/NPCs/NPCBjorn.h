#pragma once

#include "../Sistemas/SistemaPersonagem.h"
#include <string>
#include <vector>

class NPCBjorn 
{
public:
    static void interagir(SistemaPersonagem* jogadorAtual);
    static std::vector<std::string> obterMapaForja();
};
