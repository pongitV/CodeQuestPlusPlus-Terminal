#include "NPCCavaleiroGenerico.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

#include "../Classes/Guerreiro.h"
#include "../Racas/Humano.h"
#include "../Inventario/EquipamentoArmadura.h"
#include "../Inventario/EquipamentoArma.h"
#include "../Inventario/FabricaItens.h"
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
            equipamentos.push_back(std::make_unique<EquipamentoArma>(FabricaItens::obterNomeDeID(ItemID::EspadaCavaleiro), 12, 0, 0, 0, 0, 0, 0));
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
        SimplificacoesAparencia::exibirCabecalho(titulo, Cor::CIANO);
        SimplificacoesAparencia::imprimirLadoALado(textoEsquerda, arteCavaleiro, 55, 0, Cor::RESET, Cor::CIANO);
    }
}

std::unique_ptr<SistemaPersonagem> NPCCavaleiroGenerico::criarCavaleiro(const std::string& nome) {
    auto cavaleiro = std::make_unique<SistemaPersonagem>(nome, std::make_unique<Humano>(), std::make_unique<ClasseCavaleiro>());
    std::string nomeArmadura = FabricaItens::obterNomeDeID(ItemID::ArmaduraCavaleiro);
    std::string nomeEspada = FabricaItens::obterNomeDeID(ItemID::EspadaCavaleiro);
    auto armadura = std::make_unique<EquipamentoArmadura>(nomeArmadura, 12, 0, 0, 0);
    cavaleiro->obterInventario()->adicionarItem(std::move(armadura));
    cavaleiro->equiparItem(buscarPorNome(cavaleiro->obterInventario(), nomeArmadura));
    cavaleiro->equiparItem(buscarPorNome(cavaleiro->obterInventario(), nomeEspada));
    cavaleiro->calcularAtributos();
    cavaleiro->modificarVida(cavaleiro->obterVidaMaxima());
    return cavaleiro;
}

void NPCCavaleiroGenerico::interagir(SistemaPersonagem* jogadorAtual, bool& trollDerrotado, bool& conviteRecebido, int larguraDoTerminal, std::vector<std::string>& matrizDoMapaAtual, bool exploracaoEstaAtiva, const std::function<void()>& restaurarTela, char celulaDestino, int proximaPosicaoX, int proximaPosicaoY) {
    if (!trollDerrotado && (celulaDestino == 'T' || celulaDestino == 'C')) {
        int posicaoTrollX = -1, posicaoTrollY = -1;
        
        if (celulaDestino == 'T') {
            posicaoTrollX = proximaPosicaoX;
            posicaoTrollY = proximaPosicaoY;
        } else if (celulaDestino == 'C') {
            for (int deslocamentoY = -2; deslocamentoY <= 2; ++deslocamentoY) {
                for (int deslocamentoX = -5; deslocamentoX <= 5; ++deslocamentoX) {
                    int coordenadaYAvaliada = proximaPosicaoY + deslocamentoY;
                    int coordenadaXAvaliada = proximaPosicaoX + deslocamentoX;
                    if (coordenadaYAvaliada >= 0 && coordenadaYAvaliada < static_cast<int>(matrizDoMapaAtual.size()) && coordenadaXAvaliada >= 0 && coordenadaXAvaliada < static_cast<int>(matrizDoMapaAtual[coordenadaYAvaliada].size())) {
                        if (matrizDoMapaAtual[coordenadaYAvaliada][coordenadaXAvaliada] == 'T') {
                            posicaoTrollX = coordenadaXAvaliada;
                            posicaoTrollY = coordenadaYAvaliada;
                            break;
                        }
                    }
                }
                if (posicaoTrollX != -1) break;
            }
        }

        if (posicaoTrollX == -1) {
            std::vector<std::string> texto = {
                "[Cavaleiro Real]: Ja temos Trolls tentando",
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
        SimplificacoesAparencia::exibirPrompt("Escolha: ");
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
                matrizDoMapaAtual[posicaoTrollY][posicaoTrollX] = '.';
                
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
            jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem(ItemID::ConviteReal));
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
