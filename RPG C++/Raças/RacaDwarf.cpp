#include <iostream>

#include "RacaDwarf.h"

std::string RacaDwarf::obterNomeRaca() const
{
    return "Dwarf";
}

std::vector<std::string> RacaDwarf::obterAparenciaRaca() const 
{
    return { "     ", " [O] ", " /|\\ ", " / \\ " };
}

Atributos RacaDwarf::obterAtributosRaca() const
{
    return { 20, 12, 6, 15, 12, 5, 8 };
}

std::string RacaDwarf::obterNomeHabilidadeRaca() const { return "Forjado com determinacao"; }
std::string RacaDwarf::obterDescricaoHabilidadeRaca() const { return "Escudos tem o dobro de durabilidade"; }

int RacaDwarf::processarDanoDefensivo(int danoFinal, Personagem* defensor) 
{
    if (defensor->obterEscudo() != nullptr) 
    {
        double bEscudo = defensor->obterEscudo()->obterReducaoPercentual();
        std::cout << "[PASSIVA]: Forjado com determinacao absorveu mais dano!\n";
        return static_cast<int>(danoFinal * (1.0 - bEscudo));
    }
    return danoFinal;
}