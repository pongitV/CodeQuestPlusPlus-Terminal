#include "../Sistema/Personagem.h"
#include "../Inventario/Inventario.h"
#include "../Inventario/Item.h"

#pragma once

class Personagem;
class Item;

class ClasseBase 
{
public:
    // Metodos para criação de todas classes
    virtual ~ClasseBase() {}
    virtual std::string obterNomeClasse() const = 0;
    virtual std::vector<std::string> obterAparenciaClasseMenu() const = 0;
    virtual Atributos obterAtributosClasse() const = 0;
    virtual std::vector<Item*> obterEquipamentoClasse() const = 0;
 
    // Metodos para criação e definição da habilidade de classe
    virtual std::string obterNomeHabilidadeClasse() const = 0;
    virtual std::string obterDescricaoHabilidadeClasse() const = 0;
    virtual void usarHabilidadeClasse(Personagem* usuario, std::vector<Personagem*>& inimigos) = 0;
};