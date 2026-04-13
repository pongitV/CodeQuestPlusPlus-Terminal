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

void Inventario::exibirInventario(Item* armaEquipada, Item* escudoEquipado, Item* armaduraEquipada, bool mostrarPrecos) const 
{
    int larguraDoTerminal = Menu::obterLarguraDoTerminalEmColunas();

    std::map<std::string, std::vector<Item*>> equipamentosAgrupados;  
    std::map<std::string, int> contagemDeConsumiveis;  
    std::map<std::string, int> contagemDeMateriais;  
    std::map<std::string, std::vector<Item*>> missoesAgrupadas;  

    // Organiza os itens do inventario por categoria para exibicao
    for (Item* itemAtual : listaDeItens) {
        TipoEquipamento tipoDoItem = itemAtual->obterTipo();
        
        if (tipoDoItem == TipoEquipamento::CONSUMIVEL) {
            contagemDeConsumiveis[itemAtual->obterNomeItem()]++;
        } else if (tipoDoItem == TipoEquipamento::MISSAO) {
            missoesAgrupadas[itemAtual->obterNomeItem()].push_back(itemAtual);
        } else if (tipoDoItem == TipoEquipamento::MATERIAL) {
            contagemDeMateriais[itemAtual->obterNomeItem()]++;
        } else if ((tipoDoItem == TipoEquipamento::ARMA || tipoDoItem == TipoEquipamento::ESCUDO || tipoDoItem == TipoEquipamento::ARMADURA) 
                   && itemAtual != armaEquipada && itemAtual != escudoEquipado && itemAtual != armaduraEquipada) {
            equipamentosAgrupados[itemAtual->obterNomeItem()].push_back(itemAtual);
        }
    }
    
    auto obterPrecoVendaPorNome = [](const std::string& nome) -> int {
        if (nome == "Adaga artesanal de pedra") return 5;
        if (nome == "Gosma acida") return 5;
        if (nome == "Dente de goblin") return 1;
        if (nome == "Nucleo pegajoso") return 30;
        if (nome.find("Pocao") != std::string::npos || nome.find("Poção") != std::string::npos) return 6;
        if (nome == "Manto encantado" || nome == "Escudo medio de metal" || nome == "Capa magica" || nome == "Escudo leve de madeira") return 9;
        return 3;
    };

    std::vector<std::string> linhasParaImprimir;

    auto obterInfoStatus = [](Item* item) -> std::string {
        if (!item) return "";
        if (item->obterTipo() == TipoEquipamento::ARMA) {
            std::string ef = "";
            if (item->possuiEfeitoSangramento()) ef += " | +Sangramento";
            if (item->possuiEfeitoLentidao()) ef += " | +Lentidao";
            return " (Dano: " + std::to_string(item->obterDanoFisico()) + "F/" + std::to_string(item->obterDanoMagico()) + "M" + ef + ")";
        }
        if (item->obterTipo() == TipoEquipamento::ESCUDO) return " (Def: " + std::to_string(item->obterReducaoDanoFixaEscudo()) + " | Dur: " + std::to_string(item->obterDurabilidadeAtualEscudo()) + ")";
        if (item->obterTipo() == TipoEquipamento::ARMADURA) return " (Def: " + std::to_string(item->obterReducaoFixa()) + ")";
        return "";
    };

    linhasParaImprimir.push_back("DINHEIRO: " + std::to_string(quantidadeDeOuro) + " moedas");
    linhasParaImprimir.push_back("");  

    linhasParaImprimir.push_back("[ EQUIPAMENTO ]");
    if (armaEquipada) linhasParaImprimir.push_back(" [1E] ARMA:     " + armaEquipada->obterNomeItem() + obterInfoStatus(armaEquipada));
    if (escudoEquipado) linhasParaImprimir.push_back(" [2E] ESCUDO:   " + escudoEquipado->obterNomeItem() + obterInfoStatus(escudoEquipado));
    if (armaduraEquipada) linhasParaImprimir.push_back(" [3E] ARMADURA: " + armaduraEquipada->obterNomeItem() + obterInfoStatus(armaduraEquipada));
    linhasParaImprimir.push_back(""); 

    linhasParaImprimir.push_back("[ ARSENAL ]");
    if (equipamentosAgrupados.empty()) linhasParaImprimir.push_back(" (Vazio)");
    int indiceArs = 1;
    for (auto const& [nome, lista] : equipamentosAgrupados) {
        Item* itemExemplo = lista.front();
        std::string infoVenda = mostrarPrecos ? " (Venda: " + std::to_string(obterPrecoVendaPorNome(nome)) + "G)" : "";
        std::string prefixo = lista.size() > 1 ? std::to_string(lista.size()) + "x " : "";
        linhasParaImprimir.push_back(" [" + std::to_string(indiceArs++) + "A] " + prefixo + nome + " [" + itemExemplo->raridadeParaString() + "]" + obterInfoStatus(itemExemplo) + infoVenda);
    }
    linhasParaImprimir.push_back(""); 

    linhasParaImprimir.push_back("[ CONSUMIVEIS ]");
    if (contagemDeConsumiveis.empty()) linhasParaImprimir.push_back(" (Vazio)");
    int contadorDeConsumiveis = 1;
    for (auto const& [nomeDoItem, quantidade] : contagemDeConsumiveis) {
        std::string infoVenda = mostrarPrecos ? " (Venda: " + std::to_string(obterPrecoVendaPorNome(nomeDoItem)) + "G / un)" : "";
        std::string prefixo = quantidade > 1 ? std::to_string(quantidade) + "x " : "";
        linhasParaImprimir.push_back(" [" + std::to_string(contadorDeConsumiveis++) + "C] " + prefixo + nomeDoItem + infoVenda);
    }
    linhasParaImprimir.push_back("");

    linhasParaImprimir.push_back("[ ESTOQUE ]");
    if (contagemDeMateriais.empty()) linhasParaImprimir.push_back(" (Vazio)");
    int contadorDeMateriais = 1;
    for (auto const& [nomeDoItem, quantidade] : contagemDeMateriais) {
        std::string infoVenda = mostrarPrecos ? " (Venda: " + std::to_string(obterPrecoVendaPorNome(nomeDoItem)) + "G / un)" : "";
        std::string prefixo = quantidade > 1 ? std::to_string(quantidade) + "x " : "";
        linhasParaImprimir.push_back(" [" + std::to_string(contadorDeMateriais++) + "S] " + prefixo + nomeDoItem + infoVenda);
    }
    linhasParaImprimir.push_back("");

    linhasParaImprimir.push_back("[ ITENS DE MISSAO ]");
    if (missoesAgrupadas.empty()) linhasParaImprimir.push_back(" (Vazio)");
    int indiceMis = 1;
    for (auto const& [nome, lista] : missoesAgrupadas) {
        std::string prefixo = lista.size() > 1 ? std::to_string(lista.size()) + "x " : "";
        linhasParaImprimir.push_back(" [" + std::to_string(indiceMis++) + "M] " + prefixo + nome);
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
        return nullptr;
    }

    auto buscarPorTipoAgrupado = [&](auto condicao) -> Item* {
        std::map<std::string, Item*> itensAgrupados;
        for (Item* itemAtual : listaDeItens) {
            if (condicao(itemAtual)) {
                if (itensAgrupados.find(itemAtual->obterNomeItem()) == itensAgrupados.end()) {
                    itensAgrupados[itemAtual->obterNomeItem()] = itemAtual;
                }
            }
        }
        int contadorAtual = 1;
        for (auto const& par : itensAgrupados) {
            if (contadorAtual++ == indiceDoItem) return par.second;
        }
        return nullptr;
    };

    if (letraDaCategoria == 'A') {
        return buscarPorTipoAgrupado([&](Item* i) { 
            auto t = i->obterTipo(); 
            return (t == TipoEquipamento::ARMA || t == TipoEquipamento::ESCUDO || t == TipoEquipamento::ARMADURA) && i != armaEquipada && i != escudoEquipado && i != armaduraEquipada; 
        });
    } else if (letraDaCategoria == 'C') {
        return buscarPorTipoAgrupado([](Item* i) { return i->obterTipo() == TipoEquipamento::CONSUMIVEL; });
    } else if (letraDaCategoria == 'S') {
        return buscarPorTipoAgrupado([](Item* i) { return i->obterTipo() == TipoEquipamento::MATERIAL; });
    } else if (letraDaCategoria == 'M') {
        return buscarPorTipoAgrupado([](Item* i) { return i->obterTipo() == TipoEquipamento::MISSAO; });
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