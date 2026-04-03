#include <iostream>
#include <vector>

#include "ClasseArqueiro.h"
#include "../Inventario/Item.h"

std::string ClasseArqueiro::obterNomeClasse() const 
{
     return "Arqueiro"; 
}

std::vector<std::string> ClasseArqueiro::obterAparenciaClasseMenu() const 
{
    return 
    {
        "          ||    ",
        "          ||    ",
        "     (    ||    ",
        "      \\   ||    ",
        "       \\  ||    ",
        "        \\(O)    ",
        "      ---||---> ",
        "        /| |\\   ",
        "       / | | \\  ",
        "      /  | |  \\ ",
        "     /   | |   \\",
        "    (    | |    )",
        "     \\   | |   / ",
        "      \\  | |  /  ",
        "       \\ | | /   ",
        "        \\| |/    ",
        "         | |     ",
        "        /   \\    ",
        "       /     \\   ",
        "      /       \\  "
    };
}

Atributos ClasseArqueiro::obterAtributosClasse() const
{
    return { 0, 0, 0, 0, 0, 0, 0 };
}

std::vector<Item*> ClasseArqueiro::gerarKitInicial() const 
{
    return 
    {
        new PocaoCura(), new PocaoCura(), new PocaoCura(),
        new Arma("Arco recurvo de madeira", 5),
        new Escudo("Escudo leve de madeira", 0.05),
        new Armadura("Armadura leve de couro", 2)
    };
}

void ClasseArqueiro::usarHabilidadeClasse(Personagem* usuario, Personagem* alvo)
{
}

