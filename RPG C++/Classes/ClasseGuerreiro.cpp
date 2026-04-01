#include <iostream>
#include <vector>

#include "ClasseGuerreiro.h"
#include "../Inventario/Item.h"

std::string ClasseGuerreiro::obterNomeClasse() const 
{ 
    return "Guerreiro"; 
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
