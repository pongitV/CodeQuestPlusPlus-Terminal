#pragma once

#include "../Sistema/Personagem.h"

class NPCMorgana 
{
public:
    static void interagir(Personagem& jogadorAtual);
    static std::vector<std::string> obterMapaCabana();
};
