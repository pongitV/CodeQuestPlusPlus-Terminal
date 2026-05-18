#include "TelaIndex.h"
#include "TelaBestiario.h"
#include "TelaArsenal.h"
#include "TelaRacas.h"
#include "TelaClasses.h"
#include "TelaNPCs.h"
#include "TelaIndexLayouts.h"
#include "../TelaBase.h"
#include "../../Utilidades/Aparencia.h"
#include "../../Utilidades/ControleDeInput.h"
#include "../../Sistemas/SistemaPersonagem.h"

#include <iostream>
#include <vector>
#include <string>
#include <chrono>

void TelaIndex::exibirMenuPrincipal(SistemaPersonagem* jogador) {
    static auto ultimoAcesso = std::chrono::steady_clock::now() - std::chrono::hours(1);

    bool continuar = true;
    do {
        bool animar = TelaBase::deveAnimarEntradaDaTela(ultimoAcesso, 300);

        Aparencia::limparTela();
        Aparencia::exibirLogoAscii(ArtesIndex::logoIndex, 57, Cor::CIANO, "Index DO REINO", animar);
        
        std::cout << "\n";
        Aparencia::imprimirCentralizado("O que voce deseja consultar na enciclopedia?");
        std::cout << "\n";

        std::vector<std::string> opcoes = {
            "Bestiario (Monstros e Criaturas)",
            "Itens (Armas, Armaduras e Consumiveis)",
            "Classes (Linhagens de Batalha)",
            "Racas (Especies do Mundo)",
            "NPCs (Habitantes do Mundo)",
            "VOLTAR"
        };
        
        int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
        
        if (escolha == 0) {
            TelaBestiario::exibirLista(jogador);
        } else if (escolha == 1) {
            TelaArsenal::exibir(jogador);
        } else if (escolha == 2) {
            TelaClasses::exibir(jogador);
        } else if (escolha == 3) {
            TelaRacas::exibir(jogador);
        } else if (escolha == 4) {
            TelaNPCs::exibir(jogador);
        } else if (escolha == 5) {
            continuar = false;
        }
    } while (continuar);
}
