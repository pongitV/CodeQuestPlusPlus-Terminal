#include "NPCCavaleiroGenerico.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

#include "../../Classes/Guerreiro.h"
#include "../../Racas/Humano.h"
#include "../../../Sistemas/Inventario/FabricaItens.h"
#include "../../../Sistemas/Inventario/Itens/ItemMaterial.h"
#include "../../../Sistemas/Combate/Combate.h"
#include "../../../Core/Controladores/CriadorInimigos.h"
#include "../../../Perspectiva/TelasBase/Menu/TelaMenu.h"
#include "../../../Core/Utilidades/RendererProvider.h"
#include "../../../Core/Utilidades/FuncoesDialogo.h"
#include "../../../Sistemas/Progresso/Diario.h"
#include "../../../Sistemas/Progresso/Progressao.h"
#include "../../../Sistemas/Progresso/ProgressaoFlags.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Mapas/ControleMapa.h"
#include "NPCCavaleiroGenericoLayout.h"

namespace {
    // --- CLASSES E FUNCOES AUXILIARES ---
    Item* buscarPorNome(Inventario* inv, const std::string& nome) {
        for (auto* item : inv->obterTodosOsItens()) {
            if (item->obterNomeItem() == nome) return item;
        }
        return nullptr;
    }

    class ClasseCavaleiro : public Guerreiro {
    public:
        std::string obterNomeClasse() const override { return "Cavaleiro Real"; }
        
        std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override {
            std::vector<std::unique_ptr<Item>> equipamentos;
            Guerreiro base;
            for (auto& item : base.obterEquipamentoClasse()) {
                if (item->obterTipo() == TipoEquipamento::ESCUDO || item->obterTipo() == TipoEquipamento::CONSUMIVEL) {
                    equipamentos.push_back(std::move(item));
                }
            }
            equipamentos.push_back(FabricaItens::criarItem(ItemID::EspadaCavaleiro));
            return equipamentos;
        }
    };

    class RacaCavaleiro : public Humano {
    public:
        const std::vector<std::string>& obterAparenciaRaca() const override {
            return NPCCavaleiroGenericoLayouts::arteCavaleiro;
        }
    };

    // Comentario adicionado para forcar a recompilacao e resolver erros de linkagem do Guerreiro
    // --- APARENCIA E DIALOGOS ---
    void dialogoCavaleiro(const std::vector<std::string>& linhas) {
        FuncoesDialogo::imprimirDialogoNPC("Cavaleiro Real", Cor::CINZA, linhas);
    }

