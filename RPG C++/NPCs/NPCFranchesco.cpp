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
#include "NPCFranchescoLayouts.h"

namespace {
    struct Produto {
        ItemID idItem;
        int preco;
        int quantidade; // -1 para infinito
    };

    std::map<int, Produto> estoquePocoes = {
        {1, {ItemID::PocaoCura30, 10, -1}}
    };

    std::map<int, Produto> estoqueTalismas = {
        {1, {ItemID::TalismaUrso, 200, 1}},
        {2, {ItemID::TalismaCorvo, 200, 1}},
        {3, {ItemID::TalismaLeopardo, 200, 1}},
        {4, {ItemID::TalismaCoruja, 200, 1}}
    };

    std::map<int, Produto> estoqueIguarias = {
        {1, {ItemID::DispositivoLinguagem, 1000, 1}}
    };

    // --- APARENCIA E DIALOGOS ---
    void processarCompraGenerica(SistemaPersonagem* jogadorAtual, const std::string& tituloLoja, std::map<int, Produto>& estoqueAtual);
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
    void processarCompraGenerica(SistemaPersonagem* jogadorAtual, const std::string& tituloLoja, std::map<int, Produto>& estoqueAtual) {
        std::string opcaoCompra;
        do {
            Aparencia::limparTela();
            Aparencia::exibirCabecalho(tituloLoja, Cor::AMARELO);
            std::cout << "\n";
            
            std::vector<std::string> linhas = {
                "Seu Ouro: " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G",
                ""
            };

            int maxId = 0;
            for (auto const& [id, produto] : estoqueAtual) {
                if (id > maxId) maxId = id;
                std::string nomeItem = FabricaItens::obterNomeDeID(produto.idItem);
                if (produto.idItem == ItemID::PocaoCura30) nomeItem += " (30%VM)";
                else if (produto.idItem == ItemID::TalismaUrso) nomeItem += " (+5 Forca | -5 Int)";
                else if (produto.idItem == ItemID::TalismaCorvo) nomeItem += " (+5 Int | -5 Forca)";
                else if (produto.idItem == ItemID::TalismaLeopardo) nomeItem += " (+5 Dest | -5 Sab)";
                else if (produto.idItem == ItemID::TalismaCoruja) nomeItem += " (+5 Sab | -5 Dest)";
                
                std::string preco = std::to_string(produto.preco) + "G";
                std::string estoqueInfo = (produto.quantidade == -1) ? "" : (produto.quantidade == 0 ? " (Esgotado)" : " (Estoque: " + std::to_string(produto.quantidade) + ")");
                
                linhas.push_back("[" + std::to_string(id) + "] " + nomeItem + " - " + preco + estoqueInfo);
            }
            linhas.push_back("");
            linhas.push_back("[0] VOLTAR");

            Aparencia::imprimirBlocoCentralizado(linhas);
            std::cout << "\n";
            int id = ControleDeInput::lerInteiroComLimites("Escolha: ", 0, maxId, true);
            opcaoCompra = std::to_string(id);

            if (opcaoCompra != "0" && estoqueAtual.find(id) != estoqueAtual.end()) {
                auto& produto = estoqueAtual[id];
                if (produto.quantidade == 0) {
                    dialogoFranchesco("Sinto muito, nao tenho mais disso em estoque.");
                } else {
                    int maxComprador = jogadorAtual->obterInventario()->obterOuro() / produto.preco;
                    if (maxComprador == 0) {
                        dialogoFranchesco("Pobreta...");
                    } else {
                        int maxPossivel = (produto.quantidade == -1) ? maxComprador : std::min(maxComprador, produto.quantidade);
                        int qtdComprar = 1;
                        if (maxPossivel > 1) {
                            std::string msgQtd = "Quantidade para comprar (1 a " + std::to_string(maxPossivel) + ", ou 0 para cancelar): ";
                            qtdComprar = ControleDeInput::lerInteiroComLimites(msgQtd, 0, maxPossivel, true);
                        }
                        
                        if (qtdComprar > 0) {
                            jogadorAtual->obterInventario()->adicionarOuro(-(produto.preco * qtdComprar));
                            if (produto.quantidade != -1) produto.quantidade -= qtdComprar;
                            
                            std::string nomeNovo = FabricaItens::obterNomeDeID(produto.idItem);
                            for (int i = 0; i < qtdComprar; ++i) {
                                jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem(produto.idItem));
                            }
                            dialogoFranchesco(std::to_string(qtdComprar) + "x " + nomeNovo + " comprado!");
                        }
                    }
                }
                Aparencia::aguardarEnter();
            }
        } while (opcaoCompra != "0");
    }

    void processarCompraPocoes(SistemaPersonagem* jogadorAtual) {
        processarCompraGenerica(jogadorAtual, "LOJA - POCOES", estoquePocoes);
    }

    void processarCompraTalismas(SistemaPersonagem* jogadorAtual, int larguraDoTerminal) {
        processarCompraGenerica(jogadorAtual, "LOJA - TALISMAS", estoqueTalismas);
    }

    void processarCompraIguarias(SistemaPersonagem* jogadorAtual, int larguraDoTerminal) {
        processarCompraGenerica(jogadorAtual, "LOJA - IGUARIAS", estoqueIguarias);
    }

    void processarVendaDeItens(SistemaPersonagem* jogadorAtual, int larguraDoTerminal) {
        std::string codigoVenda;
        do {
            TelaInventario::exibir(jogadorAtual, true);
            dialogoFranchesco("Digite o codigo do item para vender ou [0] VOLTAR: ", false, false);
            std::cout << "\033[s";
            
            Item* itemParaVenda = nullptr;
            while (true) {
                codigoVenda = ControleDeInput::lerEntradaProtegida();
                if (codigoVenda == "0") break;
                itemParaVenda = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigoVenda, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
                if (itemParaVenda) break;
                std::cout << "\033[u\033[J";
            }

            if (codigoVenda != "0") {
                if (itemParaVenda == jogadorAtual->obterArma() || itemParaVenda == jogadorAtual->obterEscudo() || itemParaVenda == jogadorAtual->obterArmadura()) {
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
