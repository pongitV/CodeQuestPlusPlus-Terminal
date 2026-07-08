#pragma once

#include <string>
#include <algorithm>
#include <ostream>
#include "../../../Core/Utilidades/Aparencia.h"

// TelaBaseMenu — utilitarios compartilhados de layout para as telas de menu.
// Centraliza calculos de posicionamento horizontal para evitar repeticao
// em cada tela concreta (Raycaster, IDE, futuras perspectivas).
class TelaBaseMenu {
public:
    // Retorna o offset X para centralizar horizontalmente um bloco de
    // 'comprimentoTexto' caracteres dentro de um terminal de 'larguraConsole' colunas.
    // Nunca retorna valor negativo.
    static int calcularOffsetCentral(int comprimentoTexto, int larguraConsole) {
        return std::max(0, (larguraConsole - comprimentoTexto) / 2);
    }

    // Sobrecarga conveniente: aceita a string diretamente e automaticamente
    // ignora codigos de cor ANSI no calculo da largura visual.
    static int calcularOffsetCentral(const std::string& texto, int larguraConsole) {
        return calcularOffsetCentral(Aparencia::obterComprimentoVisual(texto), larguraConsole);
    }

    // Desenha uma caixa preta com bordas brancas usando posicionamento ANSI
    static void desenharCaixaPreta(std::ostream& out, int y, int x, int largura, int altura) {
        std::string corBorda = "\033[38;2;255;255;255m"; // Branco
        std::string corFundo = "\033[48;2;0;0;0m"; // Preto
        std::string reset = "\033[0m";

        std::string bordaTop = "\u250C";
        for (int i = 0; i < largura - 2; ++i) bordaTop += "\u2500";
        bordaTop += "\u2510";

        std::string bordaBot = "\u2514";
        for (int i = 0; i < largura - 2; ++i) bordaBot += "\u2500";
        bordaBot += "\u2518";

        std::string meio = "";
        for (int i = 0; i < largura - 2; ++i) meio += " ";

        // Topo
        out << "\033[" << (y + 1) << ";" << (x + 1) << "H" << corBorda << corFundo << bordaTop << reset;
        // Linhas intermediarias
        for (int i = 1; i < altura - 1; ++i) {
            out << "\033[" << (y + i + 1) << ";" << (x + 1) << "H" 
                << corBorda << corFundo << "\u2502" 
                << meio 
                << "\u2502" << reset;
        }
        // Base
        out << "\033[" << (y + altura) << ";" << (x + 1) << "H" << corBorda << corFundo << bordaBot << reset;
    }
};
