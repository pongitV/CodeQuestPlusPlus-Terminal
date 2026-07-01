#pragma once

#include <string>
#include <vector>

#include "../../../Entidades/Personagem.h"

class TelaAtributosTerminal {
public:
    static void exibir(Personagem* jogadorAtual);
    static void exibirAtributos(const std::string& nomeAtributo, int valor, int nivel);
    static void exibirEstatisticas(int vidaMax, int manaMax, int ataque, int defesa, int destreza, int sorte);
    static void gerenciarFicha(Personagem* jogadorAtual);
};
