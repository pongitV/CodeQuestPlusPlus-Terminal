#include "Mapa2Floresta.h"

#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <memory>
#include <utility>
#include <functional>

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
    posicaoXDoJogador(8), 
    posicaoYDoJogador(8),
    jogadorAtual(personagemJogador), 
    posicaoXSalvaAntesDeEntrarNoSubMapa(0), 
    posicaoYSalvaAntesDeEntrarNoSubMapa(0),
    jogadorEstaDentroDeUmSubMapa(false),
    cabanaJaFoiVisitada(false), 
    coracaoDaArvoreJaFoiVisitado(false), 
    labirintoJaFoiVisitado(false),
    exploracaoEstaAtiva(true), 
    tituloDoMapaAtual("FLORESTA")
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

Mapa2Floresta::~Mapa2Floresta() = default;

namespace {
    class InteracaoSlime : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            if (ctx.proximaPosicaoX > 0 && ctx.self->matrizDoMapaAtual[ctx.proximaPosicaoY][ctx.proximaPosicaoX-1] != '^') {
                ControleDeMapa::processarCombate(ctx.self->jogadorAtual, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "ENCONTRO PEGAJOSO", "Voce encontrou Slimes selvagens!", GerenciadorInimigos::criarInimigoSlime(GeradorAleatorio::obterInteiro(1, 3)), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
            } else {
                ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;
            }
        }
    };

    class InteracaoFada : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            ControleDeMapa::processarCombate(ctx.self->jogadorAtual, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "ENCONTRO MAGICO", "Voce encontrou Fadas hostis!", GerenciadorInimigos::criarInimigoFada(GeradorAleatorio::obterInteiro(1, 3)), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
        }
    };

    class InteracaoAbominacao : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            int rootX = (ctx.celula == 'A') ? ctx.proximaPosicaoX : ctx.proximaPosicaoX - 1;
            ControleDeMapa::processarCombate(ctx.self->jogadorAtual, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "ENCONTRO BOSS", "Voce encontrou a Abominacao da Floresta!", GerenciadorInimigos::criarInimigoAbominacaoFloresta(1), ctx.proximaPosicaoX, ctx.proximaPosicaoY, rootX, 2, ctx.larguraDoTerminal, ctx.restaurarTela);
        }
    };

    class InteracaoMorgana : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            NPCMorgana::interagir(ctx.self->jogadorAtual);
            if (ctx.self->exploracaoEstaAtiva) ctx.restaurarTela();
        }
    };

    class InteracaoBau : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            if (ctx.self->tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") {
                SimplificacoesAparencia::limparTela();
                GerenciadorMenu::exibirLogoDoJogo("TESOURO ESCONDIDO");
                int mE = (ctx.larguraDoTerminal - 40) / 2;
                std::string margem(mE > 0 ? mE : 0, ' ');
                std::cout << "\n" << margem << "[!] Voce encontrou um Baú ancestral!\n";
                std::cout << margem << "[0] Nao | [1] Abrir!\n" << margem << "Escolha: ";

                int opcao;
                if (std::cin >> opcao && opcao == 1) {
                    std::cout << "\n" << margem << "[SISTEMA]: O baú se abre rangendo... Voce obteve itens valiosos!\n";

                    int qtdPocoes = GeradorAleatorio::obterInteiro(2, 4);
                    for (int i = 0; i < qtdPocoes; ++i) {
                        auto pocao = std::make_unique<ItemConsumivel>("Pocao de Cura (30%VM)");
                        pocao->adicionarPropriedade(Propriedade::ConsumivelCura);
                        ctx.self->jogadorAtual->obterInventario()->adicionarItem(std::move(pocao));
                    }
                    std::cout << margem << "+ " << qtdPocoes << "x Pocoes de Cura (30%VM)\n";

                    int qtdOuro = GeradorAleatorio::obterInteiro(150, 300);
                    ctx.self->jogadorAtual->obterInventario()->adicionarOuro(qtdOuro);
                    std::cout << margem << "+ " << qtdOuro << "G\n";

                    bool isFuria = GeradorAleatorio::rolarChance(50);
                    std::string nomeBuff = isFuria ? "Pocao de Furia (Buff)" : "Elixir Arcano (Buff)";
                    auto buff = std::make_unique<ItemConsumivel>(nomeBuff);
                    buff->adicionarPropriedade(Propriedade::ConsumivelBuff);
                    ctx.self->jogadorAtual->obterInventario()->adicionarItem(std::move(buff));
                    std::cout << margem << "+ 1x " << nomeBuff << "\n";

                    ctx.self->jogadorAtual->obterInventario()->adicionarItem(std::make_unique<ItemMaterial>("Pedra magica de upgrade"));
                    std::cout << margem << "+ 1x Pedra magica de upgrade\n";

                    ctx.self->matrizDoMapaAtual[ctx.proximaPosicaoY][ctx.proximaPosicaoX] = ' ';
                    ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                    ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;
                    SimplificacoesAparencia::aguardarEnter();
                } else { std::cin.clear(); std::cin.ignore(1000, '\n'); }

                if (ctx.self->exploracaoEstaAtiva) ctx.restaurarTela();
            } else {
                ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;
            }
        }
    };

    class InteracaoTeleporte : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            char nextCell = ctx.self->matrizDoMapaAtual[ctx.proximaPosicaoY][ctx.proximaPosicaoX+1];
            
            if (nextCell == 'C' && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                ControleDeMapa::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDaCabanaSalva, ctx.self->cabanaJaFoiVisitada, NPCMorgana::obterMapaCabana(), 8, 2, "CABANA DA BRUXA", ctx.restaurarTela);
            }
            else if (nextCell == 'V') {
                TransicaoDeMapa::exibirTransicaoParaVila();
                ctx.self->exploracaoEstaAtiva = false;
            }
            else if (nextCell == 'T' && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                ControleDeMapa::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDoCoracaoDaArvoreSalva, ctx.self->coracaoDaArvoreJaFoiVisitado, AbominacaoFloresta::obterMapaCoracaoDaArvore(), 10, 3, "CORACAO DA ARVORE", ctx.restaurarTela);
            }
            else if (nextCell == 'L' && ctx.self->jogadorEstaDentroDeUmSubMapa) {
                if (!ctx.self->jogadorAtual->obterLabirintoDesbloqueado()) {
                    SimplificacoesAparencia::limparTela();
                    GerenciadorMenu::exibirLogoDoJogo("PASSAGEM BLOQUEADA");
                    int espacosM = (ctx.larguraDoTerminal - 60) / 2;
                    std::cout << "\n" << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[SISTEMA]: A passagem esta selada por magia. Fale com Morgana.\n";
                    SimplificacoesAparencia::aguardarEnter();
                    ctx.restaurarTela();
                    return;
                }

                if (ctx.self->tituloDoMapaAtual == "CABANA DA BRUXA") ctx.self->matrizDoMapaDaCabanaSalva = ctx.self->matrizDoMapaAtual;

                if (!ctx.self->labirintoJaFoiVisitado) {
                    ctx.self->matrizDoMapaAtual = {
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
                    ctx.self->labirintoJaFoiVisitado = true;
                } else {
                    ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaDoLabirintoSalva;
                }

                ctx.self->posicaoXDoJogador = 3;
                ctx.self->posicaoYDoJogador = 13;
                ctx.self->tituloDoMapaAtual = "LABIRINTO SUBTERRANEO";
                ctx.restaurarTela();
            }
            else if (nextCell == 'S' && ctx.self->jogadorEstaDentroDeUmSubMapa) {
                if (ctx.self->tituloDoMapaAtual == "CORACAO DA ARVORE") ctx.self->matrizDoMapaDoCoracaoDaArvoreSalva = ctx.self->matrizDoMapaAtual;
                else if (ctx.self->tituloDoMapaAtual == "CABANA DA BRUXA") ctx.self->matrizDoMapaDaCabanaSalva = ctx.self->matrizDoMapaAtual;
                else if (ctx.self->tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") ctx.self->matrizDoMapaDoLabirintoSalva = ctx.self->matrizDoMapaAtual;

                if (ctx.self->tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") {
                    ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaDaCabanaSalva;
                    ctx.self->posicaoXDoJogador = 20;
                    ctx.self->posicaoYDoJogador = 2;
                    ctx.self->tituloDoMapaAtual = "CABANA DA BRUXA";
                } else {
                    ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaPrincipalSalva;
                    ctx.self->posicaoXDoJogador = ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa;
                    ctx.self->posicaoYDoJogador = ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa;
                    ctx.self->jogadorEstaDentroDeUmSubMapa = false;
                    ctx.self->tituloDoMapaAtual = "FLORESTA";
                }
                ctx.restaurarTela();
            }
            else if (nextCell == 'E' && ctx.self->tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") {
                SimplificacoesAparencia::limparTela();
                GerenciadorMenu::exibirLogoDoJogo("FIM DO LABIRINTO");
                int espacosM = (ctx.larguraDoTerminal - 60) / 2;
                std::cout << "\n" << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[SISTEMA]: Voce encontrou a saida! Destino em breve...\n";
                SimplificacoesAparencia::aguardarEnter();

                ctx.self->posicaoXDoJogador = 3;
                ctx.self->posicaoYDoJogador = 13;
                if (ctx.self->exploracaoEstaAtiva) ctx.restaurarTela();
            } else {
                ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;
            }
        }
    };
}

