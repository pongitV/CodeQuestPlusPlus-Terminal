#include <string>
#include <vector>

#include "../Sistema/Personagem.h"

#pragma once

class GeradorInimigos
{
public:
    static Personagem* gerarGoblin();
    static std::vector<Personagem*> gerarHordaGoblins(int quantidade);
};
