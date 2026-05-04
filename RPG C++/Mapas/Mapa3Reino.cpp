#include "Mapa3Reino.h"

#include <iostream>
#include <vector>
#include <windows.h>

#include "../Gerenciadores/GerenciadorMenu.h"
#include "../Utilidades/SimplificacoesAparencia.h"
#include "ControleDeMapa.h"
#include "../Utilidades/ControleDeInput.h"
#include "TransicaoDeMapa.h"

Mapa3Reino::Mapa3Reino(SistemaPersonagem* personagemJogador) :
    posicaoXDoJogador(48), 
    posicaoYDoJogador(33),
    jogadorAtual(personagemJogador), 
    exploracaoEstaAtiva(true), 
    tituloDoMapaAtual("CAMINHO DO CASTELO")
{
    matrizDoMapaAtual = {
        "                                                                                                   ",
        "         #################################################################################         ",
        "        #|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||   [CASTELO]   |||||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||      ^C       |||||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||               |||||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||...............|||||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||...............|||||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||...............|||||||||||||||||||||||||||||||||||#        ",
        "        #############       ############...............############       #################        ",
        "                    #       #          #...............#          #       #                        ",
        "                    #       #          #...............#          #       #                        ",
        "                    #########          #...............#          #########                        ",
        "                                       #...............#                                           ",
        "                                       #...............#                                           ",
        "                                       #...............#                                           ",
        "                                       #...............#                                           ",
        "                                       #...............#                                           ",
        "                                       #...............#                                           ",
        "                    #########          #...............#          #########                        ",
        "                    #       #          #...............#          #       #                        ",
        "                    #       #          #...............#          #       #                        ",
        "                    #       #          #...............#          #       #                        ",
        "        #############       ############...............############       #################        ",
        "        #|||||||||||||||||||||||||||||||...............|||||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||...............|||||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||...............|||||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||...............|||||||||||||||||||||||||||||||||||#        ",
        "        ################################...............####################################        ",
        "        #==============================#...............#==================================#        ",
        "        #==============================#...............#==================================#        ",
        "        ################################...............####################################        ",
        "                                       #...............#                                           ",
        "                                       #...............#                                           ",
        "                                       #..[^Floresta]..#                                           ",
        "                                       #...............#                                           ",
        "                                                                                 "
    };
}

Mapa3Reino::~Mapa3Reino() = default;

