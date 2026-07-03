#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <map>

#include "NPCMercador.h"
#include "../../../Perspectiva/TelasBase/Menu/TelaMenu.h"
#include "../../../Sistemas/Inventario/Item.h"
#include "../../../Sistemas/Inventario/FabricaItens.h"
#include "../../../Perspectiva/TelasBase/Inventario/TelaInventario.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Core/Controladores/Loja.h"
#include "../../../Core/Utilidades/FuncoesDialogo.h"
#include "NPCMercadorLayout.h"

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
        Aparencia::exibirPopup("FRANCHESCO", linhas, Cor::AMARELO, NPCMercadorLayouts::arteMercador);
    }
    
    void dialogoFranchescoUnico(const std::string& msg) {
        dialogoFranchesco({msg});
    }
}

// --- INFORMACOES DO LUGAR ---
std::string NPCMercador::obterNomeDoLugar() const {
    return "MERCADOR AMBULANTE";
}

Cor NPCMercador::obterCorDoCabecalho() const {
    return Cor::AMARELO;
}

Cor NPCMercador::obterCorDaArte() const {
    return Cor::AMARELO;
}

const std::vector<std::string>& NPCMercador::obterArteASCII() const {
    return NPCMercadorLayouts::arteMercador;
}

// --- INTERACAO E MENU ---
void NPCMercador::interagir(Personagem* jogador) {
    ControleDeInput::executarLoopMenuPopup(
        [this, jogador]() { this->exibirDialogo(jogador); },
        [this, jogador]() { return this->obterOpcoesMenu(jogador, Aparencia::obterLarguraTerminal()); },
        [this, jogador](const std::string& op) { this->processarOpcao(jogador, op, Aparencia::obterLarguraTerminal()); return true; },
        obterNomeDoLugar(), obterCorDoCabecalho(), obterArteASCII()
    );
}

void NPCMercador::exibirDialogo(Personagem* jogador) {
    dialogoFranchesco(std::vector<std::string>{
        "Bem-vindo! De uma olhada nas",
        "minhas mercadorias."
    });
}

std::vector<std::string> NPCMercador::obterOpcoesMenu(Personagem* jogador, int larguraDoTerminal) {
    return {
        "COMPRAR Pocoes",
        "COMPRAR Talismas",
        "COMPRAR Iguarias",
        "VENDER Itens do Inventario",
        "Missoes de Franchesco",
        "VOLTAR"
    };
}

