#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <algorithm>
#include <memory>

#include "NPCBjorn.h"
#include "../Telas/TelaMenu.h"
#include "../Inventario/Item.h"
#include "../Inventario/FabricaItens.h"
#include "../Inventario/EquipamentoArmadura.h"
#include "../Telas/TelaInventario.h"
#include "../Utilidades/SimplificacoesAparencia.h"

namespace {
    std::map<int, std::string> estoqueArmas = {
        {1, "Espada longa de ferro"},
        {2, "Arco recurvo de madeira"},
        {3, "Cajado de cristal magico"},
        {4, "Violao encantado"}
    };
    
    std::map<int, std::string> estoqueArmaduras = {
        {1, "Armadura de malha e metal"},
        {2, "Armadura leve de couro com malha"},
        {3, "Tunica"},
        {4, "Traje de Couro e tecido nobre"}
    };
    
    void processarCompraDeEquipamento(SistemaPersonagem* jogadorAtual, const std::string& margemMsg, bool comprandoArmas);
    void processarMelhoriaNaBigorna(SistemaPersonagem* jogadorAtual);
    void processarUpgradePorMaterial(SistemaPersonagem* jogadorAtual);
}

void NPCBjorn::interagir(SistemaPersonagem* jogadorAtual)
{
    int larguraDoTerminal = SimplificacoesAparencia::obterLarguraTerminal();
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
        SimplificacoesAparencia::limparTela();
        TelaMenu::exibirLogoDoJogo("FORJA DO BJORN");
        
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
            "[3] MELHORAR POR FUSAO",
            "[4] MELHORAR POR MATERIAL",
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
                std::cout << SimplificacoesAparencia::cor(Cor::AMARELO) << arteBjorn[i] << SimplificacoesAparencia::cor(Cor::RESET);
            }
            std::cout << "\n";
        }
        
        std::cout << "\n" << std::string(recuo, ' ') << "Escolha: ";
        std::cin >> opcaoBjorn;

        if (opcaoBjorn == "1" || opcaoBjorn == "2") {
            processarCompraDeEquipamento(jogadorAtual, margemMsg, opcaoBjorn == "1");
        } else if (opcaoBjorn == "3") {
            processarMelhoriaNaBigorna(jogadorAtual);
        } else if (opcaoBjorn == "4") {
            processarUpgradePorMaterial(jogadorAtual);
        }
    } while (opcaoBjorn != "0");
}

namespace {
    void processarCompraDeEquipamento(SistemaPersonagem* jogadorAtual, const std::string& margemMsg, bool comprandoArmas) {
        auto& estoqueAtual = comprandoArmas ? estoqueArmas : estoqueArmaduras;
        std::string tituloLoja = comprandoArmas ? "FORJA - ARMAS" : "FORJA - ARMADURAS";

        std::string opcaoCompra;
        do {
            SimplificacoesAparencia::limparTela();
            TelaMenu::exibirLogoDoJogo(tituloLoja);
            std::cout << "\n" << margemMsg << "Seu Ouro: " << jogadorAtual->obterInventario()->obterOuro() << "G\n\n";
            for (auto const& [id, nomeItem] : estoqueAtual) {
                std::string preco = "40G";
                std::unique_ptr<Item> tempItem = FabricaItens::criarItem(nomeItem);
                std::string infoStatus = tempItem ? tempItem->obterInfoStatus() : "";
                std::cout << margemMsg << "[" << id << "] " << nomeItem << infoStatus << " (" << preco << ")\n";
            }
            std::cout << "\n" << margemMsg << "[0] VOLTAR\n\n" << margemMsg << "Escolha: ";
            std::cin >> opcaoCompra;
            
            if (opcaoCompra != "0") {
                try {
                    int idCompra = std::stoi(opcaoCompra);
                    if (estoqueAtual.count(idCompra)) {
                        if (jogadorAtual->obterInventario()->obterOuro() >= 40) {
                            jogadorAtual->obterInventario()->adicionarOuro(-40);
                            jogadorAtual->obterInventario()->adicionarItem(std::move(FabricaItens::criarItem(estoqueAtual[idCompra])));
                            std::cout << "\n" << margemMsg << "[Bjorn]: Otima escolha! Voce comprou " << estoqueAtual[idCompra] << ".\n";
                        } else {
                            std::cout << "\n" << margemMsg << "[Bjorn]: Voce nao tem ouro suficiente para isso!\n";
                        }
                        SimplificacoesAparencia::aguardarEnter();
                    }
                } catch (...) {}
            }
        } while (opcaoCompra != "0");
    }

