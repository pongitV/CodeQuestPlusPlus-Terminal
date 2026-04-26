#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <algorithm>
#include <memory>

#include "NPCBjorn.h"
#include "../Sistema/Menu.h"
#include "../Inventario/Item.h"
#include "../Inventario/Arma.h"
#include "../Inventario/Armadura.h"
#include "../Inventario/Escudo.h"
#include "../Interfaces/TelaInventario.h"

void NPCBjorn::interagir(Personagem* jogadorAtual)
{
    static std::map<int, std::pair<std::string, bool>> estoqueArmas = {
        {1, {"Espada longa de ferro", true}},
        {2, {"Arco recurvo de madeira", true}},
        {3, {"Cajado", true}},
        {4, {"Violao encantado", true}}
    };
    
    static std::map<int, std::pair<std::string, bool>> estoqueArmaduras = {
        {1, {"Armadura de malha e metal", true}},
        {2, {"Armadura leve de couro com malha", true}},
        {3, {"Tunica", true}},
        {4, {"Traje de Couro e tecido nobre", true}}
    };
    
    auto instanciarEquipamentoBjorn = [](const std::string& n) -> std::unique_ptr<Item> {
        if (n == "Espada longa de ferro") return std::make_unique<Arma>(n, 10, 0);
        if (n == "Arco recurvo de madeira") return std::make_unique<Arma>(n, 10, 0);
        if (n == "Cajado") return std::make_unique<Arma>(n, 0, 30);
        if (n == "Violao encantado") return std::make_unique<Arma>(n, 0, 10);
        if (n == "Armadura de malha e metal") return std::make_unique<Armadura>(n, 7);
        if (n == "Armadura leve de couro com malha") return std::make_unique<Armadura>(n, 5);
        if (n == "Tunica") return std::make_unique<Armadura>(n, 2);
        if (n == "Traje de Couro e tecido nobre") return std::make_unique<Armadura>(n, 4);
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
            "[1] COMPRAR Armas das Classes",
            "[2] COMPRAR Armaduras das Classes",
            "[3] MELHORAR Equipamento na Bigorna",
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

        if (opcaoBjorn == "1" || opcaoBjorn == "2") {
            bool comprandoArmas = (opcaoBjorn == "1");
            auto& estoqueAtual = comprandoArmas ? estoqueArmas : estoqueArmaduras;
            std::string tituloLoja = comprandoArmas ? "FORJA - ARMAS" : "FORJA - ARMADURAS";

            std::string opcaoCompra;
            do {
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo(tituloLoja);
                std::cout << "\n" << margemMsg << "Seu Ouro: " << jogadorAtual->obterInventario()->obterOuro() << "G\n\n";
                for (auto const& [id, par] : estoqueAtual) {
                    std::string preco = "40G";
                    std::string status = par.second ? "Em Estoque" : "ESGOTADO";
                    std::unique_ptr<Item> tempItem = instanciarEquipamentoBjorn(par.first);
                    std::string infoStatus = tempItem ? tempItem->obterInfoStatus() : "";
                    std::cout << margemMsg << "[" << id << "] " << par.first << infoStatus << " (" << preco << ") - " << status << "\n";
                }
                std::cout << "\n" << margemMsg << "[0] VOLTAR\n\n" << margemMsg << "Escolha: ";
                std::cin >> opcaoCompra;
                
                if (opcaoCompra != "0") {
                    try {
                        int idCompra = std::stoi(opcaoCompra);
                        if (estoqueAtual.count(idCompra)) {
                            if (!estoqueAtual[idCompra].second) {
                                std::cout << "\n" << margemMsg << "[Bjorn]: Ja vendi este item, nao tenho mais em estoque!\n";
                            } else if (jogadorAtual->obterInventario()->obterOuro() >= 40) {
                                jogadorAtual->obterInventario()->adicionarOuro(-40);
                                estoqueAtual[idCompra].second = false;
                                jogadorAtual->obterInventario()->adicionarItem(std::move(instanciarEquipamentoBjorn(estoqueAtual[idCompra].first)));
                                std::cout << "\n" << margemMsg << "[Bjorn]: Otima escolha! Voce comprou " << estoqueAtual[idCompra].first << ".\n";
                            } else {
                                std::cout << "\n" << margemMsg << "[Bjorn]: Voce nao tem ouro suficiente para isso!\n";
                            }
                            Menu::aguardarPressionamentoDeEnter();
                        }
                    } catch (...) {}
                }
            } while (opcaoCompra != "0");
        } else if (opcaoBjorn == "3") {
            std::string codigo1, codigo2;
            do {
                TelaInventario::exibir(jogadorAtual);
                std::cout << "\n[Bjorn]: Escolha o PRIMEIRO item para melhorar ou [0] VOLTAR: ";
                std::cin >> codigo1;
                if (codigo1 == "0") break;
                
                Item* item1 = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigo1, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
                if (!item1) { std::cout << "\n[SISTEMA]: Item invalido!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                if (item1 == jogadorAtual->obterArma() || item1 == jogadorAtual->obterEscudo() || item1 == jogadorAtual->obterArmadura()) { std::cout << "\n[Bjorn]: Voce precisa DESEQUIPAR o item antes de usa-lo na bigorna!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                if (item1->temPropriedade(Propriedade::Melhorado)) { std::cout << "\n[Bjorn]: Este item ja atingiu o limite de melhoria basica!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                if (!dynamic_cast<Arma*>(item1) && !dynamic_cast<Escudo*>(item1) && !dynamic_cast<Armadura*>(item1)) { std::cout << "\n[Bjorn]: Eu so posso melhorar Armas, Escudos e Armaduras!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }

                std::cout << "[Bjorn]: Agora, escolha o SEGUNDO item identico (copia) ou [0] CANCELAR: ";
                std::cin >> codigo2;
                if (codigo2 == "0") continue;

                Item* item2 = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigo2, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
                if (!item2) { std::cout << "\n[SISTEMA]: Item invalido!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                if (item1->obterNomeItem() != item2->obterNomeItem()) { std::cout << "\n[Bjorn]: Os itens precisam ser EXATAMENTE iguais para serem fundidos!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }

                if (jogadorAtual->obterInventario()->contarItem(item1->obterNomeItem()) < 2) {
                    std::cout << "\n[Bjorn]: Voce nao possui DUAS COPIAS deste item!\n"; Menu::aguardarPressionamentoDeEnter(); continue; 
                }

                if ((jogadorAtual->obterArma() && jogadorAtual->obterArma()->obterNomeItem() == item1->obterNomeItem()) ||
                    (jogadorAtual->obterEscudo() && jogadorAtual->obterEscudo()->obterNomeItem() == item1->obterNomeItem()) ||
                    (jogadorAtual->obterArmadura() && jogadorAtual->obterArmadura()->obterNomeItem() == item1->obterNomeItem())) {
                    std::cout << "\n[Bjorn]: Voce possui uma copia deste item equipada! DESEQUIPE antes de fundir.\n";
                    Menu::aguardarPressionamentoDeEnter(); continue;
                }

                std::string novoNome = item1->obterNomeItem() + "+";
                std::unique_ptr<Item> novoItem = nullptr;

                if (Arma* a1 = dynamic_cast<Arma*>(item1))
                    novoItem = std::make_unique<Arma>(novoNome, static_cast<int>(a1->obterDanoFisico() * 1.5), static_cast<int>(a1->obterDanoMagico() * 1.5));
                else if (Escudo* e1 = dynamic_cast<Escudo*>(item1))
                    novoItem = std::make_unique<Escudo>(novoNome, static_cast<int>(e1->obterDurabilidadeAtualEscudo() * 1.5), static_cast<int>(e1->obterReducaoDanoFixaEscudo() * 1.5));
                else if (Armadura* ar1 = dynamic_cast<Armadura*>(item1))
                    novoItem = std::make_unique<Armadura>(novoNome, static_cast<int>(ar1->obterReducaoFixa() * 1.5));

                 if (novoItem) {
                    if (item1->possuiEfeitoSangramento())
                        novoItem->aplicarEfeitoSangramento();
                    if (item1->possuiEfeitoLentidao())
                        novoItem->aplicarEfeitoLentidao();
                    for (Propriedade prop : item1->obterPropriedades())
                        novoItem->adicionarPropriedade(prop);
                    novoItem->adicionarPropriedade(Propriedade::Melhorado);

                    std::string nomeAntigo = item1->obterNomeItem();
                    for (int i = 0; i < 2; ++i)
                        jogadorAtual->obterInventario()->removerItem(nomeAntigo);
                    jogadorAtual->obterInventario()->adicionarItem(std::move(novoItem));

                    Menu::limparTelaDoTerminal();
                    Menu::exibirLogoDoJogo("FORJA - SUCESSO");
                    std::vector<std::string> arteBigorna = {
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
                    Menu::imprimirLinhasCentralizadasNaTela(arteBigorna, 29, "\033[0m");

                    std::cout << "\n[Bjorn]: Ha! Trabalho feito! Seu equipamento esta mais forte do que nunca!\n";
                    Menu::aguardarPressionamentoDeEnter();
                }
            } while (codigo1 != "0");
        }
    } while (opcaoBjorn != "0");
}

std::vector<std::string> NPCBjorn::obterMapaForja()
{
    return {
        " ##################################",
        "##.........................../--/|##",
        "##..[^S]......................B.$|##",
        "##.........................../--/|##",
        " ##################################"
    };
}
