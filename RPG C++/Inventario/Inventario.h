// Inventario/Inventario.h
#include <vector>
#include <string>
#include "Item.h"

#pragma once

class Inventario
{
private:
    std::vector<Item*> itens;
    int ouro;

public:
    Inventario();
    virtual ~Inventario();

    void listarItens(Item* arma, Item* escudo, Item* armadura) const;
    void adicionarItem(Item* novoItem);
    void removerItem(int indice);
    bool estaVazio() const;
    void removerPorNome(std::string nome);

    void adicionarOuro(int quantidade);
    int obterOuro() const;

    bool possuiPocaoCura() const;
    void consumirPocaoCura();
};