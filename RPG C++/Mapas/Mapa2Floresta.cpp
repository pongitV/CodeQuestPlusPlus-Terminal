#include "Mapa2Floresta.h"

#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <memory>
#include <utility>
#include <functional>

#include "../Telas/TelaMenu.h"
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
#include "Mapa3Reino.h"

Mapa2Floresta::Mapa2Floresta(SistemaPersonagem* personagemJogador) :
    posicaoXDoJogador(31), 
    posicaoYDoJogador(17),
    jogadorAtual(personagemJogador), 
    posicaoXSalvaAntesDeEntrarNoSubMapa(0), 
    posicaoYSalvaAntesDeEntrarNoSubMapa(0),
    jogadorEstaDentroDeUmSubMapa(false),
    cabanaJaFoiVisitada(false), 
    coracaoDaArvoreJaFoiVisitado(false), 
    labirintoJaFoiVisitado(false),
    salaDoChefeJaFoiVisitada(false),
    exploracaoEstaAtiva(true), 
    tituloDoMapaAtual("FLORESTA")
{
    matrizDoMapaAtual =
    {
        "  **        **        **        **        **        **        **        **        **        **        **        **        **        **        **        **             ",
        " ****      ****      ****      ****      ****      ****      ****      ****      ****      ****      ****      ****      ****      ****      ****      ****            ",
        "  ||   **   ||   **   ||   **   ||   **   ||   **   ||   **   ||   **   ||   **   ||   **   ||   **   ||   **   ||   **   ||   **   ||   **   ||   **   ||             ",
        " **   **** **   **** **   **** **   **** **   **** **   **** **   **** **   **** **   **** **   **** **   **** **   **** **   **** **   **** **   **** **              ",
        "****   || ****   || ****   || ****   || ****   || ****   || ****   || ****   || ****   || ****   || ****   || ****   || ****   || ****   || ****   || ****             ",
        " ||        ||        ||        ||        ||        ||        ||        ||        ||        ||        ||        ||        ||        ||        ||        ||              ",
        "             #######################################################################################################################",
        "             ###########################################################################################################################",
        "             ##############################################################################################################################", 
        "   **        ##################################################################################################################################             **   ",
        "  ****       ######################################################################################################################################        ****  ",
        "   ||   **   #################...............**.................###.......##...........**.....######.........####......[^Cabana]####################   **   ||   ",
        "       ****  #############.[^V].............****......................................****......................**.....[;;   ;;].....################ ****       ",
        "   **   ||   #############...................||...................S....................||......................****....| |######.....###############   ||   **   ",
        "  ****       ##############..................||........................................||.......................||..................################       ****  ",
        "   ||   **   ##############............................................................................S........||..................###############   **    ||   ",
        "       ****  #############.....................................**...................................................................##############   ****       ",
        "   **   ||   #############....................................****................S................................................################   ||  **   ",
        "  ****       ###############.......................S...........||....................................................**............###############       ****  ",
        "   ||   **   ##############....................................||...................................................****...........###############   **   ||   ",
        "       ****  ##############..........................................................................................||...........###############   ****       ",
        "   **   ||   ##############.................**............................................**.........................||............###############   ||   **   ",
        "  ****       ##############................****........................S.................****.......................................##############       ****  ",
        "   ||   **   ##############.................||............................................||...............S........................#############    **   ||   ",
        "       ****  ##############.................||...........S................................||.........................................#############  ****       ",
        "   **   ||   ############................................................................................................S.........#############     ||   **   ",
        "  ****       #############.......................................................................**.................................#############        ****  ",
        "   ||   **   #############......................................................................****...............................#############   **     ||   ",
        "       ****  #############....................................**..................S..............||................................#############  ****       ",
        "   **   ||   ############....................S...............****................................||................................#############   ||     **   ",
        "  ****       #############....................................||....................................................F.......F.....###############        ****  ",
        "   ||   **   ##############...................................||...............................................F.......**...........############    **    ||   ",
        "       ****  #############............................................................................................****...F......#############  ****       ",
        "   **   ||   ############............................**.................S................**..................F.........||..........##############   ||   **   ",
        "  ****       ############...........................****................................****......................F....||.......F..##############       ****  ",
        "   ||   **   #############...........................||..................................||...........F..................:--........###########    **    ||   ",
        "       ****  ###############.........................||..................................||........................--=-+==-+--..--..############  ****       ",
        "   **   ||   ##############.........................................S......................................F....:-=-====--==--+=-..#############   ||     **   ",
        "  ****       ##############........................................................................F.............-+-====-------+-=-###############       ****  ",
        "   ||   **   #############.......**.............................................................................:++==++++++--=-**-..##############   **   ||   ",
        "       ****  ########...........****.............................**........................................F....:=++-*##=##%#***#-...############## ****       ",
        "   **   ||   #####...............||.............................****...............................F..................::%***#+-......############### ||    **   ",
        "  ****       ###.................||..............................||..............................................F......-*+*.......###############        ****  ",
        "   ||   **   ##|......|####......................................||........................................F.............%+*........###############  **    ||   ",
        "       ****  ##|......|##########..................................................................F.....................**-.......###############  ****       ",
        "   **   ||   ##|......|##########..............#######.......##############....................................F.......:%^T**.....################   ||    **   ",
        "  ****       ##|......|#########################################################################################################################          ****  ",
        "   ||        ##|......|####################################################################################################################                ||   ",
        "             ##|^Reino|####################################################################################################################",
        "             ########################################################################################################################",
        "             ######################################################################################################################",
        "  **        **        **        **        **        **        **        **        **        **        **        **        **        **        **        **             ",
        " ****      ****      ****      ****      ****      ****      ****      ****      ****      ****      ****      ****      ****      ****      ****      ****            ",
        "  ||   **   ||   **   ||   **   ||   **   ||   **   ||   **   ||   **   ||   **   ||   **   ||   **   ||   **   ||   **   ||   **   ||   **   ||   **   ||             ",
        " **   **** **   **** **   **** **   **** **   **** **   **** **   **** **   **** **   **** **   **** **   **** **   **** **   **** **   **** **   **** **              ",
        "****   || ****   || ****   || ****   || ****   || ****   || ****   || ****   || ****   || ****   || ****   || ****   || ****   || ****   || ****   || ****             ",
        " ||        ||        ||        ||        ||        ||        ||        ||        ||        ||        ||        ||        ||        ||        ||        ||              "
    };
}

