#include "NPCCavaleiroGenerico.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

#include "../../Classes/Guerreiro.h"
#include "../../Racas/Humano.h"
#include "../../Inventario/FabricaItens.h"
#include "../../Inventario/InventarioMateriais/ItemMaterial.h"
#include "../../Gerenciadores/GerenciadoresCombate/GerenciadorCombate.h"
#include "../../Gerenciadores/GerenciadoresCombate/GerenciadorInimigos.h"
#include "../../Telas/TelasSistema/TelaMenu.h"
#include "../../Utilidades/Aparencia.h"
#include "../../Utilidades/ControleDeInput.h"
#include "NPCCavaleiroGenericoLayouts.h"

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

    // --- APARENCIA E DIALOGOS ---
    void dialogoCavaleiro(const std::string& texto, bool novaLinhaAntes = true, bool novaLinhaDepois = true) {
        // Removido o "  " extra para consistência com a nova função auxiliar e outros NPCs.
        Aparencia::imprimirDialogoNPC("Cavaleiro Real", Cor::CINZA, texto, novaLinhaAntes, novaLinhaDepois);
    }

    void dialogoCavaleiro(const std::vector<std::string>& linhas) {
        Aparencia::imprimirDialogoNPC("Cavaleiro Real", Cor::CINZA, linhas);
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
        Aparencia::limparTela();
        Aparencia::exibirCabecalho(tituloCabecalho, Cor::CINZA);
        Aparencia::imprimirBlocoCentralizado(NPCCavaleiroGenericoLayouts::arteCavaleiro);
        std::cout << "\n";
        dialogoCavaleiro(falas);
        std::cout << "\n";
    }
}

// --- CRIACAO DO NPC ---
std::unique_ptr<SistemaPersonagem> NPCCavaleiroGenerico::criarCavaleiro(const std::string& nome) {
    auto cavaleiro = std::make_unique<SistemaPersonagem>(nome, std::make_unique<Humano>(), std::make_unique<ClasseCavaleiro>());
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
void NPCCavaleiroGenerico::interagir(SistemaPersonagem* jogadorAtual, bool& trollDerrotado, bool& conviteRecebido, int larguraDoTerminal, std::vector<std::string>& matrizDoMapaAtual, bool exploracaoEstaAtiva, const std::function<void()>& restaurarTela, char celulaDestino, int proximaPosicaoX, int proximaPosicaoY) {
    if (!trollDerrotado && (celulaDestino == 'T' || celulaDestino == 'C')) {
        int posicaoTrollX = -1, posicaoTrollY = -1;
        
        if (celulaDestino == 'T') {
            posicaoTrollX = proximaPosicaoX;
            posicaoTrollY = proximaPosicaoY;
        } else if (celulaDestino == 'C') {
            buscarTrollProximo(matrizDoMapaAtual, proximaPosicaoX, proximaPosicaoY, posicaoTrollX, posicaoTrollY);
        }

        if (posicaoTrollX == -1) {
            exibirTelaCavaleiro("CAVALEIROS REAIS", {
                "Ainda temos invasores no reino!,",
                "voce precisa de permissao se nao quiser ser",
                "tratado como invasor tambem...",
                "Nos ajude a derrotar todos e podemos",
                "garantir sua entrar no reino!"
            });
            
            ControleDeInput::aguardarEnter();
            if (exploracaoEstaAtiva) restaurarTela();
            return;
        }

        exibirTelaCavaleiro("PEDIDO DE AJUDA", {
            "Viajante! Este Troll bloqueia a passagem.",
            "Nossas forcas estao se esgotando!",
            "Nos ajude a derrota-lo e o recompensaremos!"
        });
        
        std::vector<std::string> opcoes = { "Ajudar os Cavaleiros", "Recuar" };
        
        int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoes);
        if (escolha == 0) {
            std::vector<std::unique_ptr<SistemaPersonagem>> aliados;
            aliados.push_back(criarCavaleiro("Cavaleiro Real 1"));
            aliados.push_back(criarCavaleiro("Cavaleiro Real 2"));
            
            std::vector<std::unique_ptr<SistemaPersonagem>> inimigos;
            auto trolls = GerenciadorInimigos::criarInimigoTroll(1);
            if (!trolls.empty()) inimigos.push_back(std::move(trolls[0])); 
            
            GerenciadorCombate combate(jogadorAtual, std::move(inimigos));
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
                }
            }
        }
        if (exploracaoEstaAtiva) restaurarTela();
    } else if (celulaDestino == 'C') {
        if (!conviteRecebido) {
            exibirTelaCavaleiro("RECOMPENSA", {
                "Voce lutou bravamente e limpou o reino dos Trolls!",
                "Como prometido, aqui esta a sua recompensa."
            });
            
            std::vector<std::string> info = { Aparencia::cor(Cor::AMARELO) + "[SISTEMA]: Voce recebeu o [Convite Real]!" + Aparencia::cor(Cor::RESET) };
            Aparencia::imprimirBlocoCentralizado(info);
            
            jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem(ItemID::ConviteReal));
            conviteRecebido = true;
            ControleDeInput::aguardarEnter();
            restaurarTela();
        } else {
            exibirTelaCavaleiro("CAVALEIRO REAL", { "O Rei o aguarda no castelo. Siga em frente!" });
            ControleDeInput::aguardarEnter();
            restaurarTela();
        }
    }
}