void Mapa2Floresta::inicializarInteracoes() {
    interacoes['S'] = std::make_unique<InteracaoSlime>();
    interacoes['F'] = std::make_unique<InteracaoFada>();
    interacoes['A'] = std::make_unique<InteracaoAbominacao>();
    interacoes['m'] = std::make_unique<InteracaoAbominacao>();
    interacoes['M'] = std::make_unique<InteracaoMorgana>();
    interacoes['B'] = std::make_unique<InteracaoBau>();
    interacoes['^'] = std::make_unique<InteracaoTeleporte>();
}

void Mapa2Floresta::iniciarLoopDeExploracaoDoMapa()
{
    inicializarInteracoes();

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
            std::string corFundoRoxa = SimplificacoesAparencia::cor(Cor::FUNDO_MAGENTA);
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
        
        auto it = interacoes.find(celulaDestinoDoMapa);
        if (it != interacoes.end()) {
            ContextoInteracaoFloresta ctx = {this, proximaPosicaoX, proximaPosicaoY, larguraDoTerminal, restaurarTela, celulaDestinoDoMapa};
            it->second->processar(ctx);
        } else {
            bool ehParede = false;
            if (tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") {
                ehParede = (celulaDestinoDoMapa != ' ' && celulaDestinoDoMapa != '^' && celulaDestinoDoMapa != 'S' && celulaDestinoDoMapa != 'E' && celulaDestinoDoMapa != 'B');
            } else {
                ehParede = (celulaDestinoDoMapa == '#');
            }
            if (!ehParede) {
                posicaoXDoJogador = proximaPosicaoX;
                posicaoYDoJogador = proximaPosicaoY;
            }
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
