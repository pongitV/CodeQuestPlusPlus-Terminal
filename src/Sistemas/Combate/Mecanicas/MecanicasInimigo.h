#pragma once
#include <vector>

class Personagem;

class MecanicasInimigo {
public:
    // Determina o alvo de um ataque do inimigo baseado na prioridade (minions > aliados normais > jogador)
    static Personagem* escolherAlvo(const std::vector<Personagem*>& aliadosVivos, Personagem* jogadorAtual);
};
