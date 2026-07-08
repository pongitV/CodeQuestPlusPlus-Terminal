#pragma once

#include <string>
#include <vector>

class Personagem;

class TelaBestiarioRaycaster {
public:
    static void exibir(const std::vector<Personagem*>& inimigos);
    static void exibirDetalhe(Personagem* inimigo);
};