    bool buscarTrollProximo(const std::vector<std::string>& mapa, int startX, int startY, int& outX, int& outY) {
        for (int dy = -2; dy <= 2; ++dy) {
            for (int dx = -5; dx <= 5; ++dx) {
                int y = startY + dy;
                int x = startX + dx;
                if (y >= 0 && y < static_cast<int>(mapa.size()) && x >= 0 && x < static_cast<int>(mapa[y].size())) {
                    if (mapa[y][x] == 'T') {
                        outX = x;
                        outY = y;
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void exibirTelaCavaleiro(const std::string& tituloCabecalho, const std::vector<std::string>& falas) {
        if (RendererProvider::get()) RendererProvider::get()->exibirPopup(tituloCabecalho, falas, Cor::CINZA, NPCCavaleiroGenericoLayouts::arteCavaleiro);
    }
}

// --- CRIACAO DO NPC ---
std::unique_ptr<Personagem> NPCCavaleiroGenerico::criarCavaleiro(const std::string& nome) {
    auto cavaleiro = std::make_unique<Personagem>(nome, std::make_unique<RacaCavaleiro>(), std::make_unique<ClasseCavaleiro>());
    std::string nomeArmadura = FabricaItens::obterNomeDeID(ItemID::ArmaduraCavaleiro);
    std::string nomeEspada = FabricaItens::obterNomeDeID(ItemID::EspadaCavaleiro);
    cavaleiro->obterInventario()->adicionarItem(FabricaItens::criarItem(ItemID::ArmaduraCavaleiro));
    cavaleiro->equiparItem(buscarPorNome(cavaleiro->obterInventario(), nomeArmadura));
    cavaleiro->equiparItem(buscarPorNome(cavaleiro->obterInventario(), nomeEspada));
    cavaleiro->calcularAtributos();
    cavaleiro->modificarVida(cavaleiro->obterVidaMaxima());
    return cavaleiro;
}

// --- INTERACAO ---
void NPCCavaleiroGenerico::interagir(Personagem* jogadorAtual, bool& trollDerrotado, bool& conviteRecebido, int /*larguraDoTerminal*/, std::vector<std::string>& matrizDoMapaAtual, bool exploracaoEstaAtiva, const std::function<void()>& restaurarTela, char celulaDestino, int proximaPosicaoX, int proximaPosicaoY) {
    Diario::instancia().registrarNPC("Cavaleiro Real");
    if (!trollDerrotado && (celulaDestino == 'T' || celulaDestino == 'C')) {
        int posicaoTrollX = -1, posicaoTrollY = -1;
        
        if (celulaDestino == 'T') {
            posicaoTrollX = proximaPosicaoX;
            posicaoTrollY = proximaPosicaoY;
        } else if (celulaDestino == 'C') {
            buscarTrollProximo(matrizDoMapaAtual, proximaPosicaoX, proximaPosicaoY, posicaoTrollX, posicaoTrollY);
        }

        if (posicaoTrollX == -1) {
            if (RendererProvider::get()) RendererProvider::get()->iniciarInteracaoPopup();
            std::vector<std::string> falas = {
                "Ainda temos invasores no reino!",
                "voce precisa de permissao se nao quiser ser",
                "tratado como invasor tambem...",
                "Nos ajude a derrotar todos e podemos",
                "garantir sua entrada no reino!"
            };
            if (RendererProvider::get()) RendererProvider::get()->exibirPopup("CAVALEIROS REAIS", falas, Cor::CINZA, NPCCavaleiroGenericoLayouts::arteCavaleiro);
            return;
        }

        if (RendererProvider::get()) RendererProvider::get()->iniciarInteracaoPopup();
        std::vector<std::string> falas = {
            "Viajante! Este Troll bloqueia a passagem.",
            "Nossas forcas estao se esgotando!",
            "Nos ajude a derrota-lo e o recompensaremos!"
        };
        
        int escolha = 1; // Default
        ControleDeInput::executarLoopMenuPopup(
            [&]() { Aparencia::exibirPopup("PEDIDO DE AJUDA", falas, Cor::CINZA, NPCCavaleiroGenericoLayouts::arteCavaleiro); },
            [&]() { return std::vector<std::string>{"Ajudar os Cavaleiros", "Recuar"}; },
            [&](const std::string& op) {
                if (op == "Ajudar os Cavaleiros") escolha = 0;
                else escolha = 1;
                return false; // Exit loop after choice
            },
            "PEDIDO DE AJUDA", Cor::CINZA, NPCCavaleiroGenericoLayouts::arteCavaleiro
        );

        if (escolha == 0) {
            Diario::instancia().registrarMissaoAceita("cavaleiro_trolls");
            std::vector<std::unique_ptr<Personagem>> aliados;
            aliados.push_back(criarCavaleiro("Cavaleiro Real 1"));
            aliados.push_back(criarCavaleiro("Cavaleiro Real 2"));
            
            std::vector<std::unique_ptr<Personagem>> inimigos;
            auto trolls = CriadorInimigos::criarInimigoTroll(1);
            if (!trolls.empty()) inimigos.push_back(std::move(trolls[0])); 
            
            Combate combate(jogadorAtual, std::move(inimigos));
            combate.adicionarAliados(std::move(aliados));
            if (ControleMapa::isExploracao3DAtiva()) {
                combate.setContexto3D(
                    true, 
                    matrizDoMapaAtual, 
                    ControleMapa::obterPosCamera3DX(), 
                    ControleMapa::obterPosCamera3DY(), 
                    ControleMapa::obterAnguloCamera3D(), 
                    ControleMapa::obterTituloMapaAtual()
                );
            }
            combate.iniciarCombate();
            
            if (jogadorAtual->obterVida() > 0) {
                matrizDoMapaAtual[posicaoTrollY][posicaoTrollX] = '.';
                
                int trollsRestantes = 0;
                for (const auto& linha : matrizDoMapaAtual) {
                    trollsRestantes += std::count(linha.begin(), linha.end(), 'T');
                }
                if (trollsRestantes == 0) {
                    trollDerrotado = true;
                    Progressao::instancia().definirFlag(Flags::PonteReino_TrollDerrotado, true);
                }
            }
        }
        if (exploracaoEstaAtiva && !ControleMapa::isExploracao3DAtiva()) restaurarTela();
    } else if (celulaDestino == 'C') {
        ControleDeInput::executarLoopMenuPopup(
            [&]() {
                if (!conviteRecebido) {
                    Aparencia::exibirPopup("CAVALEIRO REAL", {"Saudacoes, viajante. O que deseja?"}, Cor::CINZA, NPCCavaleiroGenericoLayouts::arteCavaleiro);
                } else {
                    Aparencia::exibirPopup("CAVALEIRO REAL", {"O Rei o aguarda no Reino. Siga em frente!"}, Cor::CINZA, NPCCavaleiroGenericoLayouts::arteCavaleiro);
                }
            },
            [&]() { return std::vector<std::string>{"Conversar", "Missoes do Cavaleiro", "VOLTAR"}; },
            [&](const std::string& op) {
                if (op == "Conversar") {
                    if (!conviteRecebido) {
                        Aparencia::exibirPopup("CAVALEIRO REAL", { "Obrigado por nos ajudar com os Trolls!" }, Cor::CINZA, NPCCavaleiroGenericoLayouts::arteCavaleiro);
                    } else {
                        Aparencia::exibirPopup("CAVALEIRO REAL", { "O Rei o aguarda no Reino. Siga em frente!" }, Cor::CINZA, NPCCavaleiroGenericoLayouts::arteCavaleiro);
                    }
                    return true;
                } else if (op == "Missoes do Cavaleiro") {
                    if (!conviteRecebido) {
                        int escMissao = -1;
                        ControleDeInput::executarLoopMenuPopup(
                            [&]() { Aparencia::exibirPopup("MISSOES - CAVALEIRO", {"Escolha uma missao:"}, Cor::CINZA, NPCCavaleiroGenericoLayouts::arteCavaleiro); },
                            [&]() { return std::vector<std::string>{"[M] Reportar Trolls derrotados", "VOLTAR"}; },
                            [&](const std::string& subOp) {
                                if (subOp == "[M] Reportar Trolls derrotados") { escMissao = 0; return false; }
                                return false;
                            },
                            "MISSOES - CAVALEIRO", Cor::CINZA, NPCCavaleiroGenericoLayouts::arteCavaleiro
                        );

                        if (escMissao == 0) {
                            std::vector<std::string> recompensaFalas = {
                                "Voce lutou bravamente e limpou o reino dos Trolls!",
                                "Como prometido, aqui esta a sua recompensa.",
                                "",
                                "Voce recebeu o [Convite Real]!"
                            };
                            Aparencia::exibirPopup("RECOMPENSA", recompensaFalas, Cor::AMARELO, NPCCavaleiroGenericoLayouts::arteCavaleiro);
                            jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem(ItemID::ConviteReal));
                            Diario::instancia().registrarItem("Convite Real");
                            Diario::instancia().registrarMissaoConcluida("cavaleiro_trolls");
                            Progressao::instancia().definirFlag(Flags::Vila_ConviteReal, true);
                            conviteRecebido = true;
                        }
                    } else {
                        ControleDeInput::executarLoopMenuPopup(
                            [&]() { Aparencia::exibirPopup("MISSOES - CAVALEIRO", {"Escolha uma missao:"}, Cor::CINZA, NPCCavaleiroGenericoLayouts::arteCavaleiro); },
                            [&]() { return std::vector<std::string>{"(Nenhuma missao disponivel)", "VOLTAR"}; },
                            [&](const std::string& subOp) {
                                if (subOp == "(Nenhuma missao disponivel)") {
                                    Aparencia::exibirPopup("CAVALEIRO REAL", { "Nao precisamos de ajuda no momento." }, Cor::CINZA, NPCCavaleiroGenericoLayouts::arteCavaleiro);
                                }
                                return false;
                            },
                            "MISSOES - CAVALEIRO", Cor::CINZA, NPCCavaleiroGenericoLayouts::arteCavaleiro
                        );
                    }
                    return true;
                } else if (op == "VOLTAR" || op == "Sair") {
                    return false;
                }
                return true;
            },
            "CAVALEIRO REAL", Cor::CINZA, NPCCavaleiroGenericoLayouts::arteCavaleiro
        );
        if (exploracaoEstaAtiva && !ControleMapa::isExploracao3DAtiva()) restaurarTela();
    }
}
