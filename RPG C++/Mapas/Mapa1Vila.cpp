#include "Mapa1Vila.h"

#include <iostream>
#include <vector>
#include <memory>
#include <utility>

#include "Mapa2Floresta.h"
#include "../Telas/TelaMenu.h"
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
#include "../Utilidades/Aparencia.h"
#include "ControleDeMapa.h"
#include "../Utilidades/ControleDeInput.h"
#include "../Utilidades/GeradorAleatorio.h"
#include "MapaInteracao.h"
#include "LayoutsMapa1Vila.h"

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
    matrizDoMapaAtual = LayoutsMapa1Vila::obterLayoutVilaInicial();
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
            ControleDeMapa::processarCombate(ctx.self->jogadorAtual, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "ENCONTRO NA CAVERNA", "Voce encontrou um Ork!", GerenciadorInimigos::criarInimigoOrkExilado(1), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
        }
    };

    class InteracaoNPCBjorn : public InteracaoVila {
    public:
        void processar(ContextoInteracaoVila& ctx) override {
            if (ctx.self->tituloDoMapaAtual == "FORJA DA VILA" && ctx.celula == 'B') {
                NPCBjorn interacaoBjorn;
                interacaoBjorn.interagir(ctx.self->jogadorAtual);
            } else if (ctx.self->tituloDoMapaAtual == "CAVERNA DO ORK") {
                Aparencia::limparTela();
                Aparencia::exibirCabecalho("RESGATE NA CAVERNA", Cor::AMARELO);
                int espacosM = std::max(0, (ctx.larguraDoTerminal - 50) / 2);
                std::string mE(espacosM, ' ');
                std::cout << "\n" << mE << "[Bjorn]: Pelos deuses, muito obrigado por me salvar!\n";
                std::cout << mE << "[Bjorn]: Passe na Forja e eu ajudarei voce!\n";
                ctx.self->bjornResgatado = true;

                ctx.self->matrizDoMapaAtual[ctx.proximaPosicaoY][ctx.proximaPosicaoX] = '.';
                Aparencia::aguardarEnter();
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
            if (ctx.self->tituloDoMapaAtual == "LOJA DA VILA") { // This check is redundant now, as interagir() handles the loop
                NPCFranchesco interacaoFranchesco;
                interacaoFranchesco.interagir(ctx.self->jogadorAtual);
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
                ControleDeMapa::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDaCavernaSalva, ctx.self->cavernaJaFoiVisitada, LayoutsMapa1Vila::obterLayoutCaverna(ctx.self->bjornResgatado), 16, 2, "CAVERNA DO ORK", ctx.restaurarTela);
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
                    Aparencia::limparTela();
                Aparencia::exibirCabecalho(ctx.self->tituloDoMapaAtual, Cor::AMARELO);
                    int espacosM = std::max(0, (ctx.larguraDoTerminal - 60) / 2);
                    std::cout << "\n" << std::string(espacosM, ' ') << "[SISTEMA]: A Forja esta trancada. O ferreiro sumiu...\n";
                    Aparencia::aguardarEnter();
                    ctx.restaurarTela();
                    return;
                }
                ControleDeMapa::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDaForjaSalva, ctx.self->forjaJaFoiVisitada, LayoutsMapa1Vila::obterLayoutForja(), 8, 2, "FORJA DA VILA", ctx.restaurarTela);
            }
            else if (nextCell == 'L' && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                ControleDeMapa::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDaLojaSalva, ctx.self->lojaJaFoiVisitada, LayoutsMapa1Vila::obterLayoutLoja(), 8, 2, "LOJA DA VILA", ctx.restaurarTela);
            }
            else if (nextCell == 'F' && nextNextCell == 'l' && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                TransicaoDeMapa::exibirTransicaoParaFloresta();
                Mapa2Floresta mapaFloresta(ctx.self->jogadorAtual);
                mapaFloresta.iniciarLoopDeExploracaoDoMapa();
                if (ctx.self->jogadorAtual->obterVoltarProMenu()) {
                    ctx.self->exploracaoEstaAtiva = false;
                    return;
                }
                ctx.self->matrizDoMapaAtual = ctx.self->mapaBaseDaVila;
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
    interacoes['B'] = std::make_unique<InteracaoNPCBjorn>();
    interacoes['F'] = std::make_unique<InteracaoNPCFranchesco>();
    interacoes['^'] = std::make_unique<InteracaoTeleporte>();
}

