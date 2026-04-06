#include <iostream>
#include <vector>
#include <conio.h> 
#include <windows.h> 

#include "Mapa.h"
#include "Menu.h"
#include "SistemaRPG.h"
#include "GeradorInimigos.h"

Mapa::Mapa(Personagem* p) : jogador(p), jogadorX(2), jogadorY(2) 
{
    layout = {
        "####################",
        "#..................#",
        "#.###..............#",
        "#...#.......~......#",
        "#...#......~~~.....#",
        "#..........~......M#",
        "#######............#",
        "#..................#",
        "####################"
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

        std::string controles = "Use W, A, S, D para mover. Pressione 'Q' para sair.";
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

        char celula = layout[proximoY][proximoX];

        if (celula != '#') 
        {
            if (celula == 'M') 
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
                        layout[proximoY][proximoX] = '.'; // Tira o 'M' do mapa se vencer
                        jogadorX = proximoX;
                        jogadorY = proximoY;
                    }
                } 
                else 
                {
                    std::cin.clear(); std::cin.ignore(1000, '\n');
                    // Fica parado, não avança para o bloco M
                }

                // Se sobreviveu e o loop continuar, restaura a tela do mapa perfeitamente
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
