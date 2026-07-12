#include "Mapa2Floresta.h"

#include <iostream>
#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include <chrono>
#include <thread>

#include "../../Perspectiva/TelasBase/Menu/TelaMenu.h"
#include "../../Sistemas/Inventario/Item.h"
#include "../../Sistemas/Inventario/Equipamentos/EquipamentoArmadura.h"
#include "../../Sistemas/Inventario/Itens/ItemConsumivel.h"
#include "../../Sistemas/Inventario/Itens/ItemMaterial.h"
#include "../../Core/Controladores/CriadorInimigos.h"
#include "../../Sistemas/Inventario/InventarioCombate.h"
#include "../../Perspectiva/TelasBase/Atributos/TelaAtributos.h"
#include "../../Perspectiva/TelasBase/Bestiario/TelaBestiario.h"
#include "../../Sistemas/Combate/Combate.h"
#include "../ControleMapa.h"
#include "../Sistemas/AnimadorMapa.h"
#include "../Sistemas/CarregadorMapa.h"
#include "../../Entidades/NPCs/Maga/NPCMaga.h"
#include "../../Entidades/Inimigos/AbominacaoFloresta.h"
#include "../../Entidades/Inimigos/Mahoraga.h"
#include "../../Entidades/Inimigos/ClasseBaseInimigo.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Sistemas/Progresso/Diario.h"
#include "../../Core/Utilidades/FuncoesDialogo.h"
#include "../ControleMapa.h"
#include "../../Core/Utilidades/ControleDeInput.h"
#include "../../Core/Utilidades/GeradorAleatorio.h"
#include "../Reino/Mapa3PonteReino.h"
#include "Mapa2FlorestaLayout.h"

Mapa2Floresta::Mapa2Floresta(Personagem* personagemJogador) :
    posicaoXDoJogador(31), 
    posicaoYDoJogador(17),
    jogadorAtual(personagemJogador), 
    posicaoXSalvaAntesDeEntrarNoSubMapa(0), 
    posicaoYSalvaAntesDeEntrarNoSubMapa(0),
    jogadorEstaDentroDeUmSubMapa(false),
    coracaoDaArvoreJaFoiVisitado(false), 
    labirintoJaFoiVisitado(false),
    salaDoChefeJaFoiVisitada(false),
    exploracaoEstaAtiva(true), 
    tituloDoMapaAtual("FLORESTA"),
    proximoMapa(ProximaTransicaoMapa::Nenhuma)
{
    matrizDoMapaAtual = Mapa2FlorestaLayouts::obterLayoutFloresta();
    CarregadorMapa::padronizarTamanhoDoMapa(matrizDoMapaAtual);
}

Mapa2Floresta::~Mapa2Floresta() = default;

