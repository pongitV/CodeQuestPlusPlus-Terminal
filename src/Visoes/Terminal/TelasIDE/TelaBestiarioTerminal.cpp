#include "TelaBestiarioTerminal.h"
#include <iostream>
#include <vector>
#include <sstream>
#include "../TemaTerminal.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"

void TelaBestiarioTerminal::exibirLista(Personagem* jogadorAtual, const std::vector<std::string>& nomesInimigos, const std::vector<std::string>& descobertos) {
    Aparencia::limparTela();
    
    std::string titulo = "\033[38;2;86;156;214mclass\033[0m \033[38;2;78;201;176mBestiario\033[0m {";
    Aparencia::imprimirCentralizado(titulo);
    
    std::vector<std::string> conteudo = {
        "\033[38;2;86;156;214mpublic:\033[0m",
        "    \033[38;2;78;201;176mmap\033[0m<\033[38;2;156;220;254mstring\033[0m, \033[38;2;78;201;176mStats\033[0m> \033[38;2;156;220;254minimigos\033[0m;",
        "    \033[38;2;156;220;254mint\033[0m \033[38;2;156;220;254mtotalDescobertos\033[0m = \033[38;2;181;206;168m" + std::to_string(descobertos.size()) + "\033[0m;",
        "",
        "    \033[38;2;220;220;170mvoid\033[0m \033[38;2;220;220;170exibir\033[0m() {",
        "        " + TemaTerminal::variavel("std::cout") + " << " + TemaTerminal::variavel("inimigos.size") + " << " + TemaTerminal::variavel("std::endl") + ";" + "",
        "    }"
    };
    
    Aparencia::imprimirBlocoCentralizadoDigitando(conteudo, 15);
    
    std::vector<std::string> lista;
    for (const auto& nome : nomesInimigos) {
        bool encontrado = false;
        for (const auto& desc : descobertos) {
            if (desc == nome) {
                encontrado = true;
                break;
            }
        }
        if (encontrado) {
            lista.push_back("\033[38;2;181;206;168m" + nome + "\033[0m");
        } else {
            lista.push_back("\033[38;2;96;139;78m" + nome + " [N/A]\033[0m");
        }
    }
    
    Aparencia::imprimirBlocoCentralizado(lista);
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para navegar..."));
    ControleDeInput::aguardarEnter();
}

void TelaBestiarioTerminal::exibirFicha(const std::string& nomeInimigo, const std::vector<std::string>& estatisticas, const std::vector<std::string>& habilidades) {
    Aparencia::limparTela();
    
    std::vector<std::string> ficha = {
        "\033[38;2;86;156;214mclass\033[0m \033[38;2;78;201;176m" + nomeInimigo + " : public Inimigo\033[0m {",
        "\033[38;2;86;156;214mpublic:\033[0m",
    };
    
    for (const auto& stat : estatisticas) {
        ficha.push_back("    \033[38;2;156;220;254mint\033[0m \033[38;2;156;220;254m" + stat + "\033[0m;");
    }
    
    ficha.push_back("\033[38;2;86;156;214mprivate:\033[0m");
    for (const auto& hab : habilidades) {
        ficha.push_back("    \033[38;2;96;139;78m// Habilidade: " + hab + "\033[0m");
    }
    
    ficha.push_back("};");
    
    Aparencia::imprimirBlocoCentralizado(ficha);
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para fechar..."));
    ControleDeInput::aguardarEnter();
}
