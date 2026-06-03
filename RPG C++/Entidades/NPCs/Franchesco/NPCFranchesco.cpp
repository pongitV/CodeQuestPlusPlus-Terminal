#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <map>

#include "NPCFranchesco.h"
#include "../../../Interface/Telas/Menu/TelaMenu.h"
#include "../../../Sistemas/Inventario/Item.h"
#include "../../../Sistemas/Inventario/FabricaItens.h"
#include "../../../Interface/Telas/Inventario/TelaInventario.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Core/Controladores/Loja.h"
#include "../../../Core/Utilidades/FuncoesDialogo.h"
#include "NPCFranchescoLayout.h"

namespace {
    std::map<int, ProdutoLoja> estoquePocoes = {
        {1, {ItemID::PocaoCura30, 10, -1}}
    };

    std::map<int, ProdutoLoja> estoqueTalismas = {
        {1, {ItemID::TalismaUrso, 200, 1}},
        {2, {ItemID::TalismaCorvo, 200, 1}},
        {3, {ItemID::TalismaLeopardo, 200, 1}},
        {4, {ItemID::TalismaCoruja, 200, 1}}
    };

    std::map<int, ProdutoLoja> estoqueIguarias = {
        {1, {ItemID::DispositivoLinguagem, 1000, 1}}
    };

    // --- APARENCIA E DIALOGOS ---
    void processarCompraPocoes(Personagem* jogadorAtual);
    void processarCompraTalismas(Personagem* jogadorAtual);
    void processarCompraIguarias(Personagem* jogadorAtual);
    void processarVendaDeItens(Personagem* jogadorAtual);

    void dialogoFranchesco(const std::vector<std::string>& linhas) {
        Aparencia::exibirPopup("FRANCHESCO", linhas, Cor::AMARELO, NPCFranchescoLayouts::arteFranchesco);
    }
    
    void dialogoFranchescoUnico(const std::string& msg) {
        dialogoFranchesco({msg});
    }
}

// --- INFORMACOES DO LUGAR ---
std::string NPCFranchesco::obterNomeDoLugar() const {
    return "MERCADOR AMBULANTE";
}

Cor NPCFranchesco::obterCorDoCabecalho() const {
    return Cor::AMARELO;
}

Cor NPCFranchesco::obterCorDaArte() const {
    return Cor::AMARELO;
}

const std::vector<std::string>& NPCFranchesco::obterArteASCII() const {
    return NPCFranchescoLayouts::arteFranchesco;
}

// --- INTERACAO E MENU ---
void NPCFranchesco::interagir(Personagem* jogador) {
    ControleDeInput::executarLoopMenuPopup(
        [this, jogador]() { this->exibirDialogo(jogador); },
        [this, jogador]() { return this->obterOpcoesMenu(jogador, Aparencia::obterLarguraTerminal()); },
        [this, jogador](const std::string& op) { this->processarOpcao(jogador, op, Aparencia::obterLarguraTerminal()); return true; },
        obterNomeDoLugar(), obterCorDoCabecalho(), obterArteASCII()
    );
}

void NPCFranchesco::exibirDialogo(Personagem* jogador) {
    dialogoFranchesco(std::vector<std::string>{
        "Bem-vindo! De uma olhada nas",
        "minhas mercadorias."
    });
}

std::vector<std::string> NPCFranchesco::obterOpcoesMenu(Personagem* jogador, int larguraDoTerminal) {
    return {
        "COMPRAR Pocoes",
        "COMPRAR Talismas",
        "COMPRAR Iguarias",
        "VENDER Itens do Inventario",
        "Missoes de Franchesco",
        "VOLTAR"
    };
}

void NPCFranchesco::processarOpcao(Personagem* jogador, const std::string& opcao, int larguraDoTerminal) {
    if (opcao == "COMPRAR Pocoes") {
        processarCompraPocoes(jogador);
    }
    else if (opcao == "COMPRAR Talismas") {
        processarCompraTalismas(jogador);
    }
    else if (opcao == "COMPRAR Iguarias") {
        processarCompraIguarias(jogador);
    }
    else if (opcao == "VENDER Itens do Inventario") {
        processarVendaDeItens(jogador);
    }
    else if (opcao == "Missoes de Franchesco") {
        InteracaoNPC::processarMenuMissoesVazio(jogador, "MISSOES DE FRANCHESCO", Cor::AMARELO, "Franchesco", "Ah, meu amigo! Nao tenho nenhum pedido especial para voce agora.");
    }
}

namespace {
    // --- PROCESSAMENTO DE OPCOES ---
    void processarCompraPocoes(Personagem* jogadorAtual) {
        Loja::processarCompra(jogadorAtual, "LOJA - POCOES", Cor::AMARELO, estoquePocoes, 
            [](const std::string& msg) { dialogoFranchescoUnico(msg); }, InteracaoNPC::obterFormatadorStatusItem, NPCFranchescoLayouts::arteFranchesco);
    }

    void processarCompraTalismas(Personagem* jogadorAtual) {
        Loja::processarCompra(jogadorAtual, "LOJA - TALISMAS", Cor::AMARELO, estoqueTalismas, 
            [](const std::string& msg) { dialogoFranchescoUnico(msg); }, InteracaoNPC::obterFormatadorStatusItem, NPCFranchescoLayouts::arteFranchesco);
    }

    void processarCompraIguarias(Personagem* jogadorAtual) {
        Loja::processarCompra(jogadorAtual, "LOJA - IGUARIAS", Cor::AMARELO, estoqueIguarias, 
            [](const std::string& msg) { dialogoFranchescoUnico(msg); }, InteracaoNPC::obterFormatadorStatusItem, NPCFranchescoLayouts::arteFranchesco);
    }

    void processarVendaDeItens(Personagem* jogadorAtual) {
        do {
            std::vector<Item*> itensValidos;
            std::vector<std::string> opcoesItem;
            for (auto* item : jogadorAtual->obterInventario()->obterTodosOsItens()) {
                if (item->obterTipo() != TipoEquipamento::MISSAO) {
                    itensValidos.push_back(item);
                    opcoesItem.push_back(item->obterNomeItem() + " (" + std::to_string(item->obterPrecoVenda()) + "G)");
                }
            }
            if (opcoesItem.empty()) { dialogoFranchescoUnico("Voce nao tem nada que me interesse!"); break; }
            opcoesItem.push_back("VOLTAR");
            
            int escolha = ControleDeInput::lerSelecaoMenuEmPopup("VENDER ITENS", {"Seu Ouro: " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G", "Escolha um item para vender:"}, opcoesItem, Cor::AMARELO, NPCFranchescoLayouts::arteFranchesco);
            if (escolha == -1 || escolha == static_cast<int>(opcoesItem.size()) - 1) break;
            
            Item* itemParaVenda = itensValidos[escolha];
            if (!InteracaoNPC::verificarItemNaoEquipado(jogadorAtual, itemParaVenda, "Franchesco", Cor::AMARELO, "Nao e possivel vender itens que estao equipados!")) continue;
            std::string nomeItemVenda = itemParaVenda->obterNomeItem();
            int precoVenda = itemParaVenda->obterPrecoVenda();
            jogadorAtual->obterInventario()->adicionarOuro(precoVenda);
            jogadorAtual->obterInventario()->removerItem(itemParaVenda);
            dialogoFranchescoUnico("Voce vendeu " + nomeItemVenda + " por " + std::to_string(precoVenda) + "G!");
        } while (true);
    }
}
