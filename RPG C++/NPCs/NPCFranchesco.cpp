#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

#include "NPCFranchesco.h"
#include "../Telas/TelaMenu.h"
#include "../Inventario/Item.h"
#include "../Inventario/FabricaItens.h"
#include "../Telas/TelaInventario.h"
#include "../Utilidades/Aparencia.h"
#include "../Utilidades/ControleDeInput.h"

namespace {
    static const std::vector<std::string> arteFranchesco = {
        "                    .==                    ",
        "                   :%%##%%-                ",
        "                   #+=-:+%%                ",
        "                   .++++=++.               ",
        "                    =#++=+-                ",
        "                     ***%#=                ",
        "                      @%*=+*#%.            ",
        "                     #**--%###***.         ",
        "                   +%%+-*+***#+*###        ",
        "                 :##=--..-##%#%#*###.      ",
        "                #%#*-++#=.:#*#%#%##%%#     ",
        "              .#%##+=++:::%%*%##%-*--+*    ",
        "              :#%#%+*--###%%#%%#+=+=--*=   ",
        "            +=%#@%#*####*##@#%%#*-*-===#+  ",
        "           ==+*%%%#########%%%.:=.::-*+==: ",
        "          .:+-##@%%#####%#+-#=+::=.=-*#%#+ ",
        "        =**%=:=%%#***++=-==-=+-#:-#-**#+ - ",
        "      =+++#:-%*%%#%####**#%#+#*--*=#++*=   ",
        "     +*+*#@=#*#%#+%#+*#*%*%#*%#**=%++#+.   ",
        "      %+#*#+%+%%%%%%#%%###*#@@%%***#@%#    ",
        "      + * ++%@%#*#**##*##%%%@%@@@@@@#%%    ",
        "        :   %%%++++=##+***#@%%%%%#@#%#%.   ",
        "        #-  @%#+=+==%%+=###%#%#%%%%%%#%.   ",
        "        *%   *=*++=-##=-**#%%%%##%#%%#%:   ",
        "        +%  :#=*=+==+*=-+#*%%@#%*%#%%##=   ",
        "        .*  -*+*=*===*=-#####%%%*##%#%%-   ",
        "         *  ++**=*==++==#+#@@%#%#####%#=   ",
        "         %:.*=**=*==++-=#+*%@%*%*#%#%@%-   ",
        "         ##+#%@*-*==++-==**++##@*+%*#%%=   ",
        "         +@ %@@+*#####**#%%####%##%%%%%-   ",
        "         :# #%@@@@@##+=++#%%%%#%%+%%*%#=   ",
        "          #  %%@%%***-@%%#%%  ##%#%#*##-   ",
        "          %.  +%%%@#*.-%%%%.  #%%###+*#-   ",
        "          #+  +%%%%-  :%*=*-    #*##+      ",
        "          *@   %%%%=  *%#+#-    %#*        ",
        "          :#   -%%%    %%*%                ",
        "           #   %%%%+   %*#:                ",
        "           %   *%%#   *%###                ",
        "           #=:-%%%%*===%%#-:....           ",
        "    .:--=+**%%###%%%***#%%*+==-::..        ",
        "        .-*+#%*==--+**%#*##+-:..           ",
        "                     ***###.               "
    };

    void processarCompraPocoes(SistemaPersonagem* jogadorAtual);
    void processarCompraTalismas(SistemaPersonagem* jogadorAtual, int larguraDoTerminal);
    void processarCompraIguarias(SistemaPersonagem* jogadorAtual, int larguraDoTerminal);
    void processarVendaDeItens(SistemaPersonagem* jogadorAtual, int larguraDoTerminal);

    void dialogoFranchesco(const std::string& texto, bool novaLinhaAntes = true, bool novaLinhaDepois = true) {
        Aparencia::imprimirDialogoNPC("Franchesco", Cor::AMARELO, texto, novaLinhaAntes, novaLinhaDepois);
    }

    void dialogoFranchesco(const std::vector<std::string>& linhas) {
        Aparencia::imprimirDialogoNPC("Franchesco", Cor::AMARELO, linhas);
    }
}

