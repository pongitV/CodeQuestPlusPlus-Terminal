#include <iostream>
#include <map>
#include <algorithm>

#include "Inventario.h"
#include "Item.h"
#include "../Sistema/Menu.h"

Inventario::Inventario() : quantidadeDeOuro(0) {}

Inventario::~Inventario() 
{
    for (Item* itemAtual : listaDeItens) 
    {
        delete itemAtual;
    }
    listaDeItens.clear();
}

void Inventario::exibirInventario(Item* armaEquipada, Item* escudoEquipado, Item* armaduraEquipada) const 
{
    int larguraDoTerminal = Menu::obterLarguraDoTerminalEmColunas();

    std::vector<Item*> outrosEquipamentos;  
    std::map<std::string, int> contagemDeConsumiveis;  
    std::vector<Item*> itensDeMissao;  

    // Organiza os itens do inventario por categoria para exibicao
    for (Item* itemAtual : listaDeItens) {
        TipoEquipamento tipoDoItem = itemAtual->obterTipo();
        
        if (tipoDoItem == TipoEquipamento::CONSUMIVEL) {
            contagemDeConsumiveis[itemAtual->obterNomeItem()]++;
        } else if (tipoDoItem == TipoEquipamento::MISSAO) {
            itensDeMissao.push_back(itemAtual);
        } else if ((tipoDoItem == TipoEquipamento::ARMA || tipoDoItem == TipoEquipamento::ESCUDO || tipoDoItem == TipoEquipamento::ARMADURA) 
                   && itemAtual != armaEquipada && itemAtual != escudoEquipado && itemAtual != armaduraEquipada) {
            outrosEquipamentos.push_back(itemAtual);
        }
    }
    
    auto obterPrecoVendaPorNome = [](const std::string& nome) -> int {
        if (nome == "Adaga artesanal de pedra") return 5;
        if (nome.find("Pocao") != std::string::npos || nome.find("Poção") != std::string::npos) return 6;
        if (nome == "Manto encantado" || nome == "Escudo medio de metal" || nome == "Capa magica" || nome == "Escudo leve de madeira") return 9;
        return 3;
    };

    std::vector<std::string> linhasParaImprimir;

    linhasParaImprimir.push_back("DINHEIRO: " + std::to_string(quantidadeDeOuro) + " moedas");
    linhasParaImprimir.push_back("");  

    linhasParaImprimir.push_back("[ EQUIPAMENTO ]");
    if (armaEquipada) linhasParaImprimir.push_back(" [1E] ARMA:     " + armaEquipada->obterNomeItem());
    if (escudoEquipado) linhasParaImprimir.push_back(" [2E] ESCUDO:   " + escudoEquipado->obterNomeItem());
    if (armaduraEquipada) linhasParaImprimir.push_back(" [3E] ARMADURA: " + armaduraEquipada->obterNomeItem());
    linhasParaImprimir.push_back(""); 

    linhasParaImprimir.push_back("[ ARSENAL ]");
    if (outrosEquipamentos.empty()) linhasParaImprimir.push_back(" (Vazio)");
    for (size_t indice = 0; indice < outrosEquipamentos.size(); indice++) {
        linhasParaImprimir.push_back(" [" + std::to_string(indice + 1) + "A] " + outrosEquipamentos[indice]->obterNomeItem() + " [" + outrosEquipamentos[indice]->raridadeParaString() + "] (Venda: " + std::to_string(obterPrecoVendaPorNome(outrosEquipamentos[indice]->obterNomeItem())) + "G)");
    }
    linhasParaImprimir.push_back(""); 

    linhasParaImprimir.push_back("[ CONSUMIVEIS ]");
    if (contagemDeConsumiveis.empty()) linhasParaImprimir.push_back(" (Vazio)");
    int contadorDeConsumiveis = 1;
    for (auto const& [nomeDoItem, quantidade] : contagemDeConsumiveis) {
        linhasParaImprimir.push_back(" [" + std::to_string(contadorDeConsumiveis++) + "C] " + std::to_string(quantidade) + "x " + nomeDoItem + " (Venda: " + std::to_string(obterPrecoVendaPorNome(nomeDoItem)) + "G / un)");
    }
    linhasParaImprimir.push_back("");

    linhasParaImprimir.push_back("[ ITENS DE MISSAO ]");
    if (itensDeMissao.empty()) linhasParaImprimir.push_back(" (Vazio)");
    for (size_t indice = 0; indice < itensDeMissao.size(); indice++) {
        linhasParaImprimir.push_back(" [" + std::to_string(indice + 1) + "M] " + itensDeMissao[indice]->obterNomeItem());
    }

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
    }
    else if (letraDaCategoria == 'A')
    {
        int contadorAtual = 0;
        for (Item* itemAtual : listaDeItens) {
            TipoEquipamento tipoDoItem = itemAtual->obterTipo();

            if ((tipoDoItem == TipoEquipamento::ARMA || tipoDoItem == TipoEquipamento::ESCUDO || tipoDoItem == TipoEquipamento::ARMADURA) 
                && itemAtual != armaEquipada && itemAtual != escudoEquipado && itemAtual != armaduraEquipada) {
                if (++contadorAtual == indiceDoItem) return itemAtual;
            }
        }
    }
    else if (letraDaCategoria == 'C')
    {
        std::map<std::string, Item*> consumiveisUnicos;
        for (Item* itemAtual : listaDeItens) {
            if (itemAtual->obterTipo() == TipoEquipamento::CONSUMIVEL) {
                consumiveisUnicos.emplace(itemAtual->obterNomeItem(), itemAtual);
            }
        }

        int contadorAtual = 1;
        for (auto const& par : consumiveisUnicos) {
            if (contadorAtual++ == indiceDoItem) return par.second;
        }
    }
    else if (letraDaCategoria == 'M')
    {
        int contadorAtual = 0;
        for (Item* itemAtual : listaDeItens) {
            if (itemAtual->obterTipo() == TipoEquipamento::MISSAO) {
                if (++contadorAtual == indiceDoItem) return itemAtual;
            }
        }
    }
    
    return nullptr;
}

