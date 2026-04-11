#include <iostream>
#include <vector>
#include <conio.h> 
#include <windows.h> 

#include "Mapa.h"
#include "../Sistema/Menu.h"
#include "../Sistema/SistemaRPG.h"
#include "../Sistema/GeradorInimigos.h"
#include "../Inventario/Item.h"
#include "../Raças/RacaBase.h"
#include "../Classes/ClasseBase.h"

Mapa::Mapa(Personagem* personagemJogador) : jogadorAtual(personagemJogador), posicaoXDoJogador(2), posicaoYDoJogador(2), jogadorEstaDentroDeUmSubMapa(false), cavernaJaFoiVisitada(false), lojaJaFoiVisitada(false), posicaoXSalvaAntesDeEntrarNoSubMapa(0), posicaoYSalvaAntesDeEntrarNoSubMapa(0) 
{
    matrizDoMapaAtual = {
        "             #######################################################################",
        " ########################################################################################",
        "##.###############......#######################..........################################",
        "##..............................................................##########################",
        "####........#######.............................**...............#########################",
        "###.........#[^Loja]...........................****..............#########################",
        "##..........#######.............................||................#########################",
        "##..............................................||................#####^C##################",
        "###..........................G......................................##...##################",
        "##.........................................................................################",
        "###.................................~~~~~~~~~~~............................################",
        "####............................~~~~~~~~~~~~~~~~........................###################",
        "#####................**.......~~~~~~~~~~~~~~~~~~~~......................###################",
        "##..................****......~~~~~~~~~~~~~~~~~~~~~~...........G..........################",
        "##...................||..........~~~~~~~~~~~~~~........................##################",
        "####.................||..............~~~~~~~~~~~~.........................###############",
        "##.....................................................................#################",
        "###........G............................................................###############",
        "###.....................................................................##############",
        "#####...........############.............################..........###################",
        "##############################################################^R#####################",
        "    ###############################################################################",
        "             ######                                              ################",
    };
}

