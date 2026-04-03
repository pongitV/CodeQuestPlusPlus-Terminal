#include <iostream>
#include <vector>

#include "ClasseMago.h"
#include "../Inventario/Item.h"

std::string ClasseMago::obterNomeClasse() const 
{
     return "Mago"; 
}

std::vector<std::string> ClasseMago::obterAparenciaClasseMenu() const 
{
    return 
    {
        "                    ____ ",
        "                  .'* *.'",
        "               __/_*_*(_",
        "              / _______ \\",
        "             _\\_)/___\\(_/_ ",
        "            / _((\\- -/))_ \\",
        "            \\ \\())(-)(()/ /",
        "             ' \\(((()))/ '",
        "            / ' \\)).))/ ' \\",
        "           / _ \\ - | - /_  \\",
        "          (   ( .;''';. .'  )",
        "          _\\\"__ /   )\\ __\" /_",
        "            \\/  \\    ' / \\/",
        "             (.' '...' '.)",
        "              / /  |  \\ \\",
        "             / .   .   . \\",
        "            /   .     .   \\",
        "           /   /   |   \\   \\",
        "          .'   /   b    '.  '.",
        "     _.-'    /     Bb     '-. '-._ ",
        " _.-'        |      BBb       '-.  '-. ",
        "(________mrf\\____.dBBBb.________)____)"
    };
}

Atributos ClasseMago::obterAtributosClasse() const
{
    return { 0, 0, 0, 0, 0, 0, 0 };
}

std::vector<Item*> ClasseMago::gerarKitInicial() const 
{
    return 
    {
        new PocaoCura(), new PocaoCura(), new PocaoCura(),
        new Arma("Cajado de cristal", 12),
        new Escudo("Manto encantado", 0.08),
        new Armadura("Traje encantado de couro e prata", 5)
    };
}

void ClasseMago::usarHabilidadeClasse(Personagem* usuario, Personagem* alvo)
{
}
