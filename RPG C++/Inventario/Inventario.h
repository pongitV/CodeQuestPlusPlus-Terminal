#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Item.h"

class Inventario
{
private:
    std::vector<std::unique_ptr<Item>> listaDeItens;
    int quantidadeDeOuro;

public:
    std::vector<Item*> obterTodosOsItens() const { 
        std::vector<Item*> itensCrus;
        for(const auto& item : listaDeItens) itensCrus.push_back(item.get());
        return itensCrus;
    }
    
    Inventario();
    ~Inventario() = default;

    void exibirInventario(Item* armaEquipada, Item* escudoEquipado, Item* armaduraEquipada, bool mostrarPrecos = false) const;
    
    Item* buscarItemPorCodigo(const std::string& codigoDigitado, Item* armaEquipada, Item* escudoEquipado, Item* armaduraEquipada);
    Item* selecionarEscudo();
    
    void adicionarItem(std::unique_ptr<Item> novoItem);
    void removerItem(const std::string& nomeDoItem);
    void removerItem(Item* itemExato);
    int contarItem(const std::string& nomeDoItem) const;

    bool possuiPocaoDeCura() const;
    
    void adicionarOuro(int quantidadeAdicional);
    int obterOuro() const;
    bool estaVazio() const;
};