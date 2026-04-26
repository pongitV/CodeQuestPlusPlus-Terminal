#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <map>
#include <cstdlib>
#include <memory>
#include <utility>

#include "Mapa1Vila.h"
#include "Mapa2Floresta.h"
#include "../Sistema/Menu.h"
#include "../Sistema/GeradorInimigos.h"
#include "../Sistema/SistemaRPG.h"
#include "../Inventario/Item.h"
#include "../Inventario/InventarioCombate.h"
#include "../Interfaces/TelaAtributos.h"
#include "../NPCs/NPCBjorn.h"
#include "../NPCs/NPCFranchesco.h"
#include "../Inimigos/RacaOrkExilado.h"
#include "TransicaoDeMapa.h"

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

    static std::vector<std::string> matrizDoMapaDaForjaSalva;
    static bool bjornResgatado = false;
    static bool forjaJaFoiVisitada = false;
    static bool lojaJaFoiVisitada = false;
    static bool cavernaJaFoiVisitada = false;

    HANDLE manipuladorDoTerminal = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO informacoesDoCursor;
    informacoesDoCursor.dwSize = 100;
    informacoesDoCursor.bVisible = FALSE;
    SetConsoleCursorInfo(manipuladorDoTerminal, &informacoesDoCursor);

    Menu::limparTelaDoTerminal();
    Menu::exibirLogoDoJogo(tituloDoMapaAtual);

    CONSOLE_SCREEN_BUFFER_INFO informacoesDoBufferDaTela;
    GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
    int linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;

    // Lambda para restaurar a tela apos eventos sem piscar
    auto restaurarTela = [&]() {
        Menu::limparTelaDoTerminal();
        Menu::exibirLogoDoJogo(tituloDoMapaAtual);
        GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
        linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
    };

    // Mapa base da vila — reutilizado no respawn apos a floresta
    const auto mapaBaseDaVila = matrizDoMapaAtual;

    while (exploracaoEstaAtiva && jogadorAtual->obterVida() > 0)
    {
        GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
        int larguraDoTerminal = informacoesDoBufferDaTela.srWindow.Right - informacoesDoBufferDaTela.srWindow.Left + 1;

        int larguraDoMapaEmColunas = matrizDoMapaAtual.empty() ? 0 : matrizDoMapaAtual[0].length();
        int espacosParaCentralizarOMapa = (larguraDoTerminal - larguraDoMapaEmColunas) / 2;
        std::string margemEsquerdaDoMapa(espacosParaCentralizarOMapa > 0 ? espacosParaCentralizarOMapa : 0, ' ');

        std::string textoDeControlesDoJogador = "W,A,S,D: Mover | I: Inventario | C: Ficha ";
        int espacosParaCentralizarOsControles = (larguraDoTerminal - (int)textoDeControlesDoJogador.length()) / 2;
        std::string margemEsquerdaDosControles(espacosParaCentralizarOsControles > 0 ? espacosParaCentralizarOsControles : 0, ' ');

        COORD posicaoDoCursorNoTerminal;
        posicaoDoCursorNoTerminal.X = 0;
        posicaoDoCursorNoTerminal.Y = linhaInicialParaDesenharOMapa;
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
                            std::cout << "\x1b[1;32m";
                            std::cout << '@';
                            std::cout << "\x1b[0m";
                        }
                else if (matrizDoMapaAtual[y][x] == 'G')
                        {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                            std::cout << "\x1b[1;31m";
                            std::cout << 'G';
                            std::cout << "\x1b[0m";
                        }
                else if (matrizDoMapaAtual[y][x] == 'O')
                        {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                            std::cout << "\x1b[1;31m";
                    if (x + 1 < matrizDoMapaAtual[y].size() && matrizDoMapaAtual[y][x+1] == 'm') {
                        std::cout << "Om";
                        x++;
                    } else {
                        std::cout << 'O';
                    }
                            std::cout << "\x1b[0m";
                        }
                else if (matrizDoMapaAtual[y][x] == 'B')
                    {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                        std::cout << "\x1b[1;36m";
                    if (x + 1 < matrizDoMapaAtual[y].size() && matrizDoMapaAtual[y][x+1] == 'n') {
                        std::cout << "Bn";
                        x++;
                    } else {
                        std::cout << 'B';
                    }
                        std::cout << "\x1b[0m";
                    }
                else if (matrizDoMapaAtual[y][x] == 'F' && x > 0 && matrizDoMapaAtual[y][x-1] == '.')
                    {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                        std::cout << "\x1b[1;33m";
                        std::cout << 'F';
                        std::cout << "\x1b[0m";
                    }
                else linhaSendoRenderizada += matrizDoMapaAtual[y][x];
                    }
            std::cout << linhaSendoRenderizada << "\n";
                }

        std::cout << "\n" << margemEsquerdaDosControles << textoDeControlesDoJogador << "\n";

        char teclaPressionadaPeloJogador = _getch();

        int proximaPosicaoX = posicaoXDoJogador;
        int proximaPosicaoY = posicaoYDoJogador;

        if (teclaPressionadaPeloJogador == 'w' || teclaPressionadaPeloJogador == 'W' || teclaPressionadaPeloJogador == 72) proximaPosicaoY--;
        if (teclaPressionadaPeloJogador == 's' || teclaPressionadaPeloJogador == 'S' || teclaPressionadaPeloJogador == 80) proximaPosicaoY++;
        if (teclaPressionadaPeloJogador == 'a' || teclaPressionadaPeloJogador == 'A' || teclaPressionadaPeloJogador == 75) proximaPosicaoX--;
        if (teclaPressionadaPeloJogador == 'd' || teclaPressionadaPeloJogador == 'D' || teclaPressionadaPeloJogador == 77) proximaPosicaoX++;

        if (teclaPressionadaPeloJogador == 'i' || teclaPressionadaPeloJogador == 'I')
        {
            InventarioCombate::gerenciarInventario(jogadorAtual);
            restaurarTela();
            continue;
        }

        if (teclaPressionadaPeloJogador == 'c' || teclaPressionadaPeloJogador == 'C')
        {
            TelaAtributos::gerenciarFichaDoJogador(jogadorAtual);
            if (jogadorAtual->obterVoltarProMenu()) break;
            restaurarTela();
            continue;
        }

        char celulaDestinoDoMapa = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX];

        if (celulaDestinoDoMapa != '#')
        {
            // --- Goblin ---
            if (celulaDestinoDoMapa == 'G')
            {
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo("ENCONTRO INESPERADO");
                int espacosM = (larguraDoTerminal - 40) / 2;
                std::string mE(espacosM > 0 ? espacosM : 0, ' ');
                std::cout << "\n" << mE << "[!] Voce encontrou uma horda de Goblins!\n";
                std::cout << mE << "[0] Nao, recuar | [1] Sim, batalha!\n" << mE << "Escolha: ";

                int opcao;
                if (std::cin >> opcao && opcao == 1)
                {
                    int qtd = (std::rand() % 3) + 1;
                    SistemaRPG combate(jogadorAtual, GeradorInimigos::criarInimigoGoblin(qtd));
                    combate.iniciarCombate();

                    if (jogadorAtual->obterVida() > 0) {
                        matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX] = '.';
                        posicaoXDoJogador = proximaPosicaoX;
                        posicaoYDoJogador = proximaPosicaoY;
                    }
                } else { std::cin.clear(); std::cin.ignore(1000, '\n'); }

                if (exploracaoEstaAtiva) restaurarTela();
            }
            // --- Entrar na Caverna do Ork ---
            else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'C' && !jogadorEstaDentroDeUmSubMapa)
            {
                matrizDoMapaPrincipalSalva = matrizDoMapaAtual;
                posicaoXSalvaAntesDeEntrarNoSubMapa = posicaoXDoJogador;
                posicaoYSalvaAntesDeEntrarNoSubMapa = posicaoYDoJogador;

                if (!cavernaJaFoiVisitada) {
                    matrizDoMapaAtual = RacaOrkExilado::obterMapaCaverna(bjornResgatado);
                    cavernaJaFoiVisitada = true;
                } else {
                    matrizDoMapaAtual = matrizDoMapaDaCavernaSalva;
                }

                posicaoXDoJogador = 16;
                posicaoYDoJogador = 2;
                jogadorEstaDentroDeUmSubMapa = true;
                tituloDoMapaAtual = "CAVERNA DO ORK";
                restaurarTela();
                continue;
            }
            // --- Sair do sub-mapa ---
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
                continue;
            }
            // --- Ork Mini-Boss ---
            else if (celulaDestinoDoMapa == 'O' || (celulaDestinoDoMapa == 'm' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX-1] == 'O'))
            {
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo("ENCONTRO NA CAVERNA");
                int espacosM = (larguraDoTerminal - 40) / 2;
                std::string mE(espacosM > 0 ? espacosM : 0, ' ');
                std::cout << "\n" << mE << "[!] Voce encontrou um Ork [m]!\n";
                std::cout << mE << "[0] Nao, recuar | [1] Sim, batalha!\n" << mE << "Escolha: ";

                int opcao;
                if (std::cin >> opcao && opcao == 1)
                {
                    SistemaRPG combate(jogadorAtual, GeradorInimigos::criarInimigoOrkExilado());
                    combate.iniciarCombate();

                    if (jogadorAtual->obterVida() > 0) {
                        if (celulaDestinoDoMapa == 'O') {
                            matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX] = '.';
                            if (matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'm') matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] = '.';
                        } else {
                            matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX] = '.';
                            matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX-1] = '.';
                        }
                        posicaoXDoJogador = proximaPosicaoX;
                        posicaoYDoJogador = proximaPosicaoY;
                    }
                } else { std::cin.clear(); std::cin.ignore(1000, '\n'); }

                if (exploracaoEstaAtiva) restaurarTela();
            }
            // --- Bjorn NPC ---
            else if (celulaDestinoDoMapa == 'B' || (celulaDestinoDoMapa == 'n' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX-1] == 'B'))
            {
                if (tituloDoMapaAtual == "FORJA DA VILA" && celulaDestinoDoMapa == 'B') {
                    NPCBjorn::interagir(jogadorAtual);
                } else if (tituloDoMapaAtual == "CAVERNA DO ORK") {
                    Menu::limparTelaDoTerminal();
                    Menu::exibirLogoDoJogo("RESGATE NA CAVERNA");
                    int espacosM = (larguraDoTerminal - 50) / 2;
                    std::string mE(espacosM > 0 ? espacosM : 0, ' ');
                    std::cout << "\n" << mE << "[Bjorn]: Pelos deuses, muito obrigado por me salvar!\n";
                    std::cout << mE << "[Bjorn]: Passe na Forja e eu ajudarei voce!\n";
                    bjornResgatado = true;

                    if (celulaDestinoDoMapa == 'B') {
                        matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX] = '.';
                        if (matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'n') matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] = '.';
                    } else {
                        matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX] = '.';
                        matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX-1] = '.';
                    }
                    Menu::aguardarPressionamentoDeEnter();
                }
                if (exploracaoEstaAtiva) restaurarTela();
            }
            // --- Entrar na Forja ---
            else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'F' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+2] == 'o' && !jogadorEstaDentroDeUmSubMapa)
            {
                if (!bjornResgatado)
                {
                    Menu::limparTelaDoTerminal();
                    Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                    int espacosM = (larguraDoTerminal - 60) / 2;
                    std::cout << "\n" << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[SISTEMA]: A Forja esta trancada. O ferreiro sumiu...\n";
                    Menu::aguardarPressionamentoDeEnter();
                    restaurarTela();
                    continue;
                }

                matrizDoMapaPrincipalSalva = matrizDoMapaAtual;
                posicaoXSalvaAntesDeEntrarNoSubMapa = posicaoXDoJogador;
                posicaoYSalvaAntesDeEntrarNoSubMapa = posicaoYDoJogador;

                if (!forjaJaFoiVisitada) {
                    matrizDoMapaAtual = NPCBjorn::obterMapaForja();
                    forjaJaFoiVisitada = true;
                } else {
                    matrizDoMapaAtual = matrizDoMapaDaForjaSalva;
                }

                posicaoXDoJogador = 8;
                posicaoYDoJogador = 2;
                jogadorEstaDentroDeUmSubMapa = true;
                tituloDoMapaAtual = "FORJA DA VILA";
                restaurarTela();
                continue;
            }
            // --- Entrar na Loja ---
            else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'L' && !jogadorEstaDentroDeUmSubMapa)
            {
                matrizDoMapaPrincipalSalva = matrizDoMapaAtual;
                posicaoXSalvaAntesDeEntrarNoSubMapa = posicaoXDoJogador;
                posicaoYSalvaAntesDeEntrarNoSubMapa = posicaoYDoJogador;

                if (!lojaJaFoiVisitada) {
                    matrizDoMapaAtual = NPCFranchesco::obterMapaLoja();
                    lojaJaFoiVisitada = true;
                } else {
                    matrizDoMapaAtual = matrizDoMapaDaLojaSalva;
                }

                posicaoXDoJogador = 8;
                posicaoYDoJogador = 2;
                jogadorEstaDentroDeUmSubMapa = true;
                tituloDoMapaAtual = "LOJA DA VILA";
                restaurarTela();
                continue;
            }
            // --- Franchesco NPC na Loja ---
            else if (celulaDestinoDoMapa == 'F' && tituloDoMapaAtual == "LOJA DA VILA")
            {
                NPCFranchesco::interagir(jogadorAtual);
                if (exploracaoEstaAtiva) restaurarTela();
            }
            // --- Entrar na Floresta ---
            else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'F' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+2] == 'l' && !jogadorEstaDentroDeUmSubMapa)
            {
                TransicoesDeMapa::exibirTransicaoParaFloresta();

                Mapa2Floresta mapaFloresta(jogadorAtual);
                mapaFloresta.iniciarLoopDeExploracaoDoMapa();

                if (jogadorAtual->obterVoltarProMenu()) break;

                // Respawn e recarregamento do mapa
                matrizDoMapaAtual = mapaBaseDaVila;
                cavernaJaFoiVisitada = false;
                restaurarTela();
                continue;
            }
            // --- Movimento livre ---
            else
            {
                posicaoXDoJogador = proximaPosicaoX;
                posicaoYDoJogador = proximaPosicaoY;
            }
        }
    }
}
