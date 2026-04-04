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

std::string RacaElfo::obterNomeHabilidade() const { return "Agil e preciso"; }
std::string RacaElfo::obterDescricaoHabilidade() const { return "33% chance de causar 1.5x de dano"; }

std::vector<std::string> RacaElfo::obterAparenciaRaca() const 
{
    return { " <O> ", " /|\\ ", "  |  ", " / \\ " };
}