    void processarMelhoriaNaBigorna(SistemaPersonagem* jogadorAtual) {
        std::string codigo1, codigo2;
        do {
            TelaInventario::exibir(jogadorAtual);
            std::cout << "\n[Bjorn]: Escolha a ARMA, ESCUDO ou ARMADURA para melhorar (requer copia) ou [0] VOLTAR: ";
            std::cin >> codigo1;
            if (codigo1 == "0") break;
            
            Item* item1 = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigo1, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
            if (!item1) { std::cout << "\n[SISTEMA]: Item invalido!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }
            if (item1 == jogadorAtual->obterArma() || item1 == jogadorAtual->obterEscudo() || item1 == jogadorAtual->obterArmadura()) { std::cout << "\n[Bjorn]: Voce precisa DESEQUIPAR o item antes de usa-lo na bigorna!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }
            if (item1->temPropriedade(Propriedade::Melhorado)) { std::cout << "\n[Bjorn]: Este item ja atingiu o limite de melhoria basica!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }
            TipoEquipamento tipo = item1->obterTipo();
            if (tipo != TipoEquipamento::ARMA && tipo != TipoEquipamento::ESCUDO && tipo != TipoEquipamento::ARMADURA) { std::cout << "\n[Bjorn]: Eu so posso melhorar Armas, Escudos e Armaduras!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }

            std::cout << "[Bjorn]: Agora, escolha o SEGUNDO item identico (copia) ou [0] CANCELAR: ";
            std::cin >> codigo2;
            if (codigo2 == "0") continue;

            Item* item2 = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigo2, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
            if (!item2) { std::cout << "\n[SISTEMA]: Item invalido!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }
            if (item1->obterNomeItem() != item2->obterNomeItem()) { std::cout << "\n[Bjorn]: Os itens precisam ser EXATAMENTE iguais para serem fundidos!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }

            if (jogadorAtual->obterInventario()->contarItem(item1->obterNomeItem()) < 2) {
                std::cout << "\n[Bjorn]: Voce nao possui UMA COPIA deste item!\n"; SimplificacoesAparencia::aguardarEnter(); continue; 
            }

            if ((jogadorAtual->obterArma() && jogadorAtual->obterArma()->obterNomeItem() == item1->obterNomeItem()) ||
                (jogadorAtual->obterEscudo() && jogadorAtual->obterEscudo()->obterNomeItem() == item1->obterNomeItem()) ||
                (jogadorAtual->obterArmadura() && jogadorAtual->obterArmadura()->obterNomeItem() == item1->obterNomeItem())) {
                std::cout << "\n[Bjorn]: Voce possui uma copia deste item equipada! DESEQUIPE antes de fundir.\n";
                SimplificacoesAparencia::aguardarEnter(); continue;
            }

            std::unique_ptr<Item> novoItem = item1->gerarCopiaMelhorada();

             if (novoItem) {
                std::string nomeAntigo = item1->obterNomeItem();
                std::string novoNome = novoItem->obterNomeItem();
                jogadorAtual->obterInventario()->removerItem(item1);
                jogadorAtual->obterInventario()->removerItem(item2);
                jogadorAtual->obterInventario()->adicionarItem(std::move(novoItem));

                SimplificacoesAparencia::limparTela();
                TelaMenu::exibirLogoDoJogo("FORJA - SUCESSO");
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
                SimplificacoesAparencia::imprimirCentralizadoMultilinha({equacao, ""}, 0, SimplificacoesAparencia::cor(Cor::AMARELO));
                SimplificacoesAparencia::imprimirCentralizadoMultilinha(arteBigorna, 29, SimplificacoesAparencia::cor(Cor::RESET));

                std::cout << "\n[Bjorn]: Ha! Trabalho feito! Seu equipamento esta mais forte do que nunca!\n";
                SimplificacoesAparencia::aguardarEnter();
            }
        } while (codigo1 != "0");
    }

    void processarUpgradePorMaterial(SistemaPersonagem* jogadorAtual) {
        std::string codigo;
        do {
            int qtdPedra = jogadorAtual->obterInventario()->contarItem("Pedra magica de upgrade");
            if (qtdPedra < 1) {
                SimplificacoesAparencia::limparTela();
                TelaMenu::exibirLogoDoJogo("FORJA - MELHORIA POR MATERIAL");
                std::cout << "\n[Bjorn]: Voce nao tem nenhuma Pedra magica de upgrade!\n";
                SimplificacoesAparencia::aguardarEnter();
                return;
            }

            TelaInventario::exibir(jogadorAtual);
            std::cout << "\n[Bjorn]: Escolha a ARMADURA para melhorar (+3 Defesa/Resistencia) ou [0] VOLTAR: ";
            std::cin >> codigo;
            if (codigo == "0") break;

            Item* item = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigo, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
            if (!item) { std::cout << "\n[SISTEMA]: Item invalido!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }

            if (item == jogadorAtual->obterArmadura() || item == jogadorAtual->obterArma() || item == jogadorAtual->obterEscudo()) {
                std::cout << "\n[Bjorn]: Voce precisa DESEQUIPAR o item antes de usa-lo na bigorna!\n"; 
                SimplificacoesAparencia::aguardarEnter(); 
                continue; 
            }

            if (item->obterTipo() != TipoEquipamento::ARMADURA) {
                std::cout << "\n[Bjorn]: Esta pedra magica so pode ser usada em ARMADURAS!\n";
                SimplificacoesAparencia::aguardarEnter(); 
                continue;
            }

            EquipamentoArmadura* armadura = dynamic_cast<EquipamentoArmadura*>(item);
            if (!armadura) continue;

            if (armadura->temPropriedade(Propriedade::MelhoradoMaterial)) {
                std::cout << "\n[Bjorn]: Esta armadura ja foi imbuida com a pedra magica!\n";
                SimplificacoesAparencia::aguardarEnter();
                continue;
            }

            std::string nomeAntigo = armadura->obterNomeItem();
            std::string novoNome = nomeAntigo + " (Imbuida)";

            auto novaArmadura = std::make_unique<EquipamentoArmadura>(
                novoNome, 
                armadura->obterReducaoFixa() + 3, 
                armadura->obterReqResistencia(), 
                armadura->obterReqConstituicao(), 
                armadura->obterPrecoVenda() + 200
            );

            for (Propriedade prop : armadura->obterPropriedades()) novaArmadura->adicionarPropriedade(prop);
            novaArmadura->adicionarPropriedade(Propriedade::MelhoradoMaterial);

            jogadorAtual->obterInventario()->removerItem("Pedra magica de upgrade");
            jogadorAtual->obterInventario()->removerItem(armadura);
            jogadorAtual->obterInventario()->adicionarItem(std::move(novaArmadura));

            SimplificacoesAparencia::limparTela();
                TelaMenu::exibirLogoDoJogo("FORJA - SUCESSO");
                TelaMenu::exibirLogoDoJogo("FORJA - SUCESSO");
            std::string equacao = "[" + nomeAntigo + "] + [Pedra magica] = [" + novoNome + "]";
            std::cout << "\n" << SimplificacoesAparencia::cor(Cor::AMARELO) << equacao << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
            std::cout << "\n[Bjorn]: Impressionante! Essa pedra e mesmo magica. A armadura agora possui mais +3 de resistencia (defesa)!\n";
            SimplificacoesAparencia::aguardarEnter();
        } while (codigo != "0");
    }
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
