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
#include "../Utilidades/SimplificacoesAparencia.h"

namespace {
    void processarCompraPocoes(SistemaPersonagem* jogadorAtual, const std::string& margemMsg);
    void processarCompraTalismas(SistemaPersonagem* jogadorAtual, const std::string& margemMsg, int larguraDoTerminal);
    void processarCompraIguarias(SistemaPersonagem* jogadorAtual, const std::string& margemMsg, int larguraDoTerminal);
    void processarVendaDeItens(SistemaPersonagem* jogadorAtual, int larguraDoTerminal);
}

void NPCFranchesco::interagir(SistemaPersonagem* jogadorAtual)
{
    int larguraDoTerminal = SimplificacoesAparencia::obterLarguraTerminal();
    std::string opcaoFranchesco;
    
    std::vector<std::string> arteFranchesco = {
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

    do {
        SimplificacoesAparencia::limparTela();
        SimplificacoesAparencia::exibirCabecalho("LOJA AMBULANTE", Cor::AMARELO);
        
        int espacosMsg = (larguraDoTerminal - 55) / 2;
        std::string margemMsg(espacosMsg > 0 ? espacosMsg : 0, ' ');

        std::vector<std::string> menuEsquerda = {
            "[Franchesco]: Bem-vindo! De uma olhada nas",
            "minhas mercadorias.",
            "",
            "Seu Ouro: " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G",
            "",
            "[1] COMPRAR Pocoes",
            "[2] COMPRAR Talismas",
            "[3] COMPRAR Iguarias",
            "[4] VENDER Itens do Inventario",
            "[0] VOLTAR",
            ""
        };

        int recuo = SimplificacoesAparencia::imprimirLadoALado(menuEsquerda, arteFranchesco, 45, 0, Cor::RESET, Cor::AMARELO);
        std::cout << "\n" << std::string(recuo, ' ') << "Escolha: ";
        std::cin >> opcaoFranchesco;

        if (opcaoFranchesco == "1") {
            processarCompraPocoes(jogadorAtual, margemMsg);
        }
        else if (opcaoFranchesco == "2") {
            processarCompraTalismas(jogadorAtual, margemMsg, larguraDoTerminal);
        }
        else if (opcaoFranchesco == "3") {
            processarCompraIguarias(jogadorAtual, margemMsg, larguraDoTerminal);
        }
        else if (opcaoFranchesco == "4") {
            processarVendaDeItens(jogadorAtual, larguraDoTerminal);
        }
    } while (opcaoFranchesco != "0");
}

namespace {
    void processarCompraPocoes(SistemaPersonagem* jogadorAtual, const std::string& margemMsg) {
        std::string opcaoCompra;
        do {
            SimplificacoesAparencia::limparTela();
            SimplificacoesAparencia::exibirCabecalho("LOJA - POCOES", Cor::AMARELO);
            std::cout << "\n" << margemMsg << "Seu Ouro: " << jogadorAtual->obterInventario()->obterOuro() << "G\n\n";

            std::cout << margemMsg << "[1] Pocao de Cura (30%VM)                          - 10G\n";
            std::cout << margemMsg << "[0] VOLTAR\n\n";
            std::cout << "\n" << std::string(SimplificacoesAparencia::obterLarguraTerminal(), '=') << "\n";
            std::cout << "\n" << margemMsg << "Escolha: ";

            std::cin >> opcaoCompra;

            if (opcaoCompra == "1") {
                int preco = 10;
                if (jogadorAtual->obterInventario()->obterOuro() >= preco) {
                    jogadorAtual->obterInventario()->adicionarOuro(-preco);
                    jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem(ItemID::PocaoCura30));
                    std::cout << "\n" << margemMsg << "[SISTEMA]: Pocao de Cura comprada!\n";
                } else {
                    std::cout << "\n" << margemMsg << "[SISTEMA]: Ouro insuficiente!\n";
                }
                SimplificacoesAparencia::aguardarEnter();
            }
        } while (opcaoCompra != "0");
    }

    void processarCompraTalismas(SistemaPersonagem* jogadorAtual, const std::string& margemMsg, int larguraDoTerminal) {
        std::string opcaoCompra;
        do {
            SimplificacoesAparencia::limparTela();
            SimplificacoesAparencia::exibirCabecalho("LOJA - TALISMAS", Cor::AMARELO);
            std::cout << "\n" << margemMsg << "Seu Ouro: " << jogadorAtual->obterInventario()->obterOuro() << "G\n\n";

            std::cout << margemMsg << "[1] Talisma do Urso (+5 Forca | -5 Int)             - 200G\n";
            std::cout << margemMsg << "[2] Talisma do Corvo (+5 Int | -5 Forca)            - 200G\n";
            std::cout << margemMsg << "[3] Talisma do Leopardo (+5 Dest | -5 Sab)          - 200G\n";
            std::cout << margemMsg << "[4] Talisma da Coruja (+5 Sab | -5 Dest)            - 200G\n";
            std::cout << margemMsg << "[0] VOLTAR\n\n";
            std::cout << "\n" << std::string(larguraDoTerminal, '=') << "\n";
            std::cout << "\n" << margemMsg << "Escolha: ";

            std::cin >> opcaoCompra;

            if (opcaoCompra >= "1" && opcaoCompra <= "4") {
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
                        std::cout << "\n" << margemMsg << "[SISTEMA]: " << novoItem->obterNomeItem() << " comprado!\n";
                        jogadorAtual->obterInventario()->adicionarItem(std::move(novoItem));
                    }
                } else {
                    std::cout << "\n" << margemMsg << "[SISTEMA]: Ouro insuficiente!\n";
                }
                SimplificacoesAparencia::aguardarEnter();
            }
        } while (opcaoCompra != "0");
    }

    void processarCompraIguarias(SistemaPersonagem* jogadorAtual, const std::string& margemMsg, int larguraDoTerminal) {
        std::string opcaoCompra;
        do {
            SimplificacoesAparencia::limparTela();
            SimplificacoesAparencia::exibirCabecalho("LOJA - IGUARIAS", Cor::AMARELO);
            std::cout << "\n" << margemMsg << "Seu Ouro: " << jogadorAtual->obterInventario()->obterOuro() << "G\n\n";

            std::cout << margemMsg << "[1] Dispositivo de teclas de linguagem desconhecida - 1000G\n";
            std::cout << margemMsg << "[0] VOLTAR\n\n";
            std::cout << "\n" << std::string(larguraDoTerminal, '=') << "\n";
            std::cout << "\n" << margemMsg << "Escolha: ";

            std::cin >> opcaoCompra;

            if (opcaoCompra == "1") {
                int preco = 1000;
                if (jogadorAtual->obterInventario()->obterOuro() >= preco) {
                    jogadorAtual->obterInventario()->adicionarOuro(-preco);
                    jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem(ItemID::DispositivoLinguagem));
                    std::cout << "\n" << margemMsg << "[SISTEMA]: Dispositivo misterioso comprado!\n";
                } else {
                    std::cout << "\n" << margemMsg << "[SISTEMA]: Ouro insuficiente!\n";
                }
                SimplificacoesAparencia::aguardarEnter();
            }
        } while (opcaoCompra != "0");
    }

    void processarVendaDeItens(SistemaPersonagem* jogadorAtual, int larguraDoTerminal) {
        std::string codigoVenda;
        do {
            TelaInventario::exibir(jogadorAtual, true);
            TelaInventario::exibirPrompt("Digite o codigo do item para vender ou [0] VOLTAR: ");
            std::cin >> codigoVenda;

            if (codigoVenda != "0") {
                Item* itemParaVenda = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigoVenda, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
                if (itemParaVenda) {
                    if (itemParaVenda == jogadorAtual->obterArma() || itemParaVenda == jogadorAtual->obterEscudo() || itemParaVenda == jogadorAtual->obterArmadura()) {
                        std::cout << "\n[SISTEMA]: Nao e possivel vender itens que estao equipados!\n"; SimplificacoesAparencia::aguardarEnter(); continue;
                    }
                    std::string nomeItemVenda = itemParaVenda->obterNomeItem();
                    int precoVenda = itemParaVenda->obterPrecoVenda();
                    jogadorAtual->obterInventario()->adicionarOuro(precoVenda);
                    jogadorAtual->obterInventario()->removerItem(itemParaVenda);
                    std::cout << "\n[SISTEMA]: Voce vendeu " << nomeItemVenda << " por " << precoVenda << "G!\n"; SimplificacoesAparencia::aguardarEnter();
                } else { std::cout << "\n[SISTEMA]: Item invalido!\n"; SimplificacoesAparencia::aguardarEnter(); }
            }
        } while (codigoVenda != "0");
    }
}

std::vector<std::string> NPCFranchesco::obterMapaLoja()
{
    return {
        " ##################################",
        "##.........................../--/|##",
        "##..[^S]......................F.$|##",
        "##.........................../--/|##",
        " ##################################"
    };
}
