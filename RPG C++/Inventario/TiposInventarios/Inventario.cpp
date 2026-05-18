#include <algorithm>
#include <unordered_map>

#include "Inventario.h"
#include "../Item.h"
#include "../InventarioConsumiveis/ItemConsumivel.h"
#include "../../Gerenciadores/GerenciadoresSistema/GerenciadorMenu.h"
#include "../../Utilidades/Aparencia.h"

Inventario::Inventario() : quantidadeDeOuro(0) {}

bool Inventario::estaVazio() const { return listaDeItens.empty(); }

int Inventario::obterOuro() const { return quantidadeDeOuro; }

int Inventario::contarItem(const std::string& nomeDoItem) const 
{
    auto it = contagemItens_.find(nomeDoItem);
    return it != contagemItens_.end() ? it->second : 0;
}

bool Inventario::possuiPocaoDeCura() const 
{
    return std::any_of(listaDeItens.begin(), listaDeItens.end(), [](const std::unique_ptr<Item>& item) 
    {
        return item->temPropriedade(Propriedade::ConsumivelCura);
    });
}

void Inventario::adicionarOuro(int quantidadeAdicional) 
{ 
    quantidadeDeOuro = std::max(0, quantidadeDeOuro + quantidadeAdicional); 
}

void Inventario::adicionarItem(std::unique_ptr<Item> novoItem) 
{ 
    if (novoItem) {
        contagemItens_[novoItem->obterNomeItem()]++;
        listaDeItens.push_back(std::move(novoItem));
    }
}

void Inventario::removerItem(const std::string& nomeDoItem) 
{
    auto it = std::find_if(listaDeItens.begin(), listaDeItens.end(), [&](const std::unique_ptr<Item>& item) 
    {
        return item->obterNomeItem() == nomeDoItem;
    });
    
    if (it != listaDeItens.end()) 
    {
        contagemItens_[nomeDoItem]--;
        if (contagemItens_[nomeDoItem] <= 0) contagemItens_.erase(nomeDoItem);
        listaDeItens.erase(it);
    }
}

void Inventario::removerItem(Item* itemExato) 
{
    if (!itemExato) return;
    auto it = std::find_if(listaDeItens.begin(), listaDeItens.end(), [&](const std::unique_ptr<Item>& item) 
    {
        return item.get() == itemExato;
    });
    
    if (it != listaDeItens.end()) {
        std::string nome = itemExato->obterNomeItem();
        contagemItens_[nome]--;
        if (contagemItens_[nome] <= 0) contagemItens_.erase(nome);
        listaDeItens.erase(it);
    }
}

Item* Inventario::buscarItemPorCodigo(const std::string& codigoDigitado, Item* armaEquipada, Item* escudoEquipado, Item* armaduraEquipada)
{
    if (codigoDigitado.length() < 2) return nullptr;

    char letraDaCategoria = std::toupper(codigoDigitado.back());
    std::string parteNumerica = codigoDigitado.substr(0, codigoDigitado.length() - 1);
    
    if (!std::all_of(parteNumerica.begin(), parteNumerica.end(), ::isdigit)) return nullptr;
    
    int indiceDoItem = std::stoi(parteNumerica);
    if (indiceDoItem <= 0) return nullptr;

    if (letraDaCategoria == 'E')
    {
        if (indiceDoItem == 1) return armaEquipada;
        if (indiceDoItem == 2) return escudoEquipado;
        if (indiceDoItem == 3) return armaduraEquipada;
        return nullptr;
    }

    auto buscarPorTipoAgrupado = [&](auto condicao) -> Item* {
        std::unordered_map<std::string, size_t> cacheDeIndice;
        std::vector<std::string> nomesDosItensExibidos;

        for (size_t i = 0; i < listaDeItens.size(); ++i) {
            Item* itemAtual = listaDeItens[i].get();
            if (condicao(itemAtual)) {
                if (cacheDeIndice.find(itemAtual->obterNomeItem()) == cacheDeIndice.end()) {
                    cacheDeIndice[itemAtual->obterNomeItem()] = i;
                    nomesDosItensExibidos.push_back(itemAtual->obterNomeItem());
                }
            }
        }
        
        std::sort(nomesDosItensExibidos.begin(), nomesDosItensExibidos.end());

        if (indiceDoItem > 0 && indiceDoItem <= static_cast<int>(nomesDosItensExibidos.size())) {
            size_t indiceOriginalNoInventario = cacheDeIndice[nomesDosItensExibidos[indiceDoItem - 1]];
            return listaDeItens[indiceOriginalNoInventario].get();
        }
        return nullptr;
    };

    switch (letraDaCategoria) {
        case 'A':
            return buscarPorTipoAgrupado([&](Item* itemAvaliado) { 
                return itemAvaliado->isEquipavel() && itemAvaliado != armaEquipada && itemAvaliado != escudoEquipado && itemAvaliado != armaduraEquipada; 
            });
        case 'C': return buscarPorTipoAgrupado([](Item* itemAvaliado) { return itemAvaliado->obterTipo() == TipoEquipamento::CONSUMIVEL; });
        case 'S': return buscarPorTipoAgrupado([](Item* itemAvaliado) { return itemAvaliado->obterTipo() == TipoEquipamento::MATERIAL; });
        case 'M': return buscarPorTipoAgrupado([](Item* itemAvaliado) { return itemAvaliado->obterTipo() == TipoEquipamento::MISSAO; });
        default:  return nullptr;
    }
}
