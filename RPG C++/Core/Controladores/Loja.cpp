#include "Loja.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "../../Sistemas/Inventario/FabricaItens.h"
#include "../Utilidades/ControleDeInput.h"
#include "../../Sistemas/Progresso/Diario.h"
#include "../../Interface/Telas/TelaBase.h"

void Loja::processarCompra(Personagem* jogadorAtual, const std::string& tituloLoja, Cor corLoja, 
                                      std::map<int, ProdutoLoja>& estoqueAtual, 
                                      const std::function<void(const std::string&)>& exibirDialogoNPC, 
                                      const std::function<std::string(ItemID)>& formatadorNomeExtra,
                                      const std::vector<std::string>& arteAscii) {
    std::vector<std::map<int, ProdutoLoja>::iterator> itensOrdenados;
    for (auto it = estoqueAtual.begin(); it != estoqueAtual.end(); ++it) {
        itensOrdenados.push_back(it);
    }

    Aparencia::ordenarAlfabeticamente(itensOrdenados, [&formatadorNomeExtra](auto it) {
        std::string nomeItem = FabricaItens::obterNomeDeID(it->second.idItem);
        if (formatadorNomeExtra) nomeItem += formatadorNomeExtra(it->second.idItem);
        return nomeItem;
    });

    while (true) {
        std::vector<std::string> texto = { 
            "Seu Ouro: " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G" 
        };
        
            std::vector<std::string> opcoes;
            for (auto it : itensOrdenados) {
                auto& produto = it->second;
                std::string nomeItem = FabricaItens::obterNomeDeID(produto.idItem);
                if (formatadorNomeExtra) nomeItem += formatadorNomeExtra(produto.idItem);
                
                std::string preco = std::to_string(produto.preco) + "G";
                std::string estoqueInfo = (produto.quantidade == -1) ? "" : (produto.quantidade == 0 ? " (Esgotado)" : " (Estoque: " + std::to_string(produto.quantidade) + ")");
                
                opcoes.push_back(nomeItem + " - " + preco + estoqueInfo);
            }
            opcoes.push_back("VOLTAR");
            
            int escolha = ControleDeInput::lerSelecaoMenuEmPopup(tituloLoja, texto, opcoes, corLoja, arteAscii);
            
            if (escolha == -1 || escolha == static_cast<int>(itensOrdenados.size())) {
                break;
            }
            
            auto it = itensOrdenados[escolha];
            auto& produto = it->second;

            if (produto.quantidade == 0) {
                exibirDialogoNPC("Este item esta esgotado!");
            } else {
                int maxComprador = jogadorAtual->obterInventario()->obterOuro() / produto.preco;
                if (maxComprador == 0) {
                    exibirDialogoNPC("Voce nao tem ouro suficiente para isso!");
                } else {
                    int maxPossivel = (produto.quantidade == -1) ? maxComprador : std::min(maxComprador, produto.quantidade);
                    int qtdComprar = (maxPossivel > 1) ? ControleDeInput::lerInteiroComLimites("Quantidade (1 a " + std::to_string(maxPossivel) + ", 0 cancelar): ", 0, maxPossivel, true) : 1;
                    if (qtdComprar > 0) {
                        jogadorAtual->obterInventario()->adicionarOuro(-(produto.preco * qtdComprar));
                        if (produto.quantidade != -1) produto.quantidade -= qtdComprar;
                        std::string nomeNovo = FabricaItens::obterNomeDeID(produto.idItem);
                        for (int i = 0; i < qtdComprar; ++i) jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem(produto.idItem));
                        Diario::instancia().registrarItem(Aparencia::removerCoresANSI(nomeNovo));
                        exibirDialogoNPC(std::to_string(qtdComprar) + "x " + nomeNovo + " comprado(s)!");
                    }
                }
            }
    }
}
