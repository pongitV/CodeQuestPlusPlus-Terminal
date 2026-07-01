#include "TelaDiarioTerminal.h"
#include <iostream>
#include <vector>
#include <sstream>
#include "../TemaTerminal.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"

void TelaDiarioTerminal::exibir(Personagem* jogadorAtual) {
    Aparencia::limparTela();
    
    std::string titulo = "\033[38;2;86;156;214mclass\033[0m \033[38;2;78;201;176mDiario\033[0m {";
    Aparencia::imprimirCentralizado(titulo);
    
    std::vector<std::string> conteudo = {
        "\033[38;2;86;156;214mpublic:\033[0m",
        "    \033[38;2;156;220;254mstring\033[0m \033[38;2;156;220;254mnome\033[0m = \033[38;2;214;157;133m\"" + jogadorAtual->obterNome() + "\"\033[0m;",
        "    \033[38;2;156;220;254mint\033[0m \033[38;2;156;220;254mnivel\033[0m = \033[38;2;181;206;168m" + std::to_string(jogadorAtual->obterNivel()) + "\033[0m;",
        "    \033[38;2;156;220;254mvector\033[0m<\033[38;2;156;220;254mstring\033[0m> \033[38;2;156;220;254mmissoes\033[0m;",
        "    \033[38;2;156;220;254mvector\033[0m<\033[38;2;156;220;254mstring\033[0m> \033[38;2;156;220;254minimigosDerrotados\033[0m;",
        "",
        "    \033[38;2;220;220;170mvoid\033[0m \033[38;2;220;220;170exibir\033[0m() {",
        "        " + TemaTerminal::variavel("std::cout") + " << " + TemaTerminal::variavel("nome") + " << " + TemaTerminal::variavel("std::endl") + ";" + "",
        "    }"
    };
    
    Aparencia::imprimirBlocoCentralizadoDigitando(conteudo, 15);
    
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para navegar..."));
    ControleDeInput::aguardarEnter();
}

void TelaDiarioTerminal::exibirEntradaDiario(const std::string& titulo, const std::vector<std::string>& conteudo) {
    Aparencia::limparTela();
    
    std::vector<std::string> entrada = {
        "\033[38;2;86;156;214m// Entrada\033[0m: " + titulo,
        "",
    };
    
    for (const auto& linha : conteudo) {
        entrada.push_back("\033[38;2;96;139;78m// " + linha + "\033[0m");
    }
    
    Aparencia::imprimirBlocoCentralizado(entrada);
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para continuar..."));
    ControleDeInput::aguardarEnter();
}

void TelaDiarioTerminal::exibirFichaRaca(const std::string& nomeRaca, const std::vector<std::string>& descricao) {
    Aparencia::limparTela();
    
    std::vector<std::string> ficha = {
        "\033[38;2;86;156;214mclass\033[0m \033[38;2;78;201;176m" + nomeRaca + " : public RacaBase\033[0m {",
        "\033[38;2;86;156;214mpublic:\033[0m",
    };
    
    for (const auto& linha : descricao) {
        ficha.push_back("    \033[38;2;96;139;78m// " + linha + "\033[0m");
    }
    
    ficha.push_back("};");
    
    Aparencia::imprimirBlocoCentralizado(ficha);
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para fechar..."));
    ControleDeInput::aguardarEnter();
}

void TelaDiarioTerminal::exibirFichaClasse(const std::string& nomeClasse, const std::vector<std::string>& descricao) {
    Aparencia::limparTela();
    
    std::vector<std::string> ficha = {
        "\033[38;2;86;156;214mclass\033[0m \033[38;2;78;201;176m" + nomeClasse + " : public ClasseBase\033[0m {",
        "\033[38;2;86;156;214mpublic:\033[0m",
    };
    
    for (const auto& linha : descricao) {
        ficha.push_back("    \033[38;2;96;139;78m// " + linha + "\033[0m");
    }
    
    ficha.push_back("};");
    
    Aparencia::imprimirBlocoCentralizado(ficha);
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para fechar..."));
    ControleDeInput::aguardarEnter();
}

void TelaDiarioTerminal::exibirFichaNPC(const std::string& nomeNPC, const std::vector<std::string>& descricao) {
    Aparencia::limparTela();
    
    std::vector<std::string> ficha = {
        "\033[38;2;86;156;214mclass\033[0m \033[38;2;78;201;176m" + nomeNPC + " : public NPC\033[0m {",
        "\033[38;2;86;156;214mpublic:\033[0m",
    };
    
    for (const auto& linha : descricao) {
        ficha.push_back("    \033[38;2;96;139;78m// " + linha + "\033[0m");
    }
    
    ficha.push_back("};");
    
    Aparencia::imprimirBlocoCentralizado(ficha);
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para fechar..."));
    ControleDeInput::aguardarEnter();
}

void TelaDiarioTerminal::exibirFichaItem(const std::string& nomeItem, const std::vector<std::string>& descricao) {
    Aparencia::limparTela();
    
    std::vector<std::string> ficha = {
        "\033[38;2;86;156;214mstruct\033[0m \033[38;2;78;201;176mItem\033[0m {",
        "    \033[38;2;156;220;254mstring\033[0m \033[38;2;156;220;254mnome\033[0m = \033[38;2;214;157;133m\"" + nomeItem + "\"\033[0m;",
    };
    
    for (const auto& linha : descricao) {
        ficha.push_back("    \033[38;2;96;139;78m// " + linha + "\033[0m");
    }
    
    ficha.push_back("};");
    
    Aparencia::imprimirBlocoCentralizado(ficha);
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para fechar..."));
    ControleDeInput::aguardarEnter();
}

void TelaDiarioTerminal::exibirFichaMissao(const std::string& nomeMissao, const std::vector<std::string>& descricao) {
    Aparencia::limparTela();
    
    std::vector<std::string> ficha = {
        "\033[38;2;86;156;214mstruct\033[0m \033[38;2;78;201;176mMissao\033[0m {",
        "    \033[38;2;156;220;254mstring\033[0m \033[38;2;156;220;254mnome\033[0m = \033[38;2;214;157;133m\"" + nomeMissao + "\"\033[0m;",
    };
    
    for (const auto& linha : descricao) {
        ficha.push_back("    \033[38;2;96;139;78m// " + linha + "\033[0m");
    }
    
    ficha.push_back("};");
    
    Aparencia::imprimirBlocoCentralizado(ficha);
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para fechar..."));
    ControleDeInput::aguardarEnter();
}
