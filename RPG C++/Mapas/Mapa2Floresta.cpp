#include "Mapa2Floresta.h"

#include <iostream>
#include <vector>
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
#include "../Utilidades/Aparencia.h"
#include "ControleMapa.h"
#include "../Utilidades/ControleDeInput.h"
#include "../Utilidades/GeradorAleatorio.h"
#include "Mapa3Reino.h"
#include "Mapa2FlorestaLayouts.h"

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
    matrizDoMapaAtual = Mapa2FlorestaLayouts::obterLayoutFloresta();
}

Mapa2Floresta::~Mapa2Floresta() = default;

extern void exibirTituloDoMapaFloresta(const std::string& tituloDoMapa);

namespace {
    class InteracaoSlime : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            if (ctx.proximaPosicaoX > 0 && ctx.self->matrizDoMapaAtual[ctx.proximaPosicaoY][ctx.proximaPosicaoX-1] != '^') {
                ControleMapa::processarCombate(ctx.self->jogadorAtual, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "ENCONTRO PEGAJOSO", "Voce encontrou Slimes selvagens!", GerenciadorInimigos::criarInimigoSlime(GeradorAleatorio::obterInteiro(1, 3)), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
            } else {
                ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;
            }
        }
    };

    class InteracaoFada : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            ControleMapa::processarCombate(ctx.self->jogadorAtual, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "ENCONTRO MAGICO", "Voce encontrou Fadas hostis!", GerenciadorInimigos::criarInimigoFada(GeradorAleatorio::obterInteiro(1, 3)), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
        }
    };

    class InteracaoAbominacao : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            ControleMapa::processarCombate(ctx.self->jogadorAtual, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "ENCONTRO BOSS", "Voce encontrou a Abominacao da Floresta!", GerenciadorInimigos::criarInimigoAbominacaoFloresta(1), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
        }
    };

    class InteracaoMorgana : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            NPCMorgana interacaoMorgana;
            interacaoMorgana.interagir(ctx.self->jogadorAtual);
            if (ctx.self->exploracaoEstaAtiva) ctx.restaurarTela();
        }
    };

    class InteracaoBau : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            if (ctx.self->tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") {
                Aparencia::limparTela();
                Aparencia::exibirCabecalho("TESOURO ESCONDIDO", Cor::VERDE);
                int mE = (ctx.larguraDoTerminal - 40) / 2;
                std::string margem(mE > 0 ? mE : 0, ' ');
                std::cout << "\n" << margem << "[!] Voce encontrou um Baú ancestral!\n\n";
                
                std::vector<std::string> opcoesBau = { "Nao", "Abrir!" };
                int opcao = ControleDeInput::lerSelecaoMenuComSetas(opcoesBau, false, margem);

                if (opcao == 1) {
                    if (GeradorAleatorio::rolarChance(25)) { // 25% de chance de ser um Mímico
                        std::cout << "\n" << margem << Aparencia::cor(Cor::VERMELHO) << "[!] O bau se revela uma criatura viva! E UM MIMICO!" << Aparencia::cor(Cor::RESET) << "\n";
                        Aparencia::aguardarEnter();
                        ControleMapa::processarCombate(ctx.self->jogadorAtual, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "CILADA!", "O Bau era um Mimico!", GerenciadorInimigos::criarInimigoMimico(1), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
                    } else {
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
                    Aparencia::aguardarEnter();
                    }
                }

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
                ControleMapa::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDaCabanaSalva, ctx.self->cabanaJaFoiVisitada, Mapa2FlorestaLayouts::obterLayoutCabana(), 8, 2, "CABANA DA BRUXA", ctx.restaurarTela);
            }
            else if (nextCell == 'V') {
                TransicaoDeMapa::exibirTransicaoParaVila();
                ctx.self->exploracaoEstaAtiva = false;
            }
            else if (nextCell == 'T' && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                ControleMapa::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDoCoracaoDaArvoreSalva, ctx.self->coracaoDaArvoreJaFoiVisitado, Mapa2FlorestaLayouts::obterLayoutCoracaoDaArvore(), 10, 3, "CORACAO DA ARVORE", ctx.restaurarTela);
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
                    Aparencia::limparTela();
                    Aparencia::exibirCabecalho("PASSAGEM BLOQUEADA", Cor::VERDE);
                    int espacosM = (ctx.larguraDoTerminal - 60) / 2;
                    std::cout << "\n" << std::string(espacosM > 0 ? espacosM : 0, ' ') << "[SISTEMA]: A passagem esta selada por magia. Fale com Morgana.\n";
                    Aparencia::aguardarEnter();
                    ctx.restaurarTela();
                    return;
                }

                if (ctx.self->tituloDoMapaAtual == "CABANA DA BRUXA") ctx.self->matrizDoMapaDaCabanaSalva = ctx.self->matrizDoMapaAtual;

                if (!ctx.self->labirintoJaFoiVisitado) {
                    ctx.self->matrizDoMapaAtual = Mapa2FlorestaLayouts::obterLayoutLabirinto();
                    ctx.self->labirintoJaFoiVisitado = true;
                    ControleMapa::padronizarTamanhoDoMapa(ctx.self->matrizDoMapaAtual);
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
                Aparencia::limparTela();
                Aparencia::exibirCabecalho("FIM DO LABIRINTO", Cor::VERDE);
                int espacosM = (ctx.larguraDoTerminal - 60) / 2;
                std::string margem(espacosM > 0 ? espacosM : 0, ' ');
                
                std::cout << "\n" << margem << "[SISTEMA]: Voce encontrou a saida do labirinto!\n";
                std::cout << margem << "[SISTEMA]: A sua frente, uma escadaria desce para uma caverna escura.\n";
                std::cout << margem << "[SISTEMA]: No fundo, parece haver um mar de liquido preto raso...\n\n";

                std::vector<std::string> opcoesCaminho = { "Descer a escadaria", "Voltar para a Cabana da Bruxa" };
                int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoesCaminho, false, margem);

                if (escolha == 0) {
                    Aparencia::limparTela();
                    std::vector<std::string> arteSimbolo = Mapa2FlorestaLayouts::obterArteSimboloChefe();
                    
                    std::cout << "\n\n";
                    Aparencia::imprimirCentralizadoMultilinha(arteSimbolo, 109, Aparencia::cor(Cor::BRANCO));
                    
                    std::cout << "\n" << margem << "[SISTEMA]: O ar aqui embaixo e gelado, cortante\n";
                    std::cout << margem << "[SISTEMA]: o liquido preto no chao e raso e liso como vidro\n";
                    std::cout << margem << "[SISTEMA]: Tudo e escuridao, exceto pelo brilho pulsante da\n";
                    std::cout << margem << "[SISTEMA]: enorme runa magica desenhada no fundo da caverna\n\n";
                    
                    std::vector<std::string> opcoesBoss = {
                        Aparencia::cor(Cor::VERMELHO) + "Seguir em frente" + Aparencia::cor(Cor::RESET),
                        Aparencia::cor(Cor::BRANCO) + "Voltar para a seguranca da Cabana" + Aparencia::cor(Cor::RESET)
                    };
                    int escolhaBoss = ControleDeInput::lerSelecaoMenuComSetas(opcoesBoss, false, margem);
                    
                    
                    if (escolhaBoss == 0) {
                        ctx.self->matrizDoMapaDoLabirintoSalva = ctx.self->matrizDoMapaAtual;
                        if (!ctx.self->salaDoChefeJaFoiVisitada) {
                            ctx.self->matrizDoMapaAtual = Mapa2FlorestaLayouts::obterLayoutSalaDoChefe();
                            ControleMapa::padronizarTamanhoDoMapa(ctx.self->matrizDoMapaAtual);
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
    interacoes['M'] = std::make_unique<InteracaoMorgana>();
    interacoes['B'] = std::make_unique<InteracaoBau>();
    interacoes['^'] = std::make_unique<InteracaoTeleporte>();
}

void Mapa2Floresta::iniciarLoopDeExploracaoDoMapa()
{
    inicializarInteracoes();

    ControleMapa::padronizarTamanhoDoMapa(matrizDoMapaAtual);

    Aparencia::ocultarCursor();

    Aparencia::limparTela();
    exibirTituloDoMapaFloresta(tituloDoMapaAtual);

    int linhaInicialParaDesenharOMapa = Aparencia::obterPosicaoCursorY();

    // Lambda para restaurar a tela apos eventos
    auto restaurarTela = [&]() {
        Aparencia::limparTela();
        exibirTituloDoMapaFloresta(tituloDoMapaAtual);
        linhaInicialParaDesenharOMapa = Aparencia::obterPosicaoCursorY();
    };

    auto processarInteracao = [&](int proximaPosicaoX, int proximaPosicaoY, int larguraDoTerminal)
    {
        char celulaDestinoDoMapa = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX];
        
        if (tituloDoMapaAtual == "SALA DO CHEFE" && (celulaDestinoDoMapa == 'M' || celulaDestinoDoMapa == 'A' || celulaDestinoDoMapa == 'H' || celulaDestinoDoMapa == 'O')) {
            std::cout << "\n[SISTEMA]: A batalha contra MAHO estara disponivel em breve!\n";
            Aparencia::aguardarEnter();
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
        int larguraDoTerminal = Aparencia::obterLarguraTerminal();
        int alturaDoTerminal = Aparencia::obterAlturaTerminal();

        auto formatador = [&](char celula, int x, int y) -> std::string {
            if (x == posicaoXDoJogador && y == posicaoYDoJogador) return Aparencia::cor(Cor::NEGRITO, Cor::VERDE) + "@" + Aparencia::cor(Cor::RESET);
            if (celula == 'S' && (x == 0 || matrizDoMapaAtual[y][x-1] != '^')) return Aparencia::cor(Cor::NEGRITO, Cor::VERMELHO) + "S" + Aparencia::cor(Cor::RESET);
            if (celula == 'F' || celula == 'A') return Aparencia::cor(Cor::NEGRITO, Cor::VERMELHO) + std::string(1, celula) + Aparencia::cor(Cor::RESET);
            if (celula == 'M') return Aparencia::cor(Cor::NEGRITO, Cor::MAGENTA) + "M" + Aparencia::cor(Cor::RESET);
            if (celula == 'B') return Aparencia::cor(Cor::NEGRITO, Cor::AMARELO) + "B" + Aparencia::cor(Cor::RESET);
            if (tituloDoMapaAtual == "SALA DO CHEFE" && (celula == 'M' || celula == 'A' || celula == 'H' || celula == 'O')) return Aparencia::cor(Cor::NEGRITO, Cor::MAGENTA) + std::string(1, celula) + Aparencia::cor(Cor::RESET);
            if (tituloDoMapaAtual == "SALA DO CHEFE" && celula == '.') return Aparencia::cor(Cor::CINZA) + "." + Aparencia::cor(Cor::RESET);
            return std::string(1, celula);
        };
        ControleMapa::renderizarMapa(matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, larguraDoTerminal, alturaDoTerminal, linhaInicialParaDesenharOMapa, formatador);

        char teclaPressionadaPeloJogador = ControleDeInput::lerTecla();

        int proximaPosicaoX = posicaoXDoJogador;
        int proximaPosicaoY = posicaoYDoJogador;

        bool abriuMenu = ControleMapa::processarInputEComandos(teclaPressionadaPeloJogador, jogadorAtual, proximaPosicaoX, proximaPosicaoY, restaurarTela);
        
        if (jogadorAtual->obterVoltarProMenu()) break;
        if (abriuMenu) continue;

        ControleMapa::aplicarLimitesDeMapa(proximaPosicaoX, proximaPosicaoY, matrizDoMapaAtual);

        processarInteracao(proximaPosicaoX, proximaPosicaoY, larguraDoTerminal);
    }
}
