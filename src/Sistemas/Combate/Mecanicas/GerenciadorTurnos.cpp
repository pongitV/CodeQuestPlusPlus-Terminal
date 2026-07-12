#include "GerenciadorTurnos.h"
#include "../../../Entidades/Personagem.h"
#include <algorithm>

int GerenciadorTurnos::calcularMaxDestrezaInimigos(const std::vector<std::unique_ptr<Personagem>>& inimigos) {
    int maxDestreza = 0;
    for (const auto& inimigoPtr : inimigos) {
        if (inimigoPtr->obterDestreza() > maxDestreza) {
            maxDestreza = inimigoPtr->obterDestreza();
        }
    }
    return maxDestreza;
}

bool GerenciadorTurnos::inimigosSaoMaisAgeis(Personagem* jogador, int maxDestrezaInimigos) {
    return maxDestrezaInimigos > jogador->obterDestreza();
}

bool GerenciadorTurnos::inimigosTemDobroDeAgilidade(Personagem* jogador, int maxDestrezaInimigos) {
    return maxDestrezaInimigos > (jogador->obterDestreza() * 2);
}

bool GerenciadorTurnos::jogadorTemTurnoExtraNoInicio(Personagem* jogador, int maxDestrezaInimigos) {
    return jogador->obterDestreza() > (maxDestrezaInimigos * 2);
}
