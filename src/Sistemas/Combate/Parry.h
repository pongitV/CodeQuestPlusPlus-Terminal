#pragma once

#include <functional>
#include <string>

class Personagem;

class Parry 
{
public:
    static std::function<void()> onUpdateScreen;
    static std::string minigameMessage;
    static std::string minigameBar;

    // Gerencia o calculo de dificuldade e aciona o minigame, retornando true se o jogador vencer
    static bool tentarParry(Personagem* atacante, Personagem* defensor, int danoMitigado, int& quantidadeDeDanoReduzido);

    static std::string obterMensagemFeedback(bool parrySucesso, int danoFinal);

    static bool executarMinigameMovimento(int dificuldade, int danoMitigado, int& quantidadeDeDanoReduzido);
    static bool executarMinigameDigitacao(int dificuldade, int danoMitigado, int& quantidadeDeDanoReduzido);
};