std::string NPCFranchesco::obterNomeDoLugar() const {
    return "LOJA AMBULANTE";
}

Cor NPCFranchesco::obterCorDoCabecalho() const {
    return Cor::AMARELO;
}

Cor NPCFranchesco::obterCorDaArte() const {
    return Cor::AMARELO;
}

const std::vector<std::string>& NPCFranchesco::obterArteASCII() const {
    return arteFranchesco;
}

void NPCFranchesco::exibirDialogo(SistemaPersonagem* jogador) {
    dialogoFranchesco(std::vector<std::string>{
        "Bem-vindo! De uma olhada nas",
        "minhas mercadorias."
    });
}

std::vector<std::string> NPCFranchesco::obterOpcoesMenu(SistemaPersonagem* jogador, int larguraDoTerminal) {
    return {
        "Seu Ouro: " + std::to_string(jogador->obterInventario()->obterOuro()) + "G",
        "",
        "[1] COMPRAR Pocoes",
        "[2] COMPRAR Talismas",
        "[3] COMPRAR Iguarias",
        "[4] VENDER Itens do Inventario",
        "[0] VOLTAR",
        ""
    };
}

void NPCFranchesco::processarOpcao(SistemaPersonagem* jogador, const std::string& opcao, int larguraDoTerminal) {
    if (opcao == "1") {
        processarCompraPocoes(jogador);
    }
    else if (opcao == "2") {
        processarCompraTalismas(jogador, larguraDoTerminal);
    }
    else if (opcao == "3") {
        processarCompraIguarias(jogador, larguraDoTerminal);
    }
    else if (opcao == "4") {
        processarVendaDeItens(jogador, larguraDoTerminal);
    }
}

