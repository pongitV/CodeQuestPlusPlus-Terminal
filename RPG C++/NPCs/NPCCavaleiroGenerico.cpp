#include "NPCCavaleiroGenerico.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

#include "../Classes/Guerreiro.h"
#include "../Racas/Humano.h"
#include "../Inventario/EquipamentoArmadura.h"
#include "../Inventario/EquipamentoArma.h"
#include "../Inventario/ItemMaterial.h"
#include "../Gerenciadores/GerenciadorCombate.h"
#include "../Gerenciadores/GerenciadorInimigos.h"
#include "../Telas/TelaMenu.h"
#include "../Utilidades/SimplificacoesAparencia.h"

namespace {
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
            equipamentos.push_back(std::make_unique<EquipamentoArma>("Espada do Cavaleiro", 12, 0, 0, 0, 0, 0, 0));
            return equipamentos;
        }
    };

    std::vector<std::string> arteCavaleiro = {
        "      .:-*+*                             ",
        "         +.   .*# *+*#                   ",
        "          =  *: -=*-*.:+                 ",
        "           ** ::.==**=                   ",
        "            **--:-***#                   ",
        "            :*-=--#.:=                   ",
        "          =+*#++= #%** *+                ",
        "           +%@%%%*.**....:*              ",
        "        #**=#+*%#: =-.::...:*+           ",
        "        --=#**-::..-*.:::.==-*           ",
        "       %#+=*#%.::#-. -. :- .:-#          ",
        "       %*+#+*--.=:.  -++==:.- =          ",
        "       *%*+=#-::. :-..#@%%@#*#           ",
        "       #%##+*-=+=--:=:+*%%%*=*           ",
        "      @%%%##*-+=====-**%%%+-*#           ",
        "     #%#%@%#+==++++***#  %%#*=+%         ",
        "    *%+%#*+#%*=+=+**+#=  %==*-.#         ",
        "    #*%+#.#%#==+:#%%%=#  *%%%*+%         ",
        "   =#-## %%%%%#%%##*##%% %%%%#%          ",
        "   **=+ %#%@%%%%%%#%%@%%@=*=:-%          ",
        "   *#= *#*#*=**=*#++*-#*%#*-+*#          ",
        "   *+@*%=+#+---:-+++*=-*%##%*            ",
        "  ** *=+#@*+---:--*=*=:*%#=#.            ",
        "  :#*  *##=+:--:-.+=++-##%#*             ",
        "   *  .#@#==:-:-::+#+#=+*%+%+            ",
        "       #@%==:-:=::-##+*-*#*==+           ",
        "       +@#=+:=:=-:-%%*###%+.++:          ",
        "       %@*=*=*:#=.*%%*+=#**#+=+          ",
        "      -*@*%#**:#+=+%#++-%%*: *           ",
        "      =*##*@*%*** *%##*=+#*:-            ",
        "     :*#=++%***** #%**#-**##:            ",
        "     :*****#**++* :*%##=+*%#:-           ",
        "    =:+*##*+=-++*  *+%*#+#=*:=           ",
        "    =-**+*==#*++*  *+%##++-*:=           ",
        "    #%** #=-*+     ++=##*==#+#           ",
        "    =+   +==#         -*#+=.-=           ",
        "    :=   *==+          **++ #            ",
        "        .%#--*         #*+%              ",
        "        -+*+*          **+#              ",
        "      .====#.          ###*              ",
        "    #-:++:             #*#%              ",
        "                       *:*%              ",
        "                      #=::%              ",
        "                       *="
    };

    void exibirDialogoCavaleiro(const std::string& titulo, const std::vector<std::string>& textoEsquerda, int larguraTerminalAtual) {
        SimplificacoesAparencia::limparTela();
        TelaMenu::exibirLogoDoJogo(titulo);
        int maxLinhas = std::max(textoEsquerda.size(), arteCavaleiro.size());
        int larguraInfo = 55;
        int recuo = std::max(0, (larguraTerminalAtual - (larguraInfo + 40)) / 2);
        std::cout << "\n";
        for (size_t i = 0; i < maxLinhas; ++i) {
            std::cout << std::string(recuo, ' ');
            if (i < textoEsquerda.size()) {
                std::cout << std::left << std::setw(larguraInfo) << textoEsquerda[i];
            } else {
                std::cout << std::string(larguraInfo, ' ');
            }
            if (i < arteCavaleiro.size()) {
                std::cout << SimplificacoesAparencia::cor(Cor::CIANO) << arteCavaleiro[i] << SimplificacoesAparencia::cor(Cor::RESET);
            }
            std::cout << "\n";
        }
    }
}

