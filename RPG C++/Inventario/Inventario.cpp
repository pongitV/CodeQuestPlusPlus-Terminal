#include <iostream>
#include <map>
#include <algorithm>

#include "Inventario.h"
#include "Item.h"
#include "ItemConsumivel.h"
#include "../Sistema/Menu.h"

Inventario::Inventario() : quantidadeDeOuro(0) {}

Inventario::~Inventario() 
{
    equipamentos.clear();
    consumiveis.clear();
    materiais.clear();
    missoes.clear();
}

void Inventario::exibirInventario(Item* armaEquipada, Item* escudoEquipado, Item* armaduraEquipada, bool mostrarPrecos) const 
{
    int larguraDoTerminal = Menu::obterLarguraDoTerminalEmColunas();

    auto formatarGrupo = [&](const std::vector<std::unique_ptr<Item>>& lista, const std::string& titulo, std::string tag, Item* eq1 = nullptr, Item* eq2 = nullptr, Item* eq3 = nullptr) {
        std::vector<std::string> linhas;
        linhas.push_back(titulo);
        
        std::map<std::string, std::vector<Item*>> agrupados;
        for (const auto& item : lista) {
            if (item.get() != eq1 && item.get() != eq2 && item.get() != eq3) {
                agrupados[item->obterNomeItem()].push_back(item.get());
            }
        }
        
        if (agrupados.empty()) {
            linhas.push_back(" (Vazio)");
        } else {
            int indice = 1;
            for (auto const& [nome, itens] : agrupados) {
                Item* ex = itens.front();
                std::string infoVenda = mostrarPrecos ? " (Venda: " + std::to_string(ex->obterPrecoVenda()) + "G)" : "";
                std::string prefixo = itens.size() > 1 ? std::to_string(itens.size()) + "x " : "";
                std::string raridade = tag == "A" ? " [" + ex->raridadeParaString() + "]" : "";
                linhas.push_back(" [" + std::to_string(indice++) + tag + "] " + prefixo + nome + raridade + ex->obterInfoStatus() + infoVenda);
            }
        }
        linhas.push_back("");
        return linhas;
    };

    std::vector<std::string> linhasParaImprimir;

    linhasParaImprimir.push_back("DINHEIRO: " + std::to_string(quantidadeDeOuro) + " moedas");
    linhasParaImprimir.push_back("");  

    linhasParaImprimir.push_back("[ EQUIPAMENTO ]");
    if (armaEquipada) linhasParaImprimir.push_back(" [1E] ARMA:     " + armaEquipada->obterNomeItem() + armaEquipada->obterInfoStatus());
    if (escudoEquipado) linhasParaImprimir.push_back(" [2E] ESCUDO:   " + escudoEquipado->obterNomeItem() + escudoEquipado->obterInfoStatus());
    if (armaduraEquipada) linhasParaImprimir.push_back(" [3E] ARMADURA: " + armaduraEquipada->obterNomeItem() + armaduraEquipada->obterInfoStatus());
    linhasParaImprimir.push_back(""); 

    auto linEquip = formatarGrupo(equipamentos, "[ ARSENAL ]", "A", armaEquipada, escudoEquipado, armaduraEquipada);
    linhasParaImprimir.insert(linhasParaImprimir.end(), linEquip.begin(), linEquip.end());
    
    auto linCons = formatarGrupo(consumiveis, "[ CONSUMIVEIS ]", "C");
    linhasParaImprimir.insert(linhasParaImprimir.end(), linCons.begin(), linCons.end());
    
    auto linMat = formatarGrupo(materiais, "[ ESTOQUE ]", "S");
    linhasParaImprimir.insert(linhasParaImprimir.end(), linMat.begin(), linMat.end());
    
    auto linMiss = formatarGrupo(missoes, "[ ITENS DE MISSAO ]", "M");
    linhasParaImprimir.insert(linhasParaImprimir.end(), linMiss.begin(), linMiss.end());

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

    auto buscarPorTipoAgrupado = [&](const std::vector<std::unique_ptr<Item>>& lista) -> Item* {
        std::map<std::string, Item*> itensAgrupados;
        for (const auto& item : lista) {
            if (item.get() != armaEquipada && item.get() != escudoEquipado && item.get() != armaduraEquipada) {
                if (itensAgrupados.find(item->obterNomeItem()) == itensAgrupados.end()) {
                    itensAgrupados[item->obterNomeItem()] = item.get();
                }
            }
        }
        int contadorAtual = 1;
        for (auto const& par : itensAgrupados) {
            if (contadorAtual++ == indiceDoItem) return par.second;
        }
        return nullptr;
    };

    if (letraDaCategoria == 'A') return buscarPorTipoAgrupado(equipamentos);
    if (letraDaCategoria == 'C') return buscarPorTipoAgrupado(consumiveis);
    if (letraDaCategoria == 'S') return buscarPorTipoAgrupado(materiais);
    if (letraDaCategoria == 'M') return buscarPorTipoAgrupado(missoes);
    
    return nullptr;
}

