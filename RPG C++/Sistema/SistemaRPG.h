#include <vector>
#include "Personagem.h"

#pragma once

class SistemaRPG 
{
private:
    Personagem* jogador;
    std::vector<Personagem*> inimigos;
    int contadorTurno;
    int ouroObtido;
    int xpObtido;
    int danoCausadoTotal;
    int danoRecebidoTotal;

    void aplicarDano(Personagem* atacante, Personagem* alvo, int danoBruto, int turnoAtual);
    bool executarParry(int qtdNumeros, int tempoLimite, int& reducao);

public:
    SistemaRPG(Personagem* jogador, std::vector<Personagem*> inimigos);
    virtual ~SistemaRPG();

    void iniciarCombate();
    void executarTurnoInimigos();
    bool verificarVitoria();

    void realizarAtaqueFisico(Personagem* atacante, Personagem* defensor, int turnoAtual);
};