#include "Mapa3Reino.h"

#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>

#include "../Gerenciadores/GerenciadorMenu.h"
#include "../Telas/TelaMenu.h"
#include "../Utilidades/Aparencia.h"
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
        "        ################################################################################        ",
        "        #||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||...[^Castelo]..||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||...............||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||...............||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||...............||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||...............||||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||...............||||||||||||||||||||||||||||||||#        ",
        "        #############       ############...C.......C...############       ##############        ",
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
        "        #############       ############...............############       #############      ",
        "        #|||||||||||||||||||||||||||||||...............|||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||..C....T....C..|||||||||||||||||||||||||||||||#        ",
        "        #|||||||||||||||||||||||||||||||...............|||||||||||||||||||||||||||||||#        ",
        "        ################################...............################################        ",
        "        #==============================#...............#==============================#        ",
        "        #==============================#...............#==============================#        ",
        "        ################################...............################################        ",
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

    ControleDeMapa::padronizarTamanhoDoMapa(matrizDoMapaAtual);

    Aparencia::ocultarCursor();

    Aparencia::limparTela();
    Aparencia::exibirCabecalho(tituloDoMapaAtual, Cor::CIANO);

    int linhaInicialParaDesenharOMapa = Aparencia::obterPosicaoCursorY();

    auto restaurarTela = [&]() {
        Aparencia::limparTela();
        Aparencia::exibirCabecalho(tituloDoMapaAtual, Cor::CIANO);
        linhaInicialParaDesenharOMapa = Aparencia::obterPosicaoCursorY();
    };

    auto renderizarMapa = [&](int larguraDoTerminal, int alturaDoTerminal, int linhaInicial)
    {
        int startX, endX;
        ControleDeMapa::calcularCameraHorizontal(larguraDoTerminal, posicaoXDoJogador, matrizDoMapaAtual.empty() ? 0 : static_cast<int>(matrizDoMapaAtual[0].length()), startX, endX);

        std::string margemEsquerdaDoMapa = ControleDeMapa::calcularMargemCentralizada(larguraDoTerminal, endX - startX);

        std::string textoDeControlesDoJogador = "W,A,S,D: Mover | I: Inventario | C: Ficha | B: Bestiario";
        std::string margemEsquerdaDosControles = ControleDeMapa::calcularMargemCentralizada(larguraDoTerminal, textoDeControlesDoJogador.length());

        Aparencia::moverCursor(0, linhaInicial);

        int startY, endY;
        ControleDeMapa::calcularCameraVertical(alturaDoTerminal, posicaoYDoJogador, static_cast<int>(matrizDoMapaAtual.size()), startY, endY);

        for (int y = startY; y < endY; y++)
        {
            std::string linhaSendoRenderizada = margemEsquerdaDoMapa;
            linhaSendoRenderizada.reserve(margemEsquerdaDoMapa.size() + (endX - startX) + 20);
            for (int x = startX; x < endX; x++)
            {
                if (x == posicaoXDoJogador && y == posicaoYDoJogador) {
                    linhaSendoRenderizada += Aparencia::cor(Cor::NEGRITO, Cor::VERDE) + "@" + Aparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'T') {
                    linhaSendoRenderizada += Aparencia::cor(Cor::NEGRITO, Cor::VERMELHO) + "T" + Aparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'G') {
                    linhaSendoRenderizada += Aparencia::cor(Cor::NEGRITO, Cor::AMARELO) + "G" + Aparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == '=' || matrizDoMapaAtual[y][x] == '|') {
                    linhaSendoRenderizada += Aparencia::cor(Cor::CINZA) + matrizDoMapaAtual[y][x] + Aparencia::cor(Cor::RESET);
                }
                else {
                    linhaSendoRenderizada += matrizDoMapaAtual[y][x];
                }
            }
            std::cout << linhaSendoRenderizada << "\n";
        }
        std::cout << "\n" << margemEsquerdaDosControles << textoDeControlesDoJogador << std::flush;
    };

    while (exploracaoEstaAtiva && jogadorAtual->obterVida() > 0)
    {
        int larguraDoTerminal = Aparencia::obterLarguraTerminal();
        int alturaDoTerminal = Aparencia::obterAlturaTerminal();

        renderizarMapa(larguraDoTerminal, alturaDoTerminal, linhaInicialParaDesenharOMapa);

        char teclaPressionadaPeloJogador = ControleDeInput::lerTecla();

        int proximaPosicaoX = posicaoXDoJogador;
        int proximaPosicaoY = posicaoYDoJogador;

        bool abriuMenu = ControleDeMapa::processarInputEComandos(teclaPressionadaPeloJogador, jogadorAtual, proximaPosicaoX, proximaPosicaoY, restaurarTela);
        
        if (jogadorAtual->obterVoltarProMenu()) break;
        if (abriuMenu) continue;

        ControleDeMapa::aplicarLimitesDeMapa(proximaPosicaoX, proximaPosicaoY, matrizDoMapaAtual);

        char celulaDestino = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX];
        
        if (celulaDestino == '^') {
            char nextCell = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1];
            if (nextCell == 'C') {
                if (!conviteRecebido) {
                    Aparencia::limparTela();
                    Aparencia::exibirCabecalho("ACESSO NEGADO", Cor::CIANO);
                    int espacosM = (larguraDoTerminal - 60) / 2;
                    std::cout << "\n" << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[SISTEMA]: Os portoes estao trancados. Voce precisa de uma permissao real.\n";
                    Aparencia::aguardarEnter();
                    restaurarTela();
                } else {
                    Aparencia::limparTela();
                    Aparencia::exibirCabecalho("FIM DA DEMO", Cor::CIANO);
                    int espacosM = (larguraDoTerminal - 60) / 2;
                    std::cout << "\n" << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[SISTEMA]: Voce apresentou o Convite Real e os portoes se abriram!\n";
                    std::cout << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[SISTEMA]: A historia continua em breve...\n";
                    Aparencia::aguardarEnter();
                    exploracaoEstaAtiva = false;
                }
            }
        else if (nextCell == 'F') {
            TransicaoDeMapa::exibirTransicaoParaFloresta();
            exploracaoEstaAtiva = false; // Interrompe o loop do Reino, devolvendo controle pra Floresta
        }
        } else if (celulaDestino == 'G') {
            Aparencia::limparTela();
            Aparencia::exibirCabecalho("GUARDA REAL", Cor::CIANO);
            int espacosM = (larguraDoTerminal - 60) / 2;
            std::cout << "\n" << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[Guarda]: Alto la! Somente o Rei pode conceder passagem.\n";
            std::cout << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[Guarda]: (O castelo ainda esta em construcao pelos deuses/devs)\n";
            Aparencia::aguardarEnter();
            restaurarTela();
    } else if (celulaDestino == 'T' || celulaDestino == 'C') {
            NPCCavaleiroGenerico::interagir(jogadorAtual, trollDerrotado, conviteRecebido, larguraDoTerminal, matrizDoMapaAtual, exploracaoEstaAtiva, restaurarTela, celulaDestino, proximaPosicaoX, proximaPosicaoY);
    } else if (celulaDestino != '#' && celulaDestino != '=' && celulaDestino != '|' && celulaDestino != '[' && celulaDestino != ']' && celulaDestino != 'A' && celulaDestino != 'S' && celulaDestino != 'E' && celulaDestino != 'L' && celulaDestino != 'O' && celulaDestino != ' ' || jogadorAtual->isNoclip()) {
            posicaoXDoJogador = proximaPosicaoX;
            posicaoYDoJogador = proximaPosicaoY;
        }
    }
}
