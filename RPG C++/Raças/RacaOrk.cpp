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