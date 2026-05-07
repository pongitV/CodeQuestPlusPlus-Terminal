#include <iostream>
#include <vector>
#include <map>
#include <chrono>

#include "TelaInventario.h"
#include "TelaMenu.h"
#include "../Utilidades/Aparencia.h"

void TelaInventario::exibir(SistemaPersonagem* jogadorAtual, bool mostrarPrecos) 
{

  if (jogadorAtual == nullptr) return;
    Aparencia::limparTela();
    
    static auto ultimoAcesso = std::chrono::steady_clock::now() - std::chrono::hours(1);
    auto agora = std::chrono::steady_clock::now();
    bool animarEntrada = std::chrono::duration_cast<std::chrono::milliseconds>(agora - ultimoAcesso).count() > 300;
    ultimoAcesso = agora;
    int atrasoMs = animarEntrada ? 10 : 0;

    int largura = Aparencia::obterLarguraTerminal();
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

    Aparencia::exibirLogoAscii(logoInventario, 121, Cor::AMARELO, "", atrasoMs);

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

    Aparencia::imprimirBlocoCentralizado(linhasParaImprimir, "", atrasoMs);
    
    std::cout << "\n";
    Aparencia::imprimirLinhaDivisoria();
}

void TelaInventario::exibirMenuInteracaoItem(Item* itemEncontrado)
{
    Aparencia::limparTela();
    TelaMenu::exibirLogoDoJogo("OPCOES DE ITEM");
    
    std::vector<std::string> linhas = {
        "Item Selecionado: " + Aparencia::cor(Cor::CIANO) + itemEncontrado->obterNomeItem() + Aparencia::cor(Cor::RESET),
        "",
        "[1] Usar / Equipar / Desequipar",
        "[2] Inspecionar Detalhes",
        "[0] Cancelar"
    };

    std::cout << "\n";
    Aparencia::imprimirBlocoCentralizado(linhas);
    std::cout << "\n";
    Aparencia::exibirPrompt("Escolha: \033[s");
}
