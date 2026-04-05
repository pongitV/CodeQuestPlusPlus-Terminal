#include "../Sistema/Personagem.h"
#include "../Inventario/Inventario.h"
#include "../Inventario/Item.h"

#pragma once

class Personagem;
class Item;

class ClasseBase 
{
public:
    virtual ~ClasseBase() {}
    virtual std::string obterNomeClasse() const = 0;
    virtual std::vector<std::string> obterAparenciaClasseMenu() const = 0;
    virtual Atributos obterAtributosClasse() const = 0;
    virtual std::vector<Item*> gerarKitInicial() const = 0;

    virtual void usarHabilidadeClasseAtiva(Personagem* usuario, std::vector<Personagem*>& inimigos) = 0;
    virtual std::string obterNomeHabilidadeClasseAtiva() const = 0;
    virtual std::string obterDescricaoHabilidadeClasseAtiva() const = 0;
};