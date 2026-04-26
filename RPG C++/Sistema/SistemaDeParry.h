#pragma once

class Personagem;

class SistemaDeParry 
{
public:
    // Gerencia o calculo de dificuldade e aciona o minigame, retornando true se o jogador vencer
    static bool tentarParry(Personagem* atacante, int danoMitigado, int& quantidadeDeDanoReduzido);

private:
    // Lida puramente com a logica em tempo real, I/O e RNG (Isolado do Combate)
    static bool executarMinigame(int quantidadeDeNumerosParaDigitar, int tempoLimiteEmSegundos, int& quantidadeDeDanoReduzido);
};