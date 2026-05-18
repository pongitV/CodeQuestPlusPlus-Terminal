#pragma once

#include <string>
#include <vector>

class SistemaPersonagem;

class TelaBestiario
{
public:
    static void exibirLista(SistemaPersonagem* jogadorAtual);
    static void exibirFicha(SistemaPersonagem* jogadorAtual, const std::string& nomeInimigo, bool modoRaca = false);
};