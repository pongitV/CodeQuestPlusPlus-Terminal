#pragma once

#include <string>
#include <vector>

class Personagem;

class TelaBestiario
{
public:
    static void exibirLista(Personagem* jogadorAtual);

    static void exibirFicha(Personagem* jogadorAtual, const std::string& nomeInimigo, int indiceDescoberto, const std::vector<std::string>& descobertos);
};