void Mapa::iniciarLoopDeExploracaoDoMapa() 
{
    bool exploracaoEstaAtiva = true;
    std::string tituloDoMapaAtual = "EXPLORACAO";
    
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
                            std::cout << 'O';
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
            std::string codigoDoItemDigitado;
            do 
            {
                Menu::exibirTelaDeInventario(jogadorAtual);
                std::string mensagemDoPrompt = "Digite o codigo do item ou [0] VOLTAR (exploracao): ";
                int espacosParaCentralizarMensagem = (larguraDoTerminal - (int)mensagemDoPrompt.length()) / 2;
                std::cout << "\n" << std::string(espacosParaCentralizarMensagem > 0 ? espacosParaCentralizarMensagem : 0, ' ') << mensagemDoPrompt;
                std::cin >> codigoDoItemDigitado;
    
                if (codigoDoItemDigitado != "0")
                {
                    Item* itemEncontrado = jogadorAtual->obterInventario()->buscarItemPeloCodigoDigitado(
                        codigoDoItemDigitado, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura()
                    );
    
                    if (itemEncontrado && dynamic_cast<PocaoCura*>(itemEncontrado))
                    {
                        std::string nomeDoItemEncontrado = itemEncontrado->obterNomeItem(); 
                        int quantidadeDeCura = static_cast<int>(jogadorAtual->obterVidaMaxima() * 0.30);
                        
                        jogadorAtual->modificarVida(quantidadeDeCura); 
                        jogadorAtual->obterInventario()->removerItemDoInventarioPeloNome(nomeDoItemEncontrado);
                        
                        std::cout << "\n[SISTEMA]: " << nomeDoItemEncontrado << " usada! +" << quantidadeDeCura << " HP.\n";
                        Menu::aguardarPressionamentoDeEnter();
                    }
                    else if (itemEncontrado)
                    {
                        std::cout << "\n[SISTEMA]: Este item nao pode ser usado fora de combate!\n";
                        Menu::aguardarPressionamentoDeEnter();
                    }
                }
            } while (codigoDoItemDigitado != "0");

            // Restaura a renderizacao padrao do mapa apos fechar o inventario
            Menu::limparTelaDoTerminal();
            Menu::exibirLogoDoJogo(tituloDoMapaAtual);
            GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
            linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
            continue;
        }

        if (teclaPressionadaPeloJogador == 'c' || teclaPressionadaPeloJogador == 'C') 
        {
            std::string opcaoEscolhidaNoMenuJogador;
            do 
            {
                Menu::exibirTelaDeAtributosDoJogador(jogadorAtual);
                std::string mensagemDoPrompt = "[0] VOLTAR (exploracao) | [1] LIGAR/DESLIGAR PARRY";
                if (jogadorAtual->podeSubirDeNivel()) mensagemDoPrompt += " | [2] SUBIR DE NIVEL";
                mensagemDoPrompt += ": ";
                int espacosParaCentralizarMensagem = (larguraDoTerminal - (int)mensagemDoPrompt.length()) / 2;
                std::cout << "\n" << std::string(espacosParaCentralizarMensagem > 0 ? espacosParaCentralizarMensagem : 0, ' ') << mensagemDoPrompt;
                std::cin >> opcaoEscolhidaNoMenuJogador;

                if (opcaoEscolhidaNoMenuJogador == "1") {
                    jogadorAtual->definirParryAtivado(!jogadorAtual->obterParryAtivado());
                } else if (opcaoEscolhidaNoMenuJogador == "2" && jogadorAtual->podeSubirDeNivel()) {
                    std::string atributoEscolhidoParaEvoluir;
                    std::cout << "\nDigite o atributo para melhorar (Vida, Forca, Destreza, Resistencia, Constituicao, Inteligencia, Sabedoria): ";
                    std::cin >> atributoEscolhidoParaEvoluir;
                    if (jogadorAtual->subirDeNivel(atributoEscolhidoParaEvoluir)) {
                        std::cout << "[SISTEMA]: Nivel subiu! " << atributoEscolhidoParaEvoluir << " melhorado.\n";
                        Menu::aguardarPressionamentoDeEnter();
                    } else {
                        std::cout << "[ERRO]: Atributo invalido.\n";
                        Menu::aguardarPressionamentoDeEnter();
                    }
                }
            } while (opcaoEscolhidaNoMenuJogador != "0");

            // Restaura a tela do mapa
            Menu::limparTelaDoTerminal();
            Menu::exibirLogoDoJogo(tituloDoMapaAtual);
            GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
            linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
            continue;
        }

        char celulaDestinoDoMapa = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX];

        if (celulaDestinoDoMapa != '#' && celulaDestinoDoMapa != '~') 
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
                    std::vector<Personagem*> listaDeInimigosGerados = GeradorInimigos::criarHordaDeGoblins(3);
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
                    matrizDoMapaAtual = {
                        "  ########################################",
                        "#############################################",
                        "###########.........###########################",
                        "######[^S]......................O.....##########",
                        "#######.............................###########",
                        "###############################################",
                        "  ########################################",
                    };
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

                matrizDoMapaAtual = matrizDoMapaPrincipalSalva;
                posicaoXDoJogador = posicaoXSalvaAntesDeEntrarNoSubMapa;
                posicaoYDoJogador = posicaoYSalvaAntesDeEntrarNoSubMapa;
                jogadorEstaDentroDeUmSubMapa = false;
                tituloDoMapaAtual = "EXPLORACAO";
                
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
                linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
                continue;
            }
            else if (celulaDestinoDoMapa == 'O') 
            {
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo("ENCONTRO NA CAVERNA");
                
                int espacosParaCentralizarMensagem = (larguraDoTerminal - 40) / 2;
                std::string margemEsquerdaDaMensagem(espacosParaCentralizarMensagem > 0 ? espacosParaCentralizarMensagem : 0, ' ');

                std::cout << "\n" << margemEsquerdaDaMensagem << "[!] Voce encontrou um Ork!\n";
                std::cout << margemEsquerdaDaMensagem << "A criatura ruge desafiando voce!\n";
                std::cout << margemEsquerdaDaMensagem << "Deseja enfrentar o Ork?\n";
                std::cout << margemEsquerdaDaMensagem << "[0] Nao, recuar com cuidado\n";
                std::cout << margemEsquerdaDaMensagem << "[1] Sim, para a batalha!\n";
                std::cout << "\n" << margemEsquerdaDaMensagem << "Escolha: ";

                int opcaoEscolhidaNoDialogo;
                if (std::cin >> opcaoEscolhidaNoDialogo && opcaoEscolhidaNoDialogo == 1) 
                {
                    // Inicia o combate com o Ork Mini-Boss gerado pelo Gerador de Inimigos
                    std::vector<Personagem*> listaDeInimigosGerados = { GeradorInimigos::criarInimigoOrkMiniBoss() };
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
            else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'L' && !jogadorEstaDentroDeUmSubMapa)
            {
                matrizDoMapaPrincipalSalva = matrizDoMapaAtual;
                posicaoXSalvaAntesDeEntrarNoSubMapa = posicaoXDoJogador;
                posicaoYSalvaAntesDeEntrarNoSubMapa = posicaoYDoJogador;
                
                if (!lojaJaFoiVisitada) {
                    matrizDoMapaAtual = {
                        " ##################################",
                        "##.........................../--/|##",
                        "##..[^S]......................V.$|##",
                        "##.........................../--/|##",
                        " ##################################",
                    };
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
            else if (celulaDestinoDoMapa == 'V')
            {
                std::string opcaoEscolhidaNoMenuJogador;
                do {
                    Menu::limparTelaDoTerminal();
                    Menu::exibirLogoDoJogo("LOJA AMBULANTE");
                    
                    int espacosParaCentralizarMensagem = (larguraDoTerminal - 55) / 2;
                    std::string margemEsquerdaDaMensagem(espacosParaCentralizarMensagem > 0 ? espacosParaCentralizarMensagem : 0, ' ');

                    std::cout << "\n" << margemEsquerdaDaMensagem << "[Lojista]: Bem-vindo! De uma olhada nas minhas mercadorias.\n";
                    std::cout << margemEsquerdaDaMensagem << "Seu Ouro: " << jogadorAtual->obterInventario()->obterQuantidadeDeOuro() << "G\n\n";

                    std::cout << margemEsquerdaDaMensagem << "[1] Pocao de Cura (30% HP)         - 10G\n";
                    std::cout << margemEsquerdaDaMensagem << "[2] Manto Encantado (Mago)         - 15G\n";
                    std::cout << margemEsquerdaDaMensagem << "[3] Escudo Medio (Guerreiro)       - 15G\n";
                    std::cout << margemEsquerdaDaMensagem << "[4] Capa Magica (Bardo)            - 15G\n";
                    std::cout << margemEsquerdaDaMensagem << "[5] Escudo Leve (Arqueiro)         - 15G\n";
                    std::cout << margemEsquerdaDaMensagem << "[0] VOLTAR (exploracao)\n\n";
                    std::cout << "\n" << std::string(larguraDoTerminal, '=') << "\n";
                    std::cout << "\n" << margemEsquerdaDaMensagem << "Escolha: ";

                    std::cin >> opcaoEscolhidaNoMenuJogador;

                    if (opcaoEscolhidaNoMenuJogador >= "1" && opcaoEscolhidaNoMenuJogador <= "5") 
                    {
                        int preco = (opcaoEscolhidaNoMenuJogador == "1") ? 10 : 15;
                        if (jogadorAtual->obterInventario()->obterQuantidadeDeOuro() >= preco) 
                        {
                            jogadorAtual->obterInventario()->adicionarOuroAoInventario(-preco); // Subtrai o ouro

                            if (opcaoEscolhidaNoMenuJogador == "1") { jogadorAtual->obterInventario()->adicionarItemAoInventario(new PocaoCura()); std::cout << "\n" << margemEsquerdaDaMensagem << "[SISTEMA]: Pocao de Cura comprada!\n"; }
                            else if (opcaoEscolhidaNoMenuJogador == "2") { jogadorAtual->obterInventario()->adicionarItemAoInventario(new Escudo("Manto encantado", 5, 3)); std::cout << "\n" << margemEsquerdaDaMensagem << "[SISTEMA]: Manto Encantado comprado!\n"; }
                            else if (opcaoEscolhidaNoMenuJogador == "3") { jogadorAtual->obterInventario()->adicionarItemAoInventario(new Escudo("Escudo medio de metal", 12, 6)); std::cout << "\n" << margemEsquerdaDaMensagem << "[SISTEMA]: Escudo Medio comprado!\n"; }
                            else if (opcaoEscolhidaNoMenuJogador == "4") { jogadorAtual->obterInventario()->adicionarItemAoInventario(new Escudo("Capa magica", 5, 2)); std::cout << "\n" << margemEsquerdaDaMensagem << "[SISTEMA]: Capa Magica comprada!\n"; }
                            else if (opcaoEscolhidaNoMenuJogador == "5") { jogadorAtual->obterInventario()->adicionarItemAoInventario(new Escudo("Escudo leve de madeira", 8, 4)); std::cout << "\n" << margemEsquerdaDaMensagem << "[SISTEMA]: Escudo Leve comprado!\n"; }
                        } 
                        else 
                        {
                            std::cout << "\n" << margemEsquerdaDaMensagem << "[SISTEMA]: Ouro insuficiente!\n";
                        }
                        Menu::aguardarPressionamentoDeEnter();
                    }
                } while (opcaoEscolhidaNoMenuJogador != "0");

                if (exploracaoEstaAtiva) {
                    Menu::limparTelaDoTerminal();
                    Menu::exibirLogoDoJogo(tituloDoMapaAtual);
                    GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
                    linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
                }
            }
            else 
            {
                posicaoXDoJogador = proximaPosicaoX;
                posicaoYDoJogador = proximaPosicaoY;
            }
        }
    }
}
