#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <map>

#include "NPCFranchesco.h"
#include "../Telas/TelaMenu.h"
#include "../Inventario/Item.h"
#include "../Inventario/FabricaItens.h"
#include "../Telas/TelaInventario.h"
#include "../Utilidades/Aparencia.h"
#include "../Utilidades/ControleDeInput.h"
#include "../Gerenciadores/GerenciadorLoja.h"
#include "NPCFranchescoLayouts.h"

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
    void processarCompraPocoes(SistemaPersonagem* jogadorAtual);
    void processarCompraTalismas(SistemaPersonagem* jogadorAtual, int larguraDoTerminal);
    void processarCompraIguarias(SistemaPersonagem* jogadorAtual, int larguraDoTerminal);
    void processarVendaDeItens(SistemaPersonagem* jogadorAtual, int larguraDoTerminal);

    void dialogoFranchesco(const std::string& texto, bool novaLinhaAntes = true, bool novaLinhaDepois = true) {
        Aparencia::imprimirDialogoNPC("Franchesco", Cor::AMARELO, texto, novaLinhaAntes, novaLinhaDepois);
    }

    void dialogoFranchesco(const std::vector<std::string>& linhas) {
        Aparencia::imprimirDialogoNPC("Franchesco", Cor::AMARELO, linhas);
    }
}

// --- INFORMACOES DO LUGAR ---
std::string NPCFranchesco::obterNomeDoLugar() const {
    return "LOJA AMBULANTE";
}

Cor NPCFranchesco::obterCorDoCabecalho() const {
    return Cor::AMARELO;
}

Cor NPCFranchesco::obterCorDaArte() const {
    return Cor::AMARELO;
}

const std::vector<std::string>& NPCFranchesco::obterArteASCII() const {
    static std::vector<std::string> arte = NPCFranchescoLayouts::obterArteFranchesco();
    return arte;
}

// --- INTERACAO E MENU ---
void NPCFranchesco::exibirDialogo(SistemaPersonagem* jogador) {
    dialogoFranchesco(std::vector<std::string>{
        "Bem-vindo! De uma olhada nas",
        "minhas mercadorias."
    });
}

std::vector<std::string> NPCFranchesco::obterOpcoesMenu(SistemaPersonagem* jogador, int larguraDoTerminal) {
    return {
        "Seu Ouro: " + std::to_string(jogador->obterInventario()->obterOuro()) + "G",
        "",
        "[1] COMPRAR Pocoes",
        "[2] COMPRAR Talismas",
        "[3] COMPRAR Iguarias",
        "[4] VENDER Itens do Inventario",
        "[0] VOLTAR",
        ""
    };
}

void NPCFranchesco::processarOpcao(SistemaPersonagem* jogador, const std::string& opcao, int larguraDoTerminal) {
    if (opcao == "1") {
        processarCompraPocoes(jogador);
    }
    else if (opcao == "2") {
        processarCompraTalismas(jogador, larguraDoTerminal);
    }
    else if (opcao == "3") {
        processarCompraIguarias(jogador, larguraDoTerminal);
    }
    else if (opcao == "4") {
        processarVendaDeItens(jogador, larguraDoTerminal);
    }
}

namespace {
    // --- PROCESSAMENTO DE OPCOES ---
    void processarCompraPocoes(SistemaPersonagem* jogadorAtual) {
        auto formatador = [](ItemID id) {
            if (id == ItemID::PocaoCura30) return std::string(" (30%VM)");
            return std::string("");
        };
        GerenciadorLoja::processarCompra(jogadorAtual, "LOJA - POCOES", Cor::AMARELO, estoquePocoes, 
            [](const std::string& msg) { dialogoFranchesco(msg); }, formatador);
    }

    void processarCompraTalismas(SistemaPersonagem* jogadorAtual, int larguraDoTerminal) {
        auto formatador = [](ItemID id) {
            if (id == ItemID::TalismaUrso) return std::string(" (+5 Forca | -5 Int)");
            if (id == ItemID::TalismaCorvo) return std::string(" (+5 Int | -5 Forca)");
            if (id == ItemID::TalismaLeopardo) return std::string(" (+5 Dest | -5 Sab)");
            if (id == ItemID::TalismaCoruja) return std::string(" (+5 Sab | -5 Dest)");
            return std::string("");
        };
        GerenciadorLoja::processarCompra(jogadorAtual, "LOJA - TALISMAS", Cor::AMARELO, estoqueTalismas, 
            [](const std::string& msg) { dialogoFranchesco(msg); }, formatador);
    }

    void processarCompraIguarias(SistemaPersonagem* jogadorAtual, int larguraDoTerminal) {
        GerenciadorLoja::processarCompra(jogadorAtual, "LOJA - IGUARIAS", Cor::AMARELO, estoqueIguarias, 
            [](const std::string& msg) { dialogoFranchesco(msg); });
    }

    void processarVendaDeItens(SistemaPersonagem* jogadorAtual, int larguraDoTerminal) {
        std::string codigoVenda;
        do {
            TelaInventario::exibir(jogadorAtual, true);
            dialogoFranchesco("Digite o codigo do item para vender ou [0] VOLTAR: ", false, false);
            std::cout << "\033[s";
            
            Item* itemParaVenda = TelaInventario::lerSelecaoDeItem(jogadorAtual, codigoVenda);

            if (codigoVenda != "0") {
                if (jogadorAtual->isItemEquipado(itemParaVenda)) {
                    dialogoFranchesco("Nao e possivel vender itens que estao equipados!"); Aparencia::aguardarEnter(); continue;
                }
                std::string nomeItemVenda = itemParaVenda->obterNomeItem();
                int precoVenda = itemParaVenda->obterPrecoVenda();
                jogadorAtual->obterInventario()->adicionarOuro(precoVenda);
                jogadorAtual->obterInventario()->removerItem(itemParaVenda);
                dialogoFranchesco("Voce vendeu " + nomeItemVenda + " por " + std::to_string(precoVenda) + "G!"); Aparencia::aguardarEnter();
            }
        } while (codigoVenda != "0");
    }
}
