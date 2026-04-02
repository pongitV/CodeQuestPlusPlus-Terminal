#include <iostream>

#include "RacaDwarf.h"

std::string RacaDwarf::obterNomeRaca() const
{
    return "Dwarf";
}

Atributos RacaDwarf::obterAtributosRaca() const
{
    return { 0, 0, 0, 0, 0, 0, 0 };
}

void RacaDwarf::usarHabilidadeRaca(Personagem* usuario, Personagem* alvo)
{
    std::cout << "" << std::endl;
}

std::vector<std::string> RacaDwarf::obterAparencia() const 
{
    return { "     ", " [O] ", " /|\\ ", " / \\ " };
}