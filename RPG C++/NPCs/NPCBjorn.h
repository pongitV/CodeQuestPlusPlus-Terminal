#pragma once

#include "../Sistema/Personagem.h"

class NPCBjorn 
{
public:
    static void interagir(Personagem& jogadorAtual);
    static std::vector<std::string> obterMapaForja();
};
