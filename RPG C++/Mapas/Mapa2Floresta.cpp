#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <memory>
#include <utility>

#include "Mapa2Floresta.h"
#include "../Gerenciadores/GerenciadorMenu.h"
#include "../Inventario/Item.h"
#include "../Inventario/EquipamentoArmadura.h"
#include "../Inventario/ItemConsumivel.h"
#include "../Inventario/ItemMaterial.h"
#include "../Gerenciadores/GerenciadorInimigos.h"
#include "../Inventario/InventarioCombate.h"
#include "../Telas/TelaAtributos.h"
#include "../Telas/TelaBestiario.h"
#include "../Gerenciadores/GerenciadorCombate.h"
#include "../Inimigos/Fada.h"
#include "../Inimigos/ClasseBaseInimigo.h"
#include "../NPCs/NPCMorgana.h"
#include "../Inimigos/AbominacaoFloresta.h"
#include "TransicaoDeMapa.h"
#include "../Utilidades/SimplificacoesAparencia.h"
#include "ControleDeMapa.h"
#include "../Utilidades/GeradorAleatorio.h"

Mapa2Floresta::Mapa2Floresta(SistemaPersonagem* personagemJogador) :
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

    SimplificacoesAparencia::limparTela();
    GerenciadorMenu::exibirLogoDoJogo(tituloDoMapaAtual);

    CONSOLE_SCREEN_BUFFER_INFO informacoesDoBufferDaTela;
    GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
    int linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;

    // Lambda para restaurar a tela apos eventos
    auto restaurarTela = [&]() {
        SimplificacoesAparencia::limparTela();
        GerenciadorMenu::exibirLogoDoJogo(tituloDoMapaAtual);
        GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
        linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
    };

    auto renderizarMapa = [&](int larguraDoTerminal, int linhaInicial)
    {
        int larguraDoMapaEmColunas = matrizDoMapaAtual.empty() ? 0 : matrizDoMapaAtual[0].length();
        int espacosParaCentralizarOMapa = (larguraDoTerminal - larguraDoMapaEmColunas) / 2;
        std::string margemEsquerdaDoMapa(espacosParaCentralizarOMapa > 0 ? espacosParaCentralizarOMapa : 0, ' ');
        std::string margemDireitaDoMapa = "";

        if (tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") {
            std::string corFundoRoxa = SimplificacoesAparencia::corRGBFundo(54);
            margemEsquerdaDoMapa = corFundoRoxa + std::string(espacosParaCentralizarOMapa > 0 ? espacosParaCentralizarOMapa : 0, ' ') + SimplificacoesAparencia::cor(Cor::RESET);
            int espacosDireita = larguraDoTerminal - espacosParaCentralizarOMapa - larguraDoMapaEmColunas;
            margemDireitaDoMapa = corFundoRoxa + std::string(espacosDireita > 0 ? espacosDireita : 0, ' ') + SimplificacoesAparencia::cor(Cor::RESET);
        }

        std::string textoDeControlesDoJogador = "W,A,S,D: Mover | I: Inventario | C: Ficha | B: Bestiario";
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
                    linhaSendoRenderizada += SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::VERDE) + "@" + SimplificacoesAparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'S' && (x == 0 || matrizDoMapaAtual[y][x-1] != '^'))
                {
                    linhaSendoRenderizada += SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::VERMELHO) + "S" + SimplificacoesAparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'F')
                {
                    linhaSendoRenderizada += SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::VERMELHO) + "F" + SimplificacoesAparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'A')
                {
                    linhaSendoRenderizada += SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::VERMELHO);
                    if (x + 1 < matrizDoMapaAtual[y].size() && matrizDoMapaAtual[y][x+1] == 'm') {
                        linhaSendoRenderizada += "Am";
                        x++;
                    } else {
                        linhaSendoRenderizada += 'A';
                    }
                    linhaSendoRenderizada += SimplificacoesAparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'M')
                {
                    linhaSendoRenderizada += SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::MAGENTA) + "M" + SimplificacoesAparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'B')
                {
                    linhaSendoRenderizada += SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::AMARELO) + "B" + SimplificacoesAparencia::cor(Cor::RESET);
                }
                else
                {
                    linhaSendoRenderizada += matrizDoMapaAtual[y][x];
                }
            }
            std::cout << linhaSendoRenderizada << margemDireitaDoMapa << "\n";
        }
        std::cout << "\n" << margemEsquerdaDosControles << textoDeControlesDoJogador << "\n";
    };

    auto processarInteracao = [&](int proximaPosicaoX, int proximaPosicaoY, int larguraDoTerminal)
    {
        char celulaDestinoDoMapa = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX];

        bool ehParede = false;
        if (tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") {
            ehParede = (celulaDestinoDoMapa != ' ' && celulaDestinoDoMapa != '^' && celulaDestinoDoMapa != 'S' && celulaDestinoDoMapa != 'E' && celulaDestinoDoMapa != 'B');
        } else {
            ehParede = (celulaDestinoDoMapa == '#');
        }

        if (ehParede) return;

        if (celulaDestinoDoMapa == 'S' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX-1] != '^')
        {
            ControleDeMapa::processarCombate(jogadorAtual, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, exploracaoEstaAtiva, "ENCONTRO PEGAJOSO", "Voce encontrou Slimes selvagens!", GerenciadorInimigos::criarInimigoSlime(GeradorAleatorio::obterInteiro(1, 3)), proximaPosicaoX, proximaPosicaoY, proximaPosicaoX, 1, larguraDoTerminal, restaurarTela);
        }
        else if (celulaDestinoDoMapa == 'F')
        {
            ControleDeMapa::processarCombate(jogadorAtual, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, exploracaoEstaAtiva, "ENCONTRO MAGICO", "Voce encontrou Fadas hostis!", GerenciadorInimigos::criarInimigoFada(GeradorAleatorio::obterInteiro(1, 3)), proximaPosicaoX, proximaPosicaoY, proximaPosicaoX, 1, larguraDoTerminal, restaurarTela);
        }
        else if (celulaDestinoDoMapa == 'A' || (celulaDestinoDoMapa == 'm' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX-1] == 'A'))
        {
            int rootX = (celulaDestinoDoMapa == 'A') ? proximaPosicaoX : proximaPosicaoX - 1;
            ControleDeMapa::processarCombate(jogadorAtual, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, exploracaoEstaAtiva, "ENCONTRO BOSS", "Voce encontrou a Abominacao da Floresta!", GerenciadorInimigos::criarInimigoAbominacaoFloresta(1), proximaPosicaoX, proximaPosicaoY, rootX, 2, larguraDoTerminal, restaurarTela);
        }
        else if (celulaDestinoDoMapa == 'M')
        {
            NPCMorgana::interagir(jogadorAtual);
            if (exploracaoEstaAtiva) restaurarTela();
        }
        else if (celulaDestinoDoMapa == 'B' && tituloDoMapaAtual == "LABIRINTO SUBTERRANEO")
        {
            SimplificacoesAparencia::limparTela();
            GerenciadorMenu::exibirLogoDoJogo("TESOURO ESCONDIDO");
            int mE = (larguraDoTerminal - 40) / 2;
            std::string margem(mE > 0 ? mE : 0, ' ');
            std::cout << "\n" << margem << "[!] Voce encontrou um Baú ancestral!\n";
            std::cout << margem << "[0] Nao | [1] Abrir!\n" << margem << "Escolha: ";

            int opcao;
            if (std::cin >> opcao && opcao == 1)
            {
                std::cout << "\n" << margem << "[SISTEMA]: O baú se abre rangendo... Voce obteve itens valiosos!\n";

            int qtdPocoes = GeradorAleatorio::obterInteiro(2, 4);
                for (int i = 0; i < qtdPocoes; ++i) {
                    auto pocao = std::make_unique<ItemConsumivel>("Pocao de Cura (30%VM)");
                    pocao->adicionarPropriedade(Propriedade::ConsumivelCura);
                    jogadorAtual->obterInventario()->adicionarItem(std::move(pocao));
                }
                std::cout << margem << "+ " << qtdPocoes << "x Pocoes de Cura (30%VM)\n";

            int qtdOuro = GeradorAleatorio::obterInteiro(150, 300);
                jogadorAtual->obterInventario()->adicionarOuro(qtdOuro);
                std::cout << margem << "+ " << qtdOuro << "G\n";

            bool isFuria = GeradorAleatorio::rolarChance(50);
                std::string nomeBuff = isFuria ? "Pocao de Furia (Buff)" : "Elixir Arcano (Buff)";
                auto buff = std::make_unique<ItemConsumivel>(nomeBuff);
                buff->adicionarPropriedade(Propriedade::ConsumivelBuff);
                jogadorAtual->obterInventario()->adicionarItem(std::move(buff));
                std::cout << margem << "+ 1x " << nomeBuff << "\n";

                jogadorAtual->obterInventario()->adicionarItem(std::make_unique<ItemMaterial>("Pedra magica de upgrade"));
                std::cout << margem << "+ 1x Pedra magica de upgrade\n";

                matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX] = ' ';
                posicaoXDoJogador = proximaPosicaoX;
                posicaoYDoJogador = proximaPosicaoY;
                SimplificacoesAparencia::aguardarEnter();
            } else { std::cin.clear(); std::cin.ignore(1000, '\n'); }

            if (exploracaoEstaAtiva) restaurarTela();
        }
        else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'C' && !jogadorEstaDentroDeUmSubMapa)
        {
            ControleDeMapa::entrarSubMapa(matrizDoMapaAtual, matrizDoMapaPrincipalSalva, posicaoXSalvaAntesDeEntrarNoSubMapa, posicaoYSalvaAntesDeEntrarNoSubMapa, posicaoXDoJogador, posicaoYDoJogador, jogadorEstaDentroDeUmSubMapa, tituloDoMapaAtual, matrizDoMapaDaCabanaSalva, cabanaJaFoiVisitada, NPCMorgana::obterMapaCabana(), 8, 2, "CABANA DA BRUXA", restaurarTela);
            return;
        }
        else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'V')
        {
            TransicaoDeMapa::exibirTransicaoParaVila();
            exploracaoEstaAtiva = false;
            return;
        }
        else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'T' && !jogadorEstaDentroDeUmSubMapa)
        {
            ControleDeMapa::entrarSubMapa(matrizDoMapaAtual, matrizDoMapaPrincipalSalva, posicaoXSalvaAntesDeEntrarNoSubMapa, posicaoYSalvaAntesDeEntrarNoSubMapa, posicaoXDoJogador, posicaoYDoJogador, jogadorEstaDentroDeUmSubMapa, tituloDoMapaAtual, matrizDoMapaDoCoracaoDaArvoreSalva, coracaoDaArvoreJaFoiVisitado, AbominacaoFloresta::obterMapaCoracaoDaArvore(), 10, 3, "CORACAO DA ARVORE", restaurarTela);
            return;
        }
        else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'L' && jogadorEstaDentroDeUmSubMapa)
        {
            if (!jogadorAtual->obterLabirintoDesbloqueado()) {
                SimplificacoesAparencia::limparTela();
                GerenciadorMenu::exibirLogoDoJogo("PASSAGEM BLOQUEADA");
                int espacosM = (larguraDoTerminal - 60) / 2;
                std::cout << "\n" << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[SISTEMA]: A passagem esta selada por magia. Fale com Morgana.\n";
                SimplificacoesAparencia::aguardarEnter();
                restaurarTela();
                return;
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
            restaurarTela();
            return;
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
            restaurarTela();
            return;
        }
        else if (celulaDestinoDoMapa == '^' && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX+1] == 'E' && tituloDoMapaAtual == "LABIRINTO SUBTERRANEO")
        {
            SimplificacoesAparencia::limparTela();
            GerenciadorMenu::exibirLogoDoJogo("FIM DO LABIRINTO");
            int espacosM = (larguraDoTerminal - 60) / 2;
            std::cout << "\n" << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[SISTEMA]: Voce encontrou a saida! Destino em breve...\n";
            SimplificacoesAparencia::aguardarEnter();

            posicaoXDoJogador = 3;
            posicaoYDoJogador = 13;
            if (exploracaoEstaAtiva) restaurarTela();
            return;
        }
        else
        {
            posicaoXDoJogador = proximaPosicaoX;
            posicaoYDoJogador = proximaPosicaoY;
        }
    };

    while (exploracaoEstaAtiva && jogadorAtual->obterVida() > 0)
    {
        GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
        int larguraDoTerminal = informacoesDoBufferDaTela.srWindow.Right - informacoesDoBufferDaTela.srWindow.Left + 1;

        renderizarMapa(larguraDoTerminal, linhaInicialParaDesenharOMapa);

        char teclaPressionadaPeloJogador = _getch();

        int proximaPosicaoX = posicaoXDoJogador;
        int proximaPosicaoY = posicaoYDoJogador;

        bool abriuMenu = ControleDeMapa::processarInputEComandos(teclaPressionadaPeloJogador, jogadorAtual, proximaPosicaoX, proximaPosicaoY, restaurarTela);
        
        if (jogadorAtual->obterVoltarProMenu()) break;
        if (abriuMenu) continue;

        processarInteracao(proximaPosicaoX, proximaPosicaoY, larguraDoTerminal);
    }
}
