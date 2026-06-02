#include "Mapa1Vila.h"

#include <iostream>
#include <vector>
#include <memory>
#include <utility>
#include <chrono>
#include <thread>

#include "../Floresta/Mapa2Floresta.h"
#include "../../../Interface/Telas/Menu/TelaMenu.h"
#include "../../../Core/Controladores/CriadorInimigos.h"
#include "../../Combate/Combate.h"
#include "../../Inventario/Item.h"
#include "../../Inventario/InventarioCombate.h"
#include "../../../Interface/Telas/Atributos/TelaAtributos.h"
#include "../../../Interface/Telas/Bestiario/TelaBestiario.h"
#include "../../../Entidades/NPCs/Bjorn/NPCBjorn.h"
#include "../../../Entidades/NPCs/Franchesco/NPCFranchesco.h"
#include "../../../Entidades/Inimigos/OrcExilado.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/FuncoesDialogo.h"
#include "../ControleMapa.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Core/Utilidades/GeradorAleatorio.h"
#include "../MapaInteracao.h"
#include "../../Progresso/Progressao.h"
#include "../../Progresso/ProgressaoFlags.h"
#include "../../Progresso/Diario.h"
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
    forjaJaFoiVisitada(false), 
    lojaJaFoiVisitada(false), 
    cavernaJaFoiVisitada(false),
    spawnJaFoiVisitado(true),
    proximoMapa(ProximaTransicaoMapa::Nenhuma),
    veioDaFloresta(false)
{
    matrizDoMapaPrincipalSalva = Mapa1VilaLayouts::obterLayoutVilaInicial();
    matrizDoMapaAtual = Mapa1VilaLayouts::obterLayoutSpawn();
    mapaBaseDaVila = Mapa1VilaLayouts::obterLayoutVilaInicial();
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

    class InteracaoNPCBjorn : public InteracaoVila {
    public:
        void processar(ContextoInteracaoVila& ctx) override {
            if (ctx.self->tituloDoMapaAtual == "FORJA DA VILA" && ctx.celula == 'B') {
                NPCBjorn interacaoBjorn;
                interacaoBjorn.interagir(ctx.self->jogadorAtual);
                Diario::instancia().registrarNPC("Bjorn (Ferreiro)");
            } else if (ctx.self->tituloDoMapaAtual == "CAVERNA DO ORK") {
                Aparencia::limparTela();
            Aparencia::exibirPainelTexto("RESGATE NA CAVERNA", Cor::AMARELO);
                std::vector<std::string> falasBjorn = {
                    Aparencia::cor(Cor::CIANO) + "Bjorn:" + Aparencia::cor(Cor::RESET) + " Pelos deuses, muito obrigado por me salvar!",
                    Aparencia::cor(Cor::CIANO) + "Bjorn:" + Aparencia::cor(Cor::RESET) + " Passe na Forja e eu ajudarei voce!"
                };
                std::cout << "\n";
                Aparencia::imprimirBlocoCentralizado(falasBjorn);
                std::cout << "\n";
                ctx.self->bjornResgatado = true;
                Progressao::instancia().definirFlag(Flags::Vila_BjornResgatado, true);

                ctx.self->matrizDoMapaAtual[ctx.proximaPosicaoY][ctx.proximaPosicaoX] = '.';
                ControleDeInput::aguardarEnter();
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
                Diario::instancia().registrarNPC("Franchesco (Mercador)");
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
                ControleMapa::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDaCavernaSalva, ctx.self->cavernaJaFoiVisitada, Mapa1VilaLayouts::obterLayoutCaverna(ctx.self->bjornResgatado), 8, 3, "CAVERNA DO ORK", ctx.restaurarTela);
            }
            else if (nextCell == 'V' && nextNextCell == 'i') {
                if (ctx.self->tituloDoMapaAtual == "CAMINHO DO INICIO") {
                    ctx.self->matrizDoMapaDoSpawnSalva = ctx.self->matrizDoMapaAtual;
                    ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaPrincipalSalva;
                    ctx.self->posicaoXDoJogador = 10;
                    ctx.self->posicaoYDoJogador = 4;
                    ctx.self->jogadorEstaDentroDeUmSubMapa = false;
                    ctx.self->tituloDoMapaAtual = "VILA INICIAL";
                    ctx.animarTela();
                }
            }
            else if (nextCell == 'S' && ctx.self->jogadorEstaDentroDeUmSubMapa) {
                if (ctx.self->tituloDoMapaAtual == "CAVERNA DO ORK") {
                    ctx.self->cavernaJaFoiVisitada = false;
                }
                else if (ctx.self->tituloDoMapaAtual == "LOJA DA VILA") {
                    ctx.self->matrizDoMapaDaLojaSalva = ctx.self->matrizDoMapaAtual;
                }
                else if (ctx.self->tituloDoMapaAtual == "FORJA DA VILA") {
                    ctx.self->matrizDoMapaDaForjaSalva = ctx.self->matrizDoMapaAtual;
                }

                ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaPrincipalSalva;
                ctx.self->posicaoXDoJogador = ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa;
                ctx.self->posicaoYDoJogador = ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa;
                ctx.self->jogadorEstaDentroDeUmSubMapa = false;
                ctx.self->tituloDoMapaAtual = "VILA INICIAL";
                ctx.restaurarTela();
            }
            else if (nextCell == 'S' && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                ControleMapa::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDoSpawnSalva, ctx.self->spawnJaFoiVisitado, Mapa1VilaLayouts::obterLayoutSpawn(), 51, 5, "CAMINHO DO INICIO", ctx.restaurarTela);
            }
            else if (nextCell == 'F' && nextNextCell == 'o' && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                if (!ctx.self->bjornResgatado) {
                    Aparencia::limparTela();
                    exibirTituloDoMapaVila(ctx.self->tituloDoMapaAtual);
                    int espacosM = std::max(0, (ctx.larguraDoTerminal - 60) / 2);
                    std::cout << "\n" << std::string(espacosM, ' ') << FuncoesDialogo::formatarMsgSistema("A Forja esta trancada. O ferreiro sumiu...") << "\n";
                    ControleDeInput::aguardarEnter();
                    ctx.restaurarTela();
                    return;
                }
                ControleMapa::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDaForjaSalva, ctx.self->forjaJaFoiVisitada, Mapa1VilaLayouts::obterLayoutForja(), 8, 2, "FORJA DA VILA", ctx.restaurarTela);
            }
            else if (nextCell == 'L' && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                ControleMapa::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDaLojaSalva, ctx.self->lojaJaFoiVisitada, Mapa1VilaLayouts::obterLayoutLoja(), 8, 2, "LOJA DA VILA", ctx.restaurarTela);
            }
            else if (nextCell == 'F' && nextNextCell == 'l' && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                if (!Progressao::instancia().obterFlag(Flags::Vila_BjornResgatado)) {
                    Aparencia::limparTela();
                    Aparencia::exibirPainelTexto(ctx.self->tituloDoMapaAtual, Cor::AMARELO);
                    int espacosM = std::max(0, (ctx.larguraDoTerminal - 60) / 2);
                    std::cout << "\n" << std::string(espacosM, ' ') << FuncoesDialogo::formatarMsgSistema("Voce precisa ajudar os habitantes da vila antes de seguir jornada.") << "\n";
                    std::cout << std::string(espacosM, ' ') << FuncoesDialogo::formatarMsgSistema("(Dica: Explore a caverna a leste da vila).") << "\n";
                    ControleDeInput::aguardarEnter();
                    ctx.restaurarTela();
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
        if (titulo == "FORJA DA VILA") return Mapa1VilaLayouts::obterLayoutForja();
        if (titulo == "LOJA DA VILA") return Mapa1VilaLayouts::obterLayoutLoja();
        if (titulo == "CAMINHO DO INICIO") return Mapa1VilaLayouts::obterLayoutSpawn();
        return Mapa1VilaLayouts::obterLayoutVilaInicial();
    }
}

