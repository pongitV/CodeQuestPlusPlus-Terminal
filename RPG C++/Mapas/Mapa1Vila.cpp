#include "Mapa1Vila.h"

#include <iostream>
#include <vector>
#include <windows.h>
#include <memory>
#include <utility>

#include "Mapa2Floresta.h"
#include "../Gerenciadores/GerenciadorMenu.h"
#include "../Gerenciadores/GerenciadorInimigos.h"
#include "../Gerenciadores/GerenciadorCombate.h"
#include "../Inventario/Item.h"
#include "../Inventario/InventarioCombate.h"
#include "../Telas/TelaAtributos.h"
#include "../Telas/TelaBestiario.h"
#include "../NPCs/NPCBjorn.h"
#include "../NPCs/NPCFranchesco.h"
#include "../Inimigos/OrkExilado.h"
#include "TransicaoDeMapa.h"
#include "../Utilidades/SimplificacoesAparencia.h"
#include "ControleDeMapa.h"
#include "../Utilidades/ControleDeInput.h"
#include "../Utilidades/GeradorAleatorio.h"
#include "MapaInteracao.h"

Mapa1Vila::Mapa1Vila(SistemaPersonagem* personagemJogador) :
    posicaoXDoJogador(2), 
    posicaoYDoJogador(2), 
    jogadorAtual(personagemJogador), 
    exploracaoEstaAtiva(true),
    tituloDoMapaAtual("VILA INICIAL"),
    posicaoXSalvaAntesDeEntrarNoSubMapa(0), 
    posicaoYSalvaAntesDeEntrarNoSubMapa(0),
    jogadorEstaDentroDeUmSubMapa(false),
    bjornResgatado(false), 
    forjaJaFoiVisitada(false), 
    lojaJaFoiVisitada(false), 
    cavernaJaFoiVisitada(false)
{
    matrizDoMapaAtual = {
        "             #####################################################################################################################################",
        " #################################################################################################################################################",
        "##.###############......#######################..................................................#################################################",
        "##................................................................................................################################################",
        "##................................................................................................################################################",
        "####..............................................................................................################################################",
        "###................................_____........_____...._____....................................################################################",
        "##................................| { } |......| {%} |..| { } |.........**........................################################################",
        "##................................|  _  |......|  _  |..|  _  |........****.......................################################################",
        "###...............................| | | |......| | | |..| | | |.........||..........................##############################################",
        "##................................._____.......[^Forja].................||..**.......................####^C#######################################",
        "###...............................| {$} |..................................****........................##...######################################",
        "####..............................|  _  |...................................||...................................#################################",
        "#####.............................| | | |...................................||....................................################################",
        "##................................[^Loja].................~~~~~~~~~~~............................G......................##########################",
        "##..............G...............................~~~~~~~~~~~~~~~~~~~~~~~~..................................................########################",
        "####......................**..................~~~~~~~~~~~~~~~~~~~~~~~~~~~~..................................................######################",
        "##.......................****...............~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~........G.........................................####################",
        "###.......................||................~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~...................................................###################",
        "###.......................||..................~~~~~~~~~~~~~~~~~~~~~~~~~~~~......................................................##################",
        "#####............................................~~~~~~~~~~~~~~~~~~~~~~...................**..........G..........................#################",
        "##..................................................~~~~~~~~~~~~~~~......................****....................................#################",
        "###.................................G.....................................................||.....................................#################",
        "####......................................................................................||.....................................#################",
        "#####...........................................................................G.................................G...............################",
        "###...............................................................................................................................################",
        "##..........G....................G..........................G.....................................................................################",
        "##............................................................................................G....................................###############",
        "###................................................................................................................................###############",
        "#####...................................................................G..........................................................###############",
        "####...............................................................................................................................###############",
        "###..........................................G.............................................................G.......................###############",
        "#####..............................................................................................................................###############",
        "###................................................................................................................................###############",
        "#####...........############.............################.......................................................................##################",
        "##########################################################################################################################^Floresta###############",
        "    ##############################################################################################################################################",
        "             ######                                                                                            ###################################"
    };
}

Mapa1Vila::~Mapa1Vila() = default;

