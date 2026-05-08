#include "NPCCavaleiroGenerico.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

#include "../Classes/Guerreiro.h"
#include "../Racas/Humano.h"
#include "../Inventario/FabricaItens.h"
#include "../Inventario/ItemMaterial.h"
#include "../Gerenciadores/GerenciadorCombate.h"
#include "../Gerenciadores/GerenciadorInimigos.h"
#include "../Telas/TelaMenu.h"
#include "../Utilidades/Aparencia.h"

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

    void dialogoCavaleiro(const std::string& texto, bool novaLinhaAntes = true, bool novaLinhaDepois = true) {
        // Removido o "  " extra para consistência com a nova função auxiliar e outros NPCs.
        Aparencia::imprimirDialogoNPC("Cavaleiro Real", Cor::CINZA, texto, novaLinhaAntes, novaLinhaDepois);
    }

    void dialogoCavaleiro(const std::vector<std::string>& linhas) {
        Aparencia::imprimirDialogoNPC("Cavaleiro Real", Cor::CINZA, linhas);
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
            Aparencia::limparTela();
            Aparencia::exibirCabecalho("CAVALEIROS REAIS", Cor::CINZA);
            dialogoCavaleiro(std::vector<std::string>{
                "Ja temos Trolls tentando invadir nosso reino,",
                "voce precisa de permissao se nao quiser ser",
                "tratado como invasor tambem."
            });
            std::cout << "\n";
            
            Aparencia::imprimirLadoALado({""}, arteCavaleiro, 45, 0, Cor::RESET, Cor::CINZA);
            Aparencia::aguardarEnter();
            if (exploracaoEstaAtiva) restaurarTela();
            return;
        }

        Aparencia::limparTela();
        Aparencia::exibirCabecalho("PEDIDO DE AJUDA", Cor::CINZA);
        dialogoCavaleiro(std::vector<std::string>{
            "Viajante! Este Troll bloqueia a passagem.",
            "Nossas forcas estao se esgotando!",
            "Nos ajude a derrota-lo e o recompensaremos!"
        });
        
        std::vector<std::string> menuEsquerda = { "[1] Ajudar os Cavaleiros", "[0] Recuar" };
        int recuo = Aparencia::imprimirLadoALado(menuEsquerda, arteCavaleiro, 45, 0, Cor::RESET, Cor::CINZA);
        std::cout << "\n" << std::string(recuo, ' ') << "Escolha: ";
        
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
            Aparencia::limparTela();
            Aparencia::exibirCabecalho("RECOMPENSA", Cor::CINZA);
            dialogoCavaleiro(std::vector<std::string>{
                "Voce lutou bravamente e limpou o reino dos Trolls!",
                "Como prometido, aqui esta a sua recompensa."
            });
            
            std::vector<std::string> menuEsquerda = { Aparencia::cor(Cor::AMARELO) + "[SISTEMA]: Voce recebeu o [Convite Real]!" + Aparencia::cor(Cor::RESET) };
            Aparencia::imprimirLadoALado(menuEsquerda, arteCavaleiro, 45, 0, Cor::RESET, Cor::CINZA);
            
            jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem(ItemID::ConviteReal));
            conviteRecebido = true;
            Aparencia::aguardarEnter();
            restaurarTela();
        } else {
            Aparencia::limparTela();
            Aparencia::exibirCabecalho("CAVALEIRO REAL", Cor::CINZA);
            dialogoCavaleiro("O Rei o aguarda no castelo. Siga em frente!");
            Aparencia::imprimirLadoALado({""}, arteCavaleiro, 45, 0, Cor::RESET, Cor::CINZA);
            Aparencia::aguardarEnter();
            restaurarTela();
        }
    }
}