namespace {
    class InteracaoSlime : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            if (ctx.proximaPosicaoX > 0 && ctx.self->matrizDoMapaAtual[ctx.proximaPosicaoY][ctx.proximaPosicaoX-1] != '^') {
                ControleMapa::processarCombate(ctx.self->jogadorAtual, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "ENCONTRO PEGAJOSO", "Voce encontrou Slimes selvagens!", CriadorInimigos::criarInimigoSlime(GeradorAleatorio::obterInteiro(1, 3)), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
            } else {
                ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;
            }
        }
    };

    class InteracaoFada : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            ControleMapa::processarCombate(ctx.self->jogadorAtual, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "ENCONTRO MAGICO", "Voce encontrou Fadas hostis!", CriadorInimigos::criarInimigoFada(GeradorAleatorio::obterInteiro(1, 3)), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
        }
    };

    class InteracaoAbominacao : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            ControleMapa::processarCombate(ctx.self->jogadorAtual, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "ENCONTRO BOSS", "Voce encontrou a Abominacao da Floresta!", CriadorInimigos::criarInimigoAbominacaoFloresta(1), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
        }
    };

    class InteracaoMorgana : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            NPCMaga interacaoMorgana;
            interacaoMorgana.interagir(ctx.self->jogadorAtual);
            Diario::instancia().registrarNPC("Morgana (Bruxa)");
            if (ctx.self->exploracaoEstaAtiva && !ControleMapa::isExploracao3DAtiva()) if (!ControleMapa::isExploracao3DAtiva()) ctx.restaurarTela();
        }
    };

    class InteracaoBau : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            if (ctx.self->tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") {
                Aparencia::iniciarInteracaoPopup();
                
                std::vector<std::string> msgText = { "Voce encontrou um Bau ancestral!" };
                std::vector<std::string> opcoesBau = { "Nao", "Abrir!" };
                int opcao = ControleDeInput::lerSelecaoMenuEmPopup("TESOURO ESCONDIDO", msgText, opcoesBau, Cor::VERDE);

                if (opcao == 1) {
                    if (GeradorAleatorio::rolarChance(25)) {
                        Aparencia::iniciarInteracaoPopup();
                        Aparencia::exibirPopup("CILADA!", {"O bau se revela uma criatura viva!", "E UM MIMICO!"}, Cor::VERMELHO);
                        ControleMapa::processarCombate(ctx.self->jogadorAtual, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "CILADA!", "O Bau era um Mimico!", CriadorInimigos::criarInimigoMimico(1), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
                    } else {
                    std::vector<std::string> lootMsg = { "O bau se abre rangendo...", "Voce obteve itens valiosos!", "" };

                    int qtdPocoes = GeradorAleatorio::obterInteiro(2, 4);
                    for (int i = 0; i < qtdPocoes; ++i) {
                        auto pocao = std::make_unique<ItemConsumivel>("Pocao de Cura (30%VM)");
                        pocao->adicionarPropriedade(Propriedade::ConsumivelCura);
                        ctx.self->jogadorAtual->obterInventario()->adicionarItem(std::move(pocao));
                    }
                    lootMsg.push_back("+ " + std::to_string(qtdPocoes) + "x Pocoes de Cura (30%VM)");

                    int qtdOuro = GeradorAleatorio::obterInteiro(150, 300);
                    ctx.self->jogadorAtual->obterInventario()->adicionarOuro(qtdOuro);
                    lootMsg.push_back("+ " + std::to_string(qtdOuro) + "G");

                    bool isFuria = GeradorAleatorio::rolarChance(50);
                    std::string nomeBuff = isFuria ? "Pocao de Furia (Buff)" : "Elixir Arcano (Buff)";
                    auto buff = std::make_unique<ItemConsumivel>(nomeBuff);
                    buff->adicionarPropriedade(Propriedade::ConsumivelBuff);
                    ctx.self->jogadorAtual->obterInventario()->adicionarItem(std::move(buff));
                    lootMsg.push_back("+ 1x " + nomeBuff);

                    ctx.self->jogadorAtual->obterInventario()->adicionarItem(std::make_unique<ItemMaterial>("Pedra magica de upgrade"));
                    lootMsg.push_back("+ 1x Pedra magica de upgrade");

                    ctx.self->matrizDoMapaAtual[ctx.proximaPosicaoY][ctx.proximaPosicaoX] = ' ';
                    ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                    ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;
                    Aparencia::iniciarInteracaoPopup();
                    Aparencia::exibirPopup("TESOURO ESCONDIDO", lootMsg, Cor::VERDE);
                    }
                }
                ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;

                if (ctx.self->exploracaoEstaAtiva && !ControleMapa::isExploracao3DAtiva()) if (!ControleMapa::isExploracao3DAtiva()) ctx.restaurarTela();
            } else {
                ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;
            }
        }
    };

    class InteracaoTeleporte : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            int px = ctx.proximaPosicaoX;
            int py = ctx.proximaPosicaoY;
            std::string titulo = ctx.self->tituloDoMapaAtual;
            
            // 2. Voltar para a Vila a partir da Floresta (X=19, Y=10)
            if (px == 19 && py == 10 && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                ctx.self->exploracaoEstaAtiva = false;
                ctx.self->proximoMapa = ProximaTransicaoMapa::Vila;
            }
            // 3. Entrar no Coracao da Arvore a partir da Floresta (X=121, Y=43)
            else if (px == 121 && py == 43 && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                CarregadorMapa::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDoCoracaoDaArvoreSalva, ctx.self->coracaoDaArvoreJaFoiVisitado, Mapa2FlorestaLayouts::obterLayoutCoracaoDaArvore(), 10, 3, "CORACAO DA ARVORE", ctx.restaurarTela);
            }
            // 4. Ir para o Reino a partir da Floresta (X=18, Y=45)
            else if (px == 18 && py == 45 && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                ctx.self->exploracaoEstaAtiva = false;
                ctx.self->proximoMapa = ProximaTransicaoMapa::Reino;
            }
            // 5. Entrar no Labirinto a partir da Floresta (X=133, Y=12)
            else if (px == 133 && py == 12 && titulo == "FLORESTA") {
                if (!ctx.self->jogadorAtual->obterLabirintoDesbloqueado()) {
                    Aparencia::iniciarInteracaoPopup();
                    std::vector<std::string> msg = {
                        "A passagem esta selada por magia.",
                        "Fale com Morgana."
                    };
                    Aparencia::exibirPopup("PASSAGEM BLOQUEADA", msg, Cor::MAGENTA);
                    ctx.self->posicaoXDoJogador = 129;
                    ctx.self->posicaoYDoJogador = 10;
                    if (!ControleMapa::isExploracao3DAtiva()) ctx.restaurarTela();
                    return;
                }

                CarregadorMapa::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDoLabirintoSalva, ctx.self->labirintoJaFoiVisitado, Mapa2FlorestaLayouts::obterLayoutLabirinto(), 4, 11, "LABIRINTO SUBTERRANEO", ctx.restaurarTela);
            }
            // 6. Sair de Submapas
            else if ((titulo == "CORACAO DA ARVORE") ||
                     ((px == 1 || px == 2) && py == 11 && titulo == "LABIRINTO SUBTERRANEO") ||
                     (titulo == "SALA DO CHEFE")) {
                
                if (titulo == "CORACAO DA ARVORE") {
                    ctx.self->coracaoDaArvoreJaFoiVisitado = false; 
                }
                else if (titulo == "LABIRINTO SUBTERRANEO") {
                    ctx.self->matrizDoMapaDoLabirintoSalva = ctx.self->matrizDoMapaAtual;
                }
                else if (titulo == "SALA DO CHEFE") {
                    ctx.self->matrizDoMapaSalaDoChefeSalva = ctx.self->matrizDoMapaAtual;
                }

                if (titulo == "SALA DO CHEFE") {
                    ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaDoLabirintoSalva;
                    ctx.self->posicaoXDoJogador = 76; 
                    ctx.self->posicaoYDoJogador = 11;
                    ctx.self->tituloDoMapaAtual = "LABIRINTO SUBTERRANEO";
                } else {
                    ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaPrincipalSalva;
                    ctx.self->posicaoXDoJogador = ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa;
                    ctx.self->posicaoYDoJogador = ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa;
                    ctx.self->jogadorEstaDentroDeUmSubMapa = false;
                    ctx.self->tituloDoMapaAtual = "FLORESTA";
                }
                if (!ControleMapa::isExploracao3DAtiva()) if (!ControleMapa::isExploracao3DAtiva()) ctx.restaurarTela();
            }
            // 7. Fim do Labirinto (Escadaria para Boss)
            else if ((px == 77 || px == 78) && py == 11 && titulo == "LABIRINTO SUBTERRANEO") {
                Aparencia::iniciarInteracaoPopup();
                std::vector<std::string> msgLab = {
                    "Voce encontrou a saida do labirinto!",
                    "A sua frente, uma escadaria desce para uma caverna escura.",
                    "No fundo, parece haver um mar de liquido preto raso..."
                };
                std::vector<std::string> opcoesCaminho = { "Descer a escadaria", "Voltar para a Floresta" };
                int escolha = ControleDeInput::lerSelecaoMenuEmPopup("FIM DO LABIRINTO", msgLab, opcoesCaminho, Cor::VERDE);

                if (escolha == 0) {
                    std::vector<std::string> msgBoss = {
                        "O ar aqui embaixo e gelado, cortante.",
                        "O liquido preto no chao e raso e liso como vidro.",
                        "Tudo e escuridao, exceto pelo brilho pulsante da",
                        "enorme runa magica desenhada no fundo da caverna."
                    };
                    
                    std::vector<std::string> opcoesBoss = {
                        Aparencia::cor(Cor::VERMELHO) + "Seguir em frente" + Aparencia::cor(Cor::RESET),
                        Aparencia::cor(Cor::BRANCO) + "Voltar para a seguranca da Floresta" + Aparencia::cor(Cor::RESET)
                    };
                    int escolhaBoss = ControleDeInput::lerSelecaoMenuEmPopup("CAVERNA SOMBRIA", msgBoss, opcoesBoss, Cor::VERMELHO);
                    
                    if (escolhaBoss == 0) {
                        ctx.self->matrizDoMapaDoLabirintoSalva = ctx.self->matrizDoMapaAtual;
                        if (!ctx.self->salaDoChefeJaFoiVisitada) {
                            ctx.self->matrizDoMapaAtual = Mapa2FlorestaLayouts::obterLayoutSalaDoChefe();
                            CarregadorMapa::padronizarTamanhoDoMapa(ctx.self->matrizDoMapaAtual);
                            ctx.self->salaDoChefeJaFoiVisitada = true;
                        } else {
                            ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaSalaDoChefeSalva;
                        }
                        ctx.self->posicaoXDoJogador = 53;
                        ctx.self->posicaoYDoJogador = 53;
                        ctx.self->tituloDoMapaAtual = "SALA DO CHEFE";
                        if (!ControleMapa::isExploracao3DAtiva()) if (!ControleMapa::isExploracao3DAtiva()) ctx.restaurarTela();
                    } else {
                        ctx.self->matrizDoMapaDoLabirintoSalva = ctx.self->matrizDoMapaAtual;
                        ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaPrincipalSalva;
                        ctx.self->posicaoXDoJogador = ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa;
                        ctx.self->posicaoYDoJogador = ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa;
                        ctx.self->jogadorEstaDentroDeUmSubMapa = false;
                        ctx.self->tituloDoMapaAtual = "FLORESTA";
                    }
                } else {
                    ctx.self->matrizDoMapaDoLabirintoSalva = ctx.self->matrizDoMapaAtual;
                    ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaPrincipalSalva;
                    ctx.self->posicaoXDoJogador = ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa;
                    ctx.self->posicaoYDoJogador = ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa;
                    ctx.self->jogadorEstaDentroDeUmSubMapa = false;
                    ctx.self->tituloDoMapaAtual = "FLORESTA";
                }
                if (ctx.self->exploracaoEstaAtiva && !ControleMapa::isExploracao3DAtiva()) if (!ControleMapa::isExploracao3DAtiva()) ctx.restaurarTela();
            } else {
                ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;
            }
        }
    };

    std::vector<std::string> obterLayoutOriginalFloresta(const std::string& titulo) {
        if (titulo == "CORACAO DA ARVORE") return Mapa2FlorestaLayouts::obterLayoutCoracaoDaArvore();
        if (titulo == "LABIRINTO SUBTERRANEO") return Mapa2FlorestaLayouts::obterLayoutLabirinto();
        if (titulo == "SALA DO CHEFE") return Mapa2FlorestaLayouts::obterLayoutSalaDoChefe();
        return Mapa2FlorestaLayouts::obterLayoutFloresta();
    }
}

