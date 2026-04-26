#include <iostream>
#include <vector>
#include <windows.h>
#include <map>
#include <cstdlib>
#include <memory>
#include <utility>

#include "Mapa1Vila.h"
#include "Mapa2Floresta.h"
#include "../Sistema/FuncionalidadeMenu.h"
#include "../Sistema/GeradorInimigos.h"
#include "../Sistema/GerenciadorCombate.h"
#include "../Inventario/Item.h"
#include "../Inventario/InventarioCombate.h"
#include "../Interfaces/TelaAtributos.h"
#include "../Interfaces/TelaBestiario.h"
#include "../NPCs/NPCBjorn.h"
#include "../NPCs/NPCFranchesco.h"
#include "../Inimigos/RacaOrkExilado.h"
#include "TransicaoDeMapa.h"
#include "../Sistema/SimplificacoesAparencia.h"
#include "ControleDeMapa.h"
#include "../Sistema/ControleDeInput.h"

Mapa::Mapa(Personagem* personagemJogador) :
jogadorAtual(personagemJogador), posicaoXDoJogador(2), posicaoYDoJogador(2), jogadorEstaDentroDeUmSubMapa(false),
posicaoXSalvaAntesDeEntrarNoSubMapa(0), posicaoYSalvaAntesDeEntrarNoSubMapa(0)
{
    matrizDoMapaAtual = {
        "             #######################################################################",
        " ########################################################################################",
        "##.###############......#######################..........################################",
        "##..........._____........_____...._____.........................##########################",
        "####........| { } |......| {%} |..| { } |.........**.............##########################",
        "###.........|  _  |......|  _  |..|  _  |........****..............#########################",
        "##..........| | | |......| | | |..| | | |.........||...............#########################",
        "##..........._____.......[^Forja].................||..**...........####^C##################",
        "###.........| {$} |..................................****............##...##################",
        "##..........|  _  |...................................||...................################",
        "###.........| | | |.......G.........~~~~~~~~~~~.......||....................################",
        "####........[^Loja]...............~~~~~~~~~~~~~~~~.......................###################",
        "#####................**.......~~~~~~~~~~~~~~~~~~~~......................###################",
        "##..................****......~~~~~~~~~~~~~~~~~~~~~~...........G........##################",
        "##...................||..........~~~~~~~~~~~~~~.........................#################",
        "####.................||..............~~~~~~~~~~~~.......................#################",
        "##...................................................G.................#################",
        "###........G................G...........................................###############",
        "###.....................................................................##############",
        "#####...........############.............################..........###################",
        "##############################################################^Floresta##############",
        "    ###############################################################################",
        "             ######                                              ################",
    };
}

