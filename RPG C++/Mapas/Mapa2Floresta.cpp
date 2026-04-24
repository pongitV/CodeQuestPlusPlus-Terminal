#include <iostream>
#include <vector>
#include <conio.h> 
#include <windows.h> 
#include <cstdlib>
#include <memory>
#include <utility>

#include "Mapa2Floresta.h"
#include "../Sistema/Menu.h"
#include "../Inventario/Item.h"
#include "../Inventario/Armadura.h"
#include "../Inventario/ItemConsumivel.h"
#include "../Inventario/Material.h"
#include "../Sistema/GeradorInimigos.h"
#include "../Inventario/InventarioCombate.h"
#include "../Interfaces/TelaAtributos.h"
#include "../Sistema/SistemaRPG.h"
#include "../Inimigos/RacaFada.h"
#include "../Inimigos/ClasseInimigoPadrao.h"
#include "../NPCs/NPCMorgana.h"
#include "../Inimigos/RacaAbominacaoFloresta.h"
#include "TransicaoDeMapa.h"

extern bool g_labirintoDesbloqueado;

Mapa2Floresta::Mapa2Floresta(Personagem* personagemJogador) : 
    jogadorAtual(personagemJogador), posicaoXDoJogador(8), posicaoYDoJogador(8),
    jogadorEstaDentroDeUmSubMapa(false),
    posicaoXSalvaAntesDeEntrarNoSubMapa(0), posicaoYSalvaAntesDeEntrarNoSubMapa(0)
{
    matrizDoMapaAtual = 
    {   " #########################################################################################################         ",
        "###############################################################################################################    ",
        "###### #..............**.................###.......##...........**.....######.........####......[^Cabana]##########",
        "###.[^V].............****......................................****......................**.....[;;   ;;].....#####",
        "###...................||...................S....................||......................****....| |######.....#####",
        "####..................||........................................||.......................||..................######",
        "####............................................................................S........||..................##### ",
        "###.....................................**...................................................................####  ",
        "###....................................****................S................................................###### ",
        "#####.......................S...........||....................................................**............#####  ",
        " #####..................................||...................................................****...........#####  ",
        " #####........................................................................................||...........#####   ",
        "####.................**............................................**.........................||............#####  ",
        "####................****........................S.................****.......................................####  ",
        "####.................||............................................||...............S........................###   ",
        " ###.................||...........S................................||.........................................###  ",
        " ###................................................................................................S.........#### ",
        " ##.......................................................................**.................................####  ",
        "###......................................................................****...............................###    ",
        "###....................................**..................S..............||................................###    ",
        "##....................S...............****................................||................................###    ",
        "###....................................||....................................................F.......F.....#####   ",
        "####...................................||...............................................F.......**...........###  ",
        "###............................................................................................****...F......###   ",
        "##................................................................**..................F.........||..........####   ",
        "##...............................................................****......................F....||.......F..####   ",
        "###...............................................................||...........F..................:--........##    ",
        " #####............................................................||........................--=-+==-+--..--..##    ",
        " ####................................................................................F....:-=-====--==--+=-..###   ",
        "####........................................................................F.............-+-====-------+-=-#####  ",
        "###...................**.................................................................:++==++++++--=-**-..####  ",
        "#####................****.............................**............................F....:=++-*##=##%#***#-...#### ",
        "####..................||.............................****.................F..................::%***#+-......#####  ",
        "####..................||..............................||.................................F......-*+*.......#####   ",
        "###...................................................||..........................F.............%+*........#####   ",
        " ###......................................................................F.....................**-.......#####    ",
        " ######.............#######.......##############......................................F.......:%^T**.....######    ",
        "  ##########################################################################################################       ", 
        "       ###############################################################################################             "
    };
}

