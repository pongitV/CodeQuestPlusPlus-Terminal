#pragma once
#include "../ResultadoDano.h"

class IAttacker;

class IDamageable {
public:
    virtual ~IDamageable() = default;
    
    virtual ResultadoDano receberDano(int danoBruto, int danoPerfurante, int danoReduzidoParry, IAttacker* atacante, bool aplicarPassivas = true) = 0;
    virtual int calcularDefesaBase(int danoBruto, int danoPerfurante) = 0;
};
