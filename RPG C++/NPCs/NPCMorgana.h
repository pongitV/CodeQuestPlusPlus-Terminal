#pragma once

#include "../Sistema/Personagem.h"
#include <string>
#include <vector>

class NPCMorgana
{
public:
    static void interagir(Personagem* jogadorAtual);
    static std::vector<std::string> obterMapaCabana();
};
