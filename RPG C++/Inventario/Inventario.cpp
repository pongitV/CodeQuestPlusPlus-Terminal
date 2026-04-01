#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "Inventario.h"
#include "Item.h"

Inventario::Inventario() : ouro(0) 
{
}

Inventario::~Inventario() 
{
    for (Item* item : itens) 
    {
        delete item;
    }
    itens.clear();
}

void Inventario::listarItens(Item* armaEquipada, Item* escudoEquipado, Item* armaduraEquipada) const 
{
    std::cout << "\n================================================" << std::endl;
    std::cout << "                    INVENTARIO                    " << std::endl;
    std::cout << "=================================================" << std::endl;
    std::cout << " DINHEIRO: " << ouro << " moedas\n";
    
    // 1. SESSÃO: ARSENAL (ITENS EQUIPADOS)
    std::cout << "\n [ ARSENAL - EQUIPADOS ]" << std::endl;
    if (armaEquipada) std::cout << "  * ARMA:    " << armaEquipada->obterNomeItem() << std::endl;
    if (escudoEquipado) std::cout << "  * ESCUDO:  " << escudoEquipado->obterNomeItem() << std::endl;
    if (armaduraEquipada) std::cout << "  * ARMADURA: " << armaduraEquipada->obterNomeItem() << std::endl;
    if (!armaEquipada && !escudoEquipado && !armaduraEquipada) std::cout << "  (Nenhum item equipado)" << std::endl;

    // 2. SESSÃO: EQUIPAMENTOS (NA MOCHILA, NÃO EQUIPADOS)
    std::cout << "\n [ EQUIPAMENTOS ]" << std::endl;
    bool temEquip = false;
    for (Item* item : itens)
    {
        TipoEquipamento tipo = item->obterTipo();
        if ((tipo == TipoEquipamento::ARMA || tipo == TipoEquipamento::ESCUDO || tipo == TipoEquipamento::ARMADURA) 
            && item != armaEquipada && item != escudoEquipado && item != armaduraEquipada)
        {
            std::cout << "  - " << item->obterNomeItem() << " [" << item->raridadeParaString() << "]" << std::endl;
            temEquip = true;
        }
    }
    if (!temEquip) std::cout << "  (Vazio)" << std::endl;

    // 3. SESSÃO: CONSUMIVEIS
    std::cout << "\n [ CONSUMIVEIS ]" << std::endl;
    std::map<std::string, int> contagem;
    for (Item* item : itens) 
    {
        if (item->obterTipo() == TipoEquipamento::CONSUMIVEL)
        {
            contagem[item->obterNomeItem()]++;
        }
    }
    if (contagem.empty()) std::cout << "  (Vazio)" << std::endl;
    for (auto const& [nome, qtd] : contagem) 
    {
        std::cout << "  * " << qtd << "x " << nome << std::endl;
    }

    // 4. SESSÃO: ITENS DE MISSAO
    std::cout << "\n [ ITENS DE MISSAO ]" << std::endl;
    bool temMissao = false;
    for (Item* item : itens)
    {
        if (item->obterTipo() == TipoEquipamento::MISSAO)
        {
            std::cout << "  ! " << item->obterNomeItem() << std::endl;
            temMissao = true;
        }
    }
    if (!temMissao) std::cout << "  (Vazio)" << std::endl;
    
    std::cout << "=================================================" << std::endl;
}

void Inventario::adicionarItem(Item* novoItem) 
{
    if (novoItem != nullptr) 
    {
        this->itens.push_back(novoItem);
        std::cout << "[Inventario]: " << novoItem->obterNomeItem() << " adquirido" << std::endl;
    }
}

void Inventario::removerItem(int indice) 
{
    if (indice >= 0 && indice < static_cast<int>(itens.size())) 
    {
        std::cout << "[Inventario]: " << itens[indice]->obterNomeItem() << " removido" << std::endl;
        delete itens[indice];
        itens.erase(itens.begin() + indice);
    }
}

void Inventario::removerPorNome(std::string nome)
{
    for (size_t i = 0; i < itens.size(); i++)
    {
        if (itens[i]->obterNomeItem() == nome)
        {
            delete itens[i];
            itens.erase(itens.begin() + i);
            break;
        }
    }
}

bool Inventario::possuiPocaoCura() const 
{
    for (Item* item : itens) 
    {
        if (dynamic_cast<PocaoCura*>(item) != nullptr) 
        {
            return true;
        }
    }
    return false;
}

void Inventario::consumirPocaoCura() 
{
    for (size_t i = 0; i < itens.size(); i++) 
    {
        if (dynamic_cast<PocaoCura*>(itens[i]) != nullptr) 
        {
            delete itens[i];
            itens.erase(itens.begin() + i);
            break;
        }
    }
}

void Inventario::adicionarOuro(int quantidade) 
{
    if (quantidade > 0) 
    {
        this->ouro += quantidade;
        std::cout << "[Inventario]: +" << quantidade << " moedas de ouro" << std::endl;
    }
}

int Inventario::obterOuro() const { return this->ouro; }
bool Inventario::estaVazio() const { return this->itens.empty(); }