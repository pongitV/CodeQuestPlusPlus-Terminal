#include <iostream>
#include <map>
#include <algorithm>
#include <unordered_map>

#include "Inventario.h"
#include "Item.h"
#include "ItemConsumivel.h"
#include "../Gerenciadores/GerenciadorMenu.h"
#include "../Utilidades/SimplificacoesAparencia.h"

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
        std::vector<std::string> nomesVisual;

        for (size_t i = 0; i < listaDeItens.size(); ++i) {
            Item* itemAtual = listaDeItens[i].get();
            if (condicao(itemAtual)) {
                if (cacheDeIndice.find(itemAtual->obterNomeItem()) == cacheDeIndice.end()) {
                    cacheDeIndice[itemAtual->obterNomeItem()] = i;
                    nomesVisual.push_back(itemAtual->obterNomeItem());
                }
            }
        }
        
        std::sort(nomesVisual.begin(), nomesVisual.end());

        if (indiceDoItem > 0 && indiceDoItem <= static_cast<int>(nomesVisual.size())) {
            size_t idxOriginal = cacheDeIndice[nomesVisual[indiceDoItem - 1]];
            return listaDeItens[idxOriginal].get();
        }
        return nullptr;
    };

    switch (letraDaCategoria) {
        case 'A':
            return buscarPorTipoAgrupado([&](Item* i) { 
                auto t = i->obterTipo(); 
                return (t == TipoEquipamento::ARMA || t == TipoEquipamento::ESCUDO || t == TipoEquipamento::ARMADURA) && i != armaEquipada && i != escudoEquipado && i != armaduraEquipada; 
            });
        case 'C': return buscarPorTipoAgrupado([](Item* i) { return i->obterTipo() == TipoEquipamento::CONSUMIVEL; });
        case 'S': return buscarPorTipoAgrupado([](Item* i) { return i->obterTipo() == TipoEquipamento::MATERIAL; });
        case 'M': return buscarPorTipoAgrupado([](Item* i) { return i->obterTipo() == TipoEquipamento::MISSAO; });
        default:  return nullptr;
    }
}

Item* Inventario::selecionarEscudo() 
{
    std::vector<Item*> listaDeEscudos;
    for (const auto& itemUnique : listaDeItens) 
    {
        if (itemUnique->obterTipo() == TipoEquipamento::ESCUDO) {
            listaDeEscudos.push_back(itemUnique.get());
        }
    }

    if (listaDeEscudos.empty()) 
    {
        std::cout << "\n[!] Voce nao possui escudos no inventario para usar!\n";
        return nullptr;
    }

    std::cout << "=== SELECIONE SEU ESCUDO ===\n";
    for (size_t indice = 0; indice < listaDeEscudos.size(); indice++) 
    {
        std::cout << " [" << indice + 1 << "] " << listaDeEscudos[indice]->obterNomeItem()
                  << listaDeEscudos[indice]->obterInfoStatus() << "\n";
    }
    std::cout << " [0] Cancelar\n\nEscolha: ";

    int opcaoEscolhida;

    while (!(std::cin >> opcaoEscolhida) || opcaoEscolhida < 0 || opcaoEscolhida > static_cast<int>(listaDeEscudos.size())) 
    {
        std::cin.clear(); std::cin.ignore(1000, '\n');
        std::cout << "Opcao invalida! Escolha novamente: ";
    }
    return (opcaoEscolhida == 0) ? nullptr : listaDeEscudos[opcaoEscolhida - 1];
}