namespace {
    void processarCompraPocoes(SistemaPersonagem* jogadorAtual) {
        std::string opcaoCompra;
        do {
            Aparencia::limparTela();
            Aparencia::exibirCabecalho("LOJA - POCOES", Cor::AMARELO);
            std::cout << "\n";
            
            std::vector<std::string> linhas = {
                "Seu Ouro: " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G",
                "",
                "[1] Pocao de Cura (30%VM)                          - 10G",
                "",
                "[0] VOLTAR"
            };
            Aparencia::imprimirBlocoCentralizado(linhas);
            std::cout << "\n";
            Aparencia::exibirPrompt("Escolha: \033[s");
            
            while (true) {
                opcaoCompra = ControleDeInput::lerEntradaProtegida();
                if (opcaoCompra == "0" || opcaoCompra == "1") break;
                std::cout << "\033[u\033[J";
            }

            if (opcaoCompra == "1") {
                int preco = 10;
                if (jogadorAtual->obterInventario()->obterOuro() >= preco) {
                    jogadorAtual->obterInventario()->adicionarOuro(-preco);
                    jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem(ItemID::PocaoCura30));
                    dialogoFranchesco("Pocao de Cura comprada!");
                } else {
                    dialogoFranchesco("Pobreta...");
                }
                Aparencia::aguardarEnter();
            }
        } while (opcaoCompra != "0");
    }

    void processarCompraTalismas(SistemaPersonagem* jogadorAtual, int larguraDoTerminal) {
        std::string opcaoCompra;
        do {
            Aparencia::limparTela();
            Aparencia::exibirCabecalho("LOJA - TALISMAS", Cor::AMARELO);
            std::cout << "\n";

            std::vector<std::string> linhas = {
                "Seu Ouro: " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G",
                "",
                "[1] Talisma do Urso (+5 Forca | -5 Int)             - 200G",
                "[2] Talisma do Corvo (+5 Int | -5 Forca)            - 200G",
                "[3] Talisma do Leopardo (+5 Dest | -5 Sab)          - 200G",
                "[4] Talisma da Coruja (+5 Sab | -5 Dest)            - 200G",
                "",
                "[0] VOLTAR"
            };
            Aparencia::imprimirBlocoCentralizado(linhas);
            std::cout << "\n";
            Aparencia::exibirPrompt("Escolha: \033[s");
            
            while (true) {
                opcaoCompra = ControleDeInput::lerEntradaProtegida();
                if (opcaoCompra >= "0" && opcaoCompra <= "4") break;
                std::cout << "\033[u\033[J";
            }

            if (opcaoCompra != "0") {
                int preco = 200;
                if (jogadorAtual->obterInventario()->obterOuro() >= preco) {
                    jogadorAtual->obterInventario()->adicionarOuro(-preco);
                    
                    ItemID idTalisma = ItemID::Nenhum;
                    if (opcaoCompra == "1") idTalisma = ItemID::TalismaUrso;
                    else if (opcaoCompra == "2") idTalisma = ItemID::TalismaCorvo;
                    else if (opcaoCompra == "3") idTalisma = ItemID::TalismaLeopardo;
                    else if (opcaoCompra == "4") idTalisma = ItemID::TalismaCoruja;
                    
                    auto novoItem = FabricaItens::criarItem(idTalisma);
                    if (novoItem) {
                        dialogoFranchesco(novoItem->obterNomeItem() + " comprado!");
                        jogadorAtual->obterInventario()->adicionarItem(std::move(novoItem));
                    }
                } else {
                    dialogoFranchesco("Pobreta...");
                }
                Aparencia::aguardarEnter();
            }
        } while (opcaoCompra != "0");
    }

    void processarCompraIguarias(SistemaPersonagem* jogadorAtual, int larguraDoTerminal) {
        std::string opcaoCompra;
        do {
            Aparencia::limparTela();
            Aparencia::exibirCabecalho("LOJA - IGUARIAS", Cor::AMARELO);
            std::cout << "\n";

            std::vector<std::string> linhas = {
                "Seu Ouro: " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G",
                "",
                "[1] Dispositivo de teclas de linguagem desconhecida - 1000G",
                "",
                "[0] VOLTAR"
            };
            Aparencia::imprimirBlocoCentralizado(linhas);
            std::cout << "\n";
            Aparencia::exibirPrompt("Escolha: \033[s");
            
            while (true) {
                opcaoCompra = ControleDeInput::lerEntradaProtegida();
                if (opcaoCompra == "0" || opcaoCompra == "1") break;
                std::cout << "\033[u\033[J";
            }

            if (opcaoCompra == "1") {
                int preco = 1000;
                if (jogadorAtual->obterInventario()->obterOuro() >= preco) {
                    jogadorAtual->obterInventario()->adicionarOuro(-preco);
                    jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem(ItemID::DispositivoLinguagem));
                    dialogoFranchesco("Dispositivo misterioso comprado!");
                } else {
                    dialogoFranchesco("Pobreta...");
                }
                Aparencia::aguardarEnter();
            }
        } while (opcaoCompra != "0");
    }

    void processarVendaDeItens(SistemaPersonagem* jogadorAtual, int larguraDoTerminal) {
        std::string codigoVenda;
        do {
            TelaInventario::exibir(jogadorAtual, true);
            dialogoFranchesco("Digite o codigo do item para vender ou [0] VOLTAR: ", false, false);
            std::cout << "\033[s";
            
            Item* itemParaVenda = nullptr;
            while (true) {
                codigoVenda = ControleDeInput::lerEntradaProtegida();
                if (codigoVenda == "0") break;
                itemParaVenda = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigoVenda, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
                if (itemParaVenda) break;
                std::cout << "\033[u\033[J";
            }

            if (codigoVenda != "0") {
                if (itemParaVenda == jogadorAtual->obterArma() || itemParaVenda == jogadorAtual->obterEscudo() || itemParaVenda == jogadorAtual->obterArmadura()) {
                    dialogoFranchesco("Nao e possivel vender itens que estao equipados!"); Aparencia::aguardarEnter(); continue;
                }
                std::string nomeItemVenda = itemParaVenda->obterNomeItem();
                int precoVenda = itemParaVenda->obterPrecoVenda();
                jogadorAtual->obterInventario()->adicionarOuro(precoVenda);
                jogadorAtual->obterInventario()->removerItem(itemParaVenda);
                dialogoFranchesco("Voce vendeu " + nomeItemVenda + " por " + std::to_string(precoVenda) + "G!"); Aparencia::aguardarEnter();
            }
        } while (codigoVenda != "0");
    }
}
