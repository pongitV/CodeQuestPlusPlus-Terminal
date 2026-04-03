#include <iostream>
#include <vector>

#include "ClasseBardo.h"
#include "../Inventario/Item.h"

std::string ClasseBardo::obterNomeClasse() const 
{
     return "Bardo"; 
}

std::vector<std::string> ClasseBardo::obterAparenciaClasseMenu() const 
{
    return 
    {
        "       _______  ",
        "      /       \\ ",
        "     |  (O O)  |",
        "      \\  _w_  / ",
        "    ___|     |___",
        "   /   |     |   \\",
        "  /    |_____|    \\",
        " |   / |     | \\   |",
        " |  |  |     |  |  |",
        " |  |  |_____|  |  |",
        " |  | /       \\ |  |",
        " |__|/  [#|#]  \\|__|", // Alaúde
        "     \\  [#|#]  /   ",
        "      \\_______/    ",
        "       |     |     ",
        "       |     |     ",
        "      /       \\    ",
        "     /_________\\   "
    };
}

Atributos ClasseBardo::obterAtributosClasse() const
{
    return { 0, 0, 0, 0, 0, 0, 0 };
}

std::vector<Item*> ClasseBardo::gerarKitInicial() const 
{
    return 
    {
        new PocaoCura(), new PocaoCura(), new PocaoCura(),
        new Arma("Violao encantado", 3),
        new Escudo("Capa magica", 0.10),
        new Armadura("Roupas nobres de tecido", 1)
    };
}

void ClasseBardo::usarHabilidadeClasse(Personagem* usuario, Personagem* alvo)
{
}
