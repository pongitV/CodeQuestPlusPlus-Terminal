#include <iostream>
#include <vector>

#include "ClasseGuerreiro.h"
#include "../Inventario/Item.h"

std::string ClasseGuerreiro::obterNomeClasse() const 
{ 
    return "Guerreiro"; 
}

std::vector<std::string> ClasseGuerreiro::obterAparenciaClasseMenu() const 
{
    return 
    {
        "      /\\",
        "      ||",
        "      ||",
        "      ||",
        "      ||           {}",
        "      ||          .--.",
        "      ||         /.--.\\",
        "      ||         |====|",
        "      ||         |`::`|",
        "     _||_    .-;`\\..../`;_.-^-._",
        "      /\\\\   /  |...::..|`   :   `|",
        "      |:'\\ |   /'''::''|   .:.   |",
        "       \\ /\\;-,/\\   ::  |..:::::..|",
        "        \\ <` >  >._::_.| ':::::' |",
        "         `\"\"`  /   ^^  |   ':'   |",
        "               |       \\    :    /",
        "               |        \\   :   /",
        "               |___/\\___|`-.:.-`",
        "                \\_ || _/    `",
        "                <_ >< _>",
        "                |  ||  |",
        "                |  ||  |",
        "               _\\.:||:./_",
        "              /____/\\____\\"
    };
}

Atributos ClasseGuerreiro::obterAtributosClasse() const
{
    return { 0, 1000000, 0, 0, 0, 0, 0 };
}

std::vector<Item*> ClasseGuerreiro::gerarKitInicial() const 
{
    return 
    {
        new PocaoCura(), new PocaoCura(), new PocaoCura(),
        new Arma("Espada longa de metal", 10),
        new Escudo("Escudo medio de metal", 0.15),
        new Armadura("Armadura media de malha", 8)
    };
}

void ClasseGuerreiro::usarHabilidadeClasse(Personagem* usuario, Personagem* alvo)
{
}
