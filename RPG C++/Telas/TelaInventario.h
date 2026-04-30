#pragma once

#include "../Sistemas/SistemaPersonagem.h"

class TelaInventario 
{
public:
    static void exibir(SistemaPersonagem* jogadorAtual, bool mostrarPrecos = false);
};