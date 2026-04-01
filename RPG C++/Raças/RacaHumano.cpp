#include <iostream>

#include "RacaHumano.h"

std::string RacaHumano::obterNomeRaca() const 
{
    return "Humano ";
}

Atributos RacaHumano::obterAtributosRaca() const 
{
    return { 0, 0, 0, 0, 0, 0, 0 };
}

void RacaHumano::usarHabilidadeRaca(Personagem* usuario, Personagem* alvo) 
{
    std::cout << "" << std::endl;
}