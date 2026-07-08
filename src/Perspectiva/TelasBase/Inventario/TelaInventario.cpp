#include <map>

#include "TelaInventario.h"
#include "../../GerenciadorPerspectiva.h"
#include "../../PerspectivaIDE/TelasIDE/Inventario/TelaInventarioIDE.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Sistemas/Inventario/Item.h"

void TelaInventario::exibirCabecalhoInventario(bool animar, int startY) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        GerenciadorPerspectiva::obterInventarioUI().exibirCabecalho(animar, startY);
    } else {
        TelaInventarioIDE::exibirCabecalhoInventario(animar);
    }
}

void TelaInventario::exibirCaixaEquipados(Personagem* jogadorAtual) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        GerenciadorPerspectiva::obterInventarioUI().exibirCaixaEquipados(jogadorAtual);
    } else {
        TelaInventarioIDE::exibirCaixaEquipados(jogadorAtual);
    }
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
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        GerenciadorPerspectiva::obterInventarioUI().exibirDetalheItem(item);
    } else {
        TelaInventarioIDE::exibirInspecaoItem(item, jogadorAtual);
    }
}