void Mapa1Vila::iniciarLoopDeExploracaoDoMapa1Vila()
{
    exploracaoEstaAtiva = true;
    tituloDoMapaAtual = "VILA INICIAL";
    inicializarInteracoes();

    ControleDeMapa::padronizarTamanhoDoMapa(matrizDoMapaAtual);

    Aparencia::ocultarCursor();

    Aparencia::limparTela();
    Aparencia::exibirCabecalho(tituloDoMapaAtual, Cor::AMARELO);

    int linhaInicialParaDesenharOMapa = Aparencia::obterPosicaoCursorY();

    // Lambda para restaurar a tela apos eventos sem piscar
    auto restaurarTela = [&]() {
        Aparencia::limparTela();
        Aparencia::exibirCabecalho(tituloDoMapaAtual, Cor::AMARELO);
        linhaInicialParaDesenharOMapa = Aparencia::obterPosicaoCursorY();
    };

    // Mapa base da vila — reutilizado no respawn apos a floresta

    if (mapaBaseDaVila.empty()) mapaBaseDaVila = matrizDoMapaAtual; // Salva o estado inicial para respawn

    auto processarInteracao = [&](int proximaPosicaoX, int proximaPosicaoY, int larguraDoTerminal) {
        char celulaDestinoDoMapa = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX];
        auto it = interacoes.find(celulaDestinoDoMapa);
        if (it != interacoes.end()) {
            ContextoInteracaoVila ctx = {this, proximaPosicaoX, proximaPosicaoY, larguraDoTerminal, restaurarTela, celulaDestinoDoMapa};
            it->second->processar(ctx);
        } else {
            bool ehParede = (celulaDestinoDoMapa == '#');
            if (!ehParede || jogadorAtual->isNoclip()) {
                posicaoXDoJogador = proximaPosicaoX;
                posicaoYDoJogador = proximaPosicaoY;
            }
        }
    };

    auto renderizarMapa = [&](int larguraDoTerminal, int alturaDoTerminal, int linhaInicial) 
    {
        int startX, endX;
        ControleDeMapa::calcularCameraHorizontal(larguraDoTerminal, posicaoXDoJogador, matrizDoMapaAtual.empty() ? 0 : static_cast<int>(matrizDoMapaAtual[0].length()), startX, endX);

        std::string margemEsquerdaDoMapa1Vila = ControleDeMapa::calcularMargemCentralizada(larguraDoTerminal, endX - startX);

        std::string textoDeControlesDoJogador = "W,A,S,D: Mover | I: Inventario | C: Ficha | B: Bestiario ";
        std::string margemEsquerdaDosControles = ControleDeMapa::calcularMargemCentralizada(larguraDoTerminal, textoDeControlesDoJogador.length());

        Aparencia::moverCursor(0, linhaInicial);

        int startY, endY;
        ControleDeMapa::calcularCameraVertical(alturaDoTerminal, posicaoYDoJogador, static_cast<int>(matrizDoMapaAtual.size()), startY, endY);

        for (int y = startY; y < endY; y++)
        {
            std::string linhaSendoRenderizada = margemEsquerdaDoMapa1Vila;
            linhaSendoRenderizada.reserve(margemEsquerdaDoMapa1Vila.size() + (endX - startX) + 20);
            for (int x = startX; x < endX; x++)
            {
                if (x == posicaoXDoJogador && y == posicaoYDoJogador)
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << Aparencia::cor(Cor::NEGRITO, Cor::VERDE) << '@' << Aparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'G')
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << Aparencia::cor(Cor::NEGRITO, Cor::VERMELHO) << 'G' << Aparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'O')
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << Aparencia::cor(Cor::NEGRITO, Cor::VERMELHO) << 'O' << Aparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'B')
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << Aparencia::cor(Cor::NEGRITO, Cor::CIANO) << 'B' << Aparencia::cor(Cor::RESET);
                }
                else if (matrizDoMapaAtual[y][x] == 'F' && x > 0 && matrizDoMapaAtual[y][x-1] == '.')
                {
                    std::cout << linhaSendoRenderizada;
                    linhaSendoRenderizada = "";
                    std::cout << Aparencia::cor(Cor::NEGRITO, Cor::AMARELO) << 'F' << Aparencia::cor(Cor::RESET);
                }
                else linhaSendoRenderizada += matrizDoMapaAtual[y][x];
            }
            std::cout << linhaSendoRenderizada << "\n";
        }
        std::cout << "\n" << margemEsquerdaDosControles << textoDeControlesDoJogador << std::flush;
    };

    while (exploracaoEstaAtiva && jogadorAtual->obterVida() > 0)
    {
        int larguraDoTerminal = Aparencia::obterLarguraTerminal();
        int alturaDoTerminal = Aparencia::obterAlturaTerminal();

        renderizarMapa(larguraDoTerminal, alturaDoTerminal, linhaInicialParaDesenharOMapa);

        char teclaPressionadaPeloJogador = ControleDeInput::lerTecla();

        int proximaPosicaoX = posicaoXDoJogador;
        int proximaPosicaoY = posicaoYDoJogador;

        bool abriuMenu = ControleDeMapa::processarInputEComandos(teclaPressionadaPeloJogador, jogadorAtual, proximaPosicaoX, proximaPosicaoY, restaurarTela);
        
        if (jogadorAtual->obterVoltarProMenu()) break;
        if (abriuMenu) continue;

        ControleDeMapa::aplicarLimitesDeMapa(proximaPosicaoX, proximaPosicaoY, matrizDoMapaAtual);

        processarInteracao(proximaPosicaoX, proximaPosicaoY, larguraDoTerminal);
    }
}
