#include "Mapa3Reino.h"

#include <iostream>
#include <vector>
#include <windows.h>
#include <iomanip>
#include <algorithm>

#include "../Gerenciadores/GerenciadorMenu.h"
#include "../Telas/TelaMenu.h"
#include "../Utilidades/SimplificacoesAparencia.h"
#include "ControleDeMapa.h"
#include "../Utilidades/ControleDeInput.h"
#include "../Gerenciadores/GerenciadorInimigos.h"
#include "TransicaoDeMapa.h"
#include "../NPCs/NPCCavaleiroGenerico.h"


Mapa3Reino::Mapa3Reino(SistemaPersonagem* personagemJogador) :
    posicaoXDoJogador(48), 
    posicaoYDoJogador(33),
    jogadorAtual(personagemJogador), 
    exploracaoEstaAtiva(true), 
    tituloDoMapaAtual("CAMINHO DO CASTELO")
{
    matrizDoMapaAtual = {
        "                                                                                                   ",
        "        ###################################################################################        ",
        "        #|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||...[^Castelo]..|||||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||...............|||||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||...............|||||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||...............|||||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||...............|||||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||...............|||||||||||||||||||||||||||||||||||#        ",
        "        #############       ############...C.......C...############       #################        ",
        "                    #       #          #...............#          #       #                        ",
        "                    #       #          #.......T.......#          #       #                        ",
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
        "        #|||||||||||||||||||||||||||||||..C....T....C..|||||||||||||||||||||||||||||||||||#        ",
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
    bool trollDerrotado = false;
    bool conviteRecebido = false;

    HANDLE manipuladorDoTerminal = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO informacoesDoCursor;
    informacoesDoCursor.dwSize = 100;
    informacoesDoCursor.bVisible = FALSE;
    SetConsoleCursorInfo(manipuladorDoTerminal, &informacoesDoCursor);

    SimplificacoesAparencia::limparTela();
    TelaMenu::exibirLogoDoJogo(tituloDoMapaAtual);

    CONSOLE_SCREEN_BUFFER_INFO informacoesDoBufferDaTela;
    GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
    int linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;

    auto restaurarTela = [&]() {
        SimplificacoesAparencia::limparTela();
        TelaMenu::exibirLogoDoJogo(tituloDoMapaAtual);
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
                    linhaSendoRenderizada += SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::VERMELHO) + "T" + SimplificacoesAparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'G') {
                    linhaSendoRenderizada += SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::AMARELO) + "G" + SimplificacoesAparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == '=' || matrizDoMapaAtual[y][x] == '|') {
                    linhaSendoRenderizada += SimplificacoesAparencia::cor(Cor::CINZA) + matrizDoMapaAtual[y][x] + SimplificacoesAparencia::cor(Cor::RESET);
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

        // Limites do mapa para impedir crash quando noclip estiver ativo
        if (proximaPosicaoY < 0) proximaPosicaoY = 0; else if (proximaPosicaoY >= static_cast<int>(matrizDoMapaAtual.size())) proximaPosicaoY = static_cast<int>(matrizDoMapaAtual.size()) - 1;
        if (proximaPosicaoX < 0) proximaPosicaoX = 0; else if (proximaPosicaoX >= static_cast<int>(matrizDoMapaAtual[0].size())) proximaPosicaoX = static_cast<int>(matrizDoMapaAtual[0].size()) - 1;

        char celulaDestino = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX];
        
        if (celulaDestino == '^') {
            char nextCell = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1];
            if (nextCell == 'C') {
                if (!conviteRecebido) {
                    SimplificacoesAparencia::limparTela();
                    TelaMenu::exibirLogoDoJogo("ACESSO NEGADO");
                    int espacosM = (larguraDoTerminal - 60) / 2;
                    std::cout << "\n" << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[SISTEMA]: Os portoes estao trancados. Voce precisa de uma permissao real.\n";
                    SimplificacoesAparencia::aguardarEnter();
                    restaurarTela();
                } else {
                    SimplificacoesAparencia::limparTela();
                    TelaMenu::exibirLogoDoJogo("FIM DA DEMO");
                    int espacosM = (larguraDoTerminal - 60) / 2;
                    std::cout << "\n" << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[SISTEMA]: Voce apresentou o Convite Real e os portoes se abriram!\n";
                    std::cout << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[SISTEMA]: A historia continua em breve...\n";
                    SimplificacoesAparencia::aguardarEnter();
                    exploracaoEstaAtiva = false;
                }
            }
        else if (nextCell == 'F') {
            TransicaoDeMapa::exibirTransicaoParaFloresta();
            exploracaoEstaAtiva = false; // Interrompe o loop do Reino, devolvendo controle pra Floresta
        }
        } else if (celulaDestino == 'G') {
            SimplificacoesAparencia::limparTela();
            TelaMenu::exibirLogoDoJogo("GUARDA REAL");
            int espacosM = (larguraDoTerminal - 60) / 2;
            std::cout << "\n" << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[Guarda]: Alto la! Somente o Rei pode conceder passagem.\n";
            std::cout << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[Guarda]: (O castelo ainda esta em construcao pelos deuses/devs)\n";
            SimplificacoesAparencia::aguardarEnter();
            restaurarTela();
    } else if (celulaDestino == 'T' || celulaDestino == 'C') {
            NPCCavaleiroGenerico::interagir(jogadorAtual, trollDerrotado, conviteRecebido, larguraDoTerminal, matrizDoMapaAtual, exploracaoEstaAtiva, restaurarTela, celulaDestino, proximaPosicaoX, proximaPosicaoY);
    } else if (celulaDestino != '#' && celulaDestino != '=' && celulaDestino != '|' && celulaDestino != '[' && celulaDestino != ']' && celulaDestino != 'A' && celulaDestino != 'S' && celulaDestino != 'E' && celulaDestino != 'L' && celulaDestino != 'O' && celulaDestino != ' ' || jogadorAtual->isNoclip()) {
            posicaoXDoJogador = proximaPosicaoX;
            posicaoYDoJogador = proximaPosicaoY;
        }
    }
}
