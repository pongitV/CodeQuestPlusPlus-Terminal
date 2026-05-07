#include "InteracaoNPC.h"
#include <iostream>

void InteracaoNPC::interagir(SistemaPersonagem* jogadorAtual) {
    std::string opcao;
    do {
        Aparencia::limparTela();
        Aparencia::exibirCabecalho(obterNomeDoLugar(), obterCorDoCabecalho());

        int larguraDoTerminal = Aparencia::obterLarguraTerminal();
        exibirDialogo(jogadorAtual);

        std::vector<std::string> menuEsquerda = obterOpcoesMenu(jogadorAtual, larguraDoTerminal);
        const std::vector<std::string>& arte = obterArteASCII();
        
        int recuo = Aparencia::imprimirLadoALado(menuEsquerda, arte, 45, 0, Cor::RESET, obterCorDaArte());
        std::cout << "\n" << std::string(recuo, ' ') << "Escolha: ";
        std::cin >> opcao;

        if (opcao != "0") {
            processarOpcao(jogadorAtual, opcao, larguraDoTerminal);
        }

    } while (opcao != "0");
}
