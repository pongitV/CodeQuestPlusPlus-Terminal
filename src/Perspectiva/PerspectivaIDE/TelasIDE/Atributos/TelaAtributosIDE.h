#pragma once

#include <string>
#include <vector>

#include "../../../../Entidades/Personagem.h"

class TelaAtributosIDE {
public:
    static void exibir(Personagem* jogadorAtual);
    static void exibirDetalhesAtributos(Personagem* jogadorAtual);
};