namespace {
    class InteracaoCombateGoblin : public InteracaoVila {
    public:
        void processar(ContextoInteracaoVila& ctx) override {
            ControleDeMapa::processarCombate(ctx.self->jogadorAtual, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "ENCONTRO INESPERADO", "Voce encontrou uma horda de Goblins!", GerenciadorInimigos::criarInimigoGoblin(GeradorAleatorio::obterInteiro(1, 3)), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
        }
    };

    class InteracaoCombateOrk : public InteracaoVila {
    public:
        void processar(ContextoInteracaoVila& ctx) override {
            int rootX = (ctx.celula == 'O') ? ctx.proximaPosicaoX : ctx.proximaPosicaoX - 1;
            ControleDeMapa::processarCombate(ctx.self->jogadorAtual, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "ENCONTRO NA CAVERNA", "Voce encontrou um Ork [m]!", GerenciadorInimigos::criarInimigoOrkExilado(1), ctx.proximaPosicaoX, ctx.proximaPosicaoY, rootX, 2, ctx.larguraDoTerminal, ctx.restaurarTela);
        }
    };

    class InteracaoNPCBjorn : public InteracaoVila {
    public:
        void processar(ContextoInteracaoVila& ctx) override {
            if (ctx.self->tituloDoMapaAtual == "FORJA DA VILA" && ctx.celula == 'B') {
                NPCBjorn::interagir(ctx.self->jogadorAtual);
            } else if (ctx.self->tituloDoMapaAtual == "CAVERNA DO ORK") {
                SimplificacoesAparencia::limparTela();
                GerenciadorMenu::exibirLogoDoJogo("RESGATE NA CAVERNA");
                int espacosM = std::max(0, (ctx.larguraDoTerminal - 50) / 2);
                std::string mE(espacosM, ' ');
                std::cout << "\n" << mE << "[Bjorn]: Pelos deuses, muito obrigado por me salvar!\n";
                std::cout << mE << "[Bjorn]: Passe na Forja e eu ajudarei voce!\n";
                ctx.self->bjornResgatado = true;

                int rootX = (ctx.celula == 'B') ? ctx.proximaPosicaoX : ctx.proximaPosicaoX - 1;
                ctx.self->matrizDoMapaAtual[ctx.proximaPosicaoY][rootX] = '.';
                ctx.self->matrizDoMapaAtual[ctx.proximaPosicaoY][rootX + 1] = '.';
                SimplificacoesAparencia::aguardarEnter();
            } else {
                ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;
            }
            if (ctx.self->exploracaoEstaAtiva) ctx.restaurarTela();
        }
    };

    class InteracaoNPCFranchesco : public InteracaoVila {
    public:
        void processar(ContextoInteracaoVila& ctx) override {
            if (ctx.self->tituloDoMapaAtual == "LOJA DA VILA") {
                NPCFranchesco::interagir(ctx.self->jogadorAtual);
                if (ctx.self->exploracaoEstaAtiva) ctx.restaurarTela();
            } else {
                ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;
            }
        }
    };

