#include "TelaNPCs.h"
#include "TelaNPCsLayouts.h"
#include "../../Sistemas/SistemaPersonagem.h"
#include "../../Utilidades/Aparencia.h"
#include "../../Utilidades/ControleDeInput.h"
#include <iostream>
#include <vector>

void TelaNPCs::exibir(SistemaPersonagem* jogador) {
    bool continuar = true;
    std::vector<std::string> npcs = {"Bjorn (O Ferreiro)", "Franchesco (O Mercador)", "Morgana (A Bruxa)", "Cavaleiros (A Guarda Real)"};
    
    do {
        Aparencia::limparTela();
        Aparencia::exibirLogoAscii(ArtesNPCs::logoNPCs, 49, Cor::CIANO, "HABITANTES DO MUNDO", false);
        std::cout << "\n";
        Aparencia::imprimirCentralizado("Selecione um NPC para ver seus detalhes:");
        std::cout << "\n";

        std::vector<std::string> menuOpcoes = npcs;
        menuOpcoes.push_back("VOLTAR");

        int escolha = ControleDeInput::lerSelecaoMenuComSetas(menuOpcoes, true);

        if (escolha == static_cast<int>(menuOpcoes.size()) - 1) {
            continuar = false;
        } else if (escolha >= 0 && escolha < static_cast<int>(npcs.size())) {
            exibirFichaNPC(jogador, npcs[escolha]);
        }
    } while (continuar);
}

void TelaNPCs::exibirFichaNPC(SistemaPersonagem* jogador, const std::string& nomeNPC) {
    Aparencia::limparTela();
    Aparencia::exibirLogoAscii(ArtesNPCs::logoNPCs, 49, Cor::CIANO, "FICHA DO NPC");
    std::cout << "\n";
    Aparencia::imprimirCentralizado("NPC: " + nomeNPC);
    std::cout << "\n\n";
    
    std::vector<std::string> info;
    if (nomeNPC == "Bjorn (O Ferreiro)") {
        info = {
            "Bjorn e um talentoso ferreiro que se perdeu em uma caverna",
            "dominada por orks. Apos ser resgatado, ele oferece seus servicos",
            "de forja para melhorar seus equipamentos com materiais ou",
            "fundindo copias de armas e armaduras."
        };
    } else if (nomeNPC == "Franchesco (O Mercador)") {
        info = {
            "Um vendedor ambulante com um senso de negocios peculiar.",
            "Ele aparece pela vila oferecendo pocos, utilitarios magicos",
            "e aceita comprar seus itens indesejados por um preco justo."
        };
    } else if (nomeNPC == "Morgana (A Bruxa)") {
        info = {
            "Uma figura misteriosa que habita as profundezas da floresta.",
            "Morgana domina as artes ocultas e guarda segredos sobre o",
            "labirinto subterraneo. Ela so fala com aqueles que provam seu valor."
        };
    } else if (nomeNPC == "Cavaleiros (A Guarda Real)") {
        info = {
            "A elite militar que protege o Reino e o Caminho do Castelo.",
            "Os Cavaleiros seguem ordens estritas e nao deixam ninguem",
            "passar sem uma permissao especial do rei em pessoa."
        };
    }
    
    Aparencia::imprimirBlocoCentralizado(info);
    std::cout << "\n";
    ControleDeInput::aguardarEnter();
}