void Mapa2Floresta::inicializarInteracoes() {
    interacoes['S'] = std::make_unique<InteracaoSlime>();
    interacoes['F'] = std::make_unique<InteracaoFada>();
    interacoes['A'] = std::make_unique<InteracaoAbominacao>();
    interacoes['M'] = std::make_unique<InteracaoMorgana>();
    interacoes['B'] = std::make_unique<InteracaoBau>();
    interacoes['^'] = std::make_unique<InteracaoTeleporte>();
}

ProximaTransicaoMapa Mapa2Floresta::iniciarLoopDeExploracao()
{
    // Resgata o jogador se ele usou Viagem Rapida enquanto estava dentro de um submapa
    if (jogadorEstaDentroDeUmSubMapa) {
        matrizDoMapaAtual = matrizDoMapaPrincipalSalva;
        posicaoXDoJogador = posicaoXSalvaAntesDeEntrarNoSubMapa;
        posicaoYDoJogador = posicaoYSalvaAntesDeEntrarNoSubMapa;
        jogadorEstaDentroDeUmSubMapa = false;
        tituloDoMapaAtual = "FLORESTA";
    }

    inicializarInteracoes();

    CarregadorMapa::padronizarTamanhoDoMapa(matrizDoMapaAtual);

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
        linhaInicialParaDesenharOMapa = AnimadorMapa::animarIntroducaoMapa(tituloDoMapaAtual, {}, 0, {}, 0, Cor::VERDE, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, false, true, nullptr);
        precisaRenderizar = true;
    };

    auto animarTela = [&]() {
        std::vector<std::string> arteTitulo;
        int larguraArte = 0;
        std::vector<std::string> arteTrans;
        int larguraTrans = 0;

        if (tituloDoMapaAtual == "FLORESTA") {
            arteTitulo = Mapa2FlorestaLayouts::obterLogoFloresta();
            larguraArte = 100;
            arteTrans = Mapa2FlorestaLayouts::obterArteTransicaoFloresta();
            larguraTrans = 87;
        }

        linhaInicialParaDesenharOMapa = AnimadorMapa::animarIntroducaoMapa(tituloDoMapaAtual, arteTitulo, larguraArte, arteTrans, larguraTrans, Cor::VERDE, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, true, true, nullptr);
        precisaRenderizar = false;
    };

    animarTela();

    auto processarInteracao = [&](int proximaPosicaoX, int proximaPosicaoY, int larguraDoTerminal)
    {
        char celulaDestinoDoMapa = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX];
        
        if (tituloDoMapaAtual == "SALA DO CHEFE" && (celulaDestinoDoMapa == 'M' || celulaDestinoDoMapa == 'A' || celulaDestinoDoMapa == 'H' || celulaDestinoDoMapa == 'O' || celulaDestinoDoMapa == 'R' || celulaDestinoDoMapa == 'G')) {
            std::vector<std::unique_ptr<Personagem>> bossMaho;
            auto bossMahoraga = std::make_unique<Personagem>("Mahoraga", std::make_unique<Mahoraga>(), std::make_unique<ClasseBaseInimigo>());
            bossMahoraga->calcularAtributos();
            bossMahoraga->modificarVida(bossMahoraga->obterVidaMaxima());
            bossMaho.push_back(std::move(bossMahoraga));

            int startX = proximaPosicaoX;
            while (startX > 0 && (matrizDoMapaAtual[proximaPosicaoY][startX-1] == 'M' || matrizDoMapaAtual[proximaPosicaoY][startX-1] == 'A' || matrizDoMapaAtual[proximaPosicaoY][startX-1] == 'H' || matrizDoMapaAtual[proximaPosicaoY][startX-1] == 'O' || matrizDoMapaAtual[proximaPosicaoY][startX-1] == 'R' || matrizDoMapaAtual[proximaPosicaoY][startX-1] == 'G')) startX--;

            ControleMapa::processarCombate(jogadorAtual, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, exploracaoEstaAtiva, "O GENERAL DIVINO", "A Roda comeca a girar... Mahoraga despertou!", std::move(bossMaho), proximaPosicaoX, proximaPosicaoY, startX, 8, larguraDoTerminal, restaurarTela);
            return;
        }
        
        auto it = interacoes.find(celulaDestinoDoMapa);
        if (it != interacoes.end()) {
            ContextoInteracaoFloresta ctx = {this, proximaPosicaoX, proximaPosicaoY, larguraDoTerminal, restaurarTela, celulaDestinoDoMapa, animarTela};
            it->second->processar(ctx);
        } else {
            bool ehParede = false;
            if (tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") {
                ehParede = (celulaDestinoDoMapa != ' ' && celulaDestinoDoMapa != '^' && celulaDestinoDoMapa != 'S' && celulaDestinoDoMapa != 'E' && celulaDestinoDoMapa != 'B');
            } else if (tituloDoMapaAtual == "SALA DO CHEFE") {
                ehParede = (celulaDestinoDoMapa == ' ');
            } else {
                std::string caracteresParede = "*#|_[]{}-=";
                ehParede = (caracteresParede.find(celulaDestinoDoMapa) != std::string::npos);
            }
            if (!ehParede || jogadorAtual->isNoclip()) {
                posicaoXDoJogador = proximaPosicaoX;
                posicaoYDoJogador = proximaPosicaoY;
            }
        }
    };

    ProximaTransicaoMapa destinoViagemRapida = ControleMapa::executarLoopDeExploracao(
        jogadorAtual, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador,
        exploracaoEstaAtiva, tituloDoMapaAtual,
        [this]() { return (tituloDoMapaAtual == "SALA DO CHEFE") ? "" : "SFA"; },
        [this]() { return obterLayoutOriginalFloresta(tituloDoMapaAtual); },
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
