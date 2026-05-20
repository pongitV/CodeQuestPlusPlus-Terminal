#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "Item.h"

class Inventario
{
private:
    std::vector<std::unique_ptr<Item>> listaDeItens;
    std::unordered_map<std::string, int> contagemItens_;
    int quantidadeDeOuro;

public:
    std::vector<Item*> obterTodosOsItens() const { 
        std::vector<Item*> itensCrus;
        itensCrus.reserve(listaDeItens.size());
        for(const auto& item : listaDeItens) itensCrus.push_back(item.get());
        return itensCrus;
    }
    
    Inventario();
    ~Inventario() = default;

    // Funcoes Basicas e Estado
    bool estaVazio() const;
    int obterOuro() const;
    int contarItem(const std::string& nomeDoItem) const;
    bool possuiPocaoDeCura() const;

    // Manipulacao do Inventario
    void adicionarOuro(int quantidadeAdicional);
    void adicionarItem(std::unique_ptr<Item> novoItem);
    void removerItem(const std::string& nomeDoItem);
    void removerItem(Item* itemExato);

    // Buscas e Interacoes
    Item* buscarItemPorCodigo(const std::string& codigoDigitado, Item* armaEquipada, Item* escudoEquipado, Item* armaduraEquipada);
};
