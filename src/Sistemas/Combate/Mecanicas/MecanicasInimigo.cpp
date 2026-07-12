#include "MecanicasInimigo.h"
#include "../../../Entidades/Personagem.h"
#include "../../../Core/Utilidades/GeradorAleatorio.h"

Personagem* MecanicasInimigo::escolherAlvo(const std::vector<Personagem*>& aliadosVivos, Personagem* jogadorAtual) {
    std::vector<Personagem*> alvosPossiveis;
    std::vector<Personagem*> minionsVivos;
    std::vector<Personagem*> aliadosNormaisVivos;

    for (auto* aliado : aliadosVivos) {
        if (aliado->isMinion()) {
            minionsVivos.push_back(aliado);
        } else {
            aliadosNormaisVivos.push_back(aliado);
        }
    }

    if (!minionsVivos.empty()) {
        alvosPossiveis = minionsVivos;
    } else if (!aliadosNormaisVivos.empty()) {
        alvosPossiveis = aliadosNormaisVivos;
    } else {
        alvosPossiveis.push_back(jogadorAtual);
    }

    return alvosPossiveis[GeradorAleatorio::obterInteiro(0, static_cast<int>(alvosPossiveis.size()) - 1)];
}