    class InteracaoTeleporte : public InteracaoVila {
    public:
        void processar(ContextoInteracaoVila& ctx) override {
            char nextCell = ctx.self->matrizDoMapaAtual[ctx.proximaPosicaoY][ctx.proximaPosicaoX+1];
            char nextNextCell = ctx.self->matrizDoMapaAtual[ctx.proximaPosicaoY][ctx.proximaPosicaoX+2];
            
            if (nextCell == 'C' && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                ControleDeMapa::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDaCavernaSalva, ctx.self->cavernaJaFoiVisitada, OrkExilado::obterMapaCaverna(ctx.self->bjornResgatado), 16, 2, "CAVERNA DO ORK", ctx.restaurarTela);
            }
            else if (nextCell == 'S' && ctx.self->jogadorEstaDentroDeUmSubMapa) {
                if (ctx.self->tituloDoMapaAtual == "CAVERNA DO ORK") ctx.self->matrizDoMapaDaCavernaSalva = ctx.self->matrizDoMapaAtual;
                else if (ctx.self->tituloDoMapaAtual == "LOJA DA VILA") ctx.self->matrizDoMapaDaLojaSalva = ctx.self->matrizDoMapaAtual;
                else if (ctx.self->tituloDoMapaAtual == "FORJA DA VILA") ctx.self->matrizDoMapaDaForjaSalva = ctx.self->matrizDoMapaAtual;

                ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaPrincipalSalva;
                ctx.self->posicaoXDoJogador = ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa;
                ctx.self->posicaoYDoJogador = ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa;
                ctx.self->jogadorEstaDentroDeUmSubMapa = false;
                ctx.self->tituloDoMapaAtual = "VILA INICIAL";
                ctx.restaurarTela();
            }
            else if (nextCell == 'F' && nextNextCell == 'o' && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                if (!ctx.self->bjornResgatado) {
                    SimplificacoesAparencia::limparTela();
                    GerenciadorMenu::exibirLogoDoJogo(ctx.self->tituloDoMapaAtual);
                    int espacosM = std::max(0, (ctx.larguraDoTerminal - 60) / 2);
                    std::cout << "\n" << std::string(espacosM, ' ') << "[SISTEMA]: A Forja esta trancada. O ferreiro sumiu...\n";
                    SimplificacoesAparencia::aguardarEnter();
                    ctx.restaurarTela();
                    return;
                }
                ControleDeMapa::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDaForjaSalva, ctx.self->forjaJaFoiVisitada, NPCBjorn::obterMapaForja(), 8, 2, "FORJA DA VILA", ctx.restaurarTela);
            }
            else if (nextCell == 'L' && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                ControleDeMapa::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDaLojaSalva, ctx.self->lojaJaFoiVisitada, NPCFranchesco::obterMapaLoja(), 8, 2, "LOJA DA VILA", ctx.restaurarTela);
            }
            else if (nextCell == 'F' && nextNextCell == 'l' && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                TransicaoDeMapa::exibirTransicaoParaFloresta();
                Mapa2Floresta mapaFloresta(ctx.self->jogadorAtual);
                mapaFloresta.iniciarLoopDeExploracaoDoMapa();
                if (ctx.self->jogadorAtual->obterVoltarProMenu()) {
                    ctx.self->exploracaoEstaAtiva = false;
                    return;
                }
                ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaPrincipalSalva;
                ctx.self->cavernaJaFoiVisitada = false;
                ctx.restaurarTela();
            }
            else {
                ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;
            }
        }
    };
}

void Mapa1Vila::inicializarInteracoes() {
    interacoes['G'] = std::make_unique<InteracaoCombateGoblin>();
    interacoes['O'] = std::make_unique<InteracaoCombateOrk>();
    interacoes['m'] = std::make_unique<InteracaoCombateOrk>();
    interacoes['B'] = std::make_unique<InteracaoNPCBjorn>();
    interacoes['n'] = std::make_unique<InteracaoNPCBjorn>();
    interacoes['F'] = std::make_unique<InteracaoNPCFranchesco>();
    interacoes['^'] = std::make_unique<InteracaoTeleporte>();
}

