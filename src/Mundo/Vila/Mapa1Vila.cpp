#include "Mapa1Vila.h"

#include <iostream>
#include <vector>
#include <memory>
#include <utility>
#include <chrono>
#include <thread>

#include "../Floresta/Mapa2Floresta.h"
#include "../../Visoes/TelasBase/Menu/TelaMenu.h"
#include "../../Core/Controladores/CriadorInimigos.h"
#include "../../Sistemas/Combate/Combate.h"
#include "../../Sistemas/Inventario/Item.h"
#include "../../Sistemas/Inventario/InventarioCombate.h"
#include "../../Visoes/TelasBase/Atributos/TelaAtributos.h"
#include "../../Visoes/TelasBase/Bestiario/TelaBestiario.h"
#include "../../Entidades/NPCs/Ferreiro/NPCFerreiro.h"
#include "../../Entidades/NPCs/Mercador/NPCMercador.h"
#include "../../Entidades/Inimigos/OrcExilado.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Core/Utilidades/FuncoesDialogo.h"
#include "../ControleMapa.h"
#include "../../Core/Utilidades/ControleDeInput.h"
#include "../../Core/Utilidades/GeradorAleatorio.h"
#include "../MapaInteracao.h"
#include "../../Sistemas/Progresso/Progressao.h"
#include "../../Sistemas/Progresso/ProgressaoFlags.h"
#include "../../Sistemas/Progresso/Diario.h"
#include "Mapa1VilaLayout.h"

Mapa1Vila::Mapa1Vila(Personagem* personagemJogador) :
    posicaoXDoJogador(4), 
    posicaoYDoJogador(5), 
    jogadorAtual(personagemJogador), 
    exploracaoEstaAtiva(true),
    tituloDoMapaAtual("CAMINHO DO INICIO"),
    posicaoXSalvaAntesDeEntrarNoSubMapa(10), 
    posicaoYSalvaAntesDeEntrarNoSubMapa(4),
    jogadorEstaDentroDeUmSubMapa(true),
    bjornResgatado(Progressao::instancia().obterFlag(Flags::Vila_BjornResgatado)), 
    cavernaJaFoiVisitada(false),
    spawnJaFoiVisitado(true),
    proximoMapa(ProximaTransicaoMapa::Nenhuma),
    veioDaFloresta(false)
{
    matrizDoMapaPrincipalSalva = Mapa1VilaLayouts::obterLayoutVilaInicial();
    ControleMapa::padronizarTamanhoDoMapa(matrizDoMapaPrincipalSalva);

    matrizDoMapaAtual = Mapa1VilaLayouts::obterLayoutSpawn();
    ControleMapa::padronizarTamanhoDoMapa(matrizDoMapaAtual);

    mapaBaseDaVila = Mapa1VilaLayouts::obterLayoutVilaInicial();
    ControleMapa::padronizarTamanhoDoMapa(mapaBaseDaVila);

    if (!bjornResgatado) {
        for (auto& linha : matrizDoMapaPrincipalSalva) {
            std::replace(linha.begin(), linha.end(), 'B', 'P');
        }
        for (auto& linha : mapaBaseDaVila) {
            std::replace(linha.begin(), linha.end(), 'B', 'P');
        }
    }
}

Mapa1Vila::~Mapa1Vila() = default;

namespace {
    class InteracaoCombateGoblin : public InteracaoVila {
    public:
        void processar(ContextoInteracaoVila& ctx) override {
            ControleMapa::processarCombate(ctx.self->jogadorAtual, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "ENCONTRO INESPERADO", "Voce encontrou uma horda de Goblins!", CriadorInimigos::criarInimigoGoblin(GeradorAleatorio::obterInteiro(1, 3)), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
        }
    };

    class InteracaoCombateOrk : public InteracaoVila {
    public:
        void processar(ContextoInteracaoVila& ctx) override {
            ControleMapa::processarCombate(ctx.self->jogadorAtual, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "ENCONTRO NA CAVERNA", "Voce encontrou um Ork!", CriadorInimigos::criarInimigoOrkExilado(1), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
        }
    };

