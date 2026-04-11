#include <vector>
#include <string>
#include "Item.h"

#pragma once

class Inventario
{
private:
    std::vector<Item*> listaDeItens;
    int quantidadeDeOuro;

public:
    Inventario();
    virtual ~Inventario();

    // Listagem adaptada ao terminal com títulos horizontais
    void exibirInventarioOrganizadoPorCategorias(Item* armaEquipada, Item* escudoEquipado, Item* armaduraEquipada) const;
    
    // Sistema de busca por atalho (ex: 1C)
    Item* buscarItemPeloCodigoDigitado(std::string codigoDigitado, Item* armaEquipada, Item* escudoEquipado, Item* armaduraEquipada);
    Item* abrirMenuParaSelecionarEscudoDeDefesa();
    
    void adicionarItemAoInventario(Item* novoItem);
    void removerItemDoInventarioPeloNome(std::string nomeDoItemParaRemover);
    int contarQuantidadeDeUmItemPeloNome(std::string nomeDoItem) const;

    bool verificarSePossuiPocaoDeCura() const;
    
    void adicionarOuroAoInventario(int quantidadeAdicional);
    int obterQuantidadeDeOuro() const;
    bool verificarSeInventarioEstaVazio() const;
};