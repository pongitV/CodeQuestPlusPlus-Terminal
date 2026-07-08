#pragma once
#include "../Entidades/Personagem.h"
#include <vector>

class IBestiarioUI {
public:
    virtual ~IBestiarioUI() = default;
    virtual void exibir(const std::vector<Personagem*>& inimigos) = 0;
    virtual void exibirDetalhe(Personagem* inimigo) = 0;
};