    class InteracaoNPCFerreiro : public InteracaoVila {
    public:
        void processar(ContextoInteracaoVila& ctx) override {
            if (ctx.self->tituloDoMapaAtual == "CAVERNA DO ORK") {
                Aparencia::iniciarInteracaoPopup();
                std::vector<std::string> falasBjorn = {
                    Aparencia::cor(Cor::CIANO) + "Bjorn:" + Aparencia::cor(Cor::RESET) + " Pelos deuses, muito obrigado por me salvar!",
                    Aparencia::cor(Cor::CIANO) + "Bjorn:" + Aparencia::cor(Cor::RESET) + " Passe na Forja e eu ajudarei voce!"
                };
                Aparencia::exibirPopup("RESGATE NA CAVERNA", falasBjorn, Cor::AMARELO);
                
                ctx.self->bjornResgatado = true;
                Progressao::instancia().definirFlag(Flags::Vila_BjornResgatado, true);

                ctx.self->matrizDoMapaAtual[ctx.proximaPosicaoY][ctx.proximaPosicaoX] = '.';
                
                // Atualiza os mapas salvos para que a Placa volte a ser o Bjorn na Vila
                for (auto& linha : ctx.self->matrizDoMapaPrincipalSalva) {
                    std::replace(linha.begin(), linha.end(), 'P', 'B');
                }
                for (auto& linha : ctx.self->mapaBaseDaVila) {
                    std::replace(linha.begin(), linha.end(), 'P', 'B');
                }
            } else if (ctx.self->tituloDoMapaAtual == "VILA INICIAL") {
                NPCFerreiro interacaoBjorn;
                interacaoBjorn.interagir(ctx.self->jogadorAtual);
                Diario::instancia().registrarNPC("Bjorn (Ferreiro)");
            } else {
                ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;
            }
            if (ctx.self->exploracaoEstaAtiva && !ControleMapa::isExploracao3DAtiva()) if (!ControleMapa::isExploracao3DAtiva()) ctx.restaurarTela();
        }
    };

    class InteracaoNPCMercador : public InteracaoVila {
    public:
        void processar(ContextoInteracaoVila& ctx) override {
            NPCMercador interacaoFranchesco;
            interacaoFranchesco.interagir(ctx.self->jogadorAtual);
            Diario::instancia().registrarNPC("Franchesco (Mercador)");
            if (ctx.self->exploracaoEstaAtiva && !ControleMapa::isExploracao3DAtiva()) if (!ControleMapa::isExploracao3DAtiva()) ctx.restaurarTela();
        }
    };

    class InteracaoPlaca : public InteracaoVila {
    public:
        void processar(ContextoInteracaoVila& ctx) override {
            Aparencia::iniciarInteracaoPopup();
            std::vector<std::string> msg = {
                "A Forja esta fechada.",
                "Uma placa diz: 'Fui a caverna a leste'."
            };
            Aparencia::exibirPopup("PLACA", msg, Cor::MARROM);
            if (ctx.self->exploracaoEstaAtiva && !ControleMapa::isExploracao3DAtiva()) if (!ControleMapa::isExploracao3DAtiva()) ctx.restaurarTela();
        }
    };

