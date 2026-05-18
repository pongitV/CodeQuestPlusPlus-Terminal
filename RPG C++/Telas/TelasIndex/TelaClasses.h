#pragma once

#include <string>

class SistemaPersonagem;

class TelaClasses {
public:
    static void exibir(SistemaPersonagem* jogador);
private:
    static void exibirFichaClasse(SistemaPersonagem* jogador, const std::string& nomeClasse);
};
