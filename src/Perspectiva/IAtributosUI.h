#pragma once
#include "../Entidades/Personagem.h"

class IAtributosUI {
public:
    virtual ~IAtributosUI() = default;
    virtual void exibir(Personagem* jogador) = 0;
    virtual void exibirDetalhesAtributos(Personagem* jogadorAtual) = 0;
    virtual void gerenciarFichaDoJogador(Personagem* jogadorAtual) = 0;
};
