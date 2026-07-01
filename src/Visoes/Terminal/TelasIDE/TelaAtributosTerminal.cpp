#include "TelaAtributosTerminal.h"
#include <iostream>
#include <vector>
#include <sstream>
#include "../TemaTerminal.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"

void TelaAtributosTerminal::exibir(Personagem* jogadorAtual) {
    Aparencia::limparTela();
    
    std::string titulo = "\033[38;2;86;156;214mclass\033[0m \033[38;2;78;201;176mAtributos\033[0m {";
    Aparencia::imprimirCentralizado(titulo);
    
    std::vector<std::string> conteudo = {
        "\033[38;2;86;156;214mpublic:\033[0m",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mforca\033[0m = \033[38;2;181;206;168m" + std::to_string(jogadorAtual->obterForca()) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mdestreza\033[0m = \033[38;2;181;206;168m" + std::to_string(jogadorAtual->obterDestreza()) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254minteligencia\033[0m = \033[38;2;181;206;168m" + std::to_string(jogadorAtual->obterInteligencia()) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mresistencia\033[0m = \033[38;2;181;206;168m" + std::to_string(jogadorAtual->obterResistencia()) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254msabedoria\033[0m = \033[38;2;181;206;168m" + std::to_string(jogadorAtual->obterSabedoria()) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mnivel\033[0m = \033[38;2;181;206;168m" + std::to_string(jogadorAtual->obterNivel()) + "\033[0m;",
        "",
        "    \033[38;2;220;220;170mvoid\033[0m \033[38;2;220;220;170gerenciar\033[0m() {",
        "        " + TemaTerminal::variavel("atribuirPontos") + "(jogador);" + "",
        "    }"
    };
    
    Aparencia::imprimirBlocoCentralizadoDigitando(conteudo, 15);
    
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para continuar..."));
    ControleDeInput::aguardarEnter();
}

void TelaAtributosTerminal::exibirAtributos(const std::string& nomeAtributo, int valor, int nivel) {
    Aparencia::limparTela();
    
    std::vector<std::string> info = {
        "\033[38;2;86;156;214m// Atributo\033[0m:",
        "    \033[38;2;156;220;254mint\033[0m \033[38;2;156;220;254m" + nomeAtributo + "\033[0m = \033[38;2;181;206;168m" + std::to_string(valor) + "\033[0m;",
        "    \033[38;2;156;220;254mint\033[0m \033[38;2;156;220;254mnivel\033[0m = \033[38;2;181;206;168m" + std::to_string(nivel) + "\033[0m;",
    };
    
    Aparencia::imprimirBlocoCentralizado(info);
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para continuar..."));
    ControleDeInput::aguardarEnter();
}

void TelaAtributosTerminal::exibirEstatisticas(int vidaMax, int manaMax, int ataque, int defesa, int destreza, int sorte) {
    Aparencia::limparTela();
    
    std::vector<std::string> stats = {
        "\033[38;2;86;156;214mstruct\033[0m \033[38;2;78;201;176mEstatisticas\033[0m {",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mvidaMax\033[0m = \033[38;2;181;206;168m" + std::to_string(vidaMax) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254manaMax\033[0m = \033[38;2;181;206;168m" + std::to_string(manaMax) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mataque\033[0m = \033[38;2;181;206;168m" + std::to_string(ataque) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mdefesa\033[0m = \033[38;2;181;206;168m" + std::to_string(defesa) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mdestreza\033[0m = \033[38;2;181;206;168m" + std::to_string(destreza) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254msorte\033[0m = \033[38;2;181;206;168m" + std::to_string(sorte) + "\033[0m;",
        "};",
    };
    
    Aparencia::imprimirBlocoCentralizado(stats);
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para continuar..."));
    ControleDeInput::aguardarEnter();
}

void TelaAtributosTerminal::gerenciarFicha(Personagem* jogadorAtual) {
    Aparencia::limparTela();
    
    std::vector<std::string> ficha = {
        "\033[38;2;86;156;214mclass\033[0m \033[38;2;78;201;176mFichaJogador\033[0m {",
        "\033[38;2;86;156;214mpublic:\033[0m",
        "    \033[38;2;156;220;254mstring\033[0m \033[38;2;156;220;254mnome\033[0m = \033[38;2;214;157;133m\"" + jogadorAtual->obterNome() + "\"\033[0m;",
        "    \033[38;2;156;220;254mint\033[0m \033[38;2;156;220;254mnivel\033[0m = \033[38;2;181;206;168m" + std::to_string(jogadorAtual->obterNivel()) + "\033[0m;",
        "    \033[38;2;156;220;254mint\033[0m \033[38;2;156;220;254mpontosDisponiveis\033[0m = \033[38;2;181;206;168m0\033[0m;", 
    };
    
    Aparencia::imprimirBlocoCentralizado(ficha);
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para continuar..."));
    ControleDeInput::aguardarEnter();
}
