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
#include "../../../Interface/Telas/Menu/TelaMenu.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/FuncoesDialogo.h"
#include "../../../Sistemas/Progresso/Diario.h"
#include "../../../Sistemas/Progresso/Progressao.h"
#include "../../../Sistemas/Progresso/ProgressaoFlags.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Sistemas/Mundo/ControleMapa.h"
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
        Aparencia::exibirPopup(tituloCabecalho, falas, Cor::CINZA);
    }
}

// --- CRIACAO DO NPC ---
std::unique_ptr<Personagem> NPCCavaleiroGenerico::criarCavaleiro(const std::string& nome) {
    auto cavaleiro = std::make_unique<Personagem>(nome, std::make_unique<Humano>(), std::make_unique<ClasseCavaleiro>());
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
            Aparencia::iniciarInteracaoPopup();
            std::vector<std::string> falas = {
                "Ainda temos invasores no reino!",
                "voce precisa de permissao se nao quiser ser",
                "tratado como invasor tambem...",
                "Nos ajude a derrotar todos e podemos",
                "garantir sua entrada no reino!"
            };
            Aparencia::exibirPopup("CAVALEIROS REAIS", falas, Cor::CINZA);
            return;
        }

        Aparencia::iniciarInteracaoPopup();
        std::vector<std::string> falas = {
            "Viajante! Este Troll bloqueia a passagem.",
            "Nossas forcas estao se esgotando!",
            "Nos ajude a derrota-lo e o recompensaremos!"
        };
        
        int escolha = ControleDeInput::lerSelecaoMenuEmPopup("PEDIDO DE AJUDA", falas, {"Ajudar os Cavaleiros", "Recuar"}, Cor::CINZA);
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
            combate.iniciarCombate();
            
            if (jogadorAtual->obterVida() > 0) {
                matrizDoMapaAtual[posicaoTrollY][posicaoTrollX] = '.';
                
                int trollsRestantes = 0;
                for (const auto& linha : matrizDoMapaAtual) {
                    trollsRestantes += std::count(linha.begin(), linha.end(), 'T');
                }
                if (trollsRestantes == 0) {
                    trollDerrotado = true;
                    Progressao::instancia().definirFlag(Flags::Reino_TrollDerrotado, true);
                }
            }
        }
        if (exploracaoEstaAtiva && !ControleMapa::isExploracao3DAtiva()) restaurarTela();
    } else if (celulaDestino == 'C') {
        bool conversando = true;
        while (conversando) {
            Aparencia::iniciarInteracaoPopup();
            std::vector<std::string> opcoes = {
                "Conversar",
                "Missoes do Cavaleiro",
                "Sair"
            };
            int escolha = ControleDeInput::lerSelecaoMenuEmPopup(
                "CAVALEIRO REAL",
                {"Saudacoes, viajante. O que deseja?"},
                opcoes,
                Cor::CINZA
            );

            if (escolha == 0) {
                Aparencia::iniciarInteracaoPopup();
                if (!conviteRecebido) {
                    Aparencia::exibirPopup("CAVALEIRO REAL", { "Obrigado por nos ajudar com os Trolls!" }, Cor::CINZA);
                } else {
                    Aparencia::exibirPopup("CAVALEIRO REAL", { "O Rei o aguarda no castelo. Siga em frente!" }, Cor::CINZA);
                }
            } else if (escolha == 1) {
                Aparencia::iniciarInteracaoPopup();
                if (!conviteRecebido) {
                    std::vector<std::string> missoes = { "[M] Reportar Trolls derrotados", "Voltar" };
                    int escMissao = ControleDeInput::lerSelecaoMenuEmPopup("MISSOES - CAVALEIRO", {"Escolha uma missao:"}, missoes, Cor::CINZA);
                    if (escMissao == 0) {
                        std::vector<std::string> falas = {
                            "Voce lutou bravamente e limpou o reino dos Trolls!",
                            "Como prometido, aqui esta a sua recompensa.",
                            "",
                            "Voce recebeu o [Convite Real]!"
                        };
                        Aparencia::exibirPopup("RECOMPENSA", falas, Cor::AMARELO);
                        jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem(ItemID::ConviteReal));
                        Diario::instancia().registrarItem("Convite Real");
                        Diario::instancia().registrarMissaoConcluida("cavaleiro_trolls");
                        Progressao::instancia().definirFlag(Flags::Vila_ConviteReal, true);
                        conviteRecebido = true;
                    }
                } else {
                    std::vector<std::string> missoes = { "(Nenhuma missao disponivel)", "Voltar" };
                    int escMissao = ControleDeInput::lerSelecaoMenuEmPopup("MISSOES - CAVALEIRO", {"Escolha uma missao:"}, missoes, Cor::CINZA);
                    if (escMissao == 0) {
                        Aparencia::exibirPopup("CAVALEIRO REAL", { "Nao precisamos de ajuda no momento." }, Cor::CINZA);
                    }
                }
            } else {
                conversando = false;
            }
        }
        if (exploracaoEstaAtiva && !ControleMapa::isExploracao3DAtiva()) restaurarTela();
    }
}
