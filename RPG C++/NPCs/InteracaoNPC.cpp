#include "InteracaoNPC.h"
#include <iostream>
#include "../Utilidades/ControleDeInput.h"

// --- INTERACAO PRINCIPAL ---
void InteracaoNPC::interagir(SistemaPersonagem* jogadorAtual) {
    std::string opcao;
    bool primeiraVez = true; // Controla o efeito de transicao
    
    do {
        Aparencia::limparTela();
        Aparencia::exibirCabecalho(obterNomeDoLugar(), obterCorDoCabecalho());

        int larguraDoTerminal = Aparencia::obterLarguraTerminal();
        exibirDialogo(jogadorAtual);

        std::vector<std::string> menuEsquerda = obterOpcoesMenu(jogadorAtual, larguraDoTerminal);
        const std::vector<std::string>& arte = obterArteASCII();
        
        int atrasoAnimacao = primeiraVez ? 10 : 0; // 10ms por linha na primeira vez (Cortina "Fade In")
        int recuo = Aparencia::imprimirLadoALado(menuEsquerda, arte, 45, 0, Cor::RESET, obterCorDaArte(), atrasoAnimacao);
        std::cout << "\n" << std::string(recuo, ' ') << "Escolha: ";
        opcao = ControleDeInput::lerEntradaProtegida();

        primeiraVez = false; // Desativa a animacao apos o primeiro load

        if (opcao != "0") {
            processarOpcao(jogadorAtual, opcao, larguraDoTerminal);
        }

    } while (opcao != "0");
}
