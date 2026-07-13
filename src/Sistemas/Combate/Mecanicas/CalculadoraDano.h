#pragma once
#include <utility>

class IAttacker;
class IDamageable;

class CalculadoraDano {
public:
    // Calcula o dano base e perfurante de um atacante (baseado em arma, atributos e propriedades)
    static std::pair<int, int> calcularDanoOfensivoBase(IAttacker* atacante);
    
    // Mitigação básica (exclui a lógica de Parry que depende de chance, trata apenas a redução garantida de armadura)
    static int calcularMitigacaoDefensiva(IDamageable* alvo, int danoBruto, int danoPerfurante);
};
