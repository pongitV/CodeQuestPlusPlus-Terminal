#include "GerenciadorLoja.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "../Inventario/FabricaItens.h"
#include "../Utilidades/ControleDeInput.h"

void GerenciadorLoja::processarCompra(SistemaPersonagem* jogadorAtual, const std::string& tituloLoja, Cor corLoja, 
                                      std::map<int, ProdutoLoja>& estoqueAtual, 
                                      const std::function<void(const std::string&)>& exibirDialogoNPC, 
                                      const std::function<std::string(ItemID)>& formatadorNomeExtra) {
    std::string opcaoCompra;
    do {
        Aparencia::limparTela();
        Aparencia::exibirCabecalho(tituloLoja, corLoja);
        
        std::vector<std::string> linhas = {
            "Seu Ouro: " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G", ""
        };

        int maxId = 0;
        for (auto const& [id, produto] : estoqueAtual) {
            if (id > maxId) maxId = id;
            std::string nomeItem = FabricaItens::obterNomeDeID(produto.idItem);
            if (formatadorNomeExtra) nomeItem += formatadorNomeExtra(produto.idItem);
            
            std::string preco = std::to_string(produto.preco) + "G";
            std::string estoqueInfo = (produto.quantidade == -1) ? "" : (produto.quantidade == 0 ? " (Esgotado)" : " (Estoque: " + std::to_string(produto.quantidade) + ")");
            
            linhas.push_back("[" + std::to_string(id) + "] " + nomeItem + " - " + preco + estoqueInfo);
        }
        linhas.push_back("");
        linhas.push_back("[0] VOLTAR");

        std::cout << "\n";
        Aparencia::imprimirBlocoCentralizado(linhas);
        std::cout << "\n";
        int id = ControleDeInput::lerInteiroComLimites("Escolha: ", 0, maxId, true);
        opcaoCompra = std::to_string(id);

        if (opcaoCompra != "0" && estoqueAtual.find(id) != estoqueAtual.end()) {
            auto& produto = estoqueAtual[id];
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
                        exibirDialogoNPC(std::to_string(qtdComprar) + "x " + nomeNovo + " comprado(s)!");
                    }
                }
            }
            Aparencia::aguardarEnter();
        }
    } while (opcaoCompra != "0");
}
