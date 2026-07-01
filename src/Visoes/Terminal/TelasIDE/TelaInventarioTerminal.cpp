#include "TelaInventarioTerminal.h"
#include <iostream>
#include <vector>
#include <sstream>
#include "../TemaTerminal.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"

void TelaInventarioTerminal::exibir(Personagem* jogadorAtual) {
    Aparencia::limparTela();
    
    std::string titulo = "\033[38;2;86;156;214mclass\033[0m \033[38;2;78;201;176mInventario\033[0m {";
    Aparencia::imprimirCentralizado(titulo);
    
    std::vector<std::string> conteudo = {
        "\033[38;2;86;156;214mpublic:\033[0m",
        "    \033[38;2;78;201;176mvector\033[0m<\033[38;2;78;201;176mItem\033[0m*> \033[38;2;156;220;254mitens\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mouro\033[0m = \033[38;2;181;206;168m" + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "\033[0m;", 
        "    \033[38;2;156;220;254mstring\033[0m \033[38;2;156;220;254mnome\033[0m = \033[38;2;214;157;133m\"" + jogadorAtual->obterNome() + "\"\033[0m;",
        "",
        "    \033[38;2;220;220;170mvoid\033[0m \033[38;2;220;220;170exibir\033[0m() {",
        "        " + TemaTerminal::variavel("std::cout") + " << " + TemaTerminal::variavel("itens.size") + " << " + TemaTerminal::variavel("std::endl") + ";" + "",
        "    }"
    };
    
    Aparencia::imprimirBlocoCentralizadoDigitando(conteudo, 15);
    
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para navegar..."));
    ControleDeInput::aguardarEnter();
}

void TelaInventarioTerminal::exibirCategoria(Personagem* jogadorAtual, int categoria, bool mostrarPrecos) {
    Aparencia::limparTela();
    
    std::vector<std::string> linha;
    linha.push_back("\033[38;2;86;156;214m// Categoria\033[0m: " + std::to_string(categoria));
    
    if (mostrarPrecos) {
        linha.push_back("\033[38;2;96;139;78m// Preços visíveis\033[0m");
    }
    
    Aparencia::imprimirBlocoCentralizado(linha);
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para continuar..."));
    ControleDeInput::aguardarEnter();
}

void TelaInventarioTerminal::exibirInspecaoItem(const std::string& nomeItem, const std::string& descricao, int valor, const std::string& tipo) {
    Aparencia::limparTela();
    
    std::vector<std::string> itemInfo = {
        "\033[38;2;86;156;214mstruct\033[0m \033[38;2;78;201;176mItemInfo\033[0m {",
        "    \033[38;2;156;220;254mstring\033[0m \033[38;2;156;220;254mnome\033[0m = \033[38;2;214;157;133m\"" + nomeItem + "\"\033[0m;",
        "    \033[38;2;156;220;254mstring\033[0m \033[38;2;156;220;254mdescricao\033[0m = \033[38;2;214;157;133m\"" + descricao + "\"\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mvalor\033[0m = \033[38;2;181;206;168m" + std::to_string(valor) + "\033[0m;",
        "    \033[38;2;156;220;254mstring\033[0m \033[38;2;156;220;254mtipo\033[0m = \033[38;2;214;157;133m\"" + tipo + "\"\033[0m;",
        "};",
        "",
        "// Nome: " + nomeItem,
        "// Valor: " + std::to_string(valor),
        "// Tipo: " + tipo
    };
    
    Aparencia::imprimirBlocoCentralizado(itemInfo);
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para fechar..."));
    ControleDeInput::aguardarEnter();
}
