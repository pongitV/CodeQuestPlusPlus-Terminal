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

std::string RacaHumano::obterNomeHabilidade() const { return "Espirito indomavel"; }
std::string RacaHumano::obterDescricaoHabilidade() const { return "Revive com metade do HP uma vez ao morrer"; }

std::vector<std::string> RacaHumano::obterAparenciaRaca() const 
{
    return { "  O  ", " /|\\ ", "  |  ", " / \\ " };
}