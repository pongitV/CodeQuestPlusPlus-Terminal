#include "Aparencia.h"
#include "../../../Perspectiva/TelasBase/TelaBase.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

namespace {
    std::vector<std::string> historicoBatalha;
}

void Aparencia::registrarLogBatalha(const std::string& texto) {
    historicoBatalha.push_back(texto);
}

void Aparencia::limparLogBatalha() {
    historicoBatalha.clear();
}

void Aparencia::exibirUltimosLogs(int quantidade) {
    if (historicoBatalha.empty()) return;
    int inicio = std::max(0, static_cast<int>(historicoBatalha.size()) - quantidade);
    int total = static_cast<int>(historicoBatalha.size()) - inicio;
    for (int i = 0; i < total; ++i) {
        int index = inicio + i;
        imprimirCentralizado(removerCoresANSI(historicoBatalha[index]), cor(Cor::CINZA));
    }
    std::cout << "\n";
}

void Aparencia::exibirHistoricoCompleto() {
    if (historicoBatalha.empty()) {
        limparTela();
        exibirPainelTexto("HISTORICO DE BATALHA", Cor::CIANO);
        imprimirCentralizado("O historico esta vazio.", cor(Cor::CINZA));
        std::cout << "\n";
        TelaBase::imprimirLinhaDivisoria();
        ControleDeInput::aguardarEnter();
        return;
    }

    int linhasPorPagina = std::max(5, obterAlturaTerminal() - 12);
    int totalPaginas = (static_cast<int>(historicoBatalha.size()) + linhasPorPagina - 1) / linhasPorPagina;
    int paginaAtual = totalPaginas - 1;

    while (true) {
        limparTela();
        exibirPainelTexto("HISTORICO DE BATALHA", Cor::CIANO);

        int inicio = paginaAtual * linhasPorPagina;
        int fim = std::min(inicio + linhasPorPagina, static_cast<int>(historicoBatalha.size()));
        std::vector<std::string> pagina(historicoBatalha.begin() + inicio, historicoBatalha.begin() + fim);
        
        imprimirBlocoCentralizado(pagina);
        std::cout << "\n";
        TelaBase::imprimirLinhaDivisoria();
        imprimirCentralizado("Pagina " + std::to_string(paginaAtual + 1) + " de " + std::to_string(totalPaginas), cor(Cor::CIANO));
        
        if (totalPaginas > 1) {
            std::string escolha = ControleDeInput::lerEntradaProtegida("[A] Pagina Anterior | [D] Proxima Pagina | [0] Sair\n\nEscolha: ");
            if (escolha == "0") break;
            if ((escolha == "a" || escolha == "A") && paginaAtual > 0) paginaAtual--;
            if ((escolha == "d" || escolha == "D") && paginaAtual < totalPaginas - 1) paginaAtual++;
        } else {
            std::cout << "\n";
            ControleDeInput::aguardarEnter();
            break;
        }
    }
    std::cout << "\n";
}
