#include <iostream>
#include <vector>
#include <map>
#include <chrono>

#include "TelaInventario.h"
#include "../TelaBase.h"
#include "../Menu/TelaMenu.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "TelaInventarioLayout.h"

void TelaInventario::exibirCabecalhoInventario(bool animar) {
    Aparencia::exibirPainelArte(ArtesInventario::logoInventario, 121, Cor::AMARELO, "", animar);
}

void TelaInventario::exibirCaixaEquipados(Personagem* jogadorAtual) 
{
    Item* armaEquipada = jogadorAtual->obterArma();
    Item* escudoEquipado = jogadorAtual->obterEscudo();
    Item* armaduraEquipada = jogadorAtual->obterArmadura();
    Item* consumivelRapido = jogadorAtual->obterConsumivelRapido();

    std::vector<std::string> linhas;
    linhas.push_back("Arma     : " + (armaEquipada ? armaEquipada->obterNomeItem() + armaEquipada->obterInfoStatus() : "Nenhuma"));
    linhas.push_back("Escudo   : " + (escudoEquipado ? escudoEquipado->obterNomeItem() + escudoEquipado->obterInfoStatus() : "Nenhum"));
    linhas.push_back("Armadura : " + (armaduraEquipada ? armaduraEquipada->obterNomeItem() + armaduraEquipada->obterInfoStatus() : "Nenhuma"));
    linhas.push_back("Cura Rap.: " + (consumivelRapido ? consumivelRapido->obterNomeItem() + " (" + std::to_string(jogadorAtual->obterInventario()->contarItem(consumivelRapido->obterNomeItem())) + "x)" : "Vazio"));

    Aparencia::imprimirBlocoCentralizado(Aparencia::criarCaixa(linhas, "EQUIPAMENTOS ATUAIS", 40, Cor::AMARELO));
}

std::vector<std::pair<std::string, Item*>> TelaInventario::obterListaCategoria(Personagem* jogadorAtual, int categoria, bool mostrarPrecos)
{
    std::vector<std::pair<std::string, Item*>> lista;
    if (!jogadorAtual) return lista;

    std::map<std::string, std::vector<Item*>> itensAgrupados;

    for (Item* item : jogadorAtual->obterInventario()->obterTodosOsItens()) {
        TipoEquipamento tipo = item->obterTipo();
        if (categoria == 0 && (tipo == TipoEquipamento::ARMA || tipo == TipoEquipamento::ESCUDO || tipo == TipoEquipamento::ARMADURA)) {
            itensAgrupados[item->obterNomeItem() + item->obterInfoStatus()].push_back(item);
        } else if (categoria == 1 && tipo == TipoEquipamento::CONSUMIVEL) {
            itensAgrupados[item->obterNomeItem()].push_back(item);
        } else if (categoria == 2 && tipo == TipoEquipamento::MATERIAL) {
            itensAgrupados[item->obterNomeItem()].push_back(item);
        } else if (categoria == 3 && tipo == TipoEquipamento::MISSAO) {
            itensAgrupados[item->obterNomeItem()].push_back(item);
        }
    }
    
    std::string sufixo = (categoria == 1 || categoria == 2) ? "G / un" : "G";
    if (categoria == 3) sufixo = "";

    for (auto const& [nome, itensGrupo] : itensAgrupados) {
        Item* item = itensGrupo.front();
        std::string prefixo = (itensGrupo.size() > 1) ? std::to_string(itensGrupo.size()) + "x " : "";
        std::string infoVenda = mostrarPrecos ? Aparencia::cor(Cor::AMARELO) + " (Venda: " + std::to_string(item->obterPrecoVenda()) + sufixo + ")" + Aparencia::cor(Cor::RESET) : "";
        
        bool algumEquipado = false;
        for (Item* it : itensGrupo) {
            if (jogadorAtual->isItemEquipado(it)) {
                algumEquipado = true;
                break;
            }
        }

        std::string nomeFinal = nome;
        if (algumEquipado) {
            nomeFinal = Aparencia::cor(Cor::VERDE) + "[E] " + Aparencia::cor(Cor::RESET) + nome;
        }

        lista.push_back({prefixo + nomeFinal + infoVenda, item});
    }
    return lista;
}

void TelaInventario::exibirInspecaoItem(Item* item, Personagem* jogadorAtual)
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
