// [PT-BR] Arquivo: IDamageable.h
// [PT-BR] Proposito: Interface para entidades receptoras de dano fisico e magico.
// [EN-US] File: IDamageable.h
// [EN-US] Purpose: Interface for entities receiving physical and magical damage.

#pragma once
#include "Domain/DamageResult.h"

class IAttacker;

// [PT-BR] Interface de contrato para entidades que recebem dano e possuem calculos de defesa.
// [EN-US] Contract interface for entities that receive damage and have defense calculations.
class IDamageable {
public:
    virtual ~IDamageable() = default;
    
    // [PT-BR] Processa o recebimento de dano e retorna o resultado detalhado
    // [EN-US] Processes incoming damage and returns detailed result
    virtual DamageResult receiveDamage(int damageGross, int damagePiercing, int damageReducedParry, IAttacker* attacker, bool applyPassive = true) = 0;
    
    // [PT-BR] Calcula o valor de defesa base do alvo
    // [EN-US] Computes base defense value of target
    virtual int calculateDefenseBase(int damageGross, int damagePiercing) = 0;
};
