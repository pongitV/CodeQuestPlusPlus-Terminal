#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

#include "NPCFranchesco.h"
#include "../Sistema/Menu.h"
#include "../Inventario/Item.h"
#include "../Inventario/Escudo.h"
#include "../Inventario/ItemConsumivel.h"
#include "../Inventario/ItemMissao.h"
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
            "[1] COMPRAR Pocoes",
            "[2] COMPRAR Talismas",
            "[3] COMPRAR Iguarias",
            "[4] VENDER Itens do Inventario",
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
                Menu::exibirLogoDoJogo("LOJA - POCOES");
                std::cout << "\n" << margemMsg << "Seu Ouro: " << jogadorAtual->obterInventario()->obterOuro() << "G\n\n";

                std::cout << margemMsg << "[1] Pocao de Cura (30%VM)                          - 10G\n";
                std::cout << margemMsg << "[0] VOLTAR\n\n";
                std::cout << "\n" << std::string(larguraDoTerminal, '=') << "\n";
                std::cout << "\n" << margemMsg << "Escolha: ";

                std::cin >> opcaoCompra;

                if (opcaoCompra == "1") 
                {
                    int preco = 10;
                    if (jogadorAtual->obterInventario()->obterOuro() >= preco) 
                    {
                        jogadorAtual->obterInventario()->adicionarOuro(-preco);
                            auto pocao = std::make_unique<ItemConsumivel>("Pocao de Cura (30%)");
                            pocao->adicionarPropriedade(Propriedade::ConsumivelCura);
                            jogadorAtual->obterInventario()->adicionarItem(std::move(pocao)); 
                        std::cout << "\n" << margemMsg << "[SISTEMA]: Pocao de Cura comprada!\n"; 
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
            std::string opcaoCompra;
            do {
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo("LOJA - TALISMAS");
                std::cout << "\n" << margemMsg << "Seu Ouro: " << jogadorAtual->obterInventario()->obterOuro() << "G\n\n";

                std::cout << margemMsg << "[1] Talisma do Urso (+5 Forca | -5 Int)             - 200G\n";
                std::cout << margemMsg << "[2] Talisma do Corvo (+5 Int | -5 Forca)            - 200G\n";
                std::cout << margemMsg << "[3] Talisma do Leopardo (+5 Dest | -5 Sab)          - 200G\n";
                std::cout << margemMsg << "[4] Talisma da Coruja (+5 Sab | -5 Dest)            - 200G\n";
                std::cout << margemMsg << "[0] VOLTAR\n\n";
                std::cout << "\n" << std::string(larguraDoTerminal, '=') << "\n";
                std::cout << "\n" << margemMsg << "Escolha: ";

                std::cin >> opcaoCompra;

                if (opcaoCompra >= "1" && opcaoCompra <= "4") 
                {
                    int preco = 200;
                    if (jogadorAtual->obterInventario()->obterOuro() >= preco) 
                    {
                        jogadorAtual->obterInventario()->adicionarOuro(-preco);
                            std::unique_ptr<ItemConsumivel> novoTalisma = nullptr;
                            if (opcaoCompra == "1") { 
                                novoTalisma = std::make_unique<ItemConsumivel>("Talisma do Urso"); 
                                novoTalisma->adicionarPropriedade(Propriedade::TalismaForca); 
                                std::cout << "\n" << margemMsg << "[SISTEMA]: Talisma do Urso comprado!\n"; 
                            } else if (opcaoCompra == "2") { 
                                novoTalisma = std::make_unique<ItemConsumivel>("Talisma do Corvo"); 
                                novoTalisma->adicionarPropriedade(Propriedade::TalismaInteligencia); 
                                std::cout << "\n" << margemMsg << "[SISTEMA]: Talisma do Corvo comprado!\n"; 
                            } else if (opcaoCompra == "3") { 
                                novoTalisma = std::make_unique<ItemConsumivel>("Talisma do Leopardo"); 
                                novoTalisma->adicionarPropriedade(Propriedade::TalismaDestreza); 
                                std::cout << "\n" << margemMsg << "[SISTEMA]: Talisma do Leopardo comprado!\n"; 
                            } else if (opcaoCompra == "4") { 
                                novoTalisma = std::make_unique<ItemConsumivel>("Talisma da Coruja"); 
                                novoTalisma->adicionarPropriedade(Propriedade::TalismaSabedoria); 
                                std::cout << "\n" << margemMsg << "[SISTEMA]: Talisma da Coruja comprado!\n"; 
                            }
                            if (novoTalisma) jogadorAtual->obterInventario()->adicionarItem(std::move(novoTalisma));
                    } 
                    else 
                    {
                        std::cout << "\n" << margemMsg << "[SISTEMA]: Ouro insuficiente!\n";
                    }
                    Menu::aguardarPressionamentoDeEnter();
                }
            } while (opcaoCompra != "0");
        }
        else if (opcaoFranchesco == "3") {
            std::string opcaoCompra;
            do {
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo("LOJA - IGUARIAS");
                std::cout << "\n" << margemMsg << "Seu Ouro: " << jogadorAtual->obterInventario()->obterOuro() << "G\n\n";

                std::cout << margemMsg << "[1] Dispositivo de teclas de linguagem desconhecida - 1000G\n";
                std::cout << margemMsg << "[0] VOLTAR\n\n";
                std::cout << "\n" << std::string(larguraDoTerminal, '=') << "\n";
                std::cout << "\n" << margemMsg << "Escolha: ";

                std::cin >> opcaoCompra;

                if (opcaoCompra == "1") 
                {
                    int preco = 1000;
                    if (jogadorAtual->obterInventario()->obterOuro() >= preco) 
                    {
                        jogadorAtual->obterInventario()->adicionarOuro(-preco);
                        jogadorAtual->obterInventario()->adicionarItem(std::make_unique<ItemMissao>("Dispositivo de teclas de linguagem desconhecida")); 
                        std::cout << "\n" << margemMsg << "[SISTEMA]: Dispositivo misterioso comprado!\n";
                    } 
                    else 
                    {
                        std::cout << "\n" << margemMsg << "[SISTEMA]: Ouro insuficiente!\n";
                    }
                    Menu::aguardarPressionamentoDeEnter();
                }
            } while (opcaoCompra != "0");
        }
        else if (opcaoFranchesco == "4") {
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
                        int precoVenda = itemParaVenda->obterPrecoVenda();
                        jogadorAtual->obterInventario()->adicionarOuro(precoVenda);
                        jogadorAtual->obterInventario()->removerItem(nomeItemVenda);
                        std::cout << "\n[SISTEMA]: Voce vendeu " << nomeItemVenda << " por " << precoVenda << "G!\n"; Menu::aguardarPressionamentoDeEnter();
                    } else { std::cout << "\n[SISTEMA]: Item invalido!\n"; Menu::aguardarPressionamentoDeEnter(); }
                }
            } while (codigoVenda != "0");
        }
    } while (opcaoFranchesco != "0");
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
