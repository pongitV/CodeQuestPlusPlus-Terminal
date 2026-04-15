#include <vector>
#include <string>
#include <memory>

#include "Item.h"

#pragma once

class Inventario
{
private:
    std::vector<Item*> listaDeItens;
    int quantidadeDeOuro;

public:
    Inventario();
    ~Inventario();

    void exibirInventario(Item* armaEquipada, Item* escudoEquipado, Item* armaduraEquipada, bool mostrarPrecos = false) const;
    
    Item* buscarItemPorCodigo(const std::string& codigoDigitado, Item* armaEquipada, Item* escudoEquipado, Item* armaduraEquipada);
    Item* selecionarEscudo();
    
    void adicionarItem(std::unique_ptr<Item> novoItem);
    void removerItem(const std::string& nomeDoItem);
    int contarItem(const std::string& nomeDoItem) const;

    bool possuiPocaoDeCura() const;
    
    void adicionarOuro(int quantidadeAdicional);
    int obterOuro() const;
    bool estaVazio() const;
};