void Mapa2Floresta::iniciarLoopDeExploracaoDoMapa() 
{
    static std::vector<std::string> matrizDoMapaDaCabanaSalva;
    static std::vector<std::string> matrizDoMapaDoLabirintoSalva;
    static std::vector<std::string> matrizDoMapaDoCoracaoDaArvoreSalva;
    static bool cabanaJaFoiVisitada = false;
    static bool coracaoDaArvoreJaFoiVisitado = false;
    static bool labirintoJaFoiVisitado = false;
    bool exploracaoEstaAtiva = true;
    std::string tituloDoMapaAtual = "FLORESTA";
    
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

    while (exploracaoEstaAtiva && jogadorAtual->obterVida() > 0) 
    {
        GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
        int larguraDoTerminal = informacoesDoBufferDaTela.srWindow.Right - informacoesDoBufferDaTela.srWindow.Left + 1;

        int larguraDoMapaEmColunas = matrizDoMapaAtual.empty() ? 0 : matrizDoMapaAtual[0].length();
        int espacosParaCentralizarOMapa = (larguraDoTerminal - larguraDoMapaEmColunas) / 2;
        std::string margemEsquerdaDoMapa(espacosParaCentralizarOMapa > 0 ? espacosParaCentralizarOMapa : 0, ' ');
        std::string margemDireitaDoMapa = "";
        
        if (tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") {
            std::string corFundoRoxa = "\033[48;5;54m";
            margemEsquerdaDoMapa = corFundoRoxa + std::string(espacosParaCentralizarOMapa > 0 ? espacosParaCentralizarOMapa : 0, ' ') + "\033[0m";
            int espacosDireita = larguraDoTerminal - espacosParaCentralizarOMapa - larguraDoMapaEmColunas;
            margemDireitaDoMapa = corFundoRoxa + std::string(espacosDireita > 0 ? espacosDireita : 0, ' ') + "\033[0m";
        }

        std::string textoDeControlesDoJogador = "W,A,S,D: Mover | I: Inventario | C: Ficha";
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
                    std::cout << "\x1b[1;32m@\x1b[0m"; // Verde para o jogador
                }
                else if (matrizDoMapaAtual[y][x] == 'S' && (x == 0 || matrizDoMapaAtual[y][x-1] != '^'))
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << "\x1b[1;31mS\x1b[0m"; // Vermelho para os Slimes
                }
                else if (matrizDoMapaAtual[y][x] == 'F')
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << "\x1b[1;31mF\x1b[0m"; // Ciano para as Fadas
                }
                else if (matrizDoMapaAtual[y][x] == 'A')
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << "\x1b[1;31m"; // Vermelho para a Abominacao
                    if (x + 1 < matrizDoMapaAtual[y].size() && matrizDoMapaAtual[y][x+1] == 'm') {
                        std::cout << "Am";
                        x++; // Pula a letra 'm' no loop para nao renderizar duas vezes
                    } else {
                        std::cout << 'A';
                    }
                    std::cout << "\x1b[0m";
                }
                else if (matrizDoMapaAtual[y][x] == 'M')
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << "\x1b[1;35mM\x1b[0m"; // Magenta para a Bruxa
                }
                else if (matrizDoMapaAtual[y][x] == 'B')
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << "\x1b[1;33mB\x1b[0m"; // Amarelo para o Bau
                }
                else 
                {
                    linhaSendoRenderizada += matrizDoMapaAtual[y][x];
                }
            }
            std::cout << linhaSendoRenderizada << margemDireitaDoMapa << "\n";
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

            Menu::limparTelaDoTerminal();
            Menu::exibirLogoDoJogo(tituloDoMapaAtual);
            GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
            linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
            continue;
        }

        if (teclaPressionadaPeloJogador == 'c' || teclaPressionadaPeloJogador == 'C') 
        {
            TelaAtributos::gerenciarFichaDoJogador(jogadorAtual);

            if (jogadorAtual->obterVoltarProMenu()) break;

            Menu::limparTelaDoTerminal();
            Menu::exibirLogoDoJogo(tituloDoMapaAtual);
            GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
            linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
            continue;
        }

        char celulaDestinoDoMapa = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX];

        bool ehParede = false;
        if (tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") {
            // No labirinto, permitimos andar nos caminhos (espaço), entrada/saída (^, S, E) e Baú (B)
            ehParede = (celulaDestinoDoMapa != ' ' && celulaDestinoDoMapa != '^' && celulaDestinoDoMapa != 'S' && celulaDestinoDoMapa != 'E' && celulaDestinoDoMapa != 'B');
        } else {
            // Na floresta, a parede padrão é o '#'
            ehParede = (celulaDestinoDoMapa == '#');
        }

        if (!ehParede) 
        {
            if (celulaDestinoDoMapa == 'S' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX-1] != '^') 
            {
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo("ENCONTRO PEGAJOSO");
                
                int espacosParaCentralizarMensagem = (larguraDoTerminal - 40) / 2; 
                std::string margemEsquerdaDaMensagem(espacosParaCentralizarMensagem > 0 ? espacosParaCentralizarMensagem : 0, ' ');

                std::cout << "\n" << margemEsquerdaDaMensagem << "[!] Voce encontrou Slimes selvagens!\n";
                std::cout << margemEsquerdaDaMensagem << "Deseja enfrentar a gosma?\n";
                std::cout << margemEsquerdaDaMensagem << "[0] Nao, recuar com cuidado\n";
                std::cout << margemEsquerdaDaMensagem << "[1] Sim, para a batalha!\n";
                std::cout << "\n" << margemEsquerdaDaMensagem << "Escolha: ";

                int opcaoEscolhidaNoDialogo;
                if (std::cin >> opcaoEscolhidaNoDialogo && opcaoEscolhidaNoDialogo == 1) 
                {
                    int quantidadeDeInimigos = (std::rand() % 3) + 1;
                    std::vector<std::unique_ptr<Personagem>> listaDeInimigosGerados = GeradorInimigos::criarInimigoSlime(quantidadeDeInimigos);
                    SistemaRPG sessaoDeCombate(jogadorAtual, std::move(listaDeInimigosGerados));
                    sessaoDeCombate.iniciarCombate();

                    if (jogadorAtual->obterVida() > 0) 
                    {
                        matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX] = '.'; 
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
            else if (celulaDestinoDoMapa == 'F') 
            {
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo("ENCONTRO MAGICO");
                
                int espacosParaCentralizarMensagem = (larguraDoTerminal - 40) / 2; 
                std::string margemEsquerdaDaMensagem(espacosParaCentralizarMensagem > 0 ? espacosParaCentralizarMensagem : 0, ' ');

                std::cout << "\n" << margemEsquerdaDaMensagem << "[!] Voce encontrou Fadas hostis!\n";
                std::cout << margemEsquerdaDaMensagem << "Deseja enfrentar as fadas?\n";
                std::cout << margemEsquerdaDaMensagem << "[0] Nao, recuar com cuidado\n";
                std::cout << margemEsquerdaDaMensagem << "[1] Sim, para a batalha!\n";
                std::cout << "\n" << margemEsquerdaDaMensagem << "Escolha: ";

                int opcaoEscolhidaNoDialogo;
                if (std::cin >> opcaoEscolhidaNoDialogo && opcaoEscolhidaNoDialogo == 1) 
                {
                    int quantidadeDeInimigos = (std::rand() % 3) + 1;
                    std::vector<std::unique_ptr<Personagem>> listaDeInimigosGerados = GeradorInimigos::criarInimigoFada(quantidadeDeInimigos);
                    SistemaRPG sessaoDeCombate(jogadorAtual, std::move(listaDeInimigosGerados));
                    sessaoDeCombate.iniciarCombate();

                    if (jogadorAtual->obterVida() > 0) 
                    {
                        matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX] = '.'; 
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
            else if (celulaDestinoDoMapa == 'A' || (celulaDestinoDoMapa == 'm' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX-1] == 'A')) 
            {
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo("ENCONTRO BOSS");
                
                int espacosParaCentralizarMensagem = (larguraDoTerminal - 40) / 2; 
                std::string margemEsquerdaDaMensagem(espacosParaCentralizarMensagem > 0 ? espacosParaCentralizarMensagem : 0, ' ');

                std::cout << "\n" << margemEsquerdaDaMensagem << "[!] Voce encontrou a Abominacao da Floresta!\n";
                std::cout << margemEsquerdaDaMensagem << "Deseja enfrentar este monstro terrivel?\n";
                std::cout << margemEsquerdaDaMensagem << "[0] Nao, recuar com cuidado\n";
                std::cout << margemEsquerdaDaMensagem << "[1] Sim, para a batalha!\n";
                std::cout << "\n" << margemEsquerdaDaMensagem << "Escolha: ";

                int opcaoEscolhidaNoDialogo;
                if (std::cin >> opcaoEscolhidaNoDialogo && opcaoEscolhidaNoDialogo == 1) 
                {
                    std::vector<std::unique_ptr<Personagem>> listaDeInimigosGerados = GeradorInimigos::criarInimigoAbominacaoFloresta();
                    SistemaRPG sessaoDeCombate(jogadorAtual, std::move(listaDeInimigosGerados));
                    sessaoDeCombate.iniciarCombate();

                    if (jogadorAtual->obterVida() > 0) 
                    {
                        if (celulaDestinoDoMapa == 'A') {
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
            else if (celulaDestinoDoMapa == 'M') 
            {
                NPCMorgana::interagir(jogadorAtual);

                if (exploracaoEstaAtiva) {
                    Menu::limparTelaDoTerminal();
                    Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                    GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
                    linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
                }
            }
            else if (celulaDestinoDoMapa == 'B' && tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") 
            {
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo("TESOURO ESCONDIDO");
                
                int espacosParaCentralizarMensagem = (larguraDoTerminal - 40) / 2; 
                std::string margemEsquerdaDaMensagem(espacosParaCentralizarMensagem > 0 ? espacosParaCentralizarMensagem : 0, ' ');

                std::cout << "\n" << margemEsquerdaDaMensagem << "[!] Voce encontrou um Bau ancestral!\n";
                std::cout << margemEsquerdaDaMensagem << "Deseja abri-lo?\n";
                std::cout << margemEsquerdaDaMensagem << "[0] Nao, deixar intocado\n";
                std::cout << margemEsquerdaDaMensagem << "[1] Sim, destrancar a fechadura!\n";
                std::cout << "\n" << margemEsquerdaDaMensagem << "Escolha: ";

                int opcaoEscolhida;
                if (std::cin >> opcaoEscolhida && opcaoEscolhida == 1) 
                {
                    std::cout << "\n" << margemEsquerdaDaMensagem << "[SISTEMA]: O bau se abre rangendo... Voce obteve itens valiosos!\n";
                    
                    int qtdPocoes = (std::rand() % 3) + 2; // Gera entre 2 a 4
                    for (int i = 0; i < qtdPocoes; ++i) {
                        auto pocao = std::make_unique<ItemConsumivel>("Pocao de Cura (30%VM)");
                        pocao->adicionarPropriedade(Propriedade::ConsumivelCura);
                        jogadorAtual->obterInventario()->adicionarItem(std::move(pocao));
                    }
                    std::cout << margemEsquerdaDaMensagem << "+ " << qtdPocoes << "x Pocoes de Cura (30%VM)\n";
                    
                    int qtdOuro = (std::rand() % 151) + 150; // Gera entre 150 a 300
                    jogadorAtual->obterInventario()->adicionarOuro(qtdOuro);
                    std::cout << margemEsquerdaDaMensagem << "+ " << qtdOuro << "G\n";

                    bool isFuria = (std::rand() % 2 == 0);
                    std::string nomeBuff = isFuria ? "Pocao de Furia (Buff)" : "Elixir Arcano (Buff)";
                    auto buff = std::make_unique<ItemConsumivel>(nomeBuff);
                    buff->adicionarPropriedade(Propriedade::ConsumivelBuff);
                    jogadorAtual->obterInventario()->adicionarItem(std::move(buff));
                    std::cout << margemEsquerdaDaMensagem << "+ 1x " << nomeBuff << "\n";

                    jogadorAtual->obterInventario()->adicionarItem(std::make_unique<Material>("Pedra magica de upgrade"));
                    std::cout << margemEsquerdaDaMensagem << "+ 1x Pedra magica de upgrade\n";
                    
                    matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX] = ' '; 
                    posicaoXDoJogador = proximaPosicaoX;
                    posicaoYDoJogador = proximaPosicaoY;
                    Menu::aguardarPressionamentoDeEnter();
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
            else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'C' && !jogadorEstaDentroDeUmSubMapa)
            {
                matrizDoMapaPrincipalSalva = matrizDoMapaAtual;
                posicaoXSalvaAntesDeEntrarNoSubMapa = posicaoXDoJogador;
                posicaoYSalvaAntesDeEntrarNoSubMapa = posicaoYDoJogador;
                
                if (!cabanaJaFoiVisitada) {
                    matrizDoMapaAtual = NPCMorgana::obterMapaCabana();
                    cabanaJaFoiVisitada = true;
                } else {
                    matrizDoMapaAtual = matrizDoMapaDaCabanaSalva;
                }

                posicaoXDoJogador = 8;
                posicaoYDoJogador = 2;
                jogadorEstaDentroDeUmSubMapa = true;
                tituloDoMapaAtual = "CABANA DA BRUXA";
                
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
                linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
                continue;
            }
            else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'V') 
            {
                TransicoesDeMapa::exibirTransicaoParaVila();
                return;
            }
            else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'T' && !jogadorEstaDentroDeUmSubMapa)
            {
                matrizDoMapaPrincipalSalva = matrizDoMapaAtual;
                posicaoXSalvaAntesDeEntrarNoSubMapa = posicaoXDoJogador;
                posicaoYSalvaAntesDeEntrarNoSubMapa = posicaoYDoJogador;
                
                if (!coracaoDaArvoreJaFoiVisitado) {
                    matrizDoMapaAtual = RacaAbominacaoFloresta::obterMapaCoracaoDaArvore();
                    coracaoDaArvoreJaFoiVisitado = true;
                } else {
                    matrizDoMapaAtual = matrizDoMapaDoCoracaoDaArvoreSalva;
                }

                posicaoXDoJogador = 10;
                posicaoYDoJogador = 3;
                jogadorEstaDentroDeUmSubMapa = true;
                tituloDoMapaAtual = "CORACAO DA ARVORE";
                
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
                linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
                continue;
            }
            else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'L' && jogadorEstaDentroDeUmSubMapa)
            {
                if (!g_labirintoDesbloqueado) {
                    Menu::limparTelaDoTerminal();
                    Menu::exibirLogoDoJogo("PASSAGEM BLOQUEADA");
                    int espacosMsg = (larguraDoTerminal - 60) / 2;
                    std::cout << "\n" << std::string(espacosMsg > 0 ? espacosMsg : 0, ' ') << "[SISTEMA]: A passagem esta selada por magia. Fale com Morgana.\n";
                    Menu::aguardarPressionamentoDeEnter();
                    
                    Menu::limparTelaDoTerminal();
                    Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                    GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
                    linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
                    continue;
                }
                
                if (tituloDoMapaAtual == "CABANA DA BRUXA") matrizDoMapaDaCabanaSalva = matrizDoMapaAtual;

                if (!labirintoJaFoiVisitado) {
                    matrizDoMapaAtual = {
        " .===================================================================================================. ",
        " |B|   |       |             |     |     |                     |     |   |   |       |       |       | ",
        " | |== | ==. ==| .===. | ====' ==. | | | '=. | ==. .=========. | .=. | | | | | .===. | ==. | '=. | ==| ",
        " | |   |   |   | |   | |         |   | |   | |   | |   |     | | |B|   |   | | |   | |   | |   | |   | ",
        " | | | '=. '=. | | | |=============. | '=. | |== |=' | | .=. | | | '=======| | |== | '===| '=. '=| | | ",
        " | | |   |   |   | | |             | |   | | |   |   | | | | |   |       | |   |   |     |   |   | | | ",
        " | | '=====. '===' | | .========== | '=. | '=| .=' ==| | | | '=======. | | '===' ==+==== |== |=. '=| | ",
        " | |       |   |   | | |   |       |   | |   | |     | |   |         | |     |     |     |   | |   | | ",
        " | '=. ==. | .=' .=' | | | '=====. |===' |=. | | ====' |=. |=. ===== | |===. '==== | .===| ==| '== | | ",
        " |   |   |   |   |   | | |       | |     | | |     |   | | | |   |     |   |       | |   |   |     | | ",
        " |=. '=======' .=' .=| | |=====. | | .===' | | .===' .=' | | '=. '===. | | '=. .===| | ==' | | .===' | ",
        " | |       |   |   | |   |     | | | | |   | | |     |   | |   |     | | |   | |   | |     | | |     | ",
        " | '=====. | .=| ==| | ==' .===' | | | | | | '=' .===' | | | | |==== | | '=. '=' | | | .===' | '==== | ",
        " ^S  |   |   | |   |       |     |   |   | |     | |   | |   | | |   | |   |     |   | |     |       ^E",
        " | | | | '===' '=. '=====. | ====+=====. | '=====' | .=| '===| | | ======. |=====+===' | .=========. | ",
        " | |   |   |     |   |   | |     |     | |         | | |     | | |       | |     |     | | |       | | ",
        " |===. |== | ==. |== | | | |===. | ==. '===. ===== | | '===. | | '=====. | | ====' ====| | | .== | | | ",
        " |   | |   |   | |   | |   |   | |   |     |   |   |     | |   |       | | |   |       | |   |   | | | ",
        " | | '=' | '=. | | ======. | .=' |== |==== |== | | |===. | |===' | ==. | | | | | .===. | |===' .=' | | ",
        " | |     |   | | |       |   |   |   |     |   | | |   | | |     |   | | | | |   |   | | |     |   | | ",
        " | '=========' | |=====. '===| ==' ==| .===| .=| | | | | | | ======= | | | |=====' | |=' | .===| .=' | ",
        " |     |       | |     |     |       | |   | | | |   | | |     |     | |   |       | |   | |   |B|   | ",
        " | ==. | ======| | ==. '==== '=======' | | | | | '=====' '=====' .===' '===' .=====' | .=' | | '=' ==| ",
        " |   |         |     |                 |B|     |                 |           |         |     |       | ",
        " '===================================================================================================' "
                    };
                    labirintoJaFoiVisitado = true;
                } else {
                    matrizDoMapaAtual = matrizDoMapaDoLabirintoSalva;
                }

                posicaoXDoJogador = 3;
                posicaoYDoJogador = 13;
                tituloDoMapaAtual = "LABIRINTO SUBTERRANEO";
                
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
                linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
                continue;
            }
            else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'S' && jogadorEstaDentroDeUmSubMapa)
            {
                if (tituloDoMapaAtual == "CORACAO DA ARVORE") matrizDoMapaDoCoracaoDaArvoreSalva = matrizDoMapaAtual;
                else if (tituloDoMapaAtual == "CABANA DA BRUXA") matrizDoMapaDaCabanaSalva = matrizDoMapaAtual;
                else if (tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") matrizDoMapaDoLabirintoSalva = matrizDoMapaAtual;

                if (tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") {
                    matrizDoMapaAtual = matrizDoMapaDaCabanaSalva;
                    posicaoXDoJogador = 20; 
                    posicaoYDoJogador = 2;
                    tituloDoMapaAtual = "CABANA DA BRUXA";
                } else {
                    matrizDoMapaAtual = matrizDoMapaPrincipalSalva;
                    posicaoXDoJogador = posicaoXSalvaAntesDeEntrarNoSubMapa;
                    posicaoYDoJogador = posicaoYSalvaAntesDeEntrarNoSubMapa;
                    jogadorEstaDentroDeUmSubMapa = false;
                    tituloDoMapaAtual = "FLORESTA";
                }
                
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
                linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
                continue;
            }
            else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'E' && tituloDoMapaAtual == "LABIRINTO SUBTERRANEO")
            {
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo("FIM DO LABIRINTO");
                int espacosMsg = (larguraDoTerminal - 60) / 2;
                std::cout << "\n" << std::string(espacosMsg > 0 ? espacosMsg : 0, ' ') << "[SISTEMA]: Voce encontrou a saida! Destino em breve...\n";
                Menu::aguardarPressionamentoDeEnter();
                
                posicaoXDoJogador = 3;
                posicaoYDoJogador = 13;
                
                if (exploracaoEstaAtiva) {
                    Menu::limparTelaDoTerminal();
                    Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                    GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
                    linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
                }
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