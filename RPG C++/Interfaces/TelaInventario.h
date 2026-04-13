#pragma once

#include "../Sistema/Personagem.h"

class TelaInventario 
{
public:
    static void exibir(Personagem* jogadorAtual, bool mostrarPrecos = false);
};