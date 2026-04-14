#include <iostream>
#include <vector>
#include <conio.h> 
#include <windows.h> 
#include <map>
#include <cstdlib>

#include "Mapa1Vila.h"
#include "Mapa2Floresta.h"
#include "../Sistema/Menu.h"
#include "../Sistema/GeradorInimigos.h"
#include "../Sistema/SistemaRPG.h"
#include "../Inventario/Item.h"
#include "../NPCs/NPCBjorn.h"
#include "../NPCs/NPCFranchesco.h"
#include "../Inimigos/RacaOrkExilado.h"

Mapa::Mapa(Personagem* personagemJogador) : 
jogadorAtual(personagemJogador), posicaoXDoJogador(2), posicaoYDoJogador(2), jogadorEstaDentroDeUmSubMapa(false), 
cavernaJaFoiVisitada(false), lojaJaFoiVisitada(false), 
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

    static bool bjornResgatado = false;
    static bool forjaJaFoiVisitada = false;
    static std::vector<std::string> matrizDoMapaDaForjaSalva;
    
    HANDLE manipuladorDoTerminal = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO informacoesDoCursor;
    informacoesDoCursor.dwSize = 100;
    informacoesDoCursor.bVisible = FALSE;
    SetConsoleCursorInfo(manipuladorDoTerminal, &informacoesDoCursor);

    Menu::limparTelaDoTerminal();
    Menu::exibirLogoDoJogo(tituloDoMapaAtual);

    // Descobre onde a logo termina para renderizar o mapa perfeitamente logo abaixo dela
    CONSOLE_SCREEN_BUFFER_INFO informacoesDoBufferDaTela;
    GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
    int linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;

    while (exploracaoEstaAtiva && jogadorAtual->obterVida() > 0) 
    {
        // Calcula os espacos dinamicamente para manter o mapa e instrucoes sempre no centro da tela
        GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
        int larguraDoTerminal = informacoesDoBufferDaTela.srWindow.Right - informacoesDoBufferDaTela.srWindow.Left + 1;

        int larguraDoMapaEmColunas = matrizDoMapaAtual.empty() ? 0 : matrizDoMapaAtual[0].length();
        int espacosParaCentralizarOMapa = (larguraDoTerminal - larguraDoMapaEmColunas) / 2;
        std::string margemEsquerdaDoMapa(espacosParaCentralizarOMapa > 0 ? espacosParaCentralizarOMapa : 0, ' ');

        std::string textoDeControlesDoJogador = "W,A,S,D: Mover | I: Inventario | C: Ficha ";
        int espacosParaCentralizarOsControles = (larguraDoTerminal - (int)textoDeControlesDoJogador.length()) / 2;
        std::string margemEsquerdaDosControles(espacosParaCentralizarOsControles > 0 ? espacosParaCentralizarOsControles : 0, ' ');

        // Move o cursor para o topo do mapa invés de limpar a tela toda (evita piscar pesado)
        COORD posicaoDoCursorNoTerminal;
        posicaoDoCursorNoTerminal.X = 0;
        posicaoDoCursorNoTerminal.Y = linhaInicialParaDesenharOMapa;
        SetConsoleCursorPosition(manipuladorDoTerminal, posicaoDoCursorNoTerminal);

        for (int y = 0; y < matrizDoMapaAtual.size(); y++) 
                {
            std::string linhaSendoRenderizada = margemEsquerdaDoMapa; // Margem dinamica centralizada
            linhaSendoRenderizada.reserve(margemEsquerdaDoMapa.size() + matrizDoMapaAtual[y].size() + 20); // Evita fragmentacao e saltos na RAM
            for (int x = 0; x < matrizDoMapaAtual[y].size(); x++) 
                    {
                if (x == posicaoXDoJogador && y == posicaoYDoJogador) 
                        {
                    std::cout << linhaSendoRenderizada; 
                    linhaSendoRenderizada = "";        
                            std::cout << "\x1b[1;32m"; // Verde para o jogador
                            std::cout << '@';            
                            std::cout << "\x1b[0m";     // Reset para branco
                        }
                else if (matrizDoMapaAtual[y][x] == 'G')
                        {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                            std::cout << "\x1b[1;31m"; // Vermelho para goblins
                            std::cout << 'G';
                            std::cout << "\x1b[0m";     // Reset para branco
                        }
                else if (matrizDoMapaAtual[y][x] == 'O')
                        {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                            std::cout << "\x1b[1;31m"; // Vermelho para o Ork
                    if (x + 1 < matrizDoMapaAtual[y].size() && matrizDoMapaAtual[y][x+1] == 'm') {
                        std::cout << "Om";
                        x++; // Pula a letra 'm' no loop para nao renderizar duas vezes
                    } else {
                        std::cout << 'O';
                    }
                            std::cout << "\x1b[0m";     // Reset para branco
                        }
                else if (matrizDoMapaAtual[y][x] == 'B')
                    {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                        std::cout << "\x1b[1;36m"; // Ciano para o Bjorn
                    if (x + 1 < matrizDoMapaAtual[y].size() && matrizDoMapaAtual[y][x+1] == 'n') {
                        std::cout << "Bn";
                        x++; // Pula a letra 'n' no loop
                    } else {
                        std::cout << 'B';
                    }
                        std::cout << "\x1b[0m";     // Reset para branco
                    }
                else if (matrizDoMapaAtual[y][x] == 'F' && x > 0 && matrizDoMapaAtual[y][x-1] == '.')
                    {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                        std::cout << "\x1b[1;33m"; // Amarelo para Franchesco
                        std::cout << 'F';
                        std::cout << "\x1b[0m";     // Reset para branco
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
            Menu::gerenciarInventario(jogadorAtual);

            // Restaura a renderizacao padrao do mapa apos fechar o inventario
            Menu::limparTelaDoTerminal();
            Menu::exibirLogoDoJogo(tituloDoMapaAtual);
            GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
            linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
            continue;
        }

        if (teclaPressionadaPeloJogador == 'c' || teclaPressionadaPeloJogador == 'C') 
        {
            Menu::gerenciarFichaDoJogador(jogadorAtual);

            // Restaura a tela do mapa
            Menu::limparTelaDoTerminal();
            Menu::exibirLogoDoJogo(tituloDoMapaAtual);
            GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
            linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
            continue;
        }

        char celulaDestinoDoMapa = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX];

        if (celulaDestinoDoMapa != '#') 
        {
            if (celulaDestinoDoMapa == 'G') 
            {
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo("ENCONTRO INESPERADO");
                
                int espacosParaCentralizarMensagem = (larguraDoTerminal - 40) / 2; 
                std::string margemEsquerdaDaMensagem(espacosParaCentralizarMensagem > 0 ? espacosParaCentralizarMensagem : 0, ' ');

                std::cout << "\n" << margemEsquerdaDaMensagem << "[!] Voce encontrou uma horda de Goblins!\n";
                std::cout << margemEsquerdaDaMensagem << "Deseja enfrentar a horda?\n";
                std::cout << margemEsquerdaDaMensagem << "[0] Nao, recuar com cuidado\n";
                std::cout << margemEsquerdaDaMensagem << "[1] Sim, para a batalha!\n";
                std::cout << "\n" << margemEsquerdaDaMensagem << "Escolha: ";

                int opcaoEscolhidaNoDialogo;
                if (std::cin >> opcaoEscolhidaNoDialogo && opcaoEscolhidaNoDialogo == 1) 
                {
                    int quantidadeDeInimigos = (std::rand() % 3) + 1;
                    std::vector<Personagem*> listaDeInimigosGerados = GeradorInimigos::criarHordaDeGoblins(quantidadeDeInimigos);
                    SistemaRPG sessaoDeCombate(jogadorAtual, listaDeInimigosGerados);
                    sessaoDeCombate.iniciarCombate(); // A chamada do combate acontece aqui!

                    if (jogadorAtual->obterVida() > 0) 
                    {
                        matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX] = '.'; // Tira o 'G' do mapa se vencer
                        posicaoXDoJogador = proximaPosicaoX;
                        posicaoYDoJogador = proximaPosicaoY;
                    }
                } 
                else 
                {
                    std::cin.clear(); std::cin.ignore(1000, '\n');
                }

                // Se sobreviveu e o loop continuar, restaura a tela do mapa perfeitamente
                if (exploracaoEstaAtiva) {
                    Menu::limparTelaDoTerminal();
                    Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                    GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
                    linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
                }
            } 
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
                
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
                linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
                continue;
            }
            else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'S' && jogadorEstaDentroDeUmSubMapa)
            {
                // Salva o sub-mapa correto de acordo com a area atual antes de sair
                if (tituloDoMapaAtual == "CAVERNA DO ORK") matrizDoMapaDaCavernaSalva = matrizDoMapaAtual;
                else if (tituloDoMapaAtual == "LOJA DA VILA") matrizDoMapaDaLojaSalva = matrizDoMapaAtual;
                else if (tituloDoMapaAtual == "FORJA DA VILA") matrizDoMapaDaForjaSalva = matrizDoMapaAtual;

                matrizDoMapaAtual = matrizDoMapaPrincipalSalva;
                posicaoXDoJogador = posicaoXSalvaAntesDeEntrarNoSubMapa;
                posicaoYDoJogador = posicaoYSalvaAntesDeEntrarNoSubMapa;
                jogadorEstaDentroDeUmSubMapa = false;
                tituloDoMapaAtual = "VILA INICIAL";
                
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
                linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
                continue;
            }
            else if (celulaDestinoDoMapa == 'O' || (celulaDestinoDoMapa == 'm' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX-1] == 'O')) 
            {
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo("ENCONTRO NA CAVERNA");
                
                int espacosParaCentralizarMensagem = (larguraDoTerminal - 40) / 2;
                std::string margemEsquerdaDaMensagem(espacosParaCentralizarMensagem > 0 ? espacosParaCentralizarMensagem : 0, ' ');

                std::cout << "\n" << margemEsquerdaDaMensagem << "[!] Voce encontrou um Ork [m]!\n";
                std::cout << margemEsquerdaDaMensagem << "A criatura ruge desafiando voce!\n";
                std::cout << margemEsquerdaDaMensagem << "Deseja enfrentar o Ork?\n";
                std::cout << margemEsquerdaDaMensagem << "[0] Nao, recuar com cuidado\n";
                std::cout << margemEsquerdaDaMensagem << "[1] Sim, para a batalha!\n";
                std::cout << "\n" << margemEsquerdaDaMensagem << "Escolha: ";

                int opcaoEscolhidaNoDialogo;
                if (std::cin >> opcaoEscolhidaNoDialogo && opcaoEscolhidaNoDialogo == 1) 
                {
                    // Inicia o combate com o Ork Mini-Boss gerado pelo Gerador de Inimigos
                    std::vector<Personagem*> listaDeInimigosGerados = { GeradorInimigos::criarInimigoOrkExilado() };
                    SistemaRPG sessaoDeCombate(jogadorAtual, listaDeInimigosGerados);
                    sessaoDeCombate.iniciarCombate();

                    if (jogadorAtual->obterVida() > 0) 
                    {
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
                } 
                else 
                {
                    std::cin.clear(); std::cin.ignore(1000, '\n');
                }

                if (exploracaoEstaAtiva) {
                    Menu::limparTelaDoTerminal();
                    Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                    GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
                    linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
                }
            }
            else if (celulaDestinoDoMapa == 'B' || (celulaDestinoDoMapa == 'n' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX-1] == 'B')) 
            {
                if (tituloDoMapaAtual == "FORJA DA VILA" && celulaDestinoDoMapa == 'B')
                {
                    NPCBjorn::interagir(jogadorAtual);
                }
                else if (tituloDoMapaAtual == "CAVERNA DO ORK")
                {
                    Menu::limparTelaDoTerminal();
                    Menu::exibirLogoDoJogo("RESGATE NA CAVERNA");
                    
                    int espacosMsg = (larguraDoTerminal - 50) / 2;
                    std::string margemMsg(espacosMsg > 0 ? espacosMsg : 0, ' ');

                    std::cout << "\n" << margemMsg << "[Bjorn]: Pelos deuses, muito obrigado por me salvar!\n";
                    std::cout << margemMsg << "[Bjorn]: Aquele Ork me encurralou e eu achei que era o fim.\n";
                    std::cout << margemMsg << "[Bjorn]: Sou o ferreiro da vila. Passe na Forja e eu ajudarei voce!\n";
                    
                    bjornResgatado = true;
                    
                    if (celulaDestinoDoMapa == 'B') 
                    {
                        matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX] = '.'; 
                        if (matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'n') matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] = '.';
                    } 
                    else 
                    {
                        matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX] = '.'; 
                        matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX-1] = '.';
                    }
                    
                    Menu::aguardarPressionamentoDeEnter();
                }
                
                if (exploracaoEstaAtiva) {
                    Menu::limparTelaDoTerminal();
                    Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                    GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
                    linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
                }
            }
            else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'F' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+2] == 'o' && !jogadorEstaDentroDeUmSubMapa)
            {
                if (!bjornResgatado) 
                {
                    Menu::limparTelaDoTerminal();
                    Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                    
                    int espacosMsg = (larguraDoTerminal - 60) / 2;
                    std::cout << "\n" << std::string(espacosMsg > 0 ? espacosMsg : 0, ' ') << "[SISTEMA]: A Forja esta trancada. O ferreiro sumiu...\n";
                    Menu::aguardarPressionamentoDeEnter();
                    
                    Menu::limparTelaDoTerminal();
                    Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                    GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
                    linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
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
                
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
                linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
                continue;
            }
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
                
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
                linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
                continue;
            }
            else if (celulaDestinoDoMapa == 'F' && tituloDoMapaAtual == "LOJA DA VILA")
            {
                NPCFranchesco::interagir(jogadorAtual);

                if (exploracaoEstaAtiva) {
                    Menu::limparTelaDoTerminal();
                    Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                    GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
                    linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
                }
            }
            else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'F' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+2] == 'l' && !jogadorEstaDentroDeUmSubMapa)
            {
                Menu::limparTelaDoTerminal();
                
                std::vector<std::string> arteFloresta = {
                    "⢧⢫⡜⣣⠝⣮⢱⢫⡜⡱⣍⢮⡱⢭⣙⢦⣀⠉⠞⣬⠣⡝⣬⠣⡝⣬⠹⡜⣆⠏⡖⢭⢚⣌⠳⣌⠳⣌⠳⣌⠳⣘⠦⡍⠄⣫⡗⡜⡜⣢⠝⡢⢝⢢⠓⡜⢢⢓⡜⢢⢓⡜⢢⠓⡜⢢⠓⡜⢢⢃⡆⠘⢦⡑⢎⡔⢣⠜⡰⠱⡌⢦⠱⢌⠦⡙⢇⡰⠡⢎⠰⢌⠒⡌⠒⠬⢀⣴⢂⡱⠌⣆⠱⣲⡿⢋⠴⣉⢛⡛⠻⢷⣌⡆⢑⡂⠖⣈⠒⣄⢃⠲⡐⢌⠒⡄⢃⡒⡐⢆⠒⡐⢂⠆⡰",
                    "⣣⢓⢮⡱⣋⠶⣩⠖⣭⠳⡜⢦⡙⣦⡙⢮⡹⢷⣄⡀⠫⠜⣆⡛⡼⡐⣏⠽⣢⢛⢬⠓⣎⠦⣋⠴⣋⠴⣋⠴⣋⠦⡓⣍⠓⣺⡱⣊⠵⢢⡍⠵⣊⢥⡋⡜⢣⠎⡜⣡⠎⣜⡡⢋⡜⣡⢋⡜⡡⢎⢲⡏⢲⡘⢦⡘⢥⢊⡕⢣⠜⡢⢍⢆⠣⢍⠎⢷⡱⠌⢣⠌⢪⡐⠎⢀⠏⡔⡊⠴⡑⣬⣾⠟⡰⣉⠒⡤⢃⡌⠓⡌⡍⢻⣷⣈⠆⡅⢎⠰⣈⢆⡉⢆⡩⢐⠣⡐⡡⢊⠜⡠⠃⠔⡠",
                    "⣣⢏⠶⡱⣍⠞⣥⢛⡴⢫⡜⣣⠝⣦⡙⢦⡙⢦⣋⠿⣦⣈⠐⢙⠖⣭⠲⣩⢳⡍⠶⣙⢆⢳⠩⣖⢩⢖⡩⢖⡱⢎⡱⠀⣎⡱⢭⠱⢎⡱⢊⡵⢡⠖⣡⠝⣢⡙⡜⢤⢋⡴⢡⢋⠴⡡⢎⡔⢣⢍⡚⣦⢥⠚⡤⡙⢤⠣⣌⠣⢎⡱⠌⠦⣙⠢⢚⠔⣻⡜⡂⢎⡡⠜⢁⡤⣈⠐⡉⠖⣵⡿⢡⠎⡱⢠⠋⡔⡡⢌⠓⡰⢌⡡⠌⣿⡔⢌⢢⢑⠂⣆⠸⢄⠂⡑⢢⢑⡰⢁⠎⡐⢡⠊⡐",
                    "⠵⣊⠷⡱⢎⡝⢦⢫⡜⣣⠞⣥⢛⡴⣙⠦⡝⢦⡙⣞⡸⡝⣷⣦⣌⠂⠻⡔⣍⡚⠱⢣⢎⡣⡝⣤⢋⢦⠓⣎⠴⠃⣤⢚⠥⣚⡔⢫⢬⠱⣉⢖⡩⢎⡱⢊⠵⣘⠜⣢⠣⡜⣡⢎⡱⡑⢎⡌⢣⠎⡴⠈⠦⡙⡔⡩⢆⠳⡨⢜⠢⡱⣀⠣⠜⣈⠧⡘⠜⣷⢉⠆⠴⠁⡌⢆⣉⣳⠶⠟⡍⡒⠥⢚⠰⡡⢍⢢⡑⢊⠥⡑⢢⠜⡨⢉⡔⢊⡔⢊⡔⢢⠑⡌⠲⠌⡐⢢⠐⡡⠊⢄⡁⢂⠐",
                    "⣛⢬⢳⡙⣎⠞⣥⡓⢮⡱⣋⠶⣩⠖⣭⢚⡹⢦⡙⢦⡱⢓⢦⡙⠿⣧⡈⠘⠴⢻⠖⡶⢦⣑⣘⠰⢋⠦⠋⢀⡴⢛⡤⣋⠞⣰⢪⠱⣊⠵⣉⠦⡱⢎⠲⣍⠲⢥⠚⡥⢓⡜⡰⢊⡴⣉⠖⣌⢣⠚⡔⠀⠜⡱⢌⡱⢊⡱⡘⢆⢣⡑⢆⠳⡞⠦⢤⡉⢎⢹⣧⠚⡐⡼⢑⣶⠫⡑⡜⡘⠤⣑⠣⢊⡱⡐⠎⡔⠬⡑⠢⡅⢣⡘⣐⠣⣘⠰⡈⢆⡘⠤⡉⠶⢥⡓⡨⠄⠃⡔⠉⢄⡐⠨⠐",
                    "⢭⢎⢧⡹⣌⠻⡴⣩⢇⡳⡍⡞⣥⠺⣔⢫⡜⢦⡙⢦⡹⢍⠶⣩⢳⢩⢻⣦⡌⠃⠯⡜⢦⢹⡸⢍⡏⢻⠀⢩⢎⡱⢆⡣⡝⢤⡃⠯⣔⡩⡒⡥⡓⢬⠓⣌⠳⣌⠳⣌⠣⡜⣡⢃⠖⣡⠚⡤⢣⡙⠀⢠⡉⢖⡡⢎⡱⢢⡙⢌⠲⠌⣎⣡⡷⢟⡒⢾⣶⠦⢭⡬⢴⠞⣩⠂⡕⢡⠒⣉⠲⠤⡙⢢⠑⡜⡘⡰⠡⢎⠱⣈⠅⠲⡄⣃⠆⠥⠱⣈⠔⢣⠘⠤⣹⠧⣄⡈⠁⠢⢉⠤⠐⠡⢃",
                    "⡳⢎⠶⡱⢎⡳⣱⢣⢮⡱⡹⡜⢦⡛⣬⠳⣘⢧⡙⢦⡙⣎⢣⡓⢎⣣⠳⣘⠿⣶⣌⠘⢮⠱⣜⢪⠜⡣⢆⠀⢞⡸⡌⢵⡘⡥⢚⡱⢆⡱⡱⡡⢝⢢⣋⠬⡓⣌⠳⣈⠳⡘⡤⢋⡜⢤⢋⡔⢣⠜⢉⡇⡜⢢⡑⠎⡴⢡⠚⡬⣡⣯⠴⢫⠜⡰⡘⢢⠌⡓⠦⣿⠎⡜⡠⢍⠰⢃⡍⠴⡑⢢⠙⣄⠋⡴⢡⡑⠣⡌⢱⣀⢋⠱⡐⡂⢎⠡⢓⡰⢜⠲⢓⠋⡅⢒⠠⡉⠝⠲⠶⣀⠉⢐⠖",
                    "⠳⣍⡞⣱⢋⢶⡡⢏⠶⣱⢣⡝⢦⡙⢦⡛⡜⢦⡙⢦⡹⡌⢧⡙⢮⡔⢫⡜⢎⡭⢻⣷⣌⠳⣌⢣⠞⡱⢎⠄⢸⠰⣍⠦⢣⡙⣬⠱⣊⠵⣡⠹⡘⠦⡜⢲⠱⣌⠳⣌⢣⠱⣌⠣⡜⢢⠣⡜⡡⢮⣟⡰⣉⠦⢱⡉⢆⢣⢓⡰⢩⢄⠫⠔⢣⡑⢌⠣⠜⣐⠣⠼⡀⠠⢑⠊⣅⠣⡘⡄⢣⠡⢃⠆⣍⠰⡡⢌⡱⠘⡤⢂⠍⣢⠑⡉⠆⡍⠢⠔⡉⠦⡁⢎⠰⡈⢆⠱⣈⠑⡂⢌⡹⠶⢤",
                    "⣛⠴⣙⢦⡛⢦⣙⢎⡳⢥⠳⣜⢣⡝⢦⡹⢜⡣⡝⢦⣓⡹⢦⡙⢦⣙⢣⡜⣣⢜⡣⡝⣿⣷⡨⢇⠞⡱⢎⣥⠀⠓⠬⡚⢥⠓⡴⡙⡤⠳⣌⢣⡙⢲⣉⢆⠳⣌⠲⡌⢆⠳⡌⡓⣌⢣⠓⡬⠑⣺⢇⠲⣁⠎⡥⡘⡌⢆⠣⣌⠣⡘⣌⠣⢃⡜⠌⣌⠣⠜⣐⠣⢩⠀⠈⠜⡠⢃⢒⡉⠆⡍⡜⢨⠄⡓⡰⡁⠦⡙⢄⣃⠚⡄⢣⢉⡜⢠⡙⢌⡑⢢⠑⡌⢢⠑⡌⠒⣄⠣⠜⡉⠄⢢⠀",
                    "⢮⡹⡜⢦⡹⢲⡍⢮⡱⣋⠳⣌⢧⡚⣥⡙⢮⡱⢎⡓⢦⢓⡣⡝⢦⡙⢦⡱⢎⡲⡱⢍⡚⡿⣷⣌⣈⠑⠩⣿⢹⢲⢦⡈⠊⡝⡰⢣⡱⢣⠜⣢⠙⢦⠸⣌⠳⢌⡓⠌⣌⠳⣌⠱⡌⢦⡙⠀⡼⢋⢆⠳⡌⡜⡰⠱⡘⠬⣑⠢⢣⢑⠢⢍⠲⣈⠎⠤⢣⠩⢄⠣⢃⡹⣄⠀⠥⡉⠆⣌⡑⢢⠌⡅⢎⠱⡐⡡⢃⡜⠄⢆⠣⡘⡄⢣⠘⠤⡘⠤⡘⢄⠣⡘⢄⢋⡘⢩⢀⠣⡙⣉⠃⠍⡉",
                    "⠶⣱⡙⢦⡙⢧⡚⣥⢳⢩⠳⡜⢦⡹⠴⣙⢦⡙⣎⢝⣊⠧⣓⢭⠲⣙⠦⢳⡡⢇⡳⠭⡜⣱⠪⣝⠻⠿⣷⣦⣤⣁⠊⠙⠶⣄⣁⣁⠊⠑⠎⡥⢋⢎⡱⢌⠳⣌⡈⡴⣈⠧⢌⡓⡜⢢⠜⢀⠣⣍⢊⠖⡱⢨⡑⢣⡉⢖⡡⢎⡑⢪⢑⢊⡱⠰⣈⠇⡱⢊⠬⡑⢊⡔⣠⠀⠰⢡⡉⡔⢌⠒⡌⠜⣠⠃⡥⢑⡌⡰⢉⢆⠱⣈⠜⠤⡉⢆⠱⢌⠒⡌⢢⠑⡌⢢⠘⡄⢊⠔⠡⡐⠌⡐⠀",
                    "⡝⢦⡙⢧⣙⠦⢙⢦⣋⢎⡳⣙⢦⡙⢧⡍⠶⣙⢬⡚⣌⠧⡓⢎⡳⣡⢏⡱⢎⠳⣌⢳⡡⢇⡳⢌⣝⡾⢏⡼⢩⢛⠿⢿⢶⣶⣤⣤⣤⣤⣄⣀⠉⠂⠱⢊⡕⢢⠌⡱⢌⡚⢤⠓⣌⢣⠂⢨⡑⠦⣉⠎⣅⠣⠜⡡⠜⣂⠖⢢⡉⢦⢉⢆⢒⠱⢂⠎⡱⣈⢒⢡⠣⠔⣂⣆⠀⢣⠐⡌⢢⠩⡐⠩⢄⠓⣐⠣⡐⠅⡎⠤⢃⠆⡚⠤⡉⢆⠱⣈⠒⡘⢄⠣⡘⢄⠣⡘⢄⠊⠥⠐⡐⠀⠁",
                    "⢺⢥⡛⡼⣘⠮⡀⠶⣩⢎⠵⡩⢖⡹⢦⣙⢣⡍⠶⣑⢎⢞⡩⢎⡵⡡⢞⡌⢧⠛⣌⢖⡩⢖⡩⢎⣿⡙⢲⢌⡓⣎⡙⢆⡣⢎⠉⡌⡍⢏⡛⣛⠻⣷⣦⣤⣤⣀⣀⡀⠑⠘⠢⠙⠤⢃⠆⠐⢬⠱⠌⢎⡰⣉⠎⡥⡙⢤⢊⠥⡘⠤⠣⡌⢊⠬⡑⠬⡑⠤⡉⢆⠱⢌⠰⡸⡄⠠⢣⠘⡄⠣⢌⡑⢊⠜⡠⢃⠜⡨⠔⡑⡊⡔⢡⠒⣡⠊⡔⢄⠣⡘⢄⠣⡘⢄⠣⢘⠠⣉⠂⠅⡐⠈⠀",
                    "⠳⣎⠵⣣⠝⣮⢡⡜⡱⢎⡳⣙⠬⣓⠼⣌⢣⠞⡱⣍⢎⠮⣑⠧⣒⠝⡦⡹⢌⡏⡜⢦⡙⠦⣽⡟⡥⢚⡥⢚⡜⡤⡙⠦⠵⢮⡰⡘⡬⢣⠜⡢⢝⡰⢌⠭⣙⠛⢿⠻⠿⢷⣶⣶⣶⣄⠀⠈⠦⡙⢜⢢⠱⢄⠳⡠⠓⡬⡘⢔⠩⡜⠡⡜⢡⢊⠅⡣⢌⡱⠘⠤⡉⢆⡱⢐⡓⠐⠢⡑⢌⡑⠢⢌⠡⢊⠔⡡⢊⠔⡡⠥⡑⢌⠢⠱⣀⠣⡘⢄⠣⡘⢄⠣⡘⢄⠊⡔⠂⠤⠘⠠⠀⠄⠁",
                    "⣛⢬⠳⣥⢛⡴⢋⡼⡜⣥⠳⣌⢳⡩⠖⢁⡇⣞⡱⡜⢪⡱⢍⠶⣉⠞⡴⣙⠼⣰⠙⡦⣹⢘⣾⡱⣑⠫⣔⢋⠰⢡⡍⢳⢩⠲⡽⠱⣌⢣⢚⡱⢊⠴⣉⠖⣡⢋⡜⢆⡉⢖⡰⢌⡩⢛⣇⠀⠀⡙⡔⣊⠱⢊⢆⡱⢩⡐⡱⢊⠱⣈⠕⣨⠢⣉⠜⡐⢆⠔⠈⡔⡑⢢⠐⢣⢰⠀⠡⡘⠤⢌⠱⡈⢆⡉⠆⠥⢃⡜⢠⠃⡜⢠⡉⢆⢡⠒⡡⢊⠔⡡⢊⠔⠡⠌⡂⠔⡉⠄⡁⠂⠡⠀⠂",
                    "⢎⣎⠳⡜⢮⡜⣣⢞⣹⣀⣛⣌⣃⠀⡞⡹⡜⣢⠕⣎⢇⡣⢏⡼⢡⣋⢖⡩⢖⣡⢛⠴⣡⠾⣃⠳⣌⠳⡌⢮⠱⣄⢊⠣⢎⣽⢇⠳⣌⢒⠣⡜⡡⠞⡤⢋⡔⢣⠜⣘⣭⠒⡜⢢⡑⢣⠜⣷⠀⠐⠰⡡⢣⢉⢆⡒⣡⢒⠡⢎⠱⡐⠪⢄⠓⠤⢊⠱⣈⠜⢠⠢⣁⢃⠚⡄⢆⠂⢀⠱⡈⢆⠱⡈⢆⡘⠬⡑⢢⠘⡄⢣⠘⠤⡘⢂⠦⡑⢄⠣⡘⢄⠣⠌⡑⢂⡁⠆⡐⠠⠀⠡⢀⠁⢂",
                    "⡭⢎⡳⣙⢦⡙⢦⣋⠶⣹⡏⣭⡙⢦⡈⠱⢥⣃⠏⣖⢪⡱⢃⡞⡱⡸⠬⣜⠪⠴⣩⢚⡔⢣⢣⠝⡤⡓⣍⡒⢧⡘⡍⠶⠴⡜⢊⠵⣈⢎⡱⢌⡱⣉⠖⡡⢎⡱⢊⡔⢺⠍⣈⠥⣊⠥⢚⡌⢷⡀⠀⠱⢌⡊⡔⢒⡰⢌⠣⡘⠤⣁⠓⠌⡌⢣⢉⠒⠄⡔⢃⠆⣡⢊⠱⡈⠆⠀⡜⠤⡉⢆⠱⡈⠄⣂⠢⣁⠣⡘⢄⠣⡘⠰⡁⢎⠰⡁⢎⠰⡁⠎⡠⢃⠘⠠⠐⠠⢀⠡⢈⠐⠠⡈⠄",
                    "⡜⣣⠳⣍⠶⣙⢦⡹⢜⡱⣚⠴⣩⠖⣱⣄⠱⢬⡙⢆⡳⣘⢣⡜⢥⠓⡭⡒⡍⢧⡱⢊⡜⣣⢣⢚⠥⡓⣤⡙⢦⢱⢊⠧⣙⠤⡈⠒⡥⢊⠖⣡⠒⡥⢊⡕⢪⠔⡣⢜⡡⢄⠉⠦⡡⢎⡡⠜⡌⢿⣦⡀⠀⠱⢌⡡⢒⠬⠡⢅⠣⢌⠡⢆⠘⡄⠊⢠⠊⡜⢠⠚⡄⢊⡔⠡⠀⢠⠁⢆⠱⡈⢆⡑⠂⣀⠣⢄⠣⡘⢄⠣⢌⠱⡈⢆⠱⢈⠆⡑⢌⠰⢁⠲⡈⢁⠌⡐⢀⠂⠰⢈⠡⠐⡈",
                    "⣱⢣⡛⣬⢚⡕⢮⡱⣋⠶⣉⠶⣡⢛⡔⢎⢦⡀⡝⣪⠴⡩⢖⡜⢣⠝⡲⢍⡼⢡⢎⠳⣘⡔⢣⢎⢣⢕⡢⣙⢆⢣⢚⡜⢤⢓⢀⡝⡰⣉⠞⡤⢋⡔⢣⠜⡡⢎⠱⢢⡑⠦⡀⠣⡑⠦⡑⣊⠜⡄⢛⢿⣧⣄⣀⠘⢢⢡⠋⢆⡉⢆⡑⢊⠄⢀⠖⡡⢊⠔⡡⢊⠔⡡⠌⠁⣠⠞⡁⢢⠑⡌⠢⡵⠆⠠⡑⢌⠢⡑⢌⠒⡌⢢⠑⡌⢢⠁⢎⠰⢈⠒⡠⢉⠉⠒⡤⡀⠂⠌⡁⢂⢂⠡⠐",
                    "⢎⠶⡹⣔⢫⢜⡣⣜⡱⢎⡵⢩⢖⡩⡜⢎⡲⢣⠀⢣⠞⡱⢎⡜⢣⡹⠜⣜⡰⢋⡬⣃⠧⣘⠇⡎⢖⠪⡔⢣⢎⡱⢊⡜⢢⠃⡈⢖⡱⢌⠲⣡⠣⢜⡡⢎⠱⡊⢥⠣⠜⣢⢁⠠⠑⠦⢱⡈⠦⣉⢒⠢⡙⢿⣿⣿⡀⠢⢉⠆⡜⢠⠜⡨⢘⠀⡘⢄⠣⡘⢄⠣⡘⡐⠀⣸⠋⡔⡁⢂⠱⣈⠱⡐⢢⠀⡘⢄⠣⡘⢄⠣⡘⢄⠣⡘⠤⢉⠢⡘⠄⢣⠐⢄⡈⠅⡄⠱⣀⠐⠠⢁⠂⡄⠃",
                    "⢭⡚⠵⣌⢳⠪⡕⢎⠶⣉⠶⣩⢆⢳⡸⠱⣌⠧⢣⡀⢫⠔⣣⢚⠥⡚⡍⢦⠱⣋⡔⢣⡓⢬⠚⡜⡬⢓⡜⡡⢎⡔⢫⢌⠣⣍⠃⠰⢘⡌⢣⠆⡅⢠⠓⣌⠣⢍⢢⢃⠳⢄⡋⠕⣄⠁⢣⠘⡔⢡⠊⡅⢣⠆⡜⢻⣿⣦⣄⠊⠔⡡⢊⠔⡡⠀⠰⡈⢆⠱⡈⢆⠱⡀⠀⡁⢎⠰⡀⢌⠰⢀⠣⡘⠄⣓⠀⠌⡂⢅⠊⡔⢡⠊⠔⡡⠘⡄⠣⡐⢩⠀⠎⢠⠐⣂⠰⠁⡄⠣⡀⠂⢂⠤⠁",
                    "⢣⡝⢳⡌⢧⡛⢼⣉⢮⠱⣍⠖⣊⠧⡜⠳⣌⡚⢥⢛⣄⠪⡑⢎⡱⢣⠹⣌⠳⡔⢪⡕⡸⢡⢫⠜⣰⠣⡜⡱⢊⡜⡡⢎⡱⢌⡍⢦⠀⢈⠇⡚⡌⢆⢩⢂⡍⠎⡴⢉⢆⠣⢌⢃⠦⡡⢄⠉⠘⢤⡁⠚⡄⢎⢦⢃⡘⠻⣿⣧⣄⡐⠡⢊⠔⡂⠡⡘⢄⠣⡘⢄⠃⠴⣦⣄⡀⠥⢸⠀⠈⢆⠱⣈⠒⠤⡄⠐⢡⠊⡔⡈⠆⠱⡈⠔⡡⠘⡄⢑⢂⠡⠊⢄⠒⣀⠢⢁⡐⠄⠩⣄⡀⠂⠂",
                    "⢣⠞⡱⢎⠳⡜⣣⠜⡦⢛⡬⢚⡱⢎⡱⢋⡴⢩⠖⣩⠚⠷⠉⢎⠴⣉⠳⣌⠳⣘⢥⢊⡕⢣⠎⡜⣡⠓⡬⣑⠣⡜⡱⢊⡔⢣⠜⣢⠀⠨⣘⠱⡘⣚⣤⢇⠸⡘⠤⢋⠤⣉⠎⡔⢢⡑⢪⠙⡲⠀⠈⠥⣈⢂⡸⡇⡘⠠⡙⣿⣿⣿⣦⣄⠈⠁⠐⢡⠊⡔⢡⠊⡔⢢⠀⡝⢻⣷⡁⠐⠉⠆⡱⢀⠍⠒⠈⣠⡂⠱⢠⠡⡁⢢⠑⡌⠰⠡⠌⢂⠂⠥⠉⢦⠒⡀⠆⠠⢐⠈⠔⡀⢉⠙⠒",
                    "⢣⢫⢕⡫⡱⢍⡲⢹⡌⡳⣘⢣⡕⢮⠱⣉⠶⣡⠚⡥⢋⡝⢦⠈⢲⡉⢶⣈⠳⡌⢆⡫⢜⡡⢎⡱⢢⠝⡰⣡⠓⡬⣑⠣⡜⣡⠚⡤⢓⡀⠐⢣⠦⣐⠸⡃⢑⠰⣁⠫⠔⡌⠲⡘⢔⢨⠡⢎⢡⡉⠦⡀⠌⡱⢈⠇⠀⢑⣼⠟⡌⣉⠻⠿⣷⣶⣤⣀⡈⠐⠡⢊⠔⡹⡬⢠⣁⡙⣷⣌⠈⠐⡄⠣⠌⠁⡘⢉⠅⡃⠆⡡⠳⢠⠡⠌⢡⠑⡌⢂⠅⠢⣁⡬⡅⠐⡈⠄⠃⠌⡐⢀⠂⠌⠀",
                    "⢣⣋⠦⣓⠭⢎⡱⣃⢞⡡⡝⢲⡘⢎⡵⣉⠶⣡⢋⠴⣋⠜⣌⢳⡤⠙⠦⣌⠳⡘⣥⠚⡴⢡⠓⣌⠣⢎⡱⢢⡙⠴⣡⠓⡸⢄⠫⡔⢃⠶⡈⠒⡘⡄⣰⠗⠌⢓⡼⠰⣉⠔⢣⠑⣊⠤⠓⡌⢢⠘⣄⠫⣄⣡⡤⠶⢒⣋⠱⠈⡔⢠⢃⠒⡌⠉⡙⢻⢿⣿⣦⣀⡀⠐⠠⠃⠤⢡⠈⢻⣦⡈⠀⠃⠂⠔⡁⠎⠰⢡⠘⠠⢳⢰⠆⠉⡔⠨⠐⡌⢨⠐⠤⡄⠃⠌⡐⢈⠂⠡⠐⠀⠈⠀⠀",
                    "⢇⡎⣱⠎⣱⠏⣰⠇⡎⡶⢉⠇⡾⢁⡶⢁⡾⣀⠏⡶⢁⡾⡈⢶⢷⡈⠷⡈⢇⢱⢆⠹⡰⢇⠹⣀⠏⣆⠱⢇⡸⠱⣆⠹⣰⠉⡶⢉⡸⢰⠹⣆⠰⢱⢈⡁⠉⠇⣿⠱⢰⠎⡁⠾⡀⡎⢱⠈⡆⣉⠰⠆⡶⢀⠆⢰⠀⡈⠉⠷⠶⣀⡈⠶⠀⣀⠱⢆⡈⡈⠹⢿⣿⣶⣀⣀⠀⠀⠀⢆⢸⣿⣶⠰⠎⡰⢁⠎⢁⠆⡉⢆⠉⡎⡀⢁⠸⡀⢇⠰⢁⠈⡸⠇⠈⠰⠈⡀⡈⠁⠰⠈⠀⠀⠁",
                    "⢎⠖⡥⡛⣤⠛⣤⠛⣜⠰⣋⡜⢲⡉⣖⢩⠒⣥⠚⡌⠳⡰⢩⠒⠌⣁⠀⠙⠌⡲⢌⢣⠱⣊⠵⣈⠞⡤⢋⠦⣑⠣⢆⡓⠤⣓⡘⠆⡥⢃⠎⡔⠦⠀⠣⠜⣢⡈⢿⡐⢣⠘⡌⣑⠒⣌⠢⡑⢒⠄⡃⢎⠰⣁⠒⡢⢅⠈⡱⢈⠒⠤⡙⠂⢘⠡⢊⠔⡠⢁⠃⠆⡌⡙⠛⠿⣿⣶⣤⣄⣈⠉⠛⠷⠀⠑⡠⢊⠔⡈⠔⢨⡐⡤⠑⠌⢂⠔⢂⠡⢂⠡⠐⡈⠤⡁⠂⠡⠀⠡⠀⠀⠐⠀⡀",
                    "⣤⣭⣄⣑⠂⠛⠤⢛⡌⠳⡤⡙⢦⠱⣊⠥⣋⠴⣉⢖⠡⠃⣡⣴⠿⡛⢟⠶⣦⣄⠈⠂⠳⢌⠲⣡⠚⠴⣉⠲⣡⢙⠢⢍⠲⡄⠳⡘⠴⡉⡜⠰⣉⢦⠈⡱⠄⢎⣛⠀⢃⢣⠘⡤⢃⡄⢣⠘⡌⡘⡰⢡⠒⡄⠃⡔⡉⠧⠐⠡⠊⠔⠡⠀⠠⠑⠌⠂⡑⠌⡘⠰⢠⠡⢉⠂⠔⡀⠝⠛⠻⠿⢶⣶⣤⣄⡀⠈⠀⠉⠑⠂⢦⢥⡉⠌⢂⠌⢂⢂⡁⢂⠡⠀⠄⡉⠦⡀⠁⠀⡀⠌⠀⠄⠀",
                    "⢆⡳⠜⡭⣛⠿⠶⠶⡶⢶⢷⣭⣂⡉⠂⠳⠌⠲⢁⣪⣴⠿⣋⢇⢣⡙⣌⠚⡤⢋⠟⣶⣤⣈⠁⠂⠙⠦⣡⠓⡤⢋⡜⢂⠧⡘⢥⡑⢎⡰⢉⡕⢂⠎⢧⠐⡉⢆⢢⠱⢄⠂⠃⢲⢡⠘⡄⢣⢐⢡⠂⡅⢊⠄⠒⠀⠀⢀⣠⣤⣴⣤⣤⣄⣄⣤⣀⣀⣀⣀⣀⣀⣀⣀⣀⣀⣴⣶⣤⣤⣤⣤⣤⣼⣿⣿⣿⣷⣶⣤⠀⠀⠀⠀⠈⠁⠀⠈⠀⠀⠀⠀⠐⠰⠆⠀⢀⣑⠀⠀⠀⡀⠄⠀⠂",
                    "⡚⣬⢓⡱⣌⡚⣍⠳⣌⠇⢮⡐⢏⡛⠷⡶⣭⣶⡿⢛⠶⣌⣐⠊⠦⡱⢌⠳⣌⢣⢚⡰⢌⡛⡛⠷⣦⣀⠀⠙⡰⢃⡜⢌⠲⡉⢆⠜⡰⢂⢇⠸⣈⠚⡌⢷⡌⢂⠆⡃⢎⠱⠀⠼⠢⠑⡌⠢⠌⠂⠑⠈⠀⢰⣤⣠⣾⡿⠟⢛⡙⢍⠫⢹⠙⠛⢛⠛⠛⠿⠻⠿⠿⠿⠿⠿⠟⢿⡛⠛⣋⠉⢋⡙⣉⠛⠛⠻⢿⣿⣿⣶⣴⣶⣶⣤⣤⣶⣤⣤⣀⣤⡀⠀⠀⠀⠀⠉⠑⠀⠀⠀⠀⠂⠄",
                    "⡱⠦⡍⡖⡜⠴⣊⠵⣌⡚⢥⢪⡑⢮⠱⣜⣾⣏⠲⣉⠖⣡⠎⡝⡲⢤⣉⠐⠤⠃⢎⠴⠐⣡⠙⡜⠤⡛⢷⣤⡀⢣⠘⣌⠲⣉⠦⣉⠦⡉⢆⠣⠜⡰⢡⢂⡛⣦⡈⠁⠀⠂⠀⠀⢀⢀⣀⣀⣶⣄⣤⣴⣶⠿⢛⠋⡅⢒⡘⢃⠰⡈⠆⣡⠹⠌⣂⢉⠚⠦⡁⠘⡐⢂⠅⠒⢌⡘⠓⠡⡀⠀⢂⠰⢀⠌⡁⠣⠄⡐⠨⢉⠛⠻⠛⠛⠿⠿⠿⠿⢿⢿⣿⣶⣶⣄⣠⠀⠀⠀⠀⠀⠀⡀⠀",
                    "⡱⢓⡜⣱⢊⠗⣌⠳⢤⢋⠖⣡⠚⢰⣩⡾⢏⢆⣣⠱⡚⢤⢋⠴⣡⢃⠎⣍⢋⢖⣲⡲⢦⡤⢤⣉⣒⠉⠂⠹⢷⣄⣡⡂⠃⠖⡰⢂⠖⣉⠆⣉⠦⡑⠌⢂⡨⠔⠀⠀⣤⣶⣿⣿⡿⢿⠻⠟⠿⣯⣍⡁⠐⠈⠆⠱⡈⠍⠄⠠⢃⠰⠡⠀⢺⡚⠠⠌⡘⡐⠌⡑⠎⢤⣊⣨⢄⣐⣂⢀⢁⠎⠄⡃⠌⡐⠤⠅⣒⠈⠐⠡⠈⡤⠡⡀⠂⡐⢀⠂⠄⠂⠄⠉⠙⠻⠿⣿⣷⣿⣦⣴⣴⣷⣆",
                    "⡱⢋⡴⢃⠮⣙⠬⡙⠦⣹⣾⠗⣞⠻⣍⢚⢬⠒⡤⢓⡍⢦⡉⢖⡡⢎⡜⠤⣋⠲⣄⢣⢃⠟⡤⠣⢍⣋⢛⡙⣋⠻⣿⣏⠑⠶⣤⣬⣀⣈⣀⡡⠤⠔⠂⢉⣀⣤⣶⡿⠛⡍⢩⠐⣐⠢⢡⡈⠰⢠⢉⠛⠷⣶⣦⣄⣀⣠⣄⣁⣈⣀⡁⠀⠠⠙⡄⠱⠠⡁⠎⣀⠃⡂⠔⣀⠢⠐⣀⠛⢠⡌⡐⠤⠑⡠⠒⡈⠌⣉⠩⢉⠙⡐⠎⠘⣄⠐⠠⠈⠄⠡⠀⢀⠁⠂⡀⠀⠈⠉⠛⠻⠿⢿⣿",
                    "⡱⢋⡴⢋⢖⡩⢺⡗⠻⡍⢆⡫⢔⠳⡌⢎⢆⠫⡔⣋⠜⡤⡙⢦⠑⣎⠰⢣⢌⠱⣂⠎⡬⡘⢤⡓⢬⡐⡊⠴⢡⠒⡌⣻⣷⣄⣀⣀⣀⣀⣤⣠⣴⣶⡿⠿⣛⣭⠿⡐⢩⢐⠡⢊⠔⠈⢂⠜⢄⠐⢨⠘⡄⢃⡤⠈⢍⠩⣉⠏⡙⢛⠻⣦⡀⠁⠘⢲⢃⠌⡰⢀⠱⠐⡈⠄⣂⠱⢀⠂⡘⣷⢀⠊⢄⢁⢂⢁⠢⠄⢂⠁⠆⠰⠈⠖⡚⠆⢀⠁⠌⠀⢂⠠⠈⠐⠀⡁⢈⠀⡐⠀⠠⠀⠀",
                    "⠱⣉⠖⡭⢢⢓⣹⡷⡵⣌⠣⡜⢪⠱⡜⡌⢎⡱⡘⢤⢋⠴⣉⠦⡙⠤⡋⠖⣌⠲⢬⡜⡰⢉⠼⣑⠢⢅⠣⡅⢣⠚⣴⣿⡿⢛⣿⣛⠛⢛⡛⠭⡉⣦⣸⡴⢏⠂⢆⡑⢢⢁⠎⢢⠘⡰⢈⠨⡂⠢⣀⠃⡘⠤⣈⡭⢄⣂⡡⠈⠔⡁⠆⡌⠳⣤⣀⠀⠙⣀⡐⠨⡀⢃⠌⡰⢀⠢⠁⠞⡀⢻⡄⡈⢂⠄⠊⠄⠂⠌⡐⠈⠌⡠⠑⠠⢀⠱⣈⠂⠐⠁⠠⠀⠌⠠⠁⠠⠀⠠⠀⠄⠁⠠⠁",
                    "⢣⡒⡬⣤⠽⣞⠫⠴⣡⢋⠟⣚⠧⣗⡸⡘⠦⡑⡍⢦⡉⢖⡡⢎⡑⢣⠱⡩⢄⡋⢆⡍⢣⣙⠧⢂⠭⡘⢢⠉⢂⣽⣿⠛⠤⢋⠤⡑⢏⠓⡚⢒⠻⣅⠣⢰⠈⡜⠠⠜⡠⢊⠜⡠⢃⠔⡡⢂⠍⠱⠘⢣⣀⠘⣉⠐⡂⠔⡨⠙⣕⣌⠰⢀⠃⡌⠛⢿⣶⣁⠁⠢⣀⠁⠢⠐⢂⡐⠨⠐⡐⠠⢻⣴⠠⠈⠜⠠⢉⠐⠄⡉⠐⠠⢁⠂⠄⢂⠈⠣⣌⡑⠦⠐⠀⠂⠄⠁⠠⠐⠀⡐⠈⠀⡀",
                    "⢣⠜⡱⢌⠳⣌⠳⣉⠦⣍⠚⡤⢓⠤⢣⡙⠴⡱⡘⢦⢉⠦⡑⠎⣌⠣⡱⡘⢢⡑⠦⡘⠴⡐⡌⢎⠢⡑⢅⣼⣿⡿⠠⣉⠒⡉⢆⢡⡋⠆⣡⠣⠜⡈⠆⡅⢎⠰⡉⢆⠱⡈⢆⡑⢨⠐⡄⠣⠌⡡⢉⠆⡙⢆⠠⠑⡨⢐⠡⣁⠂⡙⠶⣈⠒⠶⠕⡒⡉⠹⢿⣆⡈⢀⠒⢤⡀⠰⢂⠡⢈⠄⠹⣿⠀⢡⠈⠒⠠⢈⠐⠠⢉⠐⠠⠈⡐⠠⠈⠄⢀⠉⠳⠲⠶⠶⠶⢶⡁⠄⠂⠀⠀⡁⠀",
                    "⢣⢚⡱⢊⡵⣈⠳⡌⠖⣌⠳⢌⠣⢎⡱⢌⠳⢰⡉⢦⢉⠖⡩⢜⡠⢣⢅⢃⠇⡜⢢⠙⡤⠓⢦⢊⠱⣨⣾⡿⢡⢂⠵⣀⠣⢲⣐⠣⡏⠒⡄⢣⠘⡰⢡⠘⡄⢣⠘⡄⠣⡐⢂⠌⢢⠑⡈⢆⠱⢀⠣⡈⠔⠉⣣⣀⣁⠂⠁⠄⠊⡐⠰⢈⠳⢄⣐⠠⠱⠂⠤⠹⣷⡄⣢⡄⠀⣀⣀⣦⠈⠄⡁⢯⣌⠀⢈⠐⡁⠂⠌⡐⠠⠈⠄⡁⠄⠂⢁⠈⠀⠄⢠⣀⠀⠠⠀⣴⠟⠛⠛⠓⠦⣤⣀",
                    "⢣⢃⠮⡑⢦⢡⠓⣌⠳⡌⠳⣌⠳⢌⠦⣉⠎⡥⡘⢆⡍⡚⠤⠣⢜⢂⢎⡸⢌⠒⣡⢃⡒⡩⢜⢎⣾⡿⢋⠔⠣⢌⠒⡄⢣⣹⠡⡑⠢⢝⣂⠁⢎⠰⡁⢎⠰⣁⠒⡨⠑⡌⢢⠘⠤⡑⢈⠔⡈⠆⢑⣨⡴⠞⡉⡙⢬⡉⠛⠛⠲⢦⣤⣀⣢⡌⠈⠙⠻⡐⠀⠁⠘⣻⣿⣽⣀⠈⠀⢉⠓⠶⢦⡈⢻⡄⠀⠂⠄⡁⠂⠄⠡⢈⠐⠀⠄⠁⠂⠈⢀⠂⡀⣷⠀⠂⣱⠏⠀⠠⠀⠂⠀⠀⡿",
                    "⣃⢎⡱⢩⢆⢣⠹⣄⠳⣘⠱⠢⡍⢆⠳⢌⡚⡰⣉⠆⢦⠩⠜⣑⢊⡌⠆⡔⡊⠎⠠⠃⠴⣑⢪⣿⡟⢡⢊⢌⠃⠎⡬⢔⣁⠞⡡⠄⠐⡂⢌⡙⢢⣄⣁⡊⢔⣤⣇⣠⣁⣈⣀⣉⣐⣈⣂⣬⡴⠞⡋⢅⡐⠢⡐⢁⠂⡙⠲⢬⣅⡂⣀⠌⠉⠛⠛⠛⠛⢿⠛⠛⠋⡍⣹⣿⣯⣄⠀⠀⠂⣰⣿⠟⠙⠻⣤⣄⣀⣀⠁⠈⠀⠂⠠⠁⠈⠀⠂⡁⢀⣆⣤⣼⠁⢸⡏⠀⠠⠁⠠⡔⠬⠞⠁",
                    "⡌⢦⡑⢣⢎⠲⡱⢌⡱⢌⡱⠣⡜⣌⠣⢎⠴⣁⠦⡙⣌⠲⣉⠔⠊⠴⣉⣦⠽⢛⡙⢛⠛⢹⣿⡟⠢⢄⡈⠢⢉⠎⡐⢆⡘⠤⡑⢌⢢⠘⠠⠌⣽⠠⢡⢉⠍⡩⢄⠡⠌⡌⢡⠁⠎⢡⠁⢆⡐⠢⢁⠂⠤⠁⠔⡈⠔⠠⢁⠂⢄⠩⢁⠊⡉⢉⠩⠉⠍⡉⠳⠶⣶⠾⣻⠛⠻⣟⣦⣀⠀⠛⠁⠠⢈⢐⡨⠀⠉⡉⠛⠛⠛⠛⠻⠷⠶⣶⠶⢶⣤⣤⣤⣦⣠⠟⠀⠀⠄⠐⠀⠀⡀⠀⠀",
                    "⠜⣢⠙⡆⢎⡱⡑⢎⡔⠣⡜⡑⠲⣄⢋⠖⡸⢠⠣⣑⢢⠱⠌⣍⠛⣶⠫⠔⡊⠴⡘⠌⣲⣿⠟⡠⢍⢢⠩⣑⠲⢤⣁⣂⠘⠤⠑⡌⠢⢄⠑⡘⢻⣀⠣⢌⠂⠥⢈⠆⡑⠨⠄⠊⠜⢠⠘⡠⠐⣁⠢⠉⡄⠩⢐⠈⠄⢃⠂⢌⠀⠆⠠⠒⠠⢁⠂⡁⠂⠄⣁⡼⠏⢉⠁⡐⠠⠸⣿⣿⣷⣄⠀⠡⠚⠉⡀⣀⠡⢀⠠⠀⢂⠁⣢⠰⠞⠁⠀⡀⢀⣤⡴⠋⠙⢆⡀⠀⠀⠄⠂⠁⠀⠀⠁",
                    "⢍⠦⡙⢤⠊⠴⣉⠖⣨⠓⢬⡑⢣⠌⢎⠬⣑⢡⠣⡘⣄⢋⡜⢠⠣⣘⠳⠭⠐⠱⣨⣾⡿⡉⢆⠱⡈⢆⠱⢠⠃⢆⠰⡈⠝⡒⠦⢤⣁⡂⠀⡈⢍⢲⡁⠆⢩⠐⢡⢂⠱⢈⠌⡁⠎⢠⠡⠐⢡⠠⢄⢃⠐⡡⠀⠎⠄⠥⣈⠠⠌⠠⠑⡈⠐⠄⠂⢡⣬⠶⠋⡐⠈⠄⠂⠔⠒⠊⡈⢷⣿⢿⣧⣀⠀⠀⠤⠞⢊⠉⠁⠐⠠⠘⣁⣀⣀⣂⡰⠴⠋⠁⢀⠠⠀⠀⠑⢦⡀⠀⠀⠐⠈⠀⠀",
                    "⢎⡒⡍⢆⠙⢦⡑⠎⡴⢉⠖⣨⠑⡎⠜⠰⢈⢂⣁⣁⣈⣂⣈⣂⣡⣤⣶⠶⢟⠻⢻⠑⢆⠱⡈⢆⠱⡈⡜⢠⠉⢆⠱⡈⢆⠱⡈⠆⡌⢉⣳⠀⠌⠰⡐⢌⢂⠩⠄⠢⠌⢂⠌⡰⢈⠂⡔⠡⢂⠰⣀⣺⠓⢊⠢⢌⠠⠁⡄⠩⡒⠡⡖⢙⠒⠒⣽⠋⠀⠄⠂⠄⠡⢈⠐⠀⠂⠡⢐⣾⠏⠑⠲⢭⣧⣀⡄⠀⠀⠬⠓⠬⣉⠉⠀⠄⠀⠀⠀⠒⠀⠈⠀⠀⠄⠂⠀⠀⠑⢤⠀⠀⠀⠂⠉",
                    "⡒⡱⢒⡛⣋⠖⡤⢭⣐⠉⣘⣄⣹⣤⠿⣛⠫⢍⡋⢭⠩⢩⠍⠭⣉⠱⠠⢎⠢⣑⠊⡜⡈⢆⠱⡈⢆⠱⡈⢆⡉⠆⡱⠈⢆⠡⡘⠰⡈⢅⠢⢄⠀⠡⢒⠈⡄⠲⢥⣃⢘⠠⢂⣭⠴⠒⠬⢌⣈⣁⡀⠡⢈⠄⢂⠄⡑⠦⣀⠡⡘⠒⡉⠡⢈⠐⡼⠁⠌⠠⠁⠌⡐⠠⢈⠠⢈⠰⣿⠋⡀⠌⡀⠘⠀⠉⠛⢶⣜⡂⠀⠀⠈⠙⣀⣠⠴⠞⠛⠉⠛⢦⡀⠀⠠⠀⠁⡀⠂⠀⠱⢄⠀⠀⠀",
                    "⣡⢃⠧⡘⢤⡙⢤⠣⢌⣿⡏⠭⡑⢢⡑⢆⡩⠒⣌⠢⡑⢣⠘⡒⢌⡡⠣⢌⠒⡄⢃⠦⢉⠆⡱⢈⠆⡱⢐⠢⡘⠤⠡⡉⠆⠱⣈⠑⡰⢈⠔⢂⠣⡀⠂⠅⢢⠁⠆⢌⡉⠋⠍⣀⠂⢡⠈⢄⠠⢀⠌⡑⠒⢦⣒⡂⢔⠠⢀⠹⡇⠐⠠⢁⠂⠄⠠⠈⠄⠡⢈⠐⢀⠂⣀⠐⣠⣾⠟⠀⠄⠂⡐⠈⠐⠈⡀⠂⠈⠙⠳⢶⣶⣋⠁⠈⠂⠀⠀⠄⠂⠀⢳⣄⠀⠀⠂⠀⢀⠂⠀⠈⠓⣄⠀",
                    "⡐⢎⠲⣉⠖⣘⠢⡍⢲⡿⢘⢢⠙⡤⢊⢆⡡⠍⡤⠓⡌⡡⢊⡑⢢⡐⢣⠘⠢⢌⠡⢊⠔⡊⠔⡡⢊⠔⡡⢂⡑⠢⠑⠤⢉⠒⠆⠱⡐⠨⡐⠌⠰⠙⢆⡈⠀⠎⡈⠤⠠⢉⠰⠠⢈⠄⠌⠠⠒⡈⠐⠠⢁⠂⠆⡉⠓⠞⢤⡈⣗⠈⡐⢀⠂⠌⠠⢁⠈⡐⠠⢈⠠⠐⠈⠛⠉⠁⡀⢂⠈⠠⠀⠌⠀⡁⢀⠘⡂⠁⣀⣿⠏⠙⠳⣶⣤⣀⡀⢠⣤⡐⠀⠘⠷⣤⠀⠈⠀⠀⠐⠀⠀⠈⠓",
                    "⡑⢎⡱⠌⡜⠤⡓⢌⢳⣿⢈⠆⠭⡐⠥⣂⢒⠩⡐⠱⣈⠴⠡⢌⡡⠘⠤⡉⠜⡠⢃⠌⢢⠑⡌⠰⣁⠒⡄⢃⠌⡡⢉⢄⣃⡸⣁⠆⠡⡑⢠⠉⡂⢅⠊⡙⠦⡀⠐⠠⢃⠐⢂⢁⠂⠌⣈⠐⠡⢀⠡⢁⠢⠘⡂⠄⠡⠈⠄⡈⢲⢤⡐⠠⢈⠠⠁⡀⢂⠐⡀⣂⢠⠈⡐⠈⠠⢁⠠⠀⠂⠁⢀⠂⠁⠀⠄⠀⡇⡐⣼⠟⠀⠠⠀⠀⢉⠙⢻⣿⣿⣿⣶⣦⣤⣾⣷⡀⠀⠀⠀⠀⠀⢀⣀",
                    "⡘⢆⡱⢊⡜⢢⢅⠈⣎⡿⠾⣬⣑⣬⣑⣐⢊⡥⠔⡃⠆⠬⡑⢢⢐⡉⢆⠱⡈⠔⡡⢊⠄⠣⢌⠡⡐⢌⠰⢁⢊⠰⢈⠤⠐⡠⠳⢈⠒⠐⣄⠡⠐⢂⠂⡔⠡⠙⢦⠀⢂⢁⠂⠄⢊⠐⠠⢈⠐⡀⠂⠄⡈⢣⡃⠈⠤⠁⡐⢀⢢⣄⡿⠦⣄⡀⠂⠐⡀⠂⡀⢀⠈⠑⢄⠀⠡⠀⡀⠂⠁⠀⢀⠠⠈⢀⠂⠀⣁⣿⠋⠀⢀⠐⠀⠁⠈⠄⠀⠱⣎⠉⠛⠻⠿⢿⢿⣿⣷⣿⣶⣿⣿⣿⡿",
                    "⠘⢆⠱⢊⠔⣡⠮⠽⢚⠰⢡⢂⠬⡐⢡⠊⡔⢨⠒⡉⠜⣠⠑⡂⢆⡘⢄⠣⠘⠤⡑⢨⠘⡰⢈⠂⡅⢊⠔⣈⠂⢅⠂⢆⠱⠠⡑⠆⠲⠌⠜⡉⠢⢀⠡⠄⢡⠈⠌⠳⣄⠠⢈⠐⡀⢊⠐⡀⠂⠌⡐⠠⠐⠠⠘⠮⣰⣖⣠⣀⡦⠏⢀⡠⠀⠉⠓⢦⣴⡀⠐⠀⡈⢀⠈⠁⠃⠐⠀⠠⠉⠀⠀⢀⠐⠀⠑⣰⡿⢟⡄⠀⡀⠀⠂⠈⠐⠐⠒⠀⢻⣇⠀⢀⠀⠠⢄⠀⠀⠀⠀⠀⠀⠀⠐",
                    "⡙⣌⠋⡍⢎⠱⢌⡑⣊⠱⠌⡌⢢⠉⢆⠱⡈⢆⠡⣉⠒⡄⢣⠘⠤⡘⢄⢊⡑⠢⢁⠆⡱⠐⠌⢒⡈⢄⠊⢄⠊⡄⠩⡀⠆⣁⠂⠹⡀⣩⣖⠀⠡⢁⠠⠈⠄⠨⠐⠡⠙⣷⡄⠂⡐⢀⠂⠄⡁⠂⠄⡁⠂⡁⠌⠀⠄⡀⠠⠈⠙⣮⠁⠀⠠⠁⠄⡀⠈⠙⢷⡀⠠⢀⠀⢹⡆⠀⢁⠠⠐⠀⠐⠀⠀⠂⣼⡟⠁⠀⠈⠑⠢⠤⣀⡁⠀⠁⠀⠀⠸⣯⠀⠀⠠⠀⠀⠲⡀⢦⠀⠂⠀⠄⠀"
                };

                std::cout << "\n\n";
                Menu::imprimirLinhasCentralizadasNaTela(arteFloresta, 140, "\033[32m");

                std::string textoDeViagem = "Viajando para a Floresta...";
                int larguraDoTerminal = Menu::obterLarguraDoTerminalEmColunas();
                int espacosParaCentralizarTexto = (larguraDoTerminal - (int)textoDeViagem.length()) / 2;
                
                std::cout << "\n\n" << std::string(espacosParaCentralizarTexto > 0 ? espacosParaCentralizarTexto : 0, ' ') << textoDeViagem << "\n";
                
                Sleep(3000); // Dá um tempo para o jogador admirar a arte e ler a mensagem

                // Instancia e inicia o mapa da Floresta
                Mapa2Floresta mapaFloresta(jogadorAtual);
                mapaFloresta.iniciarLoopDeExploracaoDoMapa();
                
                // Quando a exploracao da floresta terminar (o jogador usar a saida ^V), o jogo continuara na Vila
                // Respawn dos inimigos da Vila e recarregamento do mapa
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
                cavernaJaFoiVisitada = false;

                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
                linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
                continue;
            }
            else 
            {
                posicaoXDoJogador = proximaPosicaoX;
                posicaoYDoJogador = proximaPosicaoY;
            }
        }
    }
}
