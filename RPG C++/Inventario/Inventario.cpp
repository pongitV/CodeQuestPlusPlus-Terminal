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

void Inventario::exibirInventario(Item* armaEquipada, Item* escudoEquipado, Item* armaduraEquipada, bool mostrarPrecos) const 
{
    int larguraDoTerminal = SimplificacoesAparencia::obterLarguraTerminal();

    std::map<std::string, std::vector<Item*>> equipamentosAgrupados;  
    std::map<std::string, std::vector<Item*>> consumiveisAgrupados;  
    std::map<std::string, std::vector<Item*>> materiaisAgrupados;  
    std::map<std::string, std::vector<Item*>> missoesAgrupadas;  

    // Organiza os itens do inventario por categoria para exibicao
    for (const auto& itemUnique : listaDeItens) {
        Item* item = itemUnique.get();
        TipoEquipamento tipo = item->obterTipo();
        if (tipo == TipoEquipamento::CONSUMIVEL) consumiveisAgrupados[item->obterNomeItem()].push_back(item);
        else if (tipo == TipoEquipamento::MISSAO) missoesAgrupadas[item->obterNomeItem()].push_back(item);
        else if (tipo == TipoEquipamento::MATERIAL) materiaisAgrupados[item->obterNomeItem()].push_back(item);
        else if ((tipo == TipoEquipamento::ARMA || tipo == TipoEquipamento::ESCUDO || tipo == TipoEquipamento::ARMADURA) 
                 && item != armaEquipada && item != escudoEquipado && item != armaduraEquipada) {
            equipamentosAgrupados[item->obterNomeItem()].push_back(item);
        }
    }
    
    std::vector<std::string> linhasParaImprimir;

    linhasParaImprimir.push_back("DINHEIRO: " + std::to_string(quantidadeDeOuro) + " moedas");
    linhasParaImprimir.push_back("");  

    linhasParaImprimir.push_back("[ EQUIPAMENTO ]");
    if (armaEquipada) linhasParaImprimir.push_back(" [1E] ARMA:     " + armaEquipada->obterNomeItem() + armaEquipada->obterInfoStatus());
    if (escudoEquipado) linhasParaImprimir.push_back(" [2E] ESCUDO:   " + escudoEquipado->obterNomeItem() + escudoEquipado->obterInfoStatus());
    if (armaduraEquipada) linhasParaImprimir.push_back(" [3E] ARMADURA: " + armaduraEquipada->obterNomeItem() + armaduraEquipada->obterInfoStatus());
    linhasParaImprimir.push_back(""); 

    auto formatarAgrupamento = [&](const std::string& titulo, const std::map<std::string, std::vector<Item*>>& grupo, char letra, const std::string& sufixoVenda) {
        linhasParaImprimir.push_back(titulo);
        if (grupo.empty()) {
            linhasParaImprimir.push_back(" (Vazio)");
        } else {
            int indice = 1;
            for (auto const& [nome, lista] : grupo) {
                Item* item = lista.front();
                std::string prefixo = (lista.size() > 1) ? std::to_string(lista.size()) + "x " : "";
                std::string infoVenda = mostrarPrecos ? " (Venda: " + std::to_string(item->obterPrecoVenda()) + sufixoVenda + ")" : "";
                linhasParaImprimir.push_back(" [" + std::to_string(indice++) + letra + "] " + prefixo + nome + item->obterInfoStatus() + infoVenda);
            }
        }
        linhasParaImprimir.push_back("");
    };

    formatarAgrupamento("[ ARSENAL ]", equipamentosAgrupados, 'A', "G");
    formatarAgrupamento("[ CONSUMIVEIS ]", consumiveisAgrupados, 'C', "G / un");
    formatarAgrupamento("[ ESTOQUE ]", materiaisAgrupados, 'S', "G / un");
    formatarAgrupamento("[ ITENS DE MISSAO ]", missoesAgrupadas, 'M', "");

    // Centraliza todo o bloco de inventario na tela
    int tamanhoDaLinhaMaisLonga = 0;
    for (const std::string& linhaAtual : linhasParaImprimir) {
        tamanhoDaLinhaMaisLonga = std::max(tamanhoDaLinhaMaisLonga, static_cast<int>(linhaAtual.length()));
    }
    
    int espacosEmBranco = std::max(0, (larguraDoTerminal - tamanhoDaLinhaMaisLonga) / 2);
    std::string margemEsquerda(espacosEmBranco, ' ');

    for (const std::string& linhaAtual : linhasParaImprimir) {
        std::cout << margemEsquerda << linhaAtual << "\n";
    }
    
    std::cout << "\n" << std::string(larguraDoTerminal, '=') << "\n";
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

int Inventario::obterOuro() const { return quantidadeDeOuro; }
bool Inventario::estaVazio() const { return listaDeItens.empty(); }