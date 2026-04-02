#include <iostream>

#include "RacaElfo.h"

std::string RacaElfo::obterNomeRaca() const 
{
    return "Elfo";
}

Atributos RacaElfo::obterAtributosRaca() const 
{
    return { 0, 0, 0, 0, 0, 0, 0 };
}

void RacaElfo::usarHabilidadeRaca(Personagem* usuario, Personagem* alvo) 
{
    std::cout << "" << std::endl;
}

std::vector<std::string> RacaElfo::obterAparencia() const 
{
    return { " <O> ", " /|\\ ", "  |  ", " / \\ " };
}