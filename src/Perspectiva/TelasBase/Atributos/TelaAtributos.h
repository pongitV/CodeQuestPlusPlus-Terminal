#pragma once

#include "../../../Entidades/Personagem.h"

struct PoderCombate {
    int danoFisEst;
    int danoMagEst;
    int defFixa;
    double mitigacao;
};

struct DebuffInfo {
    int forcaPerdida;
    int destrezaPerdida;
    int resPerdida;
    int constPerdida;
    bool temBuff;
};

class TelaAtributos 
{
public:
    static void exibir(Personagem* jogadorAtual);
    static void gerenciarFichaDoJogador(Personagem* jogadorAtual);

    static PoderCombate calcularPoderCombate(Personagem* jogadorAtual, double multiplicador);
    static DebuffInfo calcularDebuff(Personagem* jogadorAtual);
};
