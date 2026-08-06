/*
 * Arquivo: DamageCalculator.h
 * Proposito: Calculo matematico de dano ofensivo (base + perfurante) e mitigacao defensiva por armadura.
 */

#pragma once
#include <utility>

class IAttacker;
class IDamageable;

/*
 * Calculadora de estatisticas de dano fisico e mitigacao.
 */
class CalculatorDamage {
public:
    // Calcula o dano base e perfurante de um atacante (baseado em arma e atributos)
    static std::pair<int, int> calculateDamageOffensiveBase(IAttacker* attacker);
    
    // Calcula a mitigacao defensiva do alvo (reducao por resistencia e constituicao)
    static int calculateMitigationDefensive(IDamageable* target, int damageGross, int damagePiercing);
};

