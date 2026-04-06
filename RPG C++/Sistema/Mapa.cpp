#include <iostream>
#include <vector>
#include <conio.h> 
#include <windows.h> 

#include "Mapa.h"
#include "Menu.h"
#include "SistemaRPG.h"
#include "GeradorInimigos.h"
#include "../Inventario/Item.h"
#include "../Raças/RacaBase.h"
#include "../Classes/ClasseBase.h"

Mapa::Mapa(Personagem* p) : jogador(p), jogadorX(2), jogadorY(2) 
{
    layout = {
        "################################################################################",
        "#........................................................##...................##",
        "#.........###............................................##...................##",
        "#.........#L#...[ VILA ].................................##.........O.........##",
        "#.........###............................................##...................##",
        "#.........................................................###..............#####",
        "#..........................G................................#.............######",
        "#.........................................................................######",
        "#.................................~~~~~~~~~~~............................#######",
        "####............................~~~~~~~~~~~~~~~~........................########",
        "#####.........................~~~~~~~~~~~~~~~~~~~~......................########",
        "##...........................~~~~~~~~~~~~~~~~~~~~~~...........G..........#######",
        "##...............................~~~~~~~~~~~~~~........................#########",
        "##................................~~~~~~~~~~~~.........................#########",
        "##.....................................................................#########",
        "##........G............................................................#########",
        "##.....................................................................#########",
        "################################################################################"
    };
}

