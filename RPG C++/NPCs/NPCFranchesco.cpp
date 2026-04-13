#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

#include "NPCFranchesco.h"
#include "../Sistema/Menu.h"
#include "../Inventario/Item.h"
#include "../Interfaces/TelaInventario.h"

void NPCFranchesco::interagir(Personagem* jogadorAtual)
{
    int larguraDoTerminal = Menu::obterLarguraDoTerminalEmColunas();
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
        Menu::limparTelaDoTerminal();
        Menu::exibirLogoDoJogo("LOJA AMBULANTE");
        
        int espacosMsg = (larguraDoTerminal - 55) / 2;
        std::string margemMsg(espacosMsg > 0 ? espacosMsg : 0, ' ');

        std::vector<std::string> menuEsquerda = {
            "[Franchesco]: Bem-vindo! De uma olhada nas",
            "minhas mercadorias.",
            "",
            "Seu Ouro: " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G",
            "",
            "[1] COMPRAR Itens e Equipamentos",
            "[2] VENDER Itens do Inventario",
            "[0] VOLTAR",
            ""
        };

        int maxLinhas = std::max(menuEsquerda.size(), arteFranchesco.size());
        int larguraInfo = 45;
        int recuo = (larguraDoTerminal - (larguraInfo + 45)) / 2;
        if (recuo < 0) recuo = 0;

        std::cout << "\n";
        for (size_t i = 0; i < maxLinhas; ++i) {
            std::cout << std::string(recuo, ' ');
            if (i < menuEsquerda.size()) {
                std::cout << std::left << std::setw(larguraInfo) << menuEsquerda[i];
            } else {
                std::cout << std::string(larguraInfo, ' ');
            }
            if (i < arteFranchesco.size()) {
                std::cout << "\033[0m" << arteFranchesco[i] << "\033[0m"; // Amarelo para destacar o comerciante
            }
            std::cout << "\n";
        }
        
        std::cout << "\n" << std::string(recuo, ' ') << "Escolha: ";
        std::cin >> opcaoFranchesco;

        if (opcaoFranchesco == "1") {
            std::string opcaoCompra;
            do {
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo("LOJA - COMPRAR");
                std::cout << "\n" << margemMsg << "Seu Ouro: " << jogadorAtual->obterInventario()->obterOuro() << "G\n\n";

                std::cout << margemMsg << "[1] Pocao de Cura (30% HP)         - 10G\n";
                std::cout << margemMsg << "[2] Manto Encantado (Mago)         - 15G\n";
                std::cout << margemMsg << "[3] Escudo Medio (Guerreiro)       - 15G\n";
                std::cout << margemMsg << "[4] Capa Magica (Bardo)            - 15G\n";
                std::cout << margemMsg << "[5] Escudo Leve (Arqueiro)         - 15G\n";
                std::cout << margemMsg << "[0] VOLTAR\n\n";
                std::cout << "\n" << std::string(larguraDoTerminal, '=') << "\n";
                std::cout << "\n" << margemMsg << "Escolha: ";

                std::cin >> opcaoCompra;

                if (opcaoCompra >= "1" && opcaoCompra <= "5") 
                {
                    int preco = (opcaoCompra == "1") ? 10 : 15;
                    if (jogadorAtual->obterInventario()->obterOuro() >= preco) 
                    {
                        jogadorAtual->obterInventario()->adicionarOuro(-preco);

                        if (opcaoCompra == "1") { jogadorAtual->obterInventario()->adicionarItem(new PocaoCura()); std::cout << "\n" << margemMsg << "[SISTEMA]: Pocao de Cura comprada!\n"; }
                        else if (opcaoCompra == "2") { jogadorAtual->obterInventario()->adicionarItem(new Escudo("Manto encantado", 5, 3)); std::cout << "\n" << margemMsg << "[SISTEMA]: Manto Encantado comprado!\n"; }
                        else if (opcaoCompra == "3") { jogadorAtual->obterInventario()->adicionarItem(new Escudo("Escudo medio de metal", 12, 6)); std::cout << "\n" << margemMsg << "[SISTEMA]: Escudo Medio comprado!\n"; }
                        else if (opcaoCompra == "4") { jogadorAtual->obterInventario()->adicionarItem(new Escudo("Capa magica", 5, 2)); std::cout << "\n" << margemMsg << "[SISTEMA]: Capa Magica comprada!\n"; }
                        else if (opcaoCompra == "5") { jogadorAtual->obterInventario()->adicionarItem(new Escudo("Escudo leve de madeira", 8, 4)); std::cout << "\n" << margemMsg << "[SISTEMA]: Escudo Leve comprado!\n"; }
                    } 
                    else 
                    {
                        std::cout << "\n" << margemMsg << "[SISTEMA]: Ouro insuficiente!\n";
                    }
                    Menu::aguardarPressionamentoDeEnter();
                }
            } while (opcaoCompra != "0");
        } 
        else if (opcaoFranchesco == "2") {
            std::string codigoVenda;
            do {
                TelaInventario::exibir(jogadorAtual, true);
                std::string promptVenda = "Digite o codigo do item para vender ou [0] VOLTAR: ";
                int espacosVenda = (larguraDoTerminal - (int)promptVenda.length()) / 2;
                std::cout << "\n" << std::string(espacosVenda > 0 ? espacosVenda : 0, ' ') << promptVenda;
                std::cin >> codigoVenda;

                if (codigoVenda != "0") {
                    Item* itemParaVenda = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigoVenda, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
                    if (itemParaVenda) {
                        if (itemParaVenda == jogadorAtual->obterArma() || itemParaVenda == jogadorAtual->obterEscudo() || itemParaVenda == jogadorAtual->obterArmadura()) {
                            std::cout << "\n[SISTEMA]: Nao e possivel vender itens que estao equipados!\n"; Menu::aguardarPressionamentoDeEnter(); continue;
                        }
                        std::string nomeItemVenda = itemParaVenda->obterNomeItem();
                        int precoVenda = 3; 
                        if (nomeItemVenda == "Adaga artesanal de pedra") precoVenda = 5;
                        else if (nomeItemVenda == "Gosma acida") precoVenda = 5;
                        else if (nomeItemVenda == "Nucleo pegajoso") precoVenda = 30;
                        else if (dynamic_cast<PocaoCura*>(itemParaVenda)) precoVenda = 6; 
                        else if (nomeItemVenda == "Manto encantado" || nomeItemVenda == "Escudo medio de metal" || nomeItemVenda == "Capa magica" || nomeItemVenda == "Escudo leve de madeira") precoVenda = 9; 
                        jogadorAtual->obterInventario()->adicionarOuro(precoVenda);
                        jogadorAtual->obterInventario()->removerItem(nomeItemVenda);
                        std::cout << "\n[SISTEMA]: Voce vendeu " << nomeItemVenda << " por " << precoVenda << "G!\n"; Menu::aguardarPressionamentoDeEnter();
                    } else { std::cout << "\n[SISTEMA]: Item invalido!\n"; Menu::aguardarPressionamentoDeEnter(); }
                }
            } while (codigoVenda != "0");
        }
    } while (opcaoFranchesco != "0");
}
