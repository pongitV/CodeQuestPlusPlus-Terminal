#include <iostream>
#include <vector>
#include <map>

#include "TelaInventario.h"
#include "TelaMenu.h"
#include "../Utilidades/SimplificacoesAparencia.h"

void TelaInventario::exibir(SistemaPersonagem* jogadorAtual, bool mostrarPrecos) 
{

  if (jogadorAtual == nullptr) return;
    SimplificacoesAparencia::limparTela();
    
    int largura = SimplificacoesAparencia::obterLarguraTerminal();
    std::vector<std::string> logoInventario = 
    {
      "  █████ ██████   █████ █████   █████ ██████████ ██████   █████ ███████████   █████████   ███████████   █████    ███████    ",
      " ░░███ ░░██████ ░░███ ░░███   ░░███ ░░███░░░░░█░░██████ ░░███ ░█░░░███░░░█  ███░░░░░███ ░░███░░░░░███ ░░███   ███░░░░░███  ",
      "  ░███  ░███░███ ░███  ░███    ░███  ░███  █ ░  ░███░███ ░███ ░   ░███  ░  ░███    ░███  ░███    ░███  ░███  ███     ░░███ ",
      "  ░███  ░███░░███░███  ░███    ░███  ░██████    ░███░░███░███     ░███     ░███████████  ░██████████   ░███ ░███      ░███ ",
      "  ░███  ░███ ░░██████  ░░███   ███   ░███░░█    ░███ ░░██████     ░███     ░███░░░░░███  ░███░░░░░███  ░███ ░███      ░███ ",
      "  ░███  ░███  ░░█████   ░░░█████░    ░███ ░   █ ░███  ░░█████     ░███     ░███    ░███  ░███    ░███  ░███ ░░███     ███  ",
      "  █████ █████  ░░█████    ░░███      ██████████ █████  ░░█████    █████    █████   █████ █████   █████ █████ ░░░███████░   ",
      " ░░░░░ ░░░░░    ░░░░░      ░░░      ░░░░░░░░░░ ░░░░░    ░░░░░    ░░░░░    ░░░░░   ░░░░░ ░░░░░   ░░░░░ ░░░░░    ░░░░░░░     "
    };

    std::cout << "\n";
    SimplificacoesAparencia::imprimirLinhaDivisoria();
    std::cout << "\n";
    SimplificacoesAparencia::imprimirCentralizadoMultilinha(logoInventario, 121, SimplificacoesAparencia::cor(Cor::AMARELO)); 
    std::cout << "\n";
    SimplificacoesAparencia::imprimirLinhaDivisoria();
    std::cout << "\n";

    int larguraDoTerminal = SimplificacoesAparencia::obterLarguraTerminal();

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

    linhasParaImprimir.push_back("[ EQUIPAMENTO ]");
    if (armaEquipada) linhasParaImprimir.push_back(" [1E] ARMA:     " + armaEquipada->obterNomeItem());
    if (escudoEquipado) linhasParaImprimir.push_back(" [2E] ESCUDO:   " + escudoEquipado->obterNomeItem());
    if (armaduraEquipada) linhasParaImprimir.push_back(" [3E] ARMADURA: " + armaduraEquipada->obterNomeItem());
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
                linhasParaImprimir.push_back(" [" + std::to_string(indice++) + letra + "] " + prefixo + nome + infoVenda);
            }
        }
        linhasParaImprimir.push_back("");
    };

    formatarAgrupamento("[ ARSENAL ]", equipamentosAgrupados, 'A', "G");
    formatarAgrupamento("[ CONSUMIVEIS ]", consumiveisAgrupados, 'C', "G / un");
    formatarAgrupamento("[ ESTOQUE ]", materiaisAgrupados, 'S', "G / un");
    formatarAgrupamento("[ ITENS DE MISSAO ]", missoesAgrupadas, 'M', "");

    int tamanhoDaLinhaMaisLonga = 0;
    for (const std::string& linhaAtual : linhasParaImprimir) {
        tamanhoDaLinhaMaisLonga = std::max(tamanhoDaLinhaMaisLonga, static_cast<int>(linhaAtual.length()));
    }
    
    std::string margemEsquerda = SimplificacoesAparencia::espacosParaCentralizar(tamanhoDaLinhaMaisLonga);

    for (const std::string& linhaAtual : linhasParaImprimir) {
        std::cout << margemEsquerda << linhaAtual << "\n";
    }
    
    std::cout << "\n";
    SimplificacoesAparencia::imprimirLinhaDivisoria();
}

void TelaInventario::exibirMenuInteracaoItem(Item* itemEncontrado)
{
    std::string mensagemBase = "Digite o codigo do item para interagir ou [0] VOLTAR: ";
    std::string margemEsquerda = SimplificacoesAparencia::espacosParaCentralizar(mensagemBase.length());

    SimplificacoesAparencia::limparTela();
    TelaMenu::exibirLogoDoJogo("OPCOES DE ITEM");
    std::string nomeItem = itemEncontrado->obterNomeItem();
    std::cout << "\n" << margemEsquerda << "Item Selecionado: " << SimplificacoesAparencia::cor(Cor::CIANO) << nomeItem << SimplificacoesAparencia::cor(Cor::RESET) << "\n\n";
    std::cout << margemEsquerda << "[1] Usar / Equipar / Desequipar\n";
    std::cout << margemEsquerda << "[2] Inspecionar Detalhes\n";
    std::cout << margemEsquerda << "[0] Cancelar\n\n";
    std::cout << margemEsquerda << "Escolha: ";
}

void TelaInventario::exibirPrompt(const std::string& mensagem)
{
    std::cout << "\n" << SimplificacoesAparencia::espacosParaCentralizar(mensagem.length()) << mensagem;
}