void Mapa::iniciarExploracao() 
{
    bool jogando = true;
    
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);

    Menu::limparTela();
    Menu::exibirLogo("EXPLORACAO");

    // Descobre onde a logo termina para renderizar o mapa perfeitamente logo abaixo dela
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(consoleHandle, &csbi);
    int linhaMapaY = csbi.dwCursorPosition.Y;

    while (jogando && jogador->obterVida() > 0) 
    {
        // Calcula os espacos dinamicamente para manter o mapa e instrucoes sempre no centro da tela
        GetConsoleScreenBufferInfo(consoleHandle, &csbi);
        int larguraConsole = csbi.srWindow.Right - csbi.srWindow.Left + 1;

        int larguraMapa = layout.empty() ? 0 : layout[0].length();
        int espacosMapa = (larguraConsole - larguraMapa) / 2;
        std::string margemMapa(espacosMapa > 0 ? espacosMapa : 0, ' ');

        std::string controles = "W,A,S,D: Mover | I: Inventario | C: Ficha | Q: Sair";
        int espacosControles = (larguraConsole - (int)controles.length()) / 2;
        std::string margemControles(espacosControles > 0 ? espacosControles : 0, ' ');

        // Move o cursor para o topo do mapa invés de limpar a tela toda (evita piscar pesado)
        COORD cursorPosition;
        cursorPosition.X = 0;
        cursorPosition.Y = linhaMapaY;
        SetConsoleCursorPosition(consoleHandle, cursorPosition);

        for (int y = 0; y < layout.size(); y++) 
        {
            std::string linhaRenderizada = margemMapa; // Margem dinamica centralizada
            for (int x = 0; x < layout[y].size(); x++) 
            {
                if (x == jogadorX && y == jogadorY) linhaRenderizada += '@'; 
                else linhaRenderizada += layout[y][x]; 
            }
            std::cout << linhaRenderizada << "\n";
        }
        
        std::cout << "\n" << margemControles << controles << "\n";

        char tecla = _getch();
        
        int proximoX = jogadorX;
        int proximoY = jogadorY;

        if (tecla == 'w' || tecla == 'W' || tecla == 72) proximoY--; // 72 = Seta pra cima
        if (tecla == 's' || tecla == 'S' || tecla == 80) proximoY++; // 80 = Seta pra baixo
        if (tecla == 'a' || tecla == 'A' || tecla == 75) proximoX--; // 75 = Seta esquerda
        if (tecla == 'd' || tecla == 'D' || tecla == 77) proximoX++; // 77 = Seta direita
        if (tecla == 'q' || tecla == 'Q') { jogando = false; continue; }

        if (tecla == 'i' || tecla == 'I') 
        {
            std::string codigo;
            do 
            {
                Menu::exibirInventario(jogador);
                std::cout << "\nDigite o codigo do item (ex: 1C) ou '0' para voltar: ";
                std::cin >> codigo;
    
                if (codigo != "0")
                {
                    Item* item = jogador->obterInventario()->buscarItemPorCodigo(
                        codigo, jogador->obterArma(), jogador->obterEscudo(), jogador->obterArmadura()
                    );
    
                    if (item && dynamic_cast<PocaoCura*>(item))
                    {
                        std::string nomeItem = item->obterNomeItem(); 
                        int cura = static_cast<int>(jogador->obterVidaMaxima() * 0.30);
                        
                        jogador->modificarVida(cura); 
                        jogador->obterInventario()->removerPorNome(nomeItem);
                        
                        std::cout << "\n[SISTEMA]: " << nomeItem << " usada! +" << cura << " HP.\n";
                        Menu::esperar();
                    }
                    else if (item)
                    {
                        std::cout << "\n[SISTEMA]: Este item nao pode ser usado fora de combate!\n";
                        Menu::esperar();
                    }
                }
            } while (codigo != "0");

            // Restaura a renderizacao padrao do mapa apos fechar o inventario
            Menu::limparTela();
            Menu::exibirLogo("EXPLORACAO");
            GetConsoleScreenBufferInfo(consoleHandle, &csbi);
            linhaMapaY = csbi.dwCursorPosition.Y;
            continue;
        }

        if (tecla == 'c' || tecla == 'C') 
        {
            std::string opcao;
            do 
            {
                Menu::exibirFichaJogador(jogador);
                std::cout << "\nDigite '0' para voltar: ";
                std::cin >> opcao;
            } while (opcao != "0");

            // Restaura a tela do mapa
            Menu::limparTela();
            Menu::exibirLogo("EXPLORACAO");
            GetConsoleScreenBufferInfo(consoleHandle, &csbi);
            linhaMapaY = csbi.dwCursorPosition.Y;
            continue;
        }

        char celula = layout[proximoY][proximoX];

        if (celula != '#' && celula != '~') 
        {
            if (celula == 'G') 
            {
                Menu::limparTela();
                Menu::exibirLogo("ENCONTRO INESPERADO");
                
                int espDialogo = (larguraConsole - 40) / 2; // 40 eh o tamanho da frase mais longa
                std::string mDialogo(espDialogo > 0 ? espDialogo : 0, ' ');

                std::cout << "\n" << mDialogo << "[!] Voce encontrou uma horda de Goblins!\n";
                std::cout << mDialogo << "Deseja enfrentar a horda?\n";
                std::cout << mDialogo << "[1] Sim, para a batalha!\n";
                std::cout << mDialogo << "[2] Nao, recuar com cuidado\n";
                std::cout << "\n" << mDialogo << "Escolha: ";

                int op;
                if (std::cin >> op && op == 1) 
                {
                    std::vector<Personagem*> horda = GeradorInimigos::gerarHordaGoblins(3);
                    SistemaRPG combate(jogador, horda);
                    combate.iniciarCombate(); // A chamada do combate acontece aqui!

                    if (jogador->obterVida() > 0) 
                    {
                        layout[proximoY][proximoX] = '.'; // Tira o 'G' do mapa se vencer
                        jogadorX = proximoX;
                        jogadorY = proximoY;
                    }
                } 
                else 
                {
                    std::cin.clear(); std::cin.ignore(1000, '\n');
                }

                // Se sobreviveu e o loop continuar, restaura a tela do mapa perfeitamente
                if (jogando) {
                    Menu::limparTela();
                    Menu::exibirLogo("EXPLORACAO");
                    GetConsoleScreenBufferInfo(consoleHandle, &csbi);
                    linhaMapaY = csbi.dwCursorPosition.Y;
                }
            } 
            else if (celula == 'O') 
            {
                Menu::limparTela();
                Menu::exibirLogo("ENCONTRO NA CAVERNA");
                
                int espDialogo = (larguraConsole - 40) / 2;
                std::string mDialogo(espDialogo > 0 ? espDialogo : 0, ' ');

                std::cout << "\n" << mDialogo << "[!] Voce encontrou um Ork assustador!\n";
                std::cout << mDialogo << "A criatura ruge desafiando voce!\n";
                std::cout << mDialogo << "Deseja enfrentar o Ork?\n";
                std::cout << mDialogo << "[1] Sim, para a batalha!\n";
                std::cout << mDialogo << "[2] Nao, recuar com cuidado\n";
                std::cout << "\n" << mDialogo << "Escolha: ";

                int op;
                if (std::cin >> op && op == 1) 
                {
                    // Inicia o combate com o Ork Mini-Boss gerado pelo Gerador de Inimigos
                    std::vector<Personagem*> horda = { GeradorInimigos::gerarOrk() };
                    SistemaRPG combate(jogador, horda);
                    combate.iniciarCombate();

                    if (jogador->obterVida() > 0) 
                    {
                        layout[proximoY][proximoX] = '.'; 
                        jogadorX = proximoX;
                        jogadorY = proximoY;
                    }
                } 
                else 
                {
                    std::cin.clear(); std::cin.ignore(1000, '\n');
                }

                if (jogando) {
                    Menu::limparTela();
                    Menu::exibirLogo("EXPLORACAO");
                    GetConsoleScreenBufferInfo(consoleHandle, &csbi);
                    linhaMapaY = csbi.dwCursorPosition.Y;
                }
            }
            else if (celula == 'L')
            {
                Menu::limparTela();
                Menu::exibirLogo("LOJA DA VILA");
                
                int espDialogo = (larguraConsole - 40) / 2;
                std::string mDialogo(espDialogo > 0 ? espDialogo : 0, ' ');

                std::cout << "\n" << mDialogo << "[Lojista]: Bem-vindo a nossa humilde vila!\n";
                std::cout << mDialogo << "Desculpe, a loja ainda esta em construcao.\n";
                Menu::esperar();

                if (jogando) {
                    Menu::limparTela();
                    Menu::exibirLogo("EXPLORACAO");
                    GetConsoleScreenBufferInfo(consoleHandle, &csbi);
                    linhaMapaY = csbi.dwCursorPosition.Y;
                }
            }
            else 
            {
                jogadorX = proximoX;
                jogadorY = proximoY;
            }
        }
    }
}
