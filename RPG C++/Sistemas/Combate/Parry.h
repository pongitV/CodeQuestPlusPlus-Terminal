#pragma once

class Personagem;

class Parry 
{
public:
    // Gerencia o calculo de dificuldade e aciona o minigame, retornando true se o jogador vencer
    static bool tentarParry(Personagem* atacante, Personagem* defensor, int danoMitigado, int& quantidadeDeDanoReduzido);

private:
    static bool executarMinigameMovimento(int dificuldade, int danoMitigado, int& quantidadeDeDanoReduzido);
    static bool executarMinigameDigitacao(int dificuldade, int danoMitigado, int& quantidadeDeDanoReduzido);
};
