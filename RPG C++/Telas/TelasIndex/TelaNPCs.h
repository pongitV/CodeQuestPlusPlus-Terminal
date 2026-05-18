#pragma once

#include <string>

class SistemaPersonagem;

class TelaNPCs {
public:
    static void exibir(SistemaPersonagem* jogador);
private:
    static void exibirFichaNPC(SistemaPersonagem* jogador, const std::string& nomeNPC);
};
