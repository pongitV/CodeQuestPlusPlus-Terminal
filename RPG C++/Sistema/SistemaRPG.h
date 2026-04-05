#include <vector>
#include "Personagem.h"

#pragma once

class SistemaRPG 
{
private:
    Personagem* jogador;
    std::vector<Personagem*> inimigos;
    int contadorTurno;

    void aplicarDano(Personagem* alvo, int danoBruto, int turnoAtual);

public:
    SistemaRPG(Personagem* jogador, std::vector<Personagem*> inimigos);
    virtual ~SistemaRPG();

    void iniciarCombate();
    void executarTurnoInimigos();
    bool verificarVitoria();

    void realizarAtaqueFisico(Personagem* atacante, Personagem* defensor, int turnoAtual);
};