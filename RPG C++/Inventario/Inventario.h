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

    // Listagem adaptada ao terminal com títulos horizontais
    void listarItens(Item* arma, Item* escudo, Item* armadura) const;
    
    // Sistema de busca por atalho (ex: 1C)
    Item* buscarItemPorCodigo(std::string codigo, Item* a, Item* e, Item* d);
    Item* escolherEscudoParaDefesa();
    
    void adicionarItem(Item* novoItem);
    void removerPorNome(std::string nome);
    int contarItensPorNome(std::string nome) const;

    bool possuiPocaoCura() const;
    
    void adicionarOuro(int quantidade);
    int obterOuro() const;
    bool estaVazio() const;
};