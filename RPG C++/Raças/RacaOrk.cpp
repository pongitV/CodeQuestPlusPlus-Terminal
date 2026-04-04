#include <iostream>

#include "RacaOrk.h"

std::string RacaOrk::obterNomeRaca() const 
{
    return "Ork";
}

Atributos RacaOrk::obterAtributosRaca() const 
{
    return { 0, 0, 0, 0, 0, 0, 0 };
}

void RacaOrk::usarHabilidadeRaca(Personagem* usuario, Personagem* alvo) 
{
    std::cout << "" << std::endl;
}

std::string RacaOrk::obterNomeHabilidade() const { return "Furia cega"; }
std::string RacaOrk::obterDescricaoHabilidade() const { return "Dano extra baseado na vida perdida"; }

std::vector<std::string> RacaOrk::obterAparenciaRaca() const 
{
    return { " (O) ", "/|#|\\", "  |  ", " / \\ " };
}