Item* Inventario::selecionarEscudo() 
{
    std::vector<Item*> listaDeEscudos;
    for (Item* itemAtual : listaDeItens) 
    {
        if (itemAtual->obterTipo() == TipoEquipamento::ESCUDO) {
            listaDeEscudos.push_back(itemAtual);
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
                  << " (Bloqueio Fixo: " << listaDeEscudos[indice]->obterReducaoDanoFixaEscudo() 
                  << " | Durabilidade: " << listaDeEscudos[indice]->obterDurabilidadeAtualEscudo() << " usos)\n";
    }
    std::cout << " [0] Cancelar\n\nEscolha: ";

    int opcaoEscolhida;

    if (!(std::cin >> opcaoEscolhida) || opcaoEscolhida < 0 || opcaoEscolhida > static_cast<int>(listaDeEscudos.size())) 
    {
        std::cin.clear(); std::cin.ignore(1000, '\n');
        std::cout << "Opcao invalida!\n";
        return nullptr;
    }
    // Permite cancelar a operacao
    if (opcaoEscolhida == 0) return nullptr;
    return listaDeEscudos[opcaoEscolhida - 1];
}

void Inventario::adicionarItem(Item* novoItem) 
{ 
    if (novoItem) listaDeItens.push_back(novoItem); 
}

void Inventario::removerItem(const std::string& nomeDoItem) 
{
    auto it = std::find_if(listaDeItens.begin(), listaDeItens.end(), [&](Item* item) 
    {
        return item->obterNomeItem() == nomeDoItem;
    });
    
    if (it != listaDeItens.end()) 
    {
        delete *it;
        listaDeItens.erase(it);
    }
}

int Inventario::contarItem(const std::string& nomeDoItem) const 
{
    return std::count_if(listaDeItens.begin(), listaDeItens.end(), [&](Item* item) 
    {
        return item->obterNomeItem() == nomeDoItem;
    });
}

bool Inventario::possuiPocaoDeCura() const 
{
    return std::any_of(listaDeItens.begin(), listaDeItens.end(), [](Item* item) 
    {
        return dynamic_cast<PocaoCura*>(item) != nullptr;
    });
}

void Inventario::adicionarOuro(int quantidadeAdicional) 
{ 
    quantidadeDeOuro = std::max(0, quantidadeDeOuro + quantidadeAdicional); 
}

int Inventario::obterOuro() const { return quantidadeDeOuro; }
bool Inventario::estaVazio() const { return listaDeItens.empty(); }