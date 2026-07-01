#include "TelaCombateTerminal.h"
#include <iostream>
#include <vector>
#include <sstream>
#include "../TemaTerminal.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"

void TelaCombateTerminal::exibir(Personagem* jogadorAtual, const std::vector<Personagem*>& inimigos, const std::string& tituloCombate) {
    Aparencia::limparTela();
    
    std::string tituloIDE = "\033[38;2;86;156;214mclass\033[0m \033[38;2;78;201;176mCombate" + std::to_string(inimigos.size()) + "\033[0m {";
    Aparencia::imprimirCentralizado(tituloIDE);
    
    std::vector<std::string> codigo = {
        "\033[38;2;86;156;214mpublic:\033[0m",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mturnoAtual\033[0m = \033[38;2;181;206;168m0\033[0m;",
        "    \033[38;2;156;220;254mstring\033[0m \033[38;2;156;220;254mtitulo\033[0m = \033[38;2;214;157;133m\"" + tituloCombate + "\"\033[0m;",
        "    \033[38;2;78;201;176mbool\033[0m \033[38;2;156;220;254mataqueRealizado\033[0m = \033[38;2;86;156;214mfalse\033[0m;",
        "",
        "    \033[38;2;220;220;170mvoid\033[0m \033[38;2;220;220;170executar\033[0m() {",
        "        " + TemaTerminal::variavel(jogadorAtual->obterNome()) + "->" + TemaTerminal::funcao("atacar") + "(inimigo);" + "",
        "    }"
    };
    
    Aparencia::imprimirBlocoCentralizadoDigitando(codigo, 15);
    
    std::vector<std::string> logInimigos;
    for (const auto& inimigo : inimigos) {
        logInimigos.push_back("\033[38;2;96;139;78m// Inimigo detectado: " + inimigo->obterNome() + " HP:" + std::to_string(inimigo->obterVida()) + "\033[0m");
    }
    
    std::cout << "\n";
    Aparencia::imprimirBlocoCentralizado(logInimigos);
    
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para iniciar o combate..."));
    ControleDeInput::aguardarEnter();
}

void TelaCombateTerminal::exibirLogCombate(const std::vector<std::string>& mensagens) {
    Aparencia::limparTela();
    
    std::vector<std::string> log;
    log.push_back("\033[38;2;86;156;214mnamespace\033[0m \033[38;2;78;201;176mCombateLog\033[0m {");
    
    for (const auto& msg : mensagens) {
        log.push_back("    " + TemaTerminal::variavel("msg") + " << " + "\033[38;2;214;157;133m\"" + msg + "\"\033[0m;");
    }
    
    log.push_back("}");
    
    Aparencia::imprimirBlocoCentralizado(log);
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Combate encerrado."));
}

void TelaCombateTerminal::exibirStatsCombate(int turnos, int danoCausado, int danoRecebido, int cura) {
    Aparencia::limparTela();
    
    std::vector<std::string> stats = {
        "\033[38;2;86;156;214mstruct\033[0m \033[38;2;78;201;176mStatsCombate\033[0m {",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mturnos\033[0m = \033[38;2;181;206;168m" + std::to_string(turnos) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mdanoCausado\033[0m = \033[38;2;181;206;168m" + std::to_string(danoCausado) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mdanoRecebido\033[0m = \033[38;2;181;206;168m" + std::to_string(danoRecebido) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mcuraRecebida\033[0m = \033[38;2;181;206;168m" + std::to_string(cura) + "\033[0m;",
        "};",
        "",
        "// Total de " + std::to_string(turnos) + " turnos",
        "// Dano total causado: " + std::to_string(danoCausado),
        "// Dano recebido: " + std::to_string(danoRecebido),
        "// Cura total: " + std::to_string(cura)
    };
    
    Aparencia::imprimirBlocoCentralizado(stats);
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para continuar..."));
    ControleDeInput::aguardarEnter();
}