void Mapa1Vila::iniciarLoopDeExploracaoDoMapa1Vila()
{
    exploracaoEstaAtiva = true;
    tituloDoMapaAtual = "VILA INICIAL";
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

    // Lambda para restaurar a tela apos eventos sem piscar
    auto restaurarTela = [&]() {
        SimplificacoesAparencia::limparTela();
        GerenciadorMenu::exibirLogoDoJogo(tituloDoMapaAtual);
        GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
        linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
    };

    // Mapa base da vila — reutilizado no respawn apos a floresta

    const auto mapaBaseDaVila = matrizDoMapaAtual; // Salva o estado inicial para respawn

    auto processarInteracao = [&](int proximaPosicaoX, int proximaPosicaoY, int larguraDoTerminal) {
        char celulaDestinoDoMapa = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX];
        auto it = interacoes.find(celulaDestinoDoMapa);
        if (it != interacoes.end()) {
            ContextoInteracaoVila ctx = {this, proximaPosicaoX, proximaPosicaoY, larguraDoTerminal, restaurarTela, celulaDestinoDoMapa};
            it->second->processar(ctx);
        } else {
            bool ehParede = (celulaDestinoDoMapa == '#');
            if (!ehParede) {
                posicaoXDoJogador = proximaPosicaoX;
                posicaoYDoJogador = proximaPosicaoY;
            }
        }
    };

    auto renderizarMapa = [&](int larguraDoTerminal, int linhaInicial) 
    {
        int larguraDoMapaEmColunas = matrizDoMapaAtual.empty() ? 0 : matrizDoMapaAtual[0].length();
        int espacosParaCentralizarOMapa = (larguraDoTerminal - larguraDoMapaEmColunas) / 2;
        std::string margemEsquerdaDoMapa1Vila(espacosParaCentralizarOMapa > 0 ? espacosParaCentralizarOMapa : 0, ' ');

        std::string textoDeControlesDoJogador = "W,A,S,D: Mover | I: Inventario | C: Ficha | B: Bestiario ";
        int espacosParaCentralizarOsControles = (larguraDoTerminal - (int)textoDeControlesDoJogador.length()) / 2;
        std::string margemEsquerdaDosControles(espacosParaCentralizarOsControles > 0 ? espacosParaCentralizarOsControles : 0, ' ');

        COORD posicaoDoCursorNoTerminal;
        posicaoDoCursorNoTerminal.X = 0;
        posicaoDoCursorNoTerminal.Y = linhaInicial;
        SetConsoleCursorPosition(manipuladorDoTerminal, posicaoDoCursorNoTerminal);

        for (int y = 0; y < matrizDoMapaAtual.size(); y++)
        {
            std::string linhaSendoRenderizada = margemEsquerdaDoMapa1Vila;
            linhaSendoRenderizada.reserve(margemEsquerdaDoMapa1Vila.size() + matrizDoMapaAtual[y].size() + 20);
            for (int x = 0; x < matrizDoMapaAtual[y].size(); x++)
            {
                if (x == posicaoXDoJogador && y == posicaoYDoJogador)
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::VERDE) << '@' << SimplificacoesAparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'G')
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::VERMELHO) << 'G' << SimplificacoesAparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'O')
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::VERMELHO);
                    if (x + 1 < matrizDoMapaAtual[y].size() && matrizDoMapaAtual[y][x+1] == 'm') {
                        std::cout << "Om";
                        x++;
                    } else {
                        std::cout << 'O';
                    }
                    std::cout << SimplificacoesAparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'B')
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::CIANO);
                    if (x + 1 < matrizDoMapaAtual[y].size() && matrizDoMapaAtual[y][x+1] == 'n') {
                        std::cout << "Bn";
                        x++;
                    } else {
                        std::cout << 'B';
                    }
                    std::cout << SimplificacoesAparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'F' && x > 0 && matrizDoMapaAtual[y][x-1] == '.')
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::AMARELO) << 'F' << SimplificacoesAparencia::cor(Cor::RESET);
                }
                else linhaSendoRenderizada += matrizDoMapaAtual[y][x];
            }
            std::cout << linhaSendoRenderizada << "\n";
        }
        std::cout << "\n" << margemEsquerdaDosControles << textoDeControlesDoJogador << "\n";
    };

    while (exploracaoEstaAtiva && jogadorAtual->obterVida() > 0)
    {
        GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
        int larguraDoTerminal = informacoesDoBufferDaTela.srWindow.Right - informacoesDoBufferDaTela.srWindow.Left + 1;

        renderizarMapa(larguraDoTerminal, linhaInicialParaDesenharOMapa);

        char teclaPressionadaPeloJogador = ControleDeInput::lerTecla();

        int proximaPosicaoX = posicaoXDoJogador;
        int proximaPosicaoY = posicaoYDoJogador;

        bool abriuMenu = ControleDeMapa::processarInputEComandos(teclaPressionadaPeloJogador, jogadorAtual, proximaPosicaoX, proximaPosicaoY, restaurarTela);
        
        if (jogadorAtual->obterVoltarProMenu()) break;
        if (abriuMenu) continue;

        processarInteracao(proximaPosicaoX, proximaPosicaoY, larguraDoTerminal);
    }
}
