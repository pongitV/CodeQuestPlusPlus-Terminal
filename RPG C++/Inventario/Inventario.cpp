#include <iostream>
#include <map>

#include "Inventario.h"
#include "Item.h"
#include "../Sistema/Menu.h"

Inventario::Inventario() : quantidadeDeOuro(0) {}

Inventario::~Inventario() 
{
    for (Item* itemAtual : listaDeItens) delete itemAtual;
    listaDeItens.clear();
}

void Inventario::exibirInventarioOrganizadoPorCategorias(Item* armaEquipada, Item* escudoEquipado, Item* armaduraEquipada) const 
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
    
    std::vector<std::string> linhasParaImprimir;

    linhasParaImprimir.push_back("DINHEIRO: " + std::to_string(quantidadeDeOuro) + " moedas");
    linhasParaImprimir.push_back("");  

    linhasParaImprimir.push_back("[ EQUIPAMENTO ]");
    if (armaEquipada) linhasParaImprimir.push_back(" [1A] ARMA:     " + armaEquipada->obterNomeItem());
    if (escudoEquipado) linhasParaImprimir.push_back(" [2A] ESCUDO:   " + escudoEquipado->obterNomeItem());
    if (armaduraEquipada) linhasParaImprimir.push_back(" [3A] ARMADURA: " + armaduraEquipada->obterNomeItem());
    linhasParaImprimir.push_back(""); 

    linhasParaImprimir.push_back("[ ARSENAL ]");
    if (outrosEquipamentos.empty()) linhasParaImprimir.push_back(" (Vazio)");
    for (size_t indice = 0; indice < outrosEquipamentos.size(); indice++) {
        linhasParaImprimir.push_back(" [" + std::to_string(indice + 1) + "E] " + outrosEquipamentos[indice]->obterNomeItem() + " [" + outrosEquipamentos[indice]->raridadeParaString() + "]");
    }
    linhasParaImprimir.push_back(""); 

    linhasParaImprimir.push_back("[ CONSUMIVEIS ]");
    if (contagemDeConsumiveis.empty()) linhasParaImprimir.push_back(" (Vazio)");
    int contadorDeConsumiveis = 1;
    for (auto const& [nomeDoItem, quantidade] : contagemDeConsumiveis) {
        linhasParaImprimir.push_back(" [" + std::to_string(contadorDeConsumiveis++) + "C] " + std::to_string(quantidade) + "x " + nomeDoItem);
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
        if ((int)linhaAtual.length() > tamanhoDaLinhaMaisLonga) {
            tamanhoDaLinhaMaisLonga = (int)linhaAtual.length();
        }
    }
    
    int espacosEmBranco = (larguraDoTerminal - tamanhoDaLinhaMaisLonga) / 2;
    std::string margemEsquerda(espacosEmBranco > 0 ? espacosEmBranco : 0, ' ');

    for (const std::string& linhaAtual : linhasParaImprimir) {
        std::cout << margemEsquerda << linhaAtual << "\n";
    }
    
    std::cout << "\n" << std::string(larguraDoTerminal, '=') << "\n";
}

Item* Inventario::buscarItemPeloCodigoDigitado(std::string codigoDigitado, Item* armaEquipada, Item* escudoEquipado, Item* armaduraEquipada)
{
    if (codigoDigitado.length() < 2) return nullptr;

    char letraDaCategoria = std::toupper(codigoDigitado.back());
    std::string parteNumerica = codigoDigitado.substr(0, codigoDigitado.length() - 1);
    
    for(char caractere : parteNumerica) {
        if(!isdigit(caractere)) return nullptr;
    }
    
    int indiceDoItem = std::stoi(parteNumerica);
    if (indiceDoItem <= 0) return nullptr;

    if (letraDaCategoria == 'A')
    {
        if (indiceDoItem == 1) return armaEquipada;
        if (indiceDoItem == 2) return escudoEquipado;
        if (indiceDoItem == 3) return armaduraEquipada;
    }
    else if (letraDaCategoria == 'E')
    {
        std::vector<Item*> outrosEquipamentos;
        for (Item* itemAtual : listaDeItens) {
            TipoEquipamento tipoDoItem = itemAtual->obterTipo();

            if ((tipoDoItem == TipoEquipamento::ARMA || tipoDoItem == TipoEquipamento::ESCUDO || tipoDoItem == TipoEquipamento::ARMADURA) 
                && itemAtual != armaEquipada && itemAtual != escudoEquipado && itemAtual != armaduraEquipada) {
                outrosEquipamentos.push_back(itemAtual);
            }
        }

        if (indiceDoItem <= static_cast<int>(outrosEquipamentos.size())) {
            return outrosEquipamentos[indiceDoItem - 1];
        }
    }
    else if (letraDaCategoria == 'C')
    {
        std::map<std::string, int> contagemDeConsumiveis;

        for (Item* itemAtual : listaDeItens) {
            if (itemAtual->obterTipo() == TipoEquipamento::CONSUMIVEL) {
                contagemDeConsumiveis[itemAtual->obterNomeItem()]++;
            }
        }

        int contadorAtual = 1;

        for (auto const& [nomeDoItem, quantidade] : contagemDeConsumiveis) {
            if (contadorAtual++ == indiceDoItem) {
                for (Item* itemAtual : listaDeItens) {
                    if (itemAtual->obterNomeItem() == nomeDoItem) return itemAtual;
                }
            }
        }
    }
    else if (letraDaCategoria == 'M')
    {
        std::vector<Item*> itensDeMissao;
        for (Item* itemAtual : listaDeItens) {
            if (itemAtual->obterTipo() == TipoEquipamento::MISSAO) {
                itensDeMissao.push_back(itemAtual);
            }
        }

        if (indiceDoItem <= static_cast<int>(itensDeMissao.size())) {
            return itensDeMissao[indiceDoItem - 1];
        }
    }
    
    return nullptr;
}

Item* Inventario::abrirMenuParaSelecionarEscudoDeDefesa() 
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
    std::cout << " [0] Cancelar\n";
    std::cout << "\nEscolha: ";

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

void Inventario::adicionarItemAoInventario(Item* novoItem) { if(novoItem) listaDeItens.push_back(novoItem); }

void Inventario::removerItemDoInventarioPeloNome(std::string nomeDoItemParaRemover) 
{
    for (size_t indice = 0; indice < listaDeItens.size(); indice++) 
    {
        if (listaDeItens[indice]->obterNomeItem() == nomeDoItemParaRemover) 
        { 
            delete listaDeItens[indice];  // Libera a memoria
            listaDeItens.erase(listaDeItens.begin() + indice);  // Remove do vetor
            break;  // Para apos remover o primeiro
        }
    }
}

int Inventario::contarQuantidadeDeUmItemPeloNome(std::string nomeDoItem) const 
{
    int quantidadeEncontrada = 0; 
    for (Item* itemAtual : listaDeItens) {
        if (itemAtual->obterNomeItem() == nomeDoItem) quantidadeEncontrada++; 
    }
    return quantidadeEncontrada;
}

bool Inventario::verificarSePossuiPocaoDeCura() const 
{
    for (Item* itemAtual : listaDeItens) {
        if (dynamic_cast<PocaoCura*>(itemAtual)) return true; 
    }
    return false;
}

void Inventario::adicionarOuroAoInventario(int quantidadeAdicional) { quantidadeDeOuro += quantidadeAdicional; if (quantidadeDeOuro < 0) quantidadeDeOuro = 0; }
int Inventario::obterQuantidadeDeOuro() const { return quantidadeDeOuro; }
bool Inventario::verificarSeInventarioEstaVazio() const { return listaDeItens.empty(); }