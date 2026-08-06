/*
 * Arquivo: IDamageable.h
 * Proposito: Interface para entidades receptores de dano fisico e magico.
 */

#pragma once
#include "Domain/DamageResult.h"

class IAttacker;

/*
 * Interface de contrato para entidades que tomam dano e possuem defesa.
 */
class IDamageable {
public:
    virtual ~IDamageable() = default;
    
    // Processa o recebimento de dano e retorna o resultado detalhado
    virtual ResultDamage receiveDamage(int damageGross, int damagePiercing, int damageReducedParry, IAttacker* attacker, bool applyPassive = true) = 0;
    
    // Calcula o valor de defesa base do alvo
    virtual int calculateDefenseBase(int damageGross, int damagePiercing) = 0;
};