Mapa2Floresta::~Mapa2Floresta() = default;

namespace {
    std::vector<std::string> obterMapaSalaDoChefe() {
        return {
            "                                                    .....                                                    ",
            "                                                 ...........                                                 ",
            "                                               ...............                                               ",
            "                                               ...............                                               ",
            "                                              .................                                              ",
            "                                              .................                                              ",
            "                                               ...............                                               ",
            "                    .......                     .............                      ......                    ",
            "                  ...........                     .........                     ...........                  ",
            "                ...............                      ...                      ...............                ",
            "                ...............                      ...                      ...............                ",
            "               .................             ...................             .................               ",
            "               .................        .............................         .................              ",
            "                ...............      ..........      ...      ..........      .................              ",
            "                 ...............   ........          ...          ........   ..................              ",
            "                    .......  ..........              ...              ........... .......                    ",
            "                               ......                ...                .......                              ",
            "                             ........                ...                ........                             ",
            "                            ...........              ...              ...........                            ",
            "                           .............            ...            .............                           ",
            "                          ...............          ...          ...............                          ",
            "                         .................        ...        .................                         ",
            "                         .....             .......      ...      .......             .....                   ",
            "                        .....               ....     ...     ....               .....                        ",
            "       ........         ....                 ...................                 ....         ........       ",
            "     ............       ....                   ...............                   ....       ............     ",
            "    ...............     ....                   ...............                   ....     ...............    ",
            "   .......................................................................................................   ",
            "   ..................................................MAHO.................................................   ",
            "   ................     ....                   ...............                   ....     ................   ",
            "    ..............      ....                   ...............                   ....      ..............    ",
            "      ..........        .....                 .................                 .....        ..........      ",
            "                         ....               .....    ...    .....               ....                         ",
            "                         .....            .....      ...      .....            .....                         ",
            "                          .....         .....        ...        .....         .....                          ",
            "                           .....      .....          ...          .....      .....                           ",
            "                            ......  .....            ...            .....  ......                            ",
            "                              .........              ...              .........                              ",
            "                               .......               ...               .......                               ",
            "                               .........             ...             .........                               ",
            "                     ......  ...............         ...         ...............  ......                     ",
            "                  ................................   ...  ................................                   ",
            "                 ..............          ...........................          ..............                 ",
            "                ................              .................              ................                ",
            "                ................                     ...                     ................                ",
            "                ................                     ...                     ................                ",
            "                 ..............                      ...                      ..............                 ",
            "                  ............                    .........                    ............                  ",
            "                     ......                     .............                     ......                     ",
            "                                               ...............                                               ",
            "                                               ...............                                               ",
            "                                               ...............                                               ",
            "                                               ...............                                               ",
            "                                                .............                                                ",
            "                                                 ...........                                                 ",
            "                                                    [^S]                                                     "
        };
    }

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
            TelaMenu::exibirLogoDoJogo("TESOURO ESCONDIDO");
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
            else if (nextCell == 'R' && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                TransicaoDeMapa::exibirTransicaoParaReino();
                Mapa3Reino mapaReino(ctx.self->jogadorAtual);
                mapaReino.iniciarLoopDeExploracaoDoMapa();
                if (ctx.self->jogadorAtual->obterVoltarProMenu()) {
                    ctx.self->exploracaoEstaAtiva = false;
                    return;
                }
                ctx.restaurarTela();
            }
            else if (nextCell == 'L' && ctx.self->jogadorEstaDentroDeUmSubMapa) {
                if (!ctx.self->jogadorAtual->obterLabirintoDesbloqueado()) {
                    SimplificacoesAparencia::limparTela();
                    TelaMenu::exibirLogoDoJogo("PASSAGEM BLOQUEADA");
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
                else if (ctx.self->tituloDoMapaAtual == "SALA DO CHEFE") ctx.self->matrizDoMapaSalaDoChefeSalva = ctx.self->matrizDoMapaAtual;

                if (ctx.self->tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") {
                    ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaDaCabanaSalva;
                    ctx.self->posicaoXDoJogador = 20;
                    ctx.self->posicaoYDoJogador = 2;
                    ctx.self->tituloDoMapaAtual = "CABANA DA BRUXA";
                } else if (ctx.self->tituloDoMapaAtual == "SALA DO CHEFE") {
                    ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaDoLabirintoSalva;
                    ctx.self->posicaoXDoJogador = 96; 
                    ctx.self->posicaoYDoJogador = 13;
                    ctx.self->tituloDoMapaAtual = "LABIRINTO SUBTERRANEO";
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
                TelaMenu::exibirLogoDoJogo("FIM DO LABIRINTO");
                int espacosM = (ctx.larguraDoTerminal - 60) / 2;
                std::string margem(espacosM > 0 ? espacosM : 0, ' ');
                
                std::cout << "\n" << margem << "[SISTEMA]: Voce encontrou a saida do labirinto!\n";
                std::cout << margem << "[SISTEMA]: A sua frente, uma escadaria desce para uma caverna escura.\n";
                std::cout << margem << "[SISTEMA]: No fundo, parece haver um mar de liquido preto raso...\n\n";
                std::cout << margem << "[1] Descer a escadaria\n";
                std::cout << margem << "[2] Voltar para a Cabana da Bruxa\n\n";
                std::cout << margem << "Escolha: ";

                int escolha;
                while (!(std::cin >> escolha) || (escolha != 1 && escolha != 2)) {
                    std::cin.clear(); std::cin.ignore(1000, '\n');
                    std::cout << margem << "Opcao invalida. Escolha (1 ou 2): ";
                }

                if (escolha == 1) {
                    SimplificacoesAparencia::limparTela();
                    std::vector<std::string> arteSimbolo = {
                        "                                                    ###%%                                                    ",
                        "                                                 *******####                                                 ",
                        "                                               ***********##%#                                               ",
                        "                                               **+++++*****##%                                               ",
                        "                                              **+++++++*****#%#                                              ",
                        "                                              **+++++++*****#%#                                              ",
                        "                                               ***+++++****###                                               ",
                        "                    ######%                     **********##%                      ######                    ",
                        "                  ******###%#                     *****###%                     #*******###                  ",
                        "                **********###%#                      +++                      ***+*******##%#                ",
                        "                **++++*****####                      +++                      *+++++++****##%                ",
                        "               **++++++****###%#             #*+++++++++++++++*#             **++++++++****###               ",
                        "               **+++++++****##%#        #++++++++++++++++++++++++++*#         *++++++++****###               ",
                        "                **+++++****####      *++++++++++     +++     ++++++++++*      **++++++****###                ",
                        "                 **********#%++#   ++++++++          +++          ++++++++   #++*********###                 ",
                        "                    *****##  ++++++++++              +++              +++++++++++ *****##                    ",
                        "                               ++++++                +++                +++++++                              ",
                        "                             *+++++++                +++                *++++++*                             ",
                        "                            +++++ ++++*              +++              *++++ +++++                            ",
                        "                           +++++    ++++*            +++            *++++    +++++                           ",
                        "                          +++++       ++++#          +++          *++++       +++++                          ",
                        "                         +++++          ++++#        +++        #++++          +++++                         ",
                        "                         ++++             ++++#      +++      #++++             ++++                         ",
                        "                        ++++*               ++++     +++     ++++               *++++                        ",
                        "       ######%#         ++++                 +++++#######%#+++++                 ++++         ########       ",
                        "     *******###%#       ++++                   ++********###++                   *+++       ********####     ",
                        "    ****++****#####     +++*                   *****+*****####                   *+++     #***********##%    ",
                        "   *+++++++****##%#++++++++++++++++++++++++++++*+++++++****##%++++++++++++++++++++++++++++**++++++*****###   ",
                        "   *+++++++****####+++++++++++++++++++++++++++++++++++++***##%++++++++++++++++++++++++++++**++++++++***##%   ",
                        "   **++++++****##%#     +++*                   **+++++++***##%                   *+++     **+++++++****###   ",
                        "    *++++******###      ++++                   #*++++++****#%#                   ++++      ***++++*****#%    ",
                        "      ********##        ++++*                 ++++********#*+++                 *++++        ********##      ",
                        "                         ++++               +++++    +**    +++++               ++++                         ",
                        "                         +++++            *++++      +++      ++++*            +++++                         ",
                        "                          +++++         *++++        +++        ++++*         +++++                          ",
                        "                           +++++      *++++          +++          ++++#      +++++                           ",
                        "                            +++++#  #++++            +++            ++++#  #+++++                            ",
                        "                              +++++++++              +++              +++++++++                              ",
                        "                               ++++++#               +++               #++++++                               ",
                        "                               ++++++++#             +++             #++++++++                               ",
                        "                     ###%%#  +++++ +++++++*#         +++         #*+++++++ ++++*  %#%%%%                     ",
                        "                  #*****###%%+++      +++++++++*##%  +++  %%#*+++++++++      +++****#####%#                  ",
                        "                 **********##%#          +++++++++++++++++++++++++++          **********##%#                 ",
                        "                ****+++*****##%#              +++++++++++++++++              ****+++*****##%#                ",
                        "                **+++++++***##%#                     +++                     **+++++++****###                ",
                        "                *++++++++***##%#                     +++                     **+++++++****###                ",
                        "                 *++++++****##%                      +++                      **++++++****#%                 ",
                        "                  *********##%                    ######%##                    **********##                  ",
                        "                     ****##                     #*******###%#                     *****#                     ",
                        "                                               ***********##%#                                               ",
                        "                                               ***++++*****##%                                               ",
                        "                                               *+++++++*****#%                                               ",
                        "                                               *++++++++***###                                               ",
                        "                                                *++++++****##                                                ",
                        "                                                 *********#%                                                 ",
                        "                                                     ###                                                     "
                    };
                    
                    std::cout << "\n\n";
                    SimplificacoesAparencia::imprimirCentralizadoMultilinha(arteSimbolo, 109, SimplificacoesAparencia::cor(Cor::BRANCO));
                    
                    std::cout << "\n" << margem << "[SISTEMA]: O ar aqui embaixo e gelado, cortante\n";
                    std::cout << margem << "[SISTEMA]: o liquido preto no chao e raso e liso como vidro\n";
                    std::cout << margem << "[SISTEMA]: Tudo e escuridao, exceto pelo brilho pulsante da\n";
                    std::cout << margem << "[SISTEMA]: enorme runa magica desenhada no fundo da caverna\n\n";
                    
                    std::cout << margem << SimplificacoesAparencia::cor(Cor::VERMELHO) << "[1] Seguir em frente" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
                    std::cout << margem << SimplificacoesAparencia::cor(Cor::BRANCO) << "[2] Voltar para a seguranca da Cabana" << SimplificacoesAparencia::cor(Cor::RESET) << "\n\n";
                    std::cout << margem << "Escolha: ";
                    
                    int escolhaBoss;
                    while (!(std::cin >> escolhaBoss) || (escolhaBoss != 1 && escolhaBoss != 2)) {
                        std::cin.clear(); std::cin.ignore(1000, '\n');
                        std::cout << margem << "Opcao invalida. Escolha (1 ou 2): ";
                    }
                    
                    if (escolhaBoss == 1) {
                        ctx.self->matrizDoMapaDoLabirintoSalva = ctx.self->matrizDoMapaAtual;
                        if (!ctx.self->salaDoChefeJaFoiVisitada) {
                            ctx.self->matrizDoMapaAtual = obterMapaSalaDoChefe();
                            ctx.self->salaDoChefeJaFoiVisitada = true;
                        } else {
                            ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaSalaDoChefeSalva;
                        }
                        ctx.self->posicaoXDoJogador = 54;
                        ctx.self->posicaoYDoJogador = 54;
                        ctx.self->tituloDoMapaAtual = "SALA DO CHEFE";
                        ctx.restaurarTela();
                    } else {
                        ctx.self->matrizDoMapaDoLabirintoSalva = ctx.self->matrizDoMapaAtual;
                        ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaDaCabanaSalva;
                        ctx.self->posicaoXDoJogador = 20;
                        ctx.self->posicaoYDoJogador = 2;
                        ctx.self->tituloDoMapaAtual = "CABANA DA BRUXA";
                    }
                } else {
                    ctx.self->matrizDoMapaDoLabirintoSalva = ctx.self->matrizDoMapaAtual;
                    ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaDaCabanaSalva;
                    ctx.self->posicaoXDoJogador = 20;
                    ctx.self->posicaoYDoJogador = 2;
                    ctx.self->tituloDoMapaAtual = "CABANA DA BRUXA";
                }
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
    TelaMenu::exibirLogoDoJogo(tituloDoMapaAtual);

    CONSOLE_SCREEN_BUFFER_INFO informacoesDoBufferDaTela;
    GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
    int linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;

    // Lambda para restaurar a tela apos eventos
    auto restaurarTela = [&]() {
        SimplificacoesAparencia::limparTela();
        TelaMenu::exibirLogoDoJogo(tituloDoMapaAtual);
        GetConsoleScreenBufferInfo(manipuladorDoTerminal, &informacoesDoBufferDaTela);
        linhaInicialParaDesenharOMapa = informacoesDoBufferDaTela.dwCursorPosition.Y;
    };

    auto renderizarMapa = [&](int larguraDoTerminal, int linhaInicial)
    {
        int larguraDoMapaEmColunas = matrizDoMapaAtual.empty() ? 0 : matrizDoMapaAtual[0].length();
        int espacosParaCentralizarOMapa = (larguraDoTerminal - larguraDoMapaEmColunas) / 2;
        std::string margemEsquerdaDoMapa(espacosParaCentralizarOMapa > 0 ? espacosParaCentralizarOMapa : 0, ' ');
        std::string margemDireitaDoMapa = "";

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
                else if (tituloDoMapaAtual == "SALA DO CHEFE" && (matrizDoMapaAtual[y][x] == 'M' || matrizDoMapaAtual[y][x] == 'A' || matrizDoMapaAtual[y][x] == 'H' || matrizDoMapaAtual[y][x] == 'O'))
                {
                    linhaSendoRenderizada += SimplificacoesAparencia::cor(Cor::NEGRITO, Cor::MAGENTA) + matrizDoMapaAtual[y][x] + SimplificacoesAparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == '.' && tituloDoMapaAtual == "SALA DO CHEFE")
                {
                    linhaSendoRenderizada += SimplificacoesAparencia::cor(Cor::CINZA) + "." + SimplificacoesAparencia::cor(Cor::RESET);
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
        
        if (tituloDoMapaAtual == "SALA DO CHEFE" && (celulaDestinoDoMapa == 'M' || celulaDestinoDoMapa == 'A' || celulaDestinoDoMapa == 'H' || celulaDestinoDoMapa == 'O')) {
            std::cout << "\n[SISTEMA]: A batalha contra MAHO estara disponivel em breve!\n";
            SimplificacoesAparencia::aguardarEnter();
            if (exploracaoEstaAtiva) restaurarTela();
            return;
        }
        
        auto it = interacoes.find(celulaDestinoDoMapa);
        if (it != interacoes.end()) {
            ContextoInteracaoFloresta ctx = {this, proximaPosicaoX, proximaPosicaoY, larguraDoTerminal, restaurarTela, celulaDestinoDoMapa};
            it->second->processar(ctx);
        } else {
            bool ehParede = false;
            if (tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") {
                ehParede = (celulaDestinoDoMapa != ' ' && celulaDestinoDoMapa != '^' && celulaDestinoDoMapa != 'S' && celulaDestinoDoMapa != 'E' && celulaDestinoDoMapa != 'B');
            } else if (tituloDoMapaAtual == "SALA DO CHEFE") {
                ehParede = (celulaDestinoDoMapa == ' ');
            } else {
                ehParede = (celulaDestinoDoMapa == '#');
            }
            if (!ehParede || jogadorAtual->isNoclip()) {
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

        // Limites do mapa para impedir crash quando noclip estiver ativo
        if (proximaPosicaoY < 0) proximaPosicaoY = 0; else if (proximaPosicaoY >= static_cast<int>(matrizDoMapaAtual.size())) proximaPosicaoY = static_cast<int>(matrizDoMapaAtual.size()) - 1;
        if (proximaPosicaoX < 0) proximaPosicaoX = 0; else if (proximaPosicaoX >= static_cast<int>(matrizDoMapaAtual[0].size())) proximaPosicaoX = static_cast<int>(matrizDoMapaAtual[0].size()) - 1;

        processarInteracao(proximaPosicaoX, proximaPosicaoY, larguraDoTerminal);
    }
}
