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

Mapa::Mapa(Personagem* p) : jogador(p), jogadorX(2), jogadorY(2), emSubMapa(false), cavernaIniciada(false), lojaIniciada(false), anteriorX(0), anteriorY(0) 
{
    layout = {
        "################################################################################",
        "#..................................................................#############",
        "#.........###.....................................................##############",
        "#.........#[Loja]................................................###############",
        "#.........###....................................................###############",
        "#.................................................................###C##########",
        "#..........................G......................................##...#########",
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
    std::string tituloMapa = "EXPLORACAO";
    
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);

    Menu::limparTela();
    Menu::exibirLogo(tituloMapa);

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

        std::string controles = "W,A,S,D: Mover | I: Inventario | C: Ficha ";
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

        if (tecla == 'i' || tecla == 'I') 
        {
            std::string codigo;
            do 
            {
                Menu::exibirInventario(jogador);
                std::string msg = "Digite o codigo do item ou [0] VOLTAR (exploracao) ";
                int esp = (larguraConsole - (int)msg.length()) / 2;
                std::cout << "\n" << std::string(esp > 0 ? esp : 0, ' ') << msg;
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
            Menu::exibirLogo(tituloMapa);
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
                std::string msg = "[0] VOLTAR (exploracao) | [1] LIGAR/DESLIGAR PARRY: ";
                int esp = (larguraConsole - (int)msg.length()) / 2;
                std::cout << "\n" << std::string(esp > 0 ? esp : 0, ' ') << msg;
                std::cin >> opcao;

                if (opcao == "1") {
                    jogador->definirParryAtivado(!jogador->obterParryAtivado());
                }
            } while (opcao != "0");

            // Restaura a tela do mapa
            Menu::limparTela();
            Menu::exibirLogo(tituloMapa);
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
                std::cout << mDialogo << "[0] Nao, recuar com cuidado\n";
                std::cout << mDialogo << "[1] Sim, para a batalha!\n";
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
                    Menu::exibirLogo(tituloMapa);
                    GetConsoleScreenBufferInfo(consoleHandle, &csbi);
                    linhaMapaY = csbi.dwCursorPosition.Y;
                }
            } 
            else if (celula == 'C' && !emSubMapa) 
            {
                mapaGeralSalvo = layout;
                anteriorX = jogadorX;
                anteriorY = jogadorY;
                
                if (!cavernaIniciada) {
                    layout = {
                        "##########################################",
                        "#########.........########################",
                        "####[S]......................O.....#######",
                        "#####.............................########",
                        "##########################################"
                    };
                    cavernaIniciada = true;
                } else {
                    layout = mapaCavernaSalvo;
                }

                jogadorX = 8;
                jogadorY = 2;
                emSubMapa = true;
                tituloMapa = "CAVERNA DO ORK";
                
                Menu::limparTela();
                Menu::exibirLogo(tituloMapa);
                GetConsoleScreenBufferInfo(consoleHandle, &csbi);
                linhaMapaY = csbi.dwCursorPosition.Y;
                continue;
            }
            else if (celula == 'S' && emSubMapa) 
            {
                // Salva o sub-mapa correto de acordo com a area atual antes de sair
                if (tituloMapa == "CAVERNA DO ORK") mapaCavernaSalvo = layout;
                else if (tituloMapa == "LOJA DA VILA") mapaLojaSalvo = layout;

                layout = mapaGeralSalvo;
                jogadorX = anteriorX;
                jogadorY = anteriorY;
                emSubMapa = false;
                tituloMapa = "EXPLORACAO";
                
                Menu::limparTela();
                Menu::exibirLogo(tituloMapa);
                GetConsoleScreenBufferInfo(consoleHandle, &csbi);
                linhaMapaY = csbi.dwCursorPosition.Y;
                continue;
            }
            else if (celula == 'O') 
            {
                Menu::limparTela();
                Menu::exibirLogo("ENCONTRO NA CAVERNA");
                
                int espDialogo = (larguraConsole - 40) / 2;
                std::string mDialogo(espDialogo > 0 ? espDialogo : 0, ' ');

                std::cout << "\n" << mDialogo << "[!] Voce encontrou um Ork!\n";
                std::cout << mDialogo << "A criatura ruge desafiando voce!\n";
                std::cout << mDialogo << "Deseja enfrentar o Ork?\n";
                std::cout << mDialogo << "[0] Nao, recuar com cuidado\n";
                std::cout << mDialogo << "[1] Sim, para a batalha!\n";
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
                    Menu::exibirLogo(tituloMapa);
                    GetConsoleScreenBufferInfo(consoleHandle, &csbi);
                    linhaMapaY = csbi.dwCursorPosition.Y;
                }
            }
            else if (celula == 'L' && !emSubMapa) 
            {
                mapaGeralSalvo = layout;
                anteriorX = jogadorX;
                anteriorY = jogadorY;
                
                if (!lojaIniciada) {
                    layout = {
                        "##########################################",
                        "##............................##........##",
                        "##..[S]........................V........##",
                        "##............................##........##",
                        "##########################################"
                    };
                    lojaIniciada = true;
                } else {
                    layout = mapaLojaSalvo;
                }

                jogadorX = 8;
                jogadorY = 2;
                emSubMapa = true;
                tituloMapa = "LOJA DA VILA";
                
                Menu::limparTela();
                Menu::exibirLogo(tituloMapa);
                GetConsoleScreenBufferInfo(consoleHandle, &csbi);
                linhaMapaY = csbi.dwCursorPosition.Y;
                continue;
            }
            else if (celula == 'V')
            {
                std::string opcaoLoja;
                do {
                    Menu::limparTela();
                    Menu::exibirLogo("LOJA AMBULANTE");
                    
                    int espDialogo = (larguraConsole - 55) / 2;
                    std::string mDialogo(espDialogo > 0 ? espDialogo : 0, ' ');

                    std::cout << "\n" << mDialogo << "[Lojista]: Bem-vindo! De uma olhada nas minhas mercadorias.\n";
                    std::cout << mDialogo << "Seu Ouro: " << jogador->obterInventario()->obterOuro() << "G\n\n";

                    std::cout << mDialogo << "[1] Pocao de Cura (30% HP)         - 10G\n";
                    std::cout << mDialogo << "[2] Manto Encantado (Mago)         - 15G\n";
                    std::cout << mDialogo << "[3] Escudo Medio (Guerreiro)       - 15G\n";
                    std::cout << mDialogo << "[4] Capa Magica (Bardo)            - 15G\n";
                    std::cout << mDialogo << "[5] Escudo Leve (Arqueiro)         - 15G\n";
                    std::cout << mDialogo << "[0] VOLTAR (exploracao)\n\n";
                    std::cout << "\n" << std::string(larguraConsole, '=') << "\n";
                    std::cout << "\n" << mDialogo << "Escolha: ";

                    std::cin >> opcaoLoja;

                    if (opcaoLoja >= "1" && opcaoLoja <= "5") 
                    {
                        int preco = (opcaoLoja == "1") ? 10 : 15;
                        if (jogador->obterInventario()->obterOuro() >= preco) 
                        {
                            jogador->ganharOuro(-preco); // Subtrai o ouro
                            
                            if (opcaoLoja == "1") { jogador->obterInventario()->adicionarItem(new PocaoCura()); std::cout << "\n" << mDialogo << "[SISTEMA]: Pocao de Cura comprada!\n"; }
                            else if (opcaoLoja == "2") { jogador->obterInventario()->adicionarItem(new Escudo("Manto encantado", 5, 3)); std::cout << "\n" << mDialogo << "[SISTEMA]: Manto Encantado comprado!\n"; }
                            else if (opcaoLoja == "3") { jogador->obterInventario()->adicionarItem(new Escudo("Escudo medio de metal", 12, 6)); std::cout << "\n" << mDialogo << "[SISTEMA]: Escudo Medio comprado!\n"; }
                            else if (opcaoLoja == "4") { jogador->obterInventario()->adicionarItem(new Escudo("Capa magica", 5, 2)); std::cout << "\n" << mDialogo << "[SISTEMA]: Capa Magica comprada!\n"; }
                            else if (opcaoLoja == "5") { jogador->obterInventario()->adicionarItem(new Escudo("Escudo leve de madeira", 8, 4)); std::cout << "\n" << mDialogo << "[SISTEMA]: Escudo Leve comprado!\n"; }
                        } 
                        else 
                        {
                            std::cout << "\n" << mDialogo << "[SISTEMA]: Ouro insuficiente!\n";
                        }
                        Menu::esperar();
                    }
                } while (opcaoLoja != "0");

                if (jogando) {
                    Menu::limparTela();
                    Menu::exibirLogo(tituloMapa);
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
