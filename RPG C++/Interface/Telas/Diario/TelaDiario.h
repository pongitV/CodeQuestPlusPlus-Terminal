#pragma once

#include <string>

class Personagem;

class TelaDiario {
public:
    static void exibir(Personagem* jogadorAtual);

private:
    static void exibirMenuItens(Personagem* jogadorAtual);
    static void exibirMenuNPCs(Personagem* jogadorAtual);
    static void exibirMenuRacas(Personagem* jogadorAtual);
    static void exibirMenuClasses(Personagem* jogadorAtual);

    static void inspecionarItem(Personagem* jogadorAtual, const std::string& nomeItem);
    static void inspecionarRacaJogavel(const std::string& nomeRaca);
    static void inspecionarClasse(const std::string& nomeClasse);
    static void inspecionarNPC(const std::string& nomeNPC);
};
