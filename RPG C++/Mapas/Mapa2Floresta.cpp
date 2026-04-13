#include <iostream>
#include <vector>
#include <conio.h> 
#include <windows.h> 
#include <cstdlib>

#include "Mapa2Floresta.h"
#include "../Sistema/Menu.h"
#include "../Inventario/Item.h"
#include "../Sistema/GeradorInimigos.h"
#include "../Sistema/SistemaRPG.h"

Mapa2Floresta::Mapa2Floresta(Personagem* personagemJogador) : 
    jogadorAtual(personagemJogador), posicaoXDoJogador(2), posicaoYDoJogador(2),
    jogadorEstaDentroDeUmSubMapa(false), coracaoDaArvoreJaFoiVisitado(false),
    posicaoXSalvaAntesDeEntrarNoSubMapa(0), posicaoYSalvaAntesDeEntrarNoSubMapa(0)
{
    matrizDoMapaAtual = 
    {   "##############################################################################################################",
        "#...................**........................................**.............................................#",
        "#...[^V]...........****......................................****......................**....................#",
        "#...................||...................S....................||......................****...................#",
        "#...................||........................................||.......................||.............S......#",
        "#............................................................................S.........||....................#",
        "#.....................................**.....................................................................#",
        "#....................................****................S...................................................#",
        "#.......................S.............||.....................................................**..............#",
        "#.....................................||....................................................****.............#",
        "#............................................................................................||..............#",
        "#.................**............................................**...........................||..............#",
        "#................****........................S.................****..........................................#",
        "#.................||............................................||...............S...........................#",
        "#.................||...........S................................||...........................................#",
        "#............................................................................................................#",
        "#.........................................................................**.................................#",
        "#........................................................................****........................S.......#",
        "#......................................**..................S..............||.................................#",
        "#.....................S...............****................................||.................................#",
        "#......................................||....................................................................#",
        "#......................................||.......................................................**...........#",
        "#..............................................................................................****..........#",
        "#...............................................................................................||...........#",
        "#...............................................................................................||...........#",
        "#................................................................................................:--.........#",
        "#..........................................................................................--=-+==-+--..--...#",
        "#.........................................................................................:-=-====--==--+=-..#",
        "#........................................................................................-+-====-------+-=-..#",
        "#.....................**.................................................................:++==++++++--=-**-..#",
        "#....................****.............................**................................:=++-*##=##%#***#-...#",
        "#.....................||.............................****....................................::%***#+-.......#",
        "#.....................||..............................||........................................-*+*.........#",
        "#.....................................................||........................................%+*..........#",
        "#...............................................................................................**-..........#",
        "#.............................................................................................:%^T**.........#",
        "##############################################################################################################"
    };
}

