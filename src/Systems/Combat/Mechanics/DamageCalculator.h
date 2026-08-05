/*
 * Arquivo: DamageCalculator.h
 * Propósito: Cálculo matemático de dano ofensivo (base + perfurante) e mitigação defensiva por armadura.
 */

#pragma once
#include <utility>

class IAttacker;
class IDamageable;

/*
 * Calculadora de estatísticas de dano físico e mitigação.
 */
class CalculatorDamage {
public:
    // Calcula o dano base e perfurante de um atacante (baseado em arma e atributos)
    static std::pair<int, int> calculateDamageOffensiveBase(IAttacker* attacker);
    
    // Calcula a mitigação defensiva do alvo (redução por resistência e constituição)
    static int calculateMitigationDefensive(IDamageable* target, int damageGross, int damagePiercing);
};

