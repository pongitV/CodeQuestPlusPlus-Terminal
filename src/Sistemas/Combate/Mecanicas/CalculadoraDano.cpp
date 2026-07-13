#include "CalculadoraDano.h"
#include "../../../Entidades/Interfaces/IAttacker.h"
#include "../../../Entidades/Interfaces/IDamageable.h"

std::pair<int, int> CalculadoraDano::calcularDanoOfensivoBase(IAttacker* atacante) {
    if (atacante) {
        return atacante->calcularDanoOfensivoBase();
    }
    return {0, 0};
}

int CalculadoraDano::calcularMitigacaoDefensiva(IDamageable* alvo, int danoBruto, int danoPerfurante) {
    if (alvo) {
        return alvo->calcularDefesaBase(danoBruto, danoPerfurante);
    }
    return danoBruto + danoPerfurante;
}
