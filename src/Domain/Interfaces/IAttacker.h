#pragma once
#include <utility>

// Interface de contrato para atacantes do sistema de combate.
class IAttacker {
public:
    virtual ~IAttacker() = default;
    
    // Calcula o dano base e perfurante gerado pelo atacante
    virtual std::pair<int, int> calculateDamageOffensiveBase() = 0;
    
    // Assegura o patamar de dano minimo considerando bonus ativos
    virtual int ensureDamageMinimum(int damageCurrent) = 0;
};
