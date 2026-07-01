#include "TelaMapaTerminal.h"
#include <iostream>
#include <vector>
#include <sstream>
#include "../TemaTerminal.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"

void TelaMapaTerminal::exibirTopDown(const std::vector<std::string>& matriz, int jogadorX, int jogadorY, const std::string& tituloMapa) {
    Aparencia::limparTela();
    
    std::string titulo = "\033[38;2;86;156;214mclass\033[0m \033[38;2;78;201;176mMapa\033[0m {";
    Aparencia::imprimirCentralizado(titulo);
    
    std::vector<std::string> header = {
        "\033[38;2;86;156;214m// Mapa\033[0m: " + tituloMapa,
        "\033[38;2;86;156;214m// Coordenadas\033[0m: (x=" + std::to_string(jogadorX) + ", y=" + std::to_string(jogadorY) + ")",
        "\033[38;2;86;156;214m// Tamanho\033[0m: " + std::to_string(matriz.size()) + " linhas",
        "",
        "\033[38;2;86;156;214mpublic:\033[0m",
        "    \033[38;2;78;201;176mvector\033[0m<\033[38;2;156;220;254mstring\033[0m> \033[38;2;156;220;254mgrid\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mjogadorX\033[0m = \033[38;2;181;206;168m" + std::to_string(jogadorX) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mjogadorY\033[0m = \033[38;2;181;206;168m" + std::to_string(jogadorY) + "\033[0m;",
        "",
        "    \033[38;2;220;220;170mvoid\033[0m \033[38;2;220;220;170renderizar\033[0m() {",
        "        " + TemaTerminal::variavel("for") + " (auto& linha : " + TemaTerminal::variavel("grid") + ") {",
        "            " + TemaTerminal::variavel("std::cout") + " << " + TemaTerminal::variavel("linha") + " << " + TemaTerminal::variavel("std::endl") + ";" + "",
        "        }" + std::string(""),
        "    }"
    };
    
    Aparencia::imprimirBlocoCentralizadoDigitando(header, 15);
    
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para navegar..."));
    ControleDeInput::aguardarEnter();
}

void TelaMapaTerminal::exibirCoordenadas(int x, int y) {
    Aparencia::limparTela();
    
    std::vector<std::string> coords = {
        "\033[38;2;86;156;214m// Coordenadas\033[0m:",
        "    \033[38;2;156;220;254mint\033[0m \033[38;2;156;220;254mx\033[0m = \033[38;2;181;206;168m" + std::to_string(x) + "\033[0m;",
        "    \033[38;2;156;220;254mint\033[0m \033[38;2;156;220;254my\033[0m = \033[38;2;181;206;168m" + std::to_string(y) + "\033[0m;",
    };
    
    Aparencia::imprimirBlocoCentralizado(coords);
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para continuar..."));
    ControleDeInput::aguardarEnter();
}

void TelaMapaTerminal::exibirInfoMapa(const std::string& titulo, const std::vector<std::string>& conteudo) {
    Aparencia::limparTela();
    
    std::vector<std::string> info = {
        "\033[38;2;86;156;214m// Info\033[0m: " + titulo,
        "",
    };
    
    for (const auto& linha : conteudo) {
        info.push_back("\033[38;2;96;139;78m// " + linha + "\033[0m");
    }
    
    Aparencia::imprimirBlocoCentralizado(info);
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para continuar..."));
    ControleDeInput::aguardarEnter();
}