void Mapa1Vila::inicializarInteracoes() {
    interacoes['G'] = std::make_unique<InteracaoCombateGoblin>();
    interacoes['O'] = std::make_unique<InteracaoCombateOrk>();
    interacoes['B'] = std::make_unique<InteracaoNPCBjorn>();
    interacoes['F'] = std::make_unique<InteracaoNPCFranchesco>();
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
            if (ic <= 32 || ic > 126) ic = '@'; // Garante que o icone seja um caractere visivel
            return Aparencia::cor(jogadorAtual->obterCorJogador()) + std::string(1, ic) + Aparencia::cor(Cor::RESET);
        }
        if (celula == 'G' || celula == 'O') return Aparencia::cor(Cor::NEGRITO, Cor::VERMELHO) + std::string(1, celula) + Aparencia::cor(Cor::RESET);
        if (celula == 'B') return Aparencia::cor(Cor::NEGRITO, Cor::CIANO) + "B" + Aparencia::cor(Cor::RESET);
        if (celula == 'F' && x > 0 && matrizDoMapaAtual[y][x-1] == '.') return Aparencia::cor(Cor::NEGRITO, Cor::AMARELO) + "F" + Aparencia::cor(Cor::RESET);
        
        if (celula == '*') {
            bool isTrunk = false;
            if (y > 0 && matrizDoMapaAtual[y-1][x] == '*') {
                int countHorizontal = 0;
                if (x > 0 && matrizDoMapaAtual[y][x-1] == '*') countHorizontal++;
                if (x + 1 < static_cast<int>(matrizDoMapaAtual[y].length()) && matrizDoMapaAtual[y][x+1] == '*') countHorizontal++;
                if (countHorizontal <= 1) isTrunk = true;
            }
            if (isTrunk) return "\033[38;2;101;67;33m*\033[0m"; // Tronco Marrom
            return "\033[38;2;34;139;34m*\033[0m"; // Folhas Verdes
        }
        if (celula == '~') return "\033[38;2;50;150;255m~\033[0m"; // Agua

        // Remove a exibicao visual dos pontos (chao) para deixar o mapa mais limpo
        if (celula == '.') return " ";
        
        return std::string(1, celula);
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

        if (tituloDoMapaAtual == "VILA INICIAL") {
            arteTitulo = Mapa1VilaLayouts::obterLogoVila();
            larguraArte = 125;
            arteTrans = Mapa1VilaLayouts::obterArteTransicaoVila();
            larguraTrans = 75;
        } else if (tituloDoMapaAtual == "CAMINHO DO INICIO") {
            arteTitulo = Mapa1VilaLayouts::obterLogoSpawn();
            larguraArte = 105;
            if (matrizDoMapaDoSpawnSalva.empty()) {
                acaoNarracao = []() {
                    std::cout << "\n\n";
                    std::vector<std::string> dialogoInicio = {
                        FuncoesDialogo::formatarMsgNarracao("Voce desperta nos arredores de um lugar desconhecido..."),
                        FuncoesDialogo::formatarMsgNarracao("Na sua vista, uma pequena vila sendo atacada por monstros."),
                        FuncoesDialogo::formatarMsgNarracao("Empunhando seu equipamento, voce sente que seu destino o aguarda."),
                        FuncoesDialogo::formatarMsgNarracao("Um novo capitulo se inicia agora.")
                    };
                    Aparencia::imprimirBlocoCentralizadoDigitando(dialogoInicio);
                    ControleDeInput::aguardarEnter("Pressione ENTER para iniciar...");
                };
            }
        }
        
        linhaInicialParaDesenharOMapa = ControleMapa::animarIntroducaoMapa(tituloDoMapaAtual, arteTitulo, larguraArte, arteTrans, larguraTrans, Cor::AMARELO, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, true, acaoNarracao);
        precisaRenderizar = false;
    };

    animarTela();

    if (mapaBaseDaVila.empty()) mapaBaseDaVila = matrizDoMapaPrincipalSalva;

    auto processarInteracao = [&](int proximaPosicaoX, int proximaPosicaoY, int larguraDoTerminal) {
        char celulaDestinoDoMapa = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX];
        auto it = interacoes.find(celulaDestinoDoMapa);
        if (it != interacoes.end()) {
            ContextoInteracaoVila ctx = {this, proximaPosicaoX, proximaPosicaoY, larguraDoTerminal, restaurarTela, celulaDestinoDoMapa, animarTela};
            it->second->processar(ctx);
        } else {
            bool ehParede = (celulaDestinoDoMapa == '#');
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