    class InteracaoTeleporte : public InteracaoVila {
    public:
        void processar(ContextoInteracaoVila& ctx) override {
            
            char nextCell = ' ';
            if (ctx.proximaPosicaoX + 1 < static_cast<int>(ctx.self->matrizDoMapaAtual[ctx.proximaPosicaoY].length())) {
                nextCell = ctx.self->matrizDoMapaAtual[ctx.proximaPosicaoY][ctx.proximaPosicaoX+1];
            }
            int px = ctx.proximaPosicaoX;
            int py = ctx.proximaPosicaoY;
            
            // 1. Entrar na Caverna a partir da Vila (X=121, Y=13)
            if (px == 121 && py == 13 && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                ControleMapa::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDaCavernaSalva, ctx.self->cavernaJaFoiVisitada, Mapa1VilaLayouts::obterLayoutCaverna(ctx.self->bjornResgatado), 14, 3, "CAVERNA DO ORK", ctx.animarTela);
            }
            // 2. Retornar dos Interiores/Caverna de volta para a Vila Inicial
            else if (nextCell == 'S' && ctx.self->jogadorEstaDentroDeUmSubMapa) {
                if (ctx.self->tituloDoMapaAtual == "CAVERNA DO ORK") {
                    ctx.self->cavernaJaFoiVisitada = false;
                }

                ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaPrincipalSalva;
                ControleMapa::padronizarTamanhoDoMapa(ctx.self->matrizDoMapaAtual);
                ctx.self->posicaoXDoJogador = ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa;
                ctx.self->posicaoYDoJogador = ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa;
                ctx.self->jogadorEstaDentroDeUmSubMapa = false;
                ctx.self->tituloDoMapaAtual = "VILA INICIAL";
                if (!ControleMapa::isExploracao3DAtiva()) ctx.restaurarTela();
            }
            // 3. Voltar para a Vila Inicial a partir do Caminho do Inicio (X=54, Y=7)
            else if (px == 54 && py == 7 && ctx.self->tituloDoMapaAtual == "CAMINHO DO INICIO") {
                ctx.self->matrizDoMapaDoSpawnSalva = ctx.self->matrizDoMapaAtual;
                ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaPrincipalSalva;
                ControleMapa::padronizarTamanhoDoMapa(ctx.self->matrizDoMapaAtual);
                ctx.self->posicaoXDoJogador = 17;
                ctx.self->posicaoYDoJogador = 5;
                ctx.self->jogadorEstaDentroDeUmSubMapa = false;
                ctx.self->tituloDoMapaAtual = "VILA INICIAL";
                if (!ControleMapa::isExploracao3DAtiva()) ctx.animarTela();
            }
            // 4. Entrar no Caminho do Inicio (Spawn) a partir da Vila Inicial (X=18, Y=5)
            else if (px == 18 && py == 5 && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                ControleMapa::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDoSpawnSalva, ctx.self->spawnJaFoiVisitado, Mapa1VilaLayouts::obterLayoutSpawn(), 53, 7, "CAMINHO DO INICIO", ctx.animarTela);
            }
            // 7. Seguir caminho para a Floresta (X=133, Y=50)
            else if (px == 133 && py == 50 && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                if (!Progressao::instancia().obterFlag(Flags::Vila_BjornResgatado)) {
                    Aparencia::iniciarInteracaoPopup();
                    std::vector<std::string> msg = {
                        "Voce precisa ajudar os habitantes da vila antes de seguir jornada.",
                        "(Dica: Explore a caverna a leste da vila)."
                    };
                    Aparencia::exibirPopup("CAMINHO BLOQUEADO", msg, Cor::AMARELO);
                    return;
                }
                ctx.self->exploracaoEstaAtiva = false;
                ctx.self->proximoMapa = ProximaTransicaoMapa::Floresta;
                ctx.self->veioDaFloresta = true;
            }
            else {
                ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;
            }
        }
    };

    std::vector<std::string> obterLayoutOriginalVila(const std::string& titulo, bool bjornResgatado) {
        if (titulo == "CAVERNA DO ORK") return Mapa1VilaLayouts::obterLayoutCaverna(bjornResgatado);
        if (titulo == "CAMINHO DO INICIO") return Mapa1VilaLayouts::obterLayoutSpawn();
        auto layout = Mapa1VilaLayouts::obterLayoutVilaInicial();
        if (!bjornResgatado) {
            for (auto& linha : layout) {
                std::replace(linha.begin(), linha.end(), 'B', 'P');
            }
        }
        return layout;
    }
}

void Mapa1Vila::inicializarInteracoes() {
    interacoes['G'] = std::make_unique<InteracaoCombateGoblin>();
    interacoes['O'] = std::make_unique<InteracaoCombateOrk>();
    interacoes['B'] = std::make_unique<InteracaoNPCFerreiro>();
    interacoes['F'] = std::make_unique<InteracaoNPCMercador>();
    interacoes['P'] = std::make_unique<InteracaoPlaca>();
    interacoes['^'] = std::make_unique<InteracaoTeleporte>();
}

