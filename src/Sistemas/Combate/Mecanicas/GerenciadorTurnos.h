#pragma once
#include <vector>
#include <memory>

class Personagem;

class GerenciadorTurnos {
public:
    // Retorna a maior destreza entre todos os inimigos
    static int calcularMaxDestrezaInimigos(const std::vector<std::unique_ptr<Personagem>>& inimigos);
    
    // Verifica se os inimigos atacam antes do jogador (destreza inimiga > destreza jogador)
    static bool inimigosSaoMaisAgeis(Personagem* jogador, int maxDestrezaInimigos);
    
    // Verifica se os inimigos possuem o dobro da destreza do jogador (atacam 2x seguidas no inicio)
    static bool inimigosTemDobroDeAgilidade(Personagem* jogador, int maxDestrezaInimigos);
    
    // Verifica se o jogador possui o dobro de agilidade (ganha turno extra no inicio)
    static bool jogadorTemTurnoExtraNoInicio(Personagem* jogador, int maxDestrezaInimigos);
};
