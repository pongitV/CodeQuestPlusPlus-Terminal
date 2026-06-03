#include "Mapa2Floresta.h"

#include <iostream>
#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include <chrono>
#include <thread>

#include "../../../Interface/Telas/Menu/TelaMenu.h"
#include "../../Inventario/Item.h"
#include "../../Inventario/Equipamentos/EquipamentoArmadura.h"
#include "../../Inventario/Itens/ItemConsumivel.h"
#include "../../Inventario/Itens/ItemMaterial.h"
#include "../../../Core/Controladores/CriadorInimigos.h"
#include "../../Inventario/InventarioCombate.h"
#include "../../../Interface/Telas/Atributos/TelaAtributos.h"
#include "../../../Interface/Telas/Bestiario/TelaBestiario.h"
#include "../../Combate/Combate.h"
#include "../../../Entidades/Inimigos/Fada.h"
#include "../../../Entidades/Inimigos/ClasseBaseInimigo.h"
#include "../../../Entidades/NPCs/Morgana/NPCMorgana.h"
#include "../../../Entidades/Inimigos/AbominacaoFloresta.h"
#include "../../../Entidades/Inimigos/Mahoraga.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../Progresso/Diario.h"
#include "../../../Core/Utilidades/FuncoesDialogo.h"
#include "../ControleMapa.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Core/Utilidades/GeradorAleatorio.h"
#include "../Reino/Mapa3Reino.h"
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
            NPCMorgana interacaoMorgana;
            interacaoMorgana.interagir(ctx.self->jogadorAtual);
            Diario::instancia().registrarNPC("Morgana (Bruxa)");
            ctx.self->posicaoXDoJogador = 123;
            ctx.self->posicaoYDoJogador = 10;
            if (ctx.self->exploracaoEstaAtiva) ctx.restaurarTela();
        }
    };

    class InteracaoBau : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            if (ctx.self->tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") {
                Aparencia::limparTela();
                Aparencia::exibirPainelTexto("TESOURO ESCONDIDO", Cor::VERDE);
                int mE = (ctx.larguraDoTerminal - 40) / 2;
                std::string margem(mE > 0 ? mE : 0, ' ');
                std::cout << "\n" << margem << FuncoesDialogo::formatarMsgInteracao("Voce encontrou um Bau ancestral!") << "\n\n";
                
                std::vector<std::string> opcoesBau = { "Nao", "Abrir!" };
                int opcao = ControleDeInput::lerSelecaoMenuComSetas(opcoesBau, false, margem);

                if (opcao == 1) {
                    if (GeradorAleatorio::rolarChance(25)) {
                        std::cout << "\n" << margem << FuncoesDialogo::formatarMsgCombate("O bau se revela uma criatura viva! E UM MIMICO!", Cor::VERMELHO) << "\n";
                        ControleDeInput::aguardarEnter();
                        ControleMapa::processarCombate(ctx.self->jogadorAtual, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "CILADA!", "O Bau era um Mimico!", CriadorInimigos::criarInimigoMimico(1), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
                    } else {
                    std::cout << "\n" << margem << FuncoesDialogo::formatarMsgInteracao("O bau se abre rangendo... Voce obteve itens valiosos!") << "\n";

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
                    ControleDeInput::aguardarEnter();
                    }
                }
                ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;

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
            int px = ctx.proximaPosicaoX;
            int py = ctx.proximaPosicaoY;
            std::string titulo = ctx.self->tituloDoMapaAtual;
            
            // 2. Voltar para a Vila a partir da Floresta (X=32, Y=10)
            if (px == 32 && py == 10 && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                ctx.self->exploracaoEstaAtiva = false;
                ctx.self->proximoMapa = ProximaTransicaoMapa::Vila;
            }
            // 3. Entrar no Coracao da Arvore a partir da Floresta (X=121, Y=43)
            else if (px == 121 && py == 43 && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                ControleMapa::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDoCoracaoDaArvoreSalva, ctx.self->coracaoDaArvoreJaFoiVisitado, Mapa2FlorestaLayouts::obterLayoutCoracaoDaArvore(), 10, 3, "CORACAO DA ARVORE", ctx.restaurarTela);
            }
            // 4. Ir para o Reino a partir da Floresta (X=18, Y=45)
            else if (px == 18 && py == 45 && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                ctx.self->exploracaoEstaAtiva = false;
                ctx.self->proximoMapa = ProximaTransicaoMapa::Reino;
            }
            // 5. Entrar no Labirinto a partir da Floresta (X=130, Y=10)
            else if (px == 130 && py == 10 && titulo == "FLORESTA") {
                if (!ctx.self->jogadorAtual->obterLabirintoDesbloqueado()) {
                    Aparencia::limparTela();
                    Aparencia::exibirPainelTexto("PASSAGEM BLOQUEADA", Cor::VERDE);
                    int espacosM = (ctx.larguraDoTerminal - 60) / 2;
                    std::cout << "\n" << std::string(espacosM > 0 ? espacosM : 0, ' ') << FuncoesDialogo::formatarMsgSistema("A passagem esta selada por magia. Fale com Morgana.") << "\n";
                    ControleDeInput::aguardarEnter();
                    ctx.self->posicaoXDoJogador = 129;
                    ctx.self->posicaoYDoJogador = 10;
                    ctx.restaurarTela();
                    return;
                }

                ControleMapa::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDoLabirintoSalva, ctx.self->labirintoJaFoiVisitado, Mapa2FlorestaLayouts::obterLayoutLabirinto(), 3, 13, "LABIRINTO SUBTERRANEO", ctx.restaurarTela);
            }
            // 6. Sair de Submapas
            else if ((px == 9 && py == 3 && titulo == "CORACAO DA ARVORE") ||
                     (px == 1 && py == 13 && titulo == "LABIRINTO SUBTERRANEO") ||
                     (px == 53 && py == 54 && titulo == "SALA DO CHEFE")) {
                
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
                    ctx.self->posicaoXDoJogador = 102; 
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
            // 7. Fim do Labirinto (Escadaria para Boss) (X=103, Y=13)
            else if (px == 103 && py == 13 && titulo == "LABIRINTO SUBTERRANEO") {
                Aparencia::limparTela();
            Aparencia::exibirPainelTexto("FIM DO LABIRINTO", Cor::VERDE);
                int espacosM = (ctx.larguraDoTerminal - 60) / 2;
                std::string margem(espacosM > 0 ? espacosM : 0, ' ');
                
                std::cout << "\n" << margem << FuncoesDialogo::formatarMsgNarracao("Voce encontrou a saida do labirinto!") << "\n";
                std::cout << margem << FuncoesDialogo::formatarMsgNarracao("A sua frente, uma escadaria desce para uma caverna escura.") << "\n";
                std::cout << margem << FuncoesDialogo::formatarMsgNarracao("No fundo, parece haver um mar de liquido preto raso...") << "\n\n";

                std::vector<std::string> opcoesCaminho = { "Descer a escadaria", "Voltar para a Floresta" };
                int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoesCaminho, false, margem);

                if (escolha == 0) {
                    Aparencia::limparTela();
                    std::vector<std::string> arteSimbolo = Mapa2FlorestaLayouts::obterArteSimboloChefe();
                    
                    std::cout << "\n\n";
                    Aparencia::imprimirCentralizadoMultilinha(arteSimbolo, 109, Aparencia::cor(Cor::BRANCO));
                    
                    std::cout << "\n" << margem << FuncoesDialogo::formatarMsgNarracao("O ar aqui embaixo e gelado, cortante") << "\n";
                    std::cout << margem << FuncoesDialogo::formatarMsgNarracao("o liquido preto no chao e raso e liso como vidro") << "\n";
                    std::cout << margem << FuncoesDialogo::formatarMsgNarracao("Tudo e escuridao, exceto pelo brilho pulsante da") << "\n";
                    std::cout << margem << FuncoesDialogo::formatarMsgNarracao("enorme runa magica desenhada no fundo da caverna") << "\n\n";
                    
                    std::vector<std::string> opcoesBoss = {
                        Aparencia::cor(Cor::VERMELHO) + "Seguir em frente" + Aparencia::cor(Cor::RESET),
                        Aparencia::cor(Cor::BRANCO) + "Voltar para a seguranca da Floresta" + Aparencia::cor(Cor::RESET)
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
                        ctx.self->posicaoXDoJogador = 53;
                        ctx.self->posicaoYDoJogador = 53;
                        ctx.self->tituloDoMapaAtual = "SALA DO CHEFE";
                        ctx.restaurarTela();
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
                if (ctx.self->exploracaoEstaAtiva) ctx.restaurarTela();
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
    inicializarInteracoes();

    ControleMapa::padronizarTamanhoDoMapa(matrizDoMapaAtual);

    Aparencia::ocultarCursor();

    auto formatador = [&](char celula, int x, int y) -> std::string {
        if (x == posicaoXDoJogador && y == posicaoYDoJogador) {
            char ic = jogadorAtual->obterIconeJogador();
            if (ic <= 32 || ic > 126) ic = '@'; // Garante que o icone seja um caractere visivel
            return Aparencia::cor(jogadorAtual->obterCorJogador()) + std::string(1, ic) + Aparencia::cor(Cor::RESET);
        }
        if (celula == 'S' && (x == 0 || matrizDoMapaAtual[y][x-1] != '^')) return Aparencia::cor(Cor::NEGRITO, Cor::VERMELHO) + "S" + Aparencia::cor(Cor::RESET);
        if (celula == 'F' || celula == 'A') return Aparencia::cor(Cor::NEGRITO, Cor::VERMELHO) + std::string(1, celula) + Aparencia::cor(Cor::RESET);
        if (celula == 'M') return Aparencia::cor(Cor::NEGRITO, Cor::MAGENTA) + "M" + Aparencia::cor(Cor::RESET);
        if (celula == 'B') return Aparencia::cor(Cor::NEGRITO, Cor::DOURADO) + "B" + Aparencia::cor(Cor::RESET);
        if (tituloDoMapaAtual == "SALA DO CHEFE" && (celula == 'M' || celula == 'A' || celula == 'H' || celula == 'O' || celula == 'R' || celula == 'G')) return Aparencia::cor(Cor::NEGRITO, Cor::BRANCO) + std::string(1, celula) + Aparencia::cor(Cor::RESET);
        if (celula == '^') return Aparencia::cor(Cor::NEGRITO, Cor::TELEPORTE) + "^" + Aparencia::cor(Cor::RESET);
        
        if (celula == '*') {
            bool isTrunk = false;
            if (y > 0 && matrizDoMapaAtual[y-1][x] == '*') {
                int countHorizontal = 0;
                if (x > 0 && matrizDoMapaAtual[y][x-1] == '*') countHorizontal++;
                if (x + 1 < static_cast<int>(matrizDoMapaAtual[y].length()) && matrizDoMapaAtual[y][x+1] == '*') countHorizontal++;
                if (countHorizontal <= 1) isTrunk = true;
            }
            if (isTrunk) return Aparencia::cor(Cor::MADEIRA) + "*" + Aparencia::cor(Cor::RESET); // Tronco Marrom
            return Aparencia::cor(Cor::VERDE) + "*" + Aparencia::cor(Cor::RESET); // Folhas Verdes
        }
        if (celula == '~') return Aparencia::corRGB(50, 150, 255) + "~" + Aparencia::cor(Cor::RESET); // Agua

        if (tituloDoMapaAtual != "LABIRINTO SUBTERRANEO") {
            std::string estruturas = "|_[]{}/\\<>;=-:+";
            if (estruturas.find(celula) != std::string::npos) return Aparencia::cor(Cor::MADEIRA) + std::string(1, celula) + Aparencia::cor(Cor::RESET); // Madeira Estruturas
            if (celula == '#') return Aparencia::cor(Cor::VERDE) + std::string(1, celula) + Aparencia::cor(Cor::RESET); // Bordas Floresta (Verdes)
        }

        // Remove a exibicao visual dos pontos (chao) para deixar o mapa mais limpo, sem quebrar a logica de colisao original
        if (celula == '.' && tituloDoMapaAtual != "LABIRINTO SUBTERRANEO") return " ";
        
        if (tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") {
            auto isHWall = [](char c) { return c == '=' || c == '.' || c == '\''; };
            auto isVWall = [](char c) { return c == '|' || c == '+' || c == 'S' || c == 'E'; };

            if (celula == '=') return Aparencia::cor(Cor::CINZA) + "═" + Aparencia::cor(Cor::RESET);
            if (celula == '|') {
                bool right = (x + 1 < static_cast<int>(matrizDoMapaAtual[y].length()) && isHWall(matrizDoMapaAtual[y][x+1]));
                bool left = (x > 0 && isHWall(matrizDoMapaAtual[y][x-1]));
                if (right && left) return Aparencia::cor(Cor::CINZA) + "╬" + Aparencia::cor(Cor::RESET);
                if (right) return Aparencia::cor(Cor::CINZA) + "╠" + Aparencia::cor(Cor::RESET);
                if (left) return Aparencia::cor(Cor::CINZA) + "╣" + Aparencia::cor(Cor::RESET);
                return Aparencia::cor(Cor::CINZA) + "║" + Aparencia::cor(Cor::RESET);
            }
            if (celula == '.') {
                bool right = (x + 1 < static_cast<int>(matrizDoMapaAtual[y].length()) && isHWall(matrizDoMapaAtual[y][x+1]));
                bool left = (x > 0 && isHWall(matrizDoMapaAtual[y][x-1]));
                bool down = (y + 1 < static_cast<int>(matrizDoMapaAtual.size()) && isVWall(matrizDoMapaAtual[y+1][x]));
                
                if (left && right && down) return Aparencia::cor(Cor::CINZA) + "╦" + Aparencia::cor(Cor::RESET);
                if (right && down) return Aparencia::cor(Cor::CINZA) + "╔" + Aparencia::cor(Cor::RESET);
                if (left && down) return Aparencia::cor(Cor::CINZA) + "╗" + Aparencia::cor(Cor::RESET);
                if (left && right) return Aparencia::cor(Cor::CINZA) + "═" + Aparencia::cor(Cor::RESET);
                return Aparencia::cor(Cor::CINZA) + "█" + Aparencia::cor(Cor::RESET);
            }
            if (celula == '\'') {
                bool right = (x + 1 < static_cast<int>(matrizDoMapaAtual[y].length()) && isHWall(matrizDoMapaAtual[y][x+1]));
                bool left = (x > 0 && isHWall(matrizDoMapaAtual[y][x-1]));
                bool up = (y > 0 && isVWall(matrizDoMapaAtual[y-1][x]));
                
                if (left && right && up) return Aparencia::cor(Cor::CINZA) + "╩" + Aparencia::cor(Cor::RESET);
                if (right && up) return Aparencia::cor(Cor::CINZA) + "╚" + Aparencia::cor(Cor::RESET);
                if (left && up) return Aparencia::cor(Cor::CINZA) + "╝" + Aparencia::cor(Cor::RESET);
                if (left && right) return Aparencia::cor(Cor::CINZA) + "═" + Aparencia::cor(Cor::RESET);
                return Aparencia::cor(Cor::CINZA) + "█" + Aparencia::cor(Cor::RESET);
            }
            if (celula == '+') {
                bool right = (x + 1 < static_cast<int>(matrizDoMapaAtual[y].length()) && isHWall(matrizDoMapaAtual[y][x+1]));
                bool left = (x > 0 && isHWall(matrizDoMapaAtual[y][x-1]));
                bool down = (y + 1 < static_cast<int>(matrizDoMapaAtual.size()) && isVWall(matrizDoMapaAtual[y+1][x]));
                bool up = (y > 0 && isVWall(matrizDoMapaAtual[y-1][x]));
                
                if (left && right && down && up) return Aparencia::cor(Cor::CINZA) + "╬" + Aparencia::cor(Cor::RESET);
                if (left && right && down) return Aparencia::cor(Cor::CINZA) + "╦" + Aparencia::cor(Cor::RESET);
                if (left && right && up) return Aparencia::cor(Cor::CINZA) + "╩" + Aparencia::cor(Cor::RESET);
                if (up && down && left) return Aparencia::cor(Cor::CINZA) + "╣" + Aparencia::cor(Cor::RESET);
                if (up && down && right) return Aparencia::cor(Cor::CINZA) + "╠" + Aparencia::cor(Cor::RESET);
                return Aparencia::cor(Cor::CINZA) + "╬" + Aparencia::cor(Cor::RESET);
            }
        }
        
        return std::string(1, celula);
    };

    bool precisaRenderizar = false;
    int linhaInicialParaDesenharOMapa = 0;

    auto restaurarTela = [&]() {
        linhaInicialParaDesenharOMapa = ControleMapa::animarIntroducaoMapa(tituloDoMapaAtual, {}, 0, {}, 0, Cor::VERDE, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, false);
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

        linhaInicialParaDesenharOMapa = ControleMapa::animarIntroducaoMapa(tituloDoMapaAtual, arteTitulo, larguraArte, arteTrans, larguraTrans, Cor::VERDE, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, true);
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
                std::string caracteresParede = "#|_[]{}-=";
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