void Mapa2Floresta::iniciarLoopDeExploracaoDoMapa() 
{
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
                else if (matrizDoMapaAtual[y][x] == 'S')
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << "\x1b[1;31mS\x1b[0m"; // Vermelho para os Slimes
                }
                else 
                {
                    linhaSendoRenderizada += matrizDoMapaAtual[y][x];
                }
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

            Menu::limparTelaDoTerminal();
            Menu::exibirLogoDoJogo(tituloDoMapaAtual);
            GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
            linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
            continue;
        }

        if (teclaPressionadaPeloJogador == 'c' || teclaPressionadaPeloJogador == 'C') 
        {
            Menu::gerenciarFichaDoJogador(jogadorAtual);

            Menu::limparTelaDoTerminal();
            Menu::exibirLogoDoJogo(tituloDoMapaAtual);
            GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
            linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
            continue;
        }

        char celulaDestinoDoMapa = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX];

        if (celulaDestinoDoMapa != '#') 
        {
            if (celulaDestinoDoMapa == 'S') 
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
                    std::vector<Personagem*> listaDeInimigosGerados = GeradorInimigos::criarHordaDeSlimes(quantidadeDeInimigos);
                    SistemaRPG sessaoDeCombate(jogadorAtual, listaDeInimigosGerados);
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
            else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'V') 
            {
                Menu::limparTelaDoTerminal();
                
                std::vector<std::string> arteVila = {
                    "                                                           |>>>",
                    "                   _                      _                |",
                    "    ____________ .' '.    _____/----/-\\ .' './========\\   / \\",
                    "   //// ////// /V_.-._\\  |.-.-.|===| _ |-----| u    u |  /___\\",
                    "  // /// // ///==\\ u |.  || | ||===||||| |T| |   ||   | .| u |_ _ _ _ _ _",
                    " ///////-\\\\////====\\==|:::::::::::::::::::::::::::::::::::|u u| U U U U U",
                    " |----/\\u |--|++++|..|'''''''''''::::::::::::::''''''''''|+++|+-+-+-+-+-+",
                    " |u u|u | |u ||||||..|              '::::::::'           |===|>=== _ _ ==",
                    " |===|  |u|==|++++|==|              .::::::::.           | T |....| V |..",
                    " |u u|u | |u ||HH||         \\|/    .::::::::::.",
                    " |===|_.|u|_.|+HH+|_              .::::::::::::.              _",
                    "                __(_)___         .::::::::::::::.         ___(_)__",
                    "---------------/  / \\  /|       .:::::;;;:::;;:::.       |\\  / \\  \\-------",
                    "______________/_______/ |      .::::::;;:::::;;:::.      | \\_______\\________",
                    "|       |     [===  =] /|     .:::::;;;::::::;;;:::.     |\\ [==  = ]   |",
                    "|_______|_____[ = == ]/ |    .:::::;;;:::::::;;;::::.    | \\[ ===  ]___|____",
                    "     |       |[  === ] /|   .:::::;;;::::::::;;;:::::.   |\\ [=  ===] |",
                    "_____|_______|[== = =]/ |  .:::::;;;::::::::::;;;:::::.  | \\[ ==  =]_|______",
                    " |       |    [ == = ] /| .::::::;;:::::::::::;;;::::::. |\\ [== == ]      |",
                    "_|_______|____[=  == ]/ |.::::::;;:::::::::::::;;;::::::.| \\[  === ]______|_",
                    "   |       |  [ === =] /.::::::;;::::::::::::::;;;:::::::.\\ [===  =]   |",
                    "___|_______|__[ == ==]/.::::::;;;:::::::::::::::;;;:::::::.\\[=  == ]___|_____"
                };

                std::cout << "\n\n";
                Menu::imprimirLinhasCentralizadasNaTela(arteVila, 75, "\033[33m");

                std::string textoTransicao = "Voltando para a Vila...";
                int larguraDoTerminal = Menu::obterLarguraDoTerminalEmColunas();
                int espacosParaCentralizar = (larguraDoTerminal - (int)textoTransicao.length()) / 2;
                std::cout << "\n\n" << std::string(espacosParaCentralizar > 0 ? espacosParaCentralizar : 0, ' ') << textoTransicao << "\n";
                Sleep(3000); 
                return;
            }
            else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'T' && !jogadorEstaDentroDeUmSubMapa)
            {
                matrizDoMapaPrincipalSalva = matrizDoMapaAtual;
                posicaoXSalvaAntesDeEntrarNoSubMapa = posicaoXDoJogador;
                posicaoYSalvaAntesDeEntrarNoSubMapa = posicaoYDoJogador;
                
                if (!coracaoDaArvoreJaFoiVisitado) {
                    matrizDoMapaAtual = {
                        "      ######################################",
                        "    ####..................................####",
                        "   ###......................................###",
                        "  ##....[^S]..................................##",
                        "  ##........................*.................##",
                        "   ###......................................###",
                        "    ####..................................####",
                        "      ######################################",
                    };
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
            else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'S' && jogadorEstaDentroDeUmSubMapa)
            {
                if (tituloDoMapaAtual == "CORACAO DA ARVORE") matrizDoMapaDoCoracaoDaArvoreSalva = matrizDoMapaAtual;

                matrizDoMapaAtual = matrizDoMapaPrincipalSalva;
                posicaoXDoJogador = posicaoXSalvaAntesDeEntrarNoSubMapa;
                posicaoYDoJogador = posicaoYSalvaAntesDeEntrarNoSubMapa;
                jogadorEstaDentroDeUmSubMapa = false;
                tituloDoMapaAtual = "FLORESTA";
                
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