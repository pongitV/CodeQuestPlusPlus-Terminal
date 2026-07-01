#include "TelaVitoriaTerminal.h"
#include <iostream>
#include <vector>
#include <sstream>
#include "../TemaTerminal.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"

void TelaVitoriaTerminal::exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate, const std::vector<std::string>& itensObtidos) {
    Aparencia::limparTela();
    
    std::string titulo = "\033[38;2;86;156;214mclass\033[0m \033[38;2;78;201;176mTelaVitoria\033[0m {";
    Aparencia::imprimirCentralizado(titulo);
    
    std::vector<std::string> codigo = {
        "\033[38;2;86;156;214mpublic:\033[0m",
        "    \033[38;2;156;220;254mstring\033[0m \033[38;2;156;220;254mnome\033[0m = \033[38;2;214;157;133m\"" + jogadorAtual->obterNome() + "\"\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mouro\033[0m = \033[38;2;181;206;168m" + std::to_string(quantidadeDeOuroObtido) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mxp\033[0m = \033[38;2;181;206;168m" + std::to_string(quantidadeDeXpObtido) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mdanoCausado\033[0m = \033[38;2;181;206;168m" + std::to_string(totalDeDanoCausado) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mdanoRecebido\033[0m = \033[38;2;181;206;168m" + std::to_string(totalDeDanoRecebido) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mcura\033[0m = \033[38;2;181;206;168m" + std::to_string(curaTotalRecebida) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mturnos\033[0m = \033[38;2;181;206;168m" + std::to_string(turnosCombate) + "\033[0m;",
        "",
        "    \033[38;2;220;220;170mvoid\033[0m \033[38;2;220;220;170exibir\033[0m() {",
        "        " + TemaTerminal::variavel("std::cout") + " << " + TemaTerminal::variavel("VITORIA!") + " << " + TemaTerminal::variavel("std::endl") + ";" + "",
        "    }"
    };
    
    Aparencia::imprimirBlocoCentralizadoDigitando(codigo, 15);
    
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para continuar..."));
    ControleDeInput::aguardarEnter();
}

void TelaVitoriaTerminal::exibirStatsVitoria(const std::string& titulo, const std::vector<std::string>& conteudo) {
    Aparencia::limparTela();
    
    std::vector<std::string> stats = {
        "\033[38;2;86;156;214mstruct\033[0m \033[38;2;78;201;176mStatsVitoria\033[0m {",
        "    \033[38;2;156;220;254mstring\033[0m \033[38;2;156;220;254mtitulo\033[0m = \033[38;2;214;157;133m\"" + titulo + "\"\033[0m;",
    };
    
    for (const auto& linha : conteudo) {
        stats.push_back("    \033[38;2;96;139;78m// " + linha + "\033[0m");
    }
    
    stats.push_back("};");
    
    Aparencia::imprimirBlocoCentralizado(stats);
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para continuar..."));
    ControleDeInput::aguardarEnter();
}