Item* Inventario::selecionarEscudo() 
{
    std::vector<Item*> listaDeEscudos;
    for (const auto& item : equipamentos) 
    {
        if (item->obterTipo() == TipoEquipamento::ESCUDO) {
            listaDeEscudos.push_back(item.get());
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

void Inventario::adicionarItem(std::unique_ptr<Item> novoItem) 
{
    if (!novoItem) return;
    TipoEquipamento tipo = novoItem->obterTipo();
    if (tipo == TipoEquipamento::ARMA || tipo == TipoEquipamento::ESCUDO || tipo == TipoEquipamento::ARMADURA) {
        equipamentos.push_back(std::move(novoItem));
    } else if (tipo == TipoEquipamento::CONSUMIVEL) {
        consumiveis.push_back(std::move(novoItem));
    } else if (tipo == TipoEquipamento::MATERIAL) {
        materiais.push_back(std::move(novoItem));
    } else if (tipo == TipoEquipamento::MISSAO) {
        missoes.push_back(std::move(novoItem));
    }
}

void Inventario::removerItem(const std::string& nomeDoItem) 
{
    auto apagarSeEncontrado = [&](std::vector<std::unique_ptr<Item>>& lista) -> bool {
        auto it = std::find_if(lista.begin(), lista.end(), [&](const std::unique_ptr<Item>& item) {
            return item->obterNomeItem() == nomeDoItem;
        });
        if (it != lista.end()) {
            lista.erase(it);
            return true;
        }
        return false;
    };

    if (apagarSeEncontrado(equipamentos)) return;
    if (apagarSeEncontrado(consumiveis)) return;
    if (apagarSeEncontrado(materiais)) return;
    apagarSeEncontrado(missoes);
}

int Inventario::contarItem(const std::string& nomeDoItem) const 
{
    int count = 0;
    auto contarNaLista = [&](const std::vector<std::unique_ptr<Item>>& lista) {
        count += std::count_if(lista.begin(), lista.end(), [&](const std::unique_ptr<Item>& item) {
            return item->obterNomeItem() == nomeDoItem;
        });
    };
    contarNaLista(equipamentos);
    contarNaLista(consumiveis);
    contarNaLista(materiais);
    contarNaLista(missoes);
    return count;
}

bool Inventario::possuiPocaoDeCura() const 
{
    return std::any_of(consumiveis.begin(), consumiveis.end(), [](const std::unique_ptr<Item>& item) {
        return item->temPropriedade(Propriedade::ConsumivelCura);
    });
}

void Inventario::adicionarOuro(int quantidadeAdicional) 
{ 
    quantidadeDeOuro = std::max(0, quantidadeDeOuro + quantidadeAdicional); 
}

int Inventario::obterOuro() const { return quantidadeDeOuro; }
bool Inventario::estaVazio() const { return equipamentos.empty() && consumiveis.empty() && materiais.empty() && missoes.empty(); }