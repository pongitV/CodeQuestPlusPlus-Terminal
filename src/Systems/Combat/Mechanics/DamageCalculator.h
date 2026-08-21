// [PT-BR] Arquivo: DamageCalculator.h
// [PT-BR] Proposito: Calculo matematico de dano ofensivo (base + perfurante) e mitigacao defensiva por armadura.
// [EN-US] File: DamageCalculator.h
// [EN-US] Purpose: Mathematical calculation of offensive damage (base + piercing) and armor mitigation.

#pragma once
#include <utility>

class IAttacker;
class IDamageable;

// [PT-BR] Calculadora de estatisticas de dano fisico e mitigacao defensiva.
// [EN-US] Calculator for physical damage stats and defensive mitigation.
class DamageCalculator {
public:
    // [PT-BR] Calcula o dano base e perfurante de um atacante (baseado em arma e atributos)
    // [EN-US] Calculates base and piercing damage of an attacker (based on weapon and attributes)
    static std::pair<int, int> calculateDamageOffensiveBase(IAttacker* attacker);
    
    // [PT-BR] Calcula a mitigacao defensiva do alvo (reducao por resistencia e constituicao)
    // [EN-US] Calculates target defensive mitigation (reduction by resistance and constitution)
    static int calculateMitigationDefensive(IDamageable* target, int damageGross, int damagePiercing);
};
