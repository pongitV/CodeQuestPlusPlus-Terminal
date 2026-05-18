#include <iostream>
#include <vector>
#include <map>
#include <chrono>

#include "TelaInventario.h"
#include "TelaBase.h"
#include "TelaMenu.h"
#include "../Utilidades/ControleDeInput.h"
#include "../Utilidades/Aparencia.h"
#include "TelaInventarioLayouts.h"

void TelaInventario::exibir(SistemaPersonagem* jogadorAtual, bool mostrarPrecos) 
{

  if (jogadorAtual == nullptr) return;
    Aparencia::limparTela();
    
    static auto ultimoAcesso = std::chrono::steady_clock::now() - std::chrono::hours(1);
    bool animar = TelaBase::deveAnimarEntradaDaTela(ultimoAcesso, 300);

    int largura = Aparencia::obterLarguraTerminal();
    Aparencia::exibirPainelArte(ArtesInventario::logoInventario, 121, Cor::AMARELO, "", animar);

    int larguraDoTerminal = Aparencia::obterLarguraTerminal();

    Item* armaEquipada = jogadorAtual->obterArma();
    Item* escudoEquipado = jogadorAtual->obterEscudo();
    Item* armaduraEquipada = jogadorAtual->obterArmadura();

    std::map<std::string, std::vector<Item*>> equipamentosAgrupados;  
    std::map<std::string, std::vector<Item*>> consumiveisAgrupados;  
    std::map<std::string, std::vector<Item*>> materiaisAgrupados;  
    std::map<std::string, std::vector<Item*>> missoesAgrupadas;  

    for (Item* item : jogadorAtual->obterInventario()->obterTodosOsItens()) {
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

    linhasParaImprimir.push_back("DINHEIRO: " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + " moedas");
    linhasParaImprimir.push_back("");  

    linhasParaImprimir.push_back("═══ EQUIPAMENTO ═══");
    if (armaEquipada) linhasParaImprimir.push_back(" [1E] ARMA:     " + armaEquipada->obterNomeItem());
    if (escudoEquipado) linhasParaImprimir.push_back(" [2E] ESCUDO:   " + escudoEquipado->obterNomeItem());
    if (armaduraEquipada) linhasParaImprimir.push_back(" [3E] ARMADURA: " + armaduraEquipada->obterNomeItem());
    linhasParaImprimir.push_back(""); 

    auto formatarAgrupamento = [&](const std::string& titulo, const std::map<std::string, std::vector<Item*>>& grupo, char letra, const std::string& sufixoVenda) {
        linhasParaImprimir.push_back("═══ " + titulo + " ═══");
        if (grupo.empty()) {
            linhasParaImprimir.push_back(" (Vazio)");
        } else {
            int indice = 1;
            for (auto const& [nome, lista] : grupo) {
                Item* item = lista.front();
                std::string prefixo = (lista.size() > 1) ? std::to_string(lista.size()) + "x " : "";
                std::string infoVenda = mostrarPrecos ? " (Venda: " + std::to_string(item->obterPrecoVenda()) + sufixoVenda + ")" : "";
                linhasParaImprimir.push_back(" [" + std::to_string(indice++) + letra + "] " + prefixo + nome + infoVenda);
            }
        }
        linhasParaImprimir.push_back("");
    };

    formatarAgrupamento("ARSENAL", equipamentosAgrupados, 'A', "G");
    formatarAgrupamento("CONSUMIVEIS", consumiveisAgrupados, 'C', "G / un");
    formatarAgrupamento("ESTOQUE", materiaisAgrupados, 'S', "G / un");
    formatarAgrupamento("ITENS DE MISSAO", missoesAgrupadas, 'M', "");

    Aparencia::imprimirBlocoCentralizado(linhasParaImprimir, "");
    
    std::cout << "\n";
    Aparencia::imprimirLinhaDivisoria();
}

void TelaInventario::exibirMenuInteracaoItem(Item* itemEncontrado)
{
    Aparencia::limparTela();
    TelaMenu::exibirPainelLogoJogo("OPCOES DE ITEM");
    
    TipoEquipamento tipo = itemEncontrado->obterTipo();
    std::string opcao1 = "[1] Usar";
    std::string opcao2 = "[2] Inspecionar Detalhes";

    if (tipo == TipoEquipamento::ARMA || tipo == TipoEquipamento::ESCUDO || tipo == TipoEquipamento::ARMADURA) {
        opcao1 = "[1] Equipar / Desequipar";
        opcao2 = "[2] Inspecionar (Dano, Durabilidade, Requisitos, etc)";
    } else if (tipo == TipoEquipamento::MISSAO) {
        opcao1 = "[1] Usar em quantidade";
        opcao2 = "[2] Inspecionar (Lore)";
    } else if (tipo == TipoEquipamento::CONSUMIVEL) {
        opcao1 = "[1] Usar em quantidade";
        opcao2 = "[2] Inspecionar Efeitos";
    }

    std::vector<std::string> linhas = {
        "Item Selecionado: " + Aparencia::cor(Cor::CIANO) + itemEncontrado->obterNomeItem() + Aparencia::cor(Cor::RESET),
        "",
        opcao1,
        opcao2,
        "[0] Cancelar"
    };

    std::cout << "\n";
    Aparencia::imprimirBlocoCentralizado(linhas);
    std::cout << "\n";
    Aparencia::exibirPrompt("Escolha: \033[s");
}

void TelaInventario::exibirInspecaoItem(Item* item, SistemaPersonagem* jogadorAtual)
{
    if (!item) return;
    
    std::vector<std::string> linhas;
    linhas.push_back(Aparencia::cor(Cor::CIANO) + " ═══ " + item->obterNomeItem() + " ═══" + Aparencia::cor(Cor::RESET));
    
    std::vector<std::string> detalhes = item->obterDetalhesInspecao(jogadorAtual);
    linhas.insert(linhas.end(), detalhes.begin(), detalhes.end());
    
    linhas.push_back(" > Preco de Venda: " + std::to_string(item->obterPrecoVenda()) + "G");

    std::cout << "\n";
    Aparencia::imprimirCentralizado(linhas[0]);
    std::cout << "\n";
    
    std::vector<std::string> resto(linhas.begin() + 1, linhas.end());
    Aparencia::imprimirBlocoCentralizado(resto);
}

Item* TelaInventario::lerSelecaoDeItem(SistemaPersonagem* jogadorAtual, std::string& outCodigoDigitado) {
    std::cout << "\033[s";
    Item* itemEncontrado = nullptr;
    while (true) {
        outCodigoDigitado = ControleDeInput::lerEntradaProtegida();
        if (outCodigoDigitado == "0") break;
        
        itemEncontrado = jogadorAtual->obterInventario()->buscarItemPorCodigo(
            outCodigoDigitado, 
            jogadorAtual->obterArma(), 
            jogadorAtual->obterEscudo(), 
            jogadorAtual->obterArmadura()
        );
        if (itemEncontrado) break;
        
        std::cout << "\033[u\033[J";
    }
    return itemEncontrado;
}
