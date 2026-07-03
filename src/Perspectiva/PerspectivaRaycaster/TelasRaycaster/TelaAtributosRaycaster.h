#pragma once

#include <string>

class Personagem;

class TelaAtributosRaycaster {
public:
    static void exibir(Personagem* jogador);
    static void exibirDetalhesAtributos(Personagem* jogadorAtual);
};