void Mapa::iniciarLoopDeExploracaoDoMapa()
{
    bool exploracaoEstaAtiva = true;
    std::string tituloDoMapaAtual = "VILA INICIAL";

    std::vector<std::string> matrizDoMapaDaForjaSalva;
    bool bjornResgatado = false;
    bool forjaJaFoiVisitada = false;
    bool lojaJaFoiVisitada = false;
    bool cavernaJaFoiVisitada = false;

    HANDLE manipuladorDoTerminal = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO informacoesDoCursor;
    informacoesDoCursor.dwSize = 100;
    informacoesDoCursor.bVisible = FALSE;
    SetConsoleCursorInfo(manipuladorDoTerminal, &informacoesDoCursor);

    SimplificacoesAparencia::limparTela();
    Menu::exibirLogoDoJogo(tituloDoMapaAtual);

    CONSOLE_SCREEN_BUFFER_INFO informacoesDoBufferDaTela;
    GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
    int linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;

    // Lambda para restaurar a tela apos eventos sem piscar
    auto restaurarTela = [&]() {
        SimplificacoesAparencia::limparTela();
        Menu::exibirLogoDoJogo(tituloDoMapaAtual);
        GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
        linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
    };

    // Mapa base da vila — reutilizado no respawn apos a floresta

    const auto mapaBaseDaVila = matrizDoMapaAtual;

    auto processarInteracao = [&](int proximaPosicaoX, int proximaPosicaoY, int larguraDoTerminal) 
    {
        char celulaDestinoDoMapa = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX];
        if (celulaDestinoDoMapa == '#') return;

        if (celulaDestinoDoMapa == 'G')
        {
            ControleDeMapa::processarCombate(jogadorAtual, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, exploracaoEstaAtiva, "ENCONTRO INESPERADO", "Voce encontrou uma horda de Goblins!", GeradorInimigos::criarInimigoGoblin((std::rand() % 3) + 1), proximaPosicaoX, proximaPosicaoY, proximaPosicaoX, 1, larguraDoTerminal, restaurarTela);
        }
        else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'C' && !jogadorEstaDentroDeUmSubMapa)
        {
            ControleDeMapa::entrarSubMapa(matrizDoMapaAtual, matrizDoMapaPrincipalSalva, posicaoXSalvaAntesDeEntrarNoSubMapa, posicaoYSalvaAntesDeEntrarNoSubMapa, posicaoXDoJogador, posicaoYDoJogador, jogadorEstaDentroDeUmSubMapa, tituloDoMapaAtual, matrizDoMapaDaCavernaSalva, cavernaJaFoiVisitada, RacaOrkExilado::obterMapaCaverna(bjornResgatado), 16, 2, "CAVERNA DO ORK", restaurarTela);
        }
        else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'S' && jogadorEstaDentroDeUmSubMapa)
        {
            if (tituloDoMapaAtual == "CAVERNA DO ORK") matrizDoMapaDaCavernaSalva = matrizDoMapaAtual;
            else if (tituloDoMapaAtual == "LOJA DA VILA") matrizDoMapaDaLojaSalva = matrizDoMapaAtual;
            else if (tituloDoMapaAtual == "FORJA DA VILA") matrizDoMapaDaForjaSalva = matrizDoMapaAtual;

            matrizDoMapaAtual = matrizDoMapaPrincipalSalva;
            posicaoXDoJogador = posicaoXSalvaAntesDeEntrarNoSubMapa;
            posicaoYDoJogador = posicaoYSalvaAntesDeEntrarNoSubMapa;
            jogadorEstaDentroDeUmSubMapa = false;
            tituloDoMapaAtual = "VILA INICIAL";
            restaurarTela();
        }
        else if (celulaDestinoDoMapa == 'O' || (celulaDestinoDoMapa == 'm' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX-1] == 'O'))
        {
            int rootX = (celulaDestinoDoMapa == 'O') ? proximaPosicaoX : proximaPosicaoX - 1;
            ControleDeMapa::processarCombate(jogadorAtual, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, exploracaoEstaAtiva, "ENCONTRO NA CAVERNA", "Voce encontrou um Ork [m]!", GeradorInimigos::criarInimigoOrkExilado(1), proximaPosicaoX, proximaPosicaoY, rootX, 2, larguraDoTerminal, restaurarTela);
        }
        else if (celulaDestinoDoMapa == 'B' || (celulaDestinoDoMapa == 'n' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX-1] == 'B'))
        {
            if (tituloDoMapaAtual == "FORJA DA VILA" && celulaDestinoDoMapa == 'B') {
                NPCBjorn::interagir(jogadorAtual);
            } else if (tituloDoMapaAtual == "CAVERNA DO ORK") {
                SimplificacoesAparencia::limparTela();
                Menu::exibirLogoDoJogo("RESGATE NA CAVERNA");
                int espacosM = std::max(0, (larguraDoTerminal - 50) / 2);
                std::string mE(espacosM, ' ');
                std::cout << "\n" << mE << "[Bjorn]: Pelos deuses, muito obrigado por me salvar!\n";
                std::cout << mE << "[Bjorn]: Passe na Forja e eu ajudarei voce!\n";
                bjornResgatado = true;

                int rootX = (celulaDestinoDoMapa == 'B') ? proximaPosicaoX : proximaPosicaoX - 1;
                matrizDoMapaAtual[proximaPosicaoY][rootX] = '.';
                matrizDoMapaAtual[proximaPosicaoY][rootX+1] = '.';
                SimplificacoesAparencia::aguardarEnter();
            }
            if (exploracaoEstaAtiva) restaurarTela();
        }
        else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'F' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+2] == 'o' && !jogadorEstaDentroDeUmSubMapa)
        {
            if (!bjornResgatado)
            {
                SimplificacoesAparencia::limparTela();
                Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                int espacosM = std::max(0, (larguraDoTerminal - 60) / 2);
                std::cout << "\n" << std::string(espacosM, ' ') << "[SISTEMA]: A Forja esta trancada. O ferreiro sumiu...\n";
                SimplificacoesAparencia::aguardarEnter();
                restaurarTela();
                return;
            }
            ControleDeMapa::entrarSubMapa(matrizDoMapaAtual, matrizDoMapaPrincipalSalva, posicaoXSalvaAntesDeEntrarNoSubMapa, posicaoYSalvaAntesDeEntrarNoSubMapa, posicaoXDoJogador, posicaoYDoJogador, jogadorEstaDentroDeUmSubMapa, tituloDoMapaAtual, matrizDoMapaDaForjaSalva, forjaJaFoiVisitada, NPCBjorn::obterMapaForja(), 8, 2, "FORJA DA VILA", restaurarTela);
        }
        else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'L' && !jogadorEstaDentroDeUmSubMapa)
        {
            ControleDeMapa::entrarSubMapa(matrizDoMapaAtual, matrizDoMapaPrincipalSalva, posicaoXSalvaAntesDeEntrarNoSubMapa, posicaoYSalvaAntesDeEntrarNoSubMapa, posicaoXDoJogador, posicaoYDoJogador, jogadorEstaDentroDeUmSubMapa, tituloDoMapaAtual, matrizDoMapaDaLojaSalva, lojaJaFoiVisitada, NPCFranchesco::obterMapaLoja(), 8, 2, "LOJA DA VILA", restaurarTela);
        }
        else if (celulaDestinoDoMapa == 'F' && tituloDoMapaAtual == "LOJA DA VILA")
        {
            NPCFranchesco::interagir(jogadorAtual);
            if (exploracaoEstaAtiva) restaurarTela();
        }
        else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'F' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+2] == 'l' && !jogadorEstaDentroDeUmSubMapa)
        {
            TransicoesDeMapa::exibirTransicaoParaFloresta();

            Mapa2Floresta mapaFloresta(jogadorAtual);
            mapaFloresta.iniciarLoopDeExploracaoDoMapa();

            if (jogadorAtual->obterVoltarProMenu()) {
                exploracaoEstaAtiva = false;
                return;
            }

            // Respawn e recarregamento do mapa
            matrizDoMapaAtual = mapaBaseDaVila;
            cavernaJaFoiVisitada = false;
            restaurarTela();
            return;
        }
        else
        {
            posicaoXDoJogador = proximaPosicaoX;
            posicaoYDoJogador = proximaPosicaoY;
        }
    };

    auto renderizarMapa = [&](int larguraDoTerminal, int linhaInicial) 
    {
        int larguraDoMapaEmColunas = matrizDoMapaAtual.empty() ? 0 : matrizDoMapaAtual[0].length();
        int espacosParaCentralizarOMapa = (larguraDoTerminal - larguraDoMapaEmColunas) / 2;
        std::string margemEsquerdaDoMapa(espacosParaCentralizarOMapa > 0 ? espacosParaCentralizarOMapa : 0, ' ');

        std::string textoDeControlesDoJogador = "W,A,S,D: Mover | I: Inventario | C: Ficha | B: Bestiario ";
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
                if (x == posicaoXDoJogador && y == posicaoYDoJogador)
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::VERDE) << '@' << SimplificacoesAparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'G')
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::VERMELHO) << 'G' << SimplificacoesAparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'O')
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::VERMELHO);
                    if (x + 1 < matrizDoMapaAtual[y].size() && matrizDoMapaAtual[y][x+1] == 'm') {
                        std::cout << "Om";
                        x++;
                    } else {
                        std::cout << 'O';
                    }
                    std::cout << SimplificacoesAparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'B')
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::CIANO);
                    if (x + 1 < matrizDoMapaAtual[y].size() && matrizDoMapaAtual[y][x+1] == 'n') {
                        std::cout << "Bn";
                        x++;
                    } else {
                        std::cout << 'B';
                    }
                    std::cout << SimplificacoesAparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'F' && x > 0 && matrizDoMapaAtual[y][x-1] == '.')
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::AMARELO) << 'F' << SimplificacoesAparencia::cor(Cor::RESET);
                }
                else linhaSendoRenderizada += matrizDoMapaAtual[y][x];
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

        processarInteracao(proximaPosicaoX, proximaPosicaoY, larguraDoTerminal);
    }
}