void Mapa3Reino::iniciarLoopDeExploracaoDoMapa()
{
    HANDLE manipuladorDoTerminal = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO informacoesDoCursor;
    informacoesDoCursor.dwSize = 100;
    informacoesDoCursor.bVisible = FALSE;
    SetConsoleCursorInfo(manipuladorDoTerminal, &informacoesDoCursor);

    SimplificacoesAparencia::limparTela();
    GerenciadorMenu::exibirLogoDoJogo(tituloDoMapaAtual);

    CONSOLE_SCREEN_BUFFER_INFO informacoesDoBufferDaTela;
    GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
    int linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;

    auto restaurarTela = [&]() {
        SimplificacoesAparencia::limparTela();
        GerenciadorMenu::exibirLogoDoJogo(tituloDoMapaAtual);
        GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
        linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
    };

    auto renderizarMapa = [&](int larguraDoTerminal, int linhaInicial)
    {
        int larguraDoMapaEmColunas = matrizDoMapaAtual.empty() ? 0 : matrizDoMapaAtual[0].length();
        int espacosParaCentralizarOMapa = (larguraDoTerminal - larguraDoMapaEmColunas) / 2;
        std::string margemEsquerdaDoMapa(espacosParaCentralizarOMapa > 0 ? espacosParaCentralizarOMapa : 0, ' ');

        std::string textoDeControlesDoJogador = "W,A,S,D: Mover | I: Inventario | C: Ficha | B: Bestiario";
        int espacosParaCentralizarOsControles = (larguraDoTerminal - (int)textoDeControlesDoJogador.length()) / 2;
        std::string margemEsquerdaDosControles(espacosParaCentralizarOsControles > 0 ? espacosParaCentralizarOsControles : 0, ' ');

        COORD posicaoDoCursorNoTerminal;
        posicaoDoCursorNoTerminal.X = 0;
        posicaoDoCursorNoTerminal.Y = linhaInicial;
        SetConsoleCursorPosition(manipuladorDoTerminal, posicaoDoCursorNoTerminal);

        for (int y = 0; y < matrizDoMapaAtual.size(); y++)
        {
            std::string linhaSendoRenderizada = margemEsquerdaDoMapa;
            linhaSendoRenderizada.reserve(margemEsquerdaDoMapa.size() + matrizDoMapaAtual[y].size() + 20);
            for (int x = 0; x < matrizDoMapaAtual[y].size(); x++)
            {
                if (x == posicaoXDoJogador && y == posicaoYDoJogador) {
                    linhaSendoRenderizada += SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::VERDE) + "@" + SimplificacoesAparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'T') {
                    linhaSendoRenderizada += SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::CINZA) + "T" + SimplificacoesAparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'G') {
                    linhaSendoRenderizada += SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::AMARELO) + "G" + SimplificacoesAparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == '=' || matrizDoMapaAtual[y][x] == '|') {
                    linhaSendoRenderizada += SimplificacoesAparencia::cor(Cor::CINZA) + matrizDoMapaAtual[y][x] + SimplificacoesAparencia::cor(Cor::RESET);
                }
            else if (matrizDoMapaAtual[y][x] == '^' || matrizDoMapaAtual[y][x] == 'C' || matrizDoMapaAtual[y][x] == 'F') {
                    linhaSendoRenderizada += SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::CIANO) + matrizDoMapaAtual[y][x] + SimplificacoesAparencia::cor(Cor::RESET);
                }
                else {
                    linhaSendoRenderizada += matrizDoMapaAtual[y][x];
                }
            }
            std::cout << linhaSendoRenderizada << "\n";
        }
        std::cout << "\n" << margemEsquerdaDosControles << textoDeControlesDoJogador << "\n";
    };

    while (exploracaoEstaAtiva && jogadorAtual->obterVida() > 0)
    {
        GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
        int larguraDoTerminal = informacoesDoBufferDaTela.srWindow.Right - informacoesDoBufferDaTela.srWindow.Left + 1;

        renderizarMapa(larguraDoTerminal, linhaInicialParaDesenharOMapa);

        char teclaPressionadaPeloJogador = ControleDeInput::lerTecla();

        int proximaPosicaoX = posicaoXDoJogador;
        int proximaPosicaoY = posicaoYDoJogador;

        bool abriuMenu = ControleDeMapa::processarInputEComandos(teclaPressionadaPeloJogador, jogadorAtual, proximaPosicaoX, proximaPosicaoY, restaurarTela);
        
        if (jogadorAtual->obterVoltarProMenu()) break;
        if (abriuMenu) continue;

        char celulaDestino = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX];
        
        if (celulaDestino == '^') {
            char nextCell = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1];
            if (nextCell == 'C') {
                SimplificacoesAparencia::limparTela();
                GerenciadorMenu::exibirLogoDoJogo("FIM DA DEMO");
                int espacosM = (larguraDoTerminal - 60) / 2;
                std::cout << "\n" << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[SISTEMA]: Voce chegou aos portoes do Castelo Real!\n";
                std::cout << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[SISTEMA]: A historia continua em breve...\n";
                SimplificacoesAparencia::aguardarEnter();
                exploracaoEstaAtiva = false;
            }
        else if (nextCell == 'F') {
            TransicaoDeMapa::exibirTransicaoParaFloresta();
            exploracaoEstaAtiva = false; // Interrompe o loop do Reino, devolvendo controle pra Floresta
        }
        } else if (celulaDestino == 'G') {
            SimplificacoesAparencia::limparTela();
            GerenciadorMenu::exibirLogoDoJogo("GUARDA REAL");
            int espacosM = (larguraDoTerminal - 60) / 2;
            std::cout << "\n" << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[Guarda]: Alto la! Somente o Rei pode conceder passagem.\n";
            std::cout << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[Guarda]: (O castelo ainda esta em construcao pelos deuses/devs)\n";
            SimplificacoesAparencia::aguardarEnter();
            restaurarTela();
    } else if (celulaDestino != '#' && celulaDestino != '=' && celulaDestino != '|' && celulaDestino != 'T' && celulaDestino != '[' && celulaDestino != ']' && celulaDestino != 'C' && celulaDestino != 'F' && celulaDestino != 'A' && celulaDestino != 'S' && celulaDestino != 'E' && celulaDestino != 'L' && celulaDestino != 'O' && celulaDestino != ' ') {
            posicaoXDoJogador = proximaPosicaoX;
            posicaoYDoJogador = proximaPosicaoY;
        }
    }
}