void NPCMercador::processarOpcao(Personagem* jogador, const std::string& opcao, int larguraDoTerminal) {
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
            [](const std::string& msg) { dialogoFranchescoUnico(msg); }, InteracaoNPC::obterFormatadorStatusItem, NPCMercadorLayouts::arteMercador);
    }

    void processarCompraTalismas(Personagem* jogadorAtual) {
        Loja::processarCompra(jogadorAtual, "LOJA - TALISMAS", Cor::AMARELO, estoqueTalismas, 
            [](const std::string& msg) { dialogoFranchescoUnico(msg); }, InteracaoNPC::obterFormatadorStatusItem, NPCMercadorLayouts::arteMercador);
    }

    void processarCompraIguarias(Personagem* jogadorAtual) {
        Loja::processarCompra(jogadorAtual, "LOJA - IGUARIAS", Cor::AMARELO, estoqueIguarias, 
            [](const std::string& msg) { dialogoFranchescoUnico(msg); }, InteracaoNPC::obterFormatadorStatusItem, NPCMercadorLayouts::arteMercador);
    }

    void processarVendaDeItens(Personagem* jogadorAtual) {
        do {
            std::vector<std::pair<std::string, std::vector<Item*>>> gruposItens;
            std::map<std::string, int> indexMap;
            
            for (auto* item : jogadorAtual->obterInventario()->obterTodosOsItens()) {
                if (item->obterTipo() != TipoEquipamento::MISSAO) {
                    std::string nomeItem = item->obterNomeItem();
                    bool equipado = jogadorAtual->isItemEquipado(item);
                    std::string chave = nomeItem;
                    if (equipado) {
                        chave += " [Equipado]";
                    }
                    
                    if (indexMap.find(chave) == indexMap.end()) {
                        indexMap[chave] = gruposItens.size();
                        gruposItens.push_back({chave, {item}});
                    } else {
                        gruposItens[indexMap[chave]].second.push_back(item);
                    }
                }
            }

            std::vector<std::vector<Item*>> itensValidos;
            std::vector<std::string> opcoesItem;
            
            for (const auto& grupo : gruposItens) {
                int qtd = grupo.second.size();
                Item* itemExemplo = grupo.second.front();
                int precoVenda = itemExemplo->obterPrecoVenda();
                
                std::string textoOpcao = grupo.first + " (" + std::to_string(precoVenda) + "G)";
                if (qtd > 1) {
                    textoOpcao += " x" + std::to_string(qtd);
                }
                
                itensValidos.push_back(grupo.second);
                opcoesItem.push_back(textoOpcao);
            }

            if (opcoesItem.empty()) { dialogoFranchescoUnico("Voce nao tem nada que me interesse!"); break; }
            opcoesItem.push_back("VOLTAR");
            
            int escolha = ControleDeInput::lerSelecaoMenuEmPopup("VENDER ITENS", {"Seu Ouro: " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G", "Escolha um item para vender:"}, opcoesItem, Cor::AMARELO, NPCMercadorLayouts::arteMercador);
            if (escolha == -1 || escolha == static_cast<int>(opcoesItem.size()) - 1) break;
            
            std::vector<Item*> itensEscolhidos = itensValidos[escolha];
            Item* itemParaVenda = itensEscolhidos.front();

            if (!InteracaoNPC::verificarItemNaoEquipado(jogadorAtual, itemParaVenda, "Franchesco", Cor::AMARELO, "Nao e possivel vender itens que estao equipados!")) continue;
            
            int qtdParaVender = 1;
            if (itensEscolhidos.size() > 1) {
                std::vector<std::string> opcoesQtd = {
                    "Vender 1 unidade",
                    "Vender Todos (" + std::to_string(itensEscolhidos.size()) + " unidades)",
                    "Digitar quantidade...",
                    "Cancelar"
                };
                
                int escolhaQtd = ControleDeInput::lerSelecaoMenuEmPopup(
                    "QUANTIDADE: " + itemParaVenda->obterNomeItem(),
                    {"Voce possui " + std::to_string(itensEscolhidos.size()) + " unidades deste item."},
                    opcoesQtd, 
                    Cor::AMARELO, 
                    NPCMercadorLayouts::arteMercador
                );
                
                if (escolhaQtd == 0) {
                    qtdParaVender = 1;
                } else if (escolhaQtd == 1) {
                    qtdParaVender = itensEscolhidos.size();
                } else if (escolhaQtd == 2) {
                    std::string msgQtd = "Quantidade (1 a " + std::to_string(itensEscolhidos.size()) + ", 0 cancelar): ";
                    qtdParaVender = Aparencia::lerInteiroEmPopupFlutuante(msgQtd, 0, itensEscolhidos.size(), Cor::AMARELO);
                } else {
                    continue; // Cancelar
                }
            }
            
            if (qtdParaVender == 0) continue;

            std::string nomeItemVenda = itemParaVenda->obterNomeItem();
            int precoVendaUnitario = itemParaVenda->obterPrecoVenda();
            int ganhoTotal = precoVendaUnitario * qtdParaVender;
            
            jogadorAtual->obterInventario()->adicionarOuro(ganhoTotal);
            for (int i = 0; i < qtdParaVender; ++i) {
                jogadorAtual->obterInventario()->removerItem(itensEscolhidos[i]);
            }
            
            if (qtdParaVender > 1) {
                dialogoFranchescoUnico("Voce vendeu " + std::to_string(qtdParaVender) + "x " + nomeItemVenda + " por " + std::to_string(ganhoTotal) + "G!");
            } else {
                dialogoFranchescoUnico("Voce vendeu " + nomeItemVenda + " por " + std::to_string(ganhoTotal) + "G!");
            }
        } while (true);
    }
}
