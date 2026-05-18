#include "TelaArsenal.h"
#include "TelaArsenalLayouts.h"
#include "../../Sistemas/SistemaIndex.h"
#include "../../Sistemas/SistemaPersonagem.h"
#include "../../Utilidades/Aparencia.h"
#include "../../Utilidades/ControleDeInput.h"
#include "../../Inventario/FabricaItens.h"
#include "../../Inventario/Item.h"
#include <iostream>
#include <vector>
#include <algorithm>

void TelaArsenal::exibir(SistemaPersonagem* jogador) {
    bool continuar = true;
    do {
        Aparencia::limparTela();
        Aparencia::exibirLogoAscii(ArtesArsenal::logoArsenal, 57, Cor::VERMELHO, "ITENS DESCOBERTOS", false);
        
        std::cout << "\n";
        Aparencia::imprimirCentralizado("Tudo que voce ja encontrou pelo mundo:");
        std::cout << "\n\n";
        
        auto itensVistos = SistemaIndex::instancia().obterItensVistos();
        
        std::vector<std::string> armas, armaduras, consumiveis, materiais, missoes;
        
        for (const auto& nome : itensVistos) {
            auto item = FabricaItens::criarItem(nome);
            if (item) {
                switch (item->obterTipo()) {
                    case TipoEquipamento::ARMA:
                    case TipoEquipamento::ESCUDO: armas.push_back(nome); break;
                    case TipoEquipamento::ARMADURA: armaduras.push_back(nome); break;
                    case TipoEquipamento::CONSUMIVEL: consumiveis.push_back(nome); break;
                    case TipoEquipamento::MATERIAL: materiais.push_back(nome); break;
                    case TipoEquipamento::MISSAO: missoes.push_back(nome); break;
                }
            }
        }
        
        std::sort(armas.begin(), armas.end());
        std::sort(armaduras.begin(), armaduras.end());
        std::sort(consumiveis.begin(), consumiveis.end());
        std::sort(materiais.begin(), materiais.end());
        std::sort(missoes.begin(), missoes.end());
        
        std::vector<std::string> linhas;
        auto adicionarCategoria = [&](const std::string& titulo, const std::vector<std::string>& lista) {
            linhas.push_back("═══ " + titulo + " ═══");
            if (lista.empty()) {
                linhas.push_back(Aparencia::cor(Cor::CINZA) + "  (Nenhum item descoberto nesta categoria)" + Aparencia::cor(Cor::RESET));
            } else {
                for (const auto& i : lista) linhas.push_back("  - " + i);
            }
            linhas.push_back("");
        };
        
        adicionarCategoria("ARMAS E ESCUDOS", armas);
        adicionarCategoria("ARMADURAS", armaduras);
        adicionarCategoria("CONSUMIVEIS", consumiveis);
        adicionarCategoria("MATERIAIS DE FORJA", materiais);
        adicionarCategoria("ITENS DE MISSAO", missoes);
        
        Aparencia::imprimirBlocoCentralizado(linhas);
        std::cout << "\n\n";

        Aparencia::exibirPrompt("[0] VOLTAR\n\nEscolha: ");
        std::string escolha = ControleDeInput::lerEntradaProtegida();
        
        if (escolha == "0") continuar = false;
    } while (continuar);
}