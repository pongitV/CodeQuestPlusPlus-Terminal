#pragma once

#include <string>

class SistemaPersonagem;

class TelaRacas {
public:
    static void exibir(SistemaPersonagem* jogador);
private:
    static void exibirFichaRacaJogavel(SistemaPersonagem* jogador, const std::string& nomeRaca);
};
