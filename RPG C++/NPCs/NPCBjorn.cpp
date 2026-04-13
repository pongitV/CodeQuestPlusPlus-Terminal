#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <algorithm>

#include "NPCBjorn.h"
#include "../Sistema/Menu.h"
#include "../Inventario/Item.h"
#include "../Interfaces/TelaInventario.h"

void NPCBjorn::interagir(Personagem* jogadorAtual)
{
    static std::map<int, std::pair<std::string, bool>> estoqueForja = {
        {1, {"Espada longa de ferro", true}},
        {2, {"Arco recurvo de madeira", true}},
        {3, {"Cajado", true}},
        {4, {"Violao encantado", true}},
        {5, {"Armadura de malha e metal", true}},
        {6, {"Armadura leve de couro com malha", true}},
        {7, {"Tunica", true}},
        {8, {"Traje de Couro e tecido nobre", true}}
    };
    
    auto instanciarEquipamentoBjorn = [](const std::string& n) -> Item* {
        if (n == "Espada longa de ferro") return new Arma(n, 10, 0);
        if (n == "Arco recurvo de madeira") return new Arma(n, 10, 0);
        if (n == "Cajado") return new Arma(n, 0, 30);
        if (n == "Violao encantado") return new Arma(n, 0, 7);
        if (n == "Armadura de malha e metal") return new Armadura(n, 7);
        if (n == "Armadura leve de couro com malha") return new Armadura(n, 5);
        if (n == "Tunica") return new Armadura(n, 2);
        if (n == "Traje de Couro e tecido nobre") return new Armadura(n, 4);
        return nullptr;
    };

    int larguraDoTerminal = Menu::obterLarguraDoTerminalEmColunas();
    std::string opcaoBjorn;
    
    std::vector<std::string> arteBjorn = {
        "                                 =%@@@%                     ",
        "                                 *::==%%                    ",
        "                                #%%*%#+@@    @%@            ",
        "                                 *@@*#@@@     +%#           ",
        "                                @@%%@@@@@ -*%@ @#*          ",
        "                                 @@@@@@*+%@     +           ",
        "                               +#**+**%@#+*+*               ",
        "                              =+-%%**=**#**###              ",
        "                            =++#@%@%**##%###%##             ",
        "                            *##**%%@@@%%##**#*%             ",
        "                            %*###@%@@@@#*=+%%#*@            ",
        "                            %#%#%@##@@@%#%**%#*@            ",
        "                            :=*+*****%@@@#%%#@#             ",
        "                           +=-*+**+***#%@@@%%@              ",
        "                           -+**++**+%##*#@@                 ",
        "                           @%#%##%%#@%@@#@+                 ",
        "                           +@%%%%%%%%@%%@%                  ",
        "                         ##%*+#=*###*++#*%#                 ",
        "                        =#%@+#####**#%%###%                 ",
        "                       ##%%#%*##***##*#%%@@                 ",
        "                       @%%%%%%###**#****%@@#                ",
        "                       %@@%+%%%##*****#@@@@%                ",
        "                        %*# %%%%#*##%##*#%@@                ",
        "                            %%#%########%%@@                ",
        "                            ##%%%%#######%@@                ",
        "                            *%%%%###**+*#@@@                ",
        "                            *#%%%##*##%@%@@@                ",
        "     #-*+-==++*#*****+*##+  *#%%%###**###%@@                ",
        "      @@%@@@@%@@@@%%%%%%#***@#%%#%%*#+#*+%@@                ",
        "       @%@@%@%@@%%%@@@@@@@@ *#%%###*****#%@@%%              ",
        "       +@@@@%%@@@%%@@@@@@#  %#%%##*#*#**+%@@%%%             ",
        "        :@@@@@@@@@@@@@@@-   *##%##%***#*+*%%%@              ",
        "          @@@@@@@@@@@@@       @%#*%+++*#**@@*%              ",
        "          @@@@@@@@@@@@#       %@@@@@    @@@%%@              ",
        "          @@@@@@@%@@@@        %@@@@     @@@#%%              ",
        "           @@@%@%%@@@@        @@@@@@     @%@%#              ",
        "          @%@@%@#%@@@@       #@%@@@@    =###%%              ",
        "          @@%%%%*%@@@%@      %@@@@@#    #####@=             ",
        "         #%%%%%%:#%%%#%    %@@@@@@@     =@%@@@+             ",
        "       -=++*#*%@%+#%#####@*%%@@@@@*     %@%%%@              ",
        "      @#*+*%@@@**#%####%@@@@=          *#%#@%@              ",
        "               @@%@@#                 %#%%%@%               "
    };

    do {
        Menu::limparTelaDoTerminal();
        Menu::exibirLogoDoJogo("FORJA DO BJORN");
        
        int espacosMsg = (larguraDoTerminal - 55) / 2;
        std::string margemMsg(espacosMsg > 0 ? espacosMsg : 0, ' ');

        std::vector<std::string> menuEsquerda = {
            "[Bjorn]: Bem-vindo a minha forja, salvador!",
            "O que vai ser hoje?",
            "",
            "Seu Ouro: " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G",
            "",
            "[1] COMPRAR Equipamentos das Classes",
            "[2] MELHORAR Equipamento na Bigorna",
            "[3] ENCANTAR Arma: Sangramento (40x Dente de Goblin)",
            "[4] ENCANTAR Arma: Lentidao (5x Nucleo pegajoso)",
            "[0] VOLTAR",
            ""
        };

        int maxLinhas = std::max(menuEsquerda.size(), arteBjorn.size());
        int larguraInfo = 45;
        int recuo = (larguraDoTerminal - (larguraInfo + 60)) / 2;
        if (recuo < 0) recuo = 0;

        std::cout << "\n";
        for (size_t i = 0; i < maxLinhas; ++i) {
            std::cout << std::string(recuo, ' ');
            if (i < menuEsquerda.size()) {
                std::cout << std::left << std::setw(larguraInfo) << menuEsquerda[i];
            } else {
                std::cout << std::string(larguraInfo, ' ');
            }
            if (i < arteBjorn.size()) {
                std::cout << "\033[0m" << arteBjorn[i] << "\033[0m";
            }
            std::cout << "\n";
        }
        
        std::cout << "\n" << std::string(recuo, ' ') << "Escolha: ";
        std::cin >> opcaoBjorn;

        if (opcaoBjorn == "1") {
            std::string opcaoCompra;
            do {
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo("FORJA - EQUIPAMENTOS");
                std::cout << "\n" << margemMsg << "Seu Ouro: " << jogadorAtual->obterInventario()->obterOuro() << "G\n\n";
                for (auto const& [id, par] : estoqueForja) {
                    std::string preco = "40G";
                    std::string status = par.second ? "Em Estoque" : "ESGOTADO";
                    std::cout << margemMsg << "[" << id << "] " << par.first << " (" << preco << ") - " << status << "\n";
                }
                std::cout << "\n" << margemMsg << "[0] VOLTAR\n\n" << margemMsg << "Escolha: ";
                std::cin >> opcaoCompra;
                
                if (opcaoCompra != "0") {
                    try {
                        int idCompra = std::stoi(opcaoCompra);
                        if (estoqueForja.count(idCompra)) {
                            if (!estoqueForja[idCompra].second) {
                                std::cout << "\n" << margemMsg << "[Bjorn]: Ja vendi este item, nao tenho mais em estoque!\n";
                            } else if (jogadorAtual->obterInventario()->obterOuro() >= 40) {
                                jogadorAtual->obterInventario()->adicionarOuro(-40);
                                estoqueForja[idCompra].second = false;
                                jogadorAtual->obterInventario()->adicionarItem(instanciarEquipamentoBjorn(estoqueForja[idCompra].first));
                                std::cout << "\n" << margemMsg << "[Bjorn]: Otima escolha! Voce comprou " << estoqueForja[idCompra].first << ".\n";
                            } else {
                                std::cout << "\n" << margemMsg << "[Bjorn]: Voce nao tem ouro suficiente para isso!\n";
                            }
                            Menu::aguardarPressionamentoDeEnter();
                        }
                    } catch (...) {}
                }
            } while (opcaoCompra != "0");
        } else if (opcaoBjorn == "2") {
            std::string codigo1, codigo2;
            do {
                TelaInventario::exibir(jogadorAtual);
                std::cout << "\n[Bjorn]: Escolha o PRIMEIRO item para melhorar ou [0] VOLTAR: ";
                std::cin >> codigo1;
                if (codigo1 == "0") break;
                
                Item* item1 = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigo1, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
                if (!item1) { std::cout << "\n[SISTEMA]: Item invalido!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                if (item1 == jogadorAtual->obterArma() || item1 == jogadorAtual->obterEscudo() || item1 == jogadorAtual->obterArmadura()) { std::cout << "\n[Bjorn]: Voce precisa DESEQUIPAR o item antes de usa-lo na bigorna!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                if (item1->obterNomeItem().back() == '+') { std::cout << "\n[Bjorn]: Este item ja atingiu o limite de melhoria basica!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                if (!dynamic_cast<Arma*>(item1) && !dynamic_cast<Escudo*>(item1) && !dynamic_cast<Armadura*>(item1)) { std::cout << "\n[Bjorn]: Eu so posso melhorar Armas, Escudos e Armaduras!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }

                std::cout << "[Bjorn]: Agora, escolha o SEGUNDO item identico (copia) ou [0] CANCELAR: ";
                std::cin >> codigo2;
                if (codigo2 == "0") continue;

                Item* item2 = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigo2, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
                if (!item2) { std::cout << "\n[SISTEMA]: Item invalido!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                if (item1 == item2) { std::cout << "\n[Bjorn]: Voce nao pode selecionar o MESMO item duas vezes! Voce precisa de DUAS COPIAS!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                if (item2 == jogadorAtual->obterArma() || item2 == jogadorAtual->obterEscudo() || item2 == jogadorAtual->obterArmadura()) { std::cout << "\n[Bjorn]: Voce precisa DESEQUIPAR a copia antes de usa-la na bigorna!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                if (item1->obterNomeItem() != item2->obterNomeItem()) { std::cout << "\n[Bjorn]: Os itens precisam ser EXATAMENTE iguais para serem fundidos!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }

                std::string novoNome = item1->obterNomeItem() + "+";
                Item* novoItem = nullptr;

                if (Arma* a1 = dynamic_cast<Arma*>(item1)) novoItem = new Arma(novoNome, static_cast<int>(a1->obterDanoFisico() * 1.5), static_cast<int>(a1->obterDanoMagico() * 1.5));
                else if (Escudo* e1 = dynamic_cast<Escudo*>(item1)) novoItem = new Escudo(novoNome, static_cast<int>(e1->obterDurabilidadeAtualEscudo() * 1.5), static_cast<int>(e1->obterReducaoDanoFixaEscudo() * 1.5));
                else if (Armadura* ar1 = dynamic_cast<Armadura*>(item1)) novoItem = new Armadura(novoNome, static_cast<int>(ar1->obterReducaoFixa() * 1.5));

                if (novoItem) {
                    std::string nomeAntigo = item1->obterNomeItem();
                    jogadorAtual->obterInventario()->removerItem(nomeAntigo); 
                    jogadorAtual->obterInventario()->removerItem(nomeAntigo); 
                    jogadorAtual->obterInventario()->adicionarItem(novoItem);
                    
                    Menu::limparTelaDoTerminal();
                    Menu::exibirLogoDoJogo("FORJA - SUCESSO");
                    std::vector<std::string> arteBigorna = 
                    {
                        "⠀⠀⠀⠀⠀⠀⠀⢰⣶⣶⣶⣶⣶⣶⣶⣶⣶⣶⣶⣶⣶⣶⣶⣶⡄⠀⠀⠀⠀⠀",
                        "⠀⠹⣿⣿⣿⣿⡇⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⢠⣄⡀⠀⠀",
                        "⠀⠀⠙⢿⣿⣿⡇⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⢸⣿⣿⡶⠀",
                        "⠀⠀⠀⠀⠉⠛⠇⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠸⠟⠋⠀⠀",
                        "⠀⠀⠀⠀⠀⠀⠀⠸⠿⠿⠿⠿⠿⠿⠿⠿⠿⠿⠿⠿⠿⠿⠿⠿⠇⠀⠀⠀⠀⠀",
                        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣶⣶⣶⣶⣶⣶⣶⣶⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
                        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀",
                        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⠀⠀⠀⠀⠀⠀⠀",
                        "⠀⠀⠀⠀⠀⠀⣀⣀⣈⣉⣉⣉⣉⣉⣉⣉⣉⣉⣉⣉⣉⣉⣉⣁⣀⣀⠀⠀⠀⠀",
                        "⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠀⠀⠀⠀"
                    };
                    std::string equacao = "[" + nomeAntigo + "] + [" + nomeAntigo + "] = [" + novoNome + "]";
                    Menu::imprimirLinhasCentralizadasNaTela({equacao, ""}, 0, "\033[33m");
                    Menu::imprimirLinhasCentralizadasNaTela(arteBigorna, 29, "\033[37m");
                    
                    std::cout << "\n[Bjorn]: Ha! Trabalho feito! Seu equipamento esta mais forte do que nunca!\n";
                    Menu::aguardarPressionamentoDeEnter();
                }
            } while (codigo1 != "0");
        }
        else if (opcaoBjorn == "3" || opcaoBjorn == "4") {
            bool isSangramento = (opcaoBjorn == "3");
            std::string itemNecessario = isSangramento ? "Dente de goblin" : "Nucleo pegajoso";
            int qtdNecessaria = isSangramento ? 40 : 5;
            
            int qtdAtual = jogadorAtual->obterInventario()->contarItem(itemNecessario);
            if (qtdAtual < qtdNecessaria) {
                std::cout << "\n[Bjorn]: Voce nao tem " << itemNecessario << " suficiente! (Possui: " << qtdAtual << "/" << qtdNecessaria << ")\n";
                Menu::aguardarPressionamentoDeEnter();
                continue;
            }
            
            std::string codigoArma;
            TelaInventario::exibir(jogadorAtual);
            std::cout << "\n[Bjorn]: Escolha a ARMA para encantar ou [0] VOLTAR: ";
            std::cin >> codigoArma;
            if (codigoArma == "0") continue;
            
            Item* itemEscolhido = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigoArma, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
            
            if (!itemEscolhido) { std::cout << "\n[SISTEMA]: Item invalido!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
            
            Arma* armaEscolhida = dynamic_cast<Arma*>(itemEscolhido);
            if (!armaEscolhida) { std::cout << "\n[Bjorn]: Eu so posso encantar ARMAS com isso!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
            
            if (isSangramento && armaEscolhida->possuiEfeitoSangramento()) { std::cout << "\n[Bjorn]: Essa arma ja causa sangramento!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
            if (!isSangramento && armaEscolhida->possuiEfeitoLentidao()) { std::cout << "\n[Bjorn]: Essa arma ja possui lentidao!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
            
            // Remove os itens necessários do inventário
            for (int i = 0; i < qtdNecessaria; ++i) {
                jogadorAtual->obterInventario()->removerItem(itemNecessario);
            }
            
            // Aplica o encantamento
            if (isSangramento) {
                armaEscolhida->aplicarEfeitoSangramento();
                armaEscolhida->alterarNome(armaEscolhida->obterNomeItem() + " (Sangrenta)");
            } else {
                armaEscolhida->aplicarEfeitoLentidao();
                armaEscolhida->alterarNome(armaEscolhida->obterNomeItem() + " (Viscosa)");
            }
            
            Menu::limparTelaDoTerminal();
            Menu::exibirLogoDoJogo("FORJA - ENCANTAMENTO SUCESSO");
            std::vector<std::string> arteBigorna = { "⠀⠀⠀⠀⠀⠀⠀⢰⣶⣶⣶⣶⣶⣶⣶⣶⣶⣶⣶⣶⣶⣶⣶⣶⡄⠀⠀⠀⠀⠀", "⠀⠹⣿⣿⣿⣿⡇⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⢠⣄⡀⠀⠀", "⠀⠀⠙⢿⣿⣿⡇⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⢸⣿⣿⡶⠀", "⠀⠀⠀⠀⠉⠛⠇⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠸⠟⠋⠀⠀", "⠀⠀⠀⠀⠀⠀⠀⠸⠿⠿⠿⠿⠿⠿⠿⠿⠿⠿⠿⠿⠿⠿⠿⠿⠇⠀⠀⠀⠀⠀", "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣶⣶⣶⣶⣶⣶⣶⣶⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀", "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀", "⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⠀⠀⠀⠀⠀⠀⠀", "⠀⠀⠀⠀⠀⠀⣀⣀⣈⣉⣉⣉⣉⣉⣉⣉⣉⣉⣉⣉⣉⣉⣉⣁⣀⣀⠀⠀⠀⠀", "⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠀⠀⠀⠀" };
            
            Menu::imprimirLinhasCentralizadasNaTela(arteBigorna, 29, "\033[37m");
            
            std::cout << "\n[Bjorn]: Ha! Trabalho feito! Este encantamento a tornara terrivel em combate!\n";
            Menu::aguardarPressionamentoDeEnter();
        }
    } while (opcaoBjorn != "0");
}