ProximaTransicaoMapa Mapa1Vila::iniciarLoopDeExploracao()
{
    inicializarInteracoes();

    if (veioDaFloresta) {
        matrizDoMapaAtual = mapaBaseDaVila;
        cavernaJaFoiVisitada = false;
        veioDaFloresta = false;
    }

    ControleMapa::padronizarTamanhoDoMapa(matrizDoMapaAtual);

    Aparencia::ocultarCursor();

    auto formatador = [&](char celula, int x, int y) -> std::string {
        if (x == posicaoXDoJogador && y == posicaoYDoJogador) {
            char ic = jogadorAtual->obterIconeJogador();
            if (ic <= 32 || ic > 126) ic = '@'; 
            return Aparencia::cor(jogadorAtual->obterCorJogador()) + std::string(1, ic) + Aparencia::cor(Cor::RESET);
        }
        return ControleMapa::formatarCelula(celula, x, y, tituloDoMapaAtual, matrizDoMapaAtual, false);
    };

    bool precisaRenderizar = false;
    int linhaInicialParaDesenharOMapa = 0;

    auto restaurarTela = [&]() {
        linhaInicialParaDesenharOMapa = ControleMapa::animarIntroducaoMapa(tituloDoMapaAtual, {}, 0, {}, 0, Cor::AMARELO, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, false);
        precisaRenderizar = true;
    };

    auto animarTela = [&]() {
        std::vector<std::string> arteTitulo;
        int larguraArte = 0;
        std::vector<std::string> arteTrans;
        int larguraTrans = 0;
        std::function<void()> acaoNarracao = nullptr;
        bool usarAnimacaoBanner = true;

        if (tituloDoMapaAtual == "VILA INICIAL") {
            arteTitulo = Mapa1VilaLayouts::obterLogoVila();
            larguraArte = 125;
            arteTrans = Mapa1VilaLayouts::obterArteTransicaoVila();
            larguraTrans = 75;
        } else if (tituloDoMapaAtual == "CAMINHO DO INICIO") {
            arteTitulo = Mapa1VilaLayouts::obterLogoSpawn();
            larguraArte = 105;
            if (matrizDoMapaDoSpawnSalva.empty()) {
                usarAnimacaoBanner = false;
                std::vector<std::string> dialogoInicio = {
                    FuncoesDialogo::formatarMsgNarracao("Voce desperta nos arredores de um lugar desconhecido..."),
                    FuncoesDialogo::formatarMsgNarracao("Na sua vista, uma pequena vila sendo atacada por monstros."),
                    FuncoesDialogo::formatarMsgNarracao("Empunhando seu equipamento, voce sente que seu destino o aguarda."),
                    FuncoesDialogo::formatarMsgNarracao("Um novo capitulo se inicia agora.")
                };
                Aparencia::exibirTelaIntro(arteTitulo, dialogoInicio, Cor::BRANCO);
            }
        }
        
        linhaInicialParaDesenharOMapa = ControleMapa::animarIntroducaoMapa(tituloDoMapaAtual, arteTitulo, larguraArte, arteTrans, larguraTrans, Cor::AMARELO, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, true, usarAnimacaoBanner, acaoNarracao);
        precisaRenderizar = false;
    };

    // Substitui o Bjorn por uma placa se ele ainda nao foi resgatado (Antes de animar a tela)
    if (tituloDoMapaAtual == "VILA INICIAL" && !bjornResgatado) {
        for (auto& linha : matrizDoMapaAtual) {
            std::replace(linha.begin(), linha.end(), 'B', 'P');
        }
    }

    animarTela();

    if (mapaBaseDaVila.empty()) mapaBaseDaVila = matrizDoMapaPrincipalSalva;

    auto processarInteracao = [&](int proximaPosicaoX, int proximaPosicaoY, int larguraDoTerminal) {
        char celulaDestinoDoMapa = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX];
        
        auto it = interacoes.find(celulaDestinoDoMapa);
        bool ehFalsoF = (celulaDestinoDoMapa == 'F' && proximaPosicaoX > 0 && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX - 1] != '{');

        if (it != interacoes.end() && !ehFalsoF) {
            ContextoInteracaoVila ctx = {this, proximaPosicaoX, proximaPosicaoY, larguraDoTerminal, restaurarTela, celulaDestinoDoMapa, animarTela};
            it->second->processar(ctx);
        } else {
            std::string colisoes = "*#|_[]{}-=";
            bool ehParede = (colisoes.find(celulaDestinoDoMapa) != std::string::npos);
            if (!ehParede || jogadorAtual->isNoclip()) {
                posicaoXDoJogador = proximaPosicaoX;
                posicaoYDoJogador = proximaPosicaoY;
            }
        }
    };

    ProximaTransicaoMapa destinoViagemRapida = ControleMapa::executarLoopDeExploracao(
        jogadorAtual, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador,
        exploracaoEstaAtiva, tituloDoMapaAtual,
        [this]() { return "GO"; },
        [this]() { return obterLayoutOriginalVila(tituloDoMapaAtual, bjornResgatado); },
        processarInteracao, formatador, restaurarTela,
        linhaInicialParaDesenharOMapa, precisaRenderizar
    );

    if (destinoViagemRapida != ProximaTransicaoMapa::Nenhuma) {
        return destinoViagemRapida;
    }

    if (jogadorAtual->obterVida() <= 0 || jogadorAtual->obterVoltarProMenu()) {
        return ProximaTransicaoMapa::VoltarMenu;
    }
    return proximoMapa;
}
