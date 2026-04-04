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

std::string RacaDwarf::obterNomeHabilidade() const { return "Forjado com determinacao"; }
std::string RacaDwarf::obterDescricaoHabilidade() const { return "Escudos tem o dobro de durabilidade"; }

std::vector<std::string> RacaDwarf::obterAparenciaRaca() const 
{
    return { "     ", " [O] ", " /|\\ ", " / \\ " };
}