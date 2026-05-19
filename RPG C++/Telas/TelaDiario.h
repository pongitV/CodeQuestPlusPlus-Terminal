#pragma once

#include <string>

class SistemaPersonagem;

class TelaDiario {
public:
    static void exibir(SistemaPersonagem* jogadorAtual);

private:
    static void exibirMenuItens(SistemaPersonagem* jogadorAtual);
    static void exibirMenuNPCs(SistemaPersonagem* jogadorAtual);
    static void exibirMenuRacas(SistemaPersonagem* jogadorAtual);
    static void exibirMenuClasses(SistemaPersonagem* jogadorAtual);

    static void inspecionarItem(SistemaPersonagem* jogadorAtual, const std::string& nomeItem);
    static void inspecionarRacaJogavel(const std::string& nomeRaca);
    static void inspecionarClasse(const std::string& nomeClasse);
    static void inspecionarNPC(const std::string& nomeNPC);
};