std::unique_ptr<SistemaPersonagem> NPCCavaleiroGenerico::criarCavaleiro(const std::string& nome) {
    auto cavaleiro = std::make_unique<SistemaPersonagem>(nome, std::make_unique<Humano>(), std::make_unique<ClasseCavaleiro>());
    auto armadura = std::make_unique<EquipamentoArmadura>("Armadura de Cavaleiro", 12, 0, 0, 0);
    cavaleiro->obterInventario()->adicionarItem(std::move(armadura));
    cavaleiro->equiparItem(buscarPorNome(cavaleiro->obterInventario(), "Armadura de Cavaleiro"));
    cavaleiro->equiparItem(buscarPorNome(cavaleiro->obterInventario(), "Espada do Cavaleiro"));
    cavaleiro->calcularAtributos();
    cavaleiro->modificarVida(cavaleiro->obterVidaMaxima());
    return cavaleiro;
}

void NPCCavaleiroGenerico::interagir(SistemaPersonagem* jogadorAtual, bool& trollDerrotado, bool& conviteRecebido, int larguraDoTerminal, std::vector<std::string>& matrizDoMapaAtual, bool exploracaoEstaAtiva, const std::function<void()>& restaurarTela, char celulaDestino, int proximaPosicaoX, int proximaPosicaoY) {
    if (!trollDerrotado && (celulaDestino == 'T' || celulaDestino == 'C')) {
        int alvoTx = -1, alvoTy = -1;
        
        if (celulaDestino == 'T') {
            alvoTx = proximaPosicaoX;
            alvoTy = proximaPosicaoY;
        } else if (celulaDestino == 'C') {
            for (int dy = -2; dy <= 2; ++dy) {
                for (int dx = -5; dx <= 5; ++dx) {
                    int ny = proximaPosicaoY + dy;
                    int nx = proximaPosicaoX + dx;
                    if (ny >= 0 && ny < static_cast<int>(matrizDoMapaAtual.size()) && nx >= 0 && nx < static_cast<int>(matrizDoMapaAtual[ny].size())) {
                        if (matrizDoMapaAtual[ny][nx] == 'T') {
                            alvoTx = nx;
                            alvoTy = ny;
                            break;
                        }
                    }
                }
                if (alvoTx != -1) break;
            }
        }

        if (alvoTx == -1) {
            std::vector<std::string> texto = {
                "{[Cavaleiro Real]: Ja temos Trolls tentando",
                "invadir nosso reino, voce precisa de permissao",
                "se nao quiser ser tratado como invasor tambem"
            };
            exibirDialogoCavaleiro("CAVALEIROS REAIS", texto, larguraDoTerminal);
            SimplificacoesAparencia::aguardarEnter();
            if (exploracaoEstaAtiva) restaurarTela();
            return;
        }

        std::vector<std::string> texto = {
            "[Cavaleiro Real]: Viajante! Este Troll bloqueia a",
            "passagem. Nossas forcas estao se esgotando!",
            "",
            "[Cavaleiro Real]: Nos ajude a derrota-lo e o",
            "recompensaremos!",
            "",
            "[1] Ajudar os Cavaleiros | [0] Recuar"
        };
        exibirDialogoCavaleiro("PEDIDO DE AJUDA", texto, larguraDoTerminal);
        int recuoEscolha = std::max(0, (larguraDoTerminal - 95) / 2);
        std::cout << "\n" << std::string(recuoEscolha, ' ') << "Escolha: ";
        int escolha;
        if (std::cin >> escolha && escolha == 1) {
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
                matrizDoMapaAtual[alvoTy][alvoTx] = '.';
                
                int trollsRestantes = 0;
                for (const auto& linha : matrizDoMapaAtual) {
                    trollsRestantes += std::count(linha.begin(), linha.end(), 'T');
                }
                if (trollsRestantes == 0) {
                    trollDerrotado = true;
                }
            }
        } else {
            std::cin.clear(); std::cin.ignore(1000, '\n');
        }
        if (exploracaoEstaAtiva) restaurarTela();
    } else if (celulaDestino == 'C') {
        if (!conviteRecebido) {
            std::vector<std::string> texto = {
                "[Cavaleiro Real]: Voce lutou bravamente e",
                "limpou o reino dos Trolls!",
                "",
                "Como prometido, aqui esta a sua recompensa.",
                "",
                "[SISTEMA]: Voce recebeu o [Convite Real]!"
            };
            exibirDialogoCavaleiro("RECOMPENSA", texto, larguraDoTerminal);
            jogadorAtual->obterInventario()->adicionarItem(std::make_unique<ItemMaterial>("Convite Real"));
            conviteRecebido = true;
            SimplificacoesAparencia::aguardarEnter();
            restaurarTela();
        } else {
            std::vector<std::string> texto = {
                "[Cavaleiro Real]: O Rei o aguarda no castelo.",
                "Siga em frente!"
            };
            exibirDialogoCavaleiro("CAVALEIRO REAL", texto, larguraDoTerminal);
            SimplificacoesAparencia::aguardarEnter();
            restaurarTela();
        }
    }
}
