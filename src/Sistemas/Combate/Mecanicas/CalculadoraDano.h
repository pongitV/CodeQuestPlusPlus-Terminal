#pragma once
#include <utility>

class Personagem;

class CalculadoraDano {
public:
    // Calcula o dano base e perfurante de um atacante (baseado em arma, atributos e propriedades)
    static std::pair<int, int> calcularDanoOfensivoBase(Personagem* atacante);
    
    // Mitigação básica (exclui a lógica de Parry que depende de chance, trata apenas a redução garantida de armadura)
    static int calcularMitigacaoDefensiva(Personagem* alvo, int danoBruto, int danoPerfurante);
};
