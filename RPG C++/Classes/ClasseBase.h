#include "../Sistema/Personagem.h"
#include "../Inventario/Inventario.h"
#include "../Inventario/Item.h"
#include <memory>
#include "../Sistema/Tipos.h"

#pragma once

class Personagem;
class Item;

class ClasseBase 
{
public:
    // Destrutor virtual para garantir a destruicao polimorfica correta das subclasses
    virtual ~ClasseBase() {}

    // INFORMACOES DA CLASSE
    virtual std::string obterNomeClasse() const = 0;
    virtual TipoClasse obterTipoClasse() const = 0;
    virtual std::vector<std::string> obterAparenciaClasseMenu() const = 0;
    virtual Atributos obterAtributosClasse() const = 0;
    virtual std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const = 0;
 
    // HABILIDADE DA CLASSE
    virtual std::string obterNomeHabilidadeClasse() const = 0;
    virtual std::string obterDescricaoHabilidadeClasse() const = 0;
    virtual void usarHabilidadeClasse(Personagem* usuario, std::vector<Personagem*>& inimigos) = 0;
    virtual TipoAtaque obterTipoAtaque() const = 0;
    virtual bool habilidadeConsomeTurno() const = 0;

};