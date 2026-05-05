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
    std::map<int, ItemID> estoqueArmas = {
        {1, ItemID::EspadaFerro},
        {2, ItemID::ArcoMadeira},
        {3, ItemID::CajadoCristal},
        {4, ItemID::ViolaoEncantado}
    };
    
    std::map<int, ItemID> estoqueArmaduras = {
        {1, ItemID::ArmaduraMalha},
        {2, ItemID::ArmaduraCouro},
        {3, ItemID::Tunica},
        {4, ItemID::TrajeNobre}
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
            for (auto const& [id, idItem] : estoqueAtual) {
                std::string preco = "40G";
                std::unique_ptr<Item> tempItem = FabricaItens::criarItem(idItem);
                std::string infoStatus = tempItem ? tempItem->obterInfoStatus() : "";
                std::cout << margemMsg << "[" << id << "] " << (tempItem ? tempItem->obterNomeItem() : "???") << infoStatus << " (" << preco << ")\n";
            }
            std::cout << "\n" << margemMsg << "[0] VOLTAR\n\n" << margemMsg << "Escolha: ";
            std::cin >> opcaoCompra;
            
            if (opcaoCompra != "0") {
                try {
                    int idCompra = std::stoi(opcaoCompra);
                    if (estoqueAtual.count(idCompra)) {
                        if (jogadorAtual->obterInventario()->obterOuro() >= 40) {
                            jogadorAtual->obterInventario()->adicionarOuro(-40);
                            std::unique_ptr<Item> novoItem = FabricaItens::criarItem(estoqueAtual[idCompra]);
                            std::string nomeNovo = novoItem->obterNomeItem();
                            jogadorAtual->obterInventario()->adicionarItem(std::move(novoItem));
                            std::cout << "\n" << margemMsg << "[Bjorn]: Otima escolha! Voce comprou " << nomeNovo << ".\n";
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
        std::string codigoDoItemBase, codigoDoItemCopia;
        do {
            TelaInventario::exibir(jogadorAtual);
            std::cout << "\n[Bjorn]: Escolha a ARMA, ESCUDO ou ARMADURA para melhorar (requer copia) ou [0] VOLTAR: ";
            std::cin >> codigoDoItemBase;
            if (codigoDoItemBase == "0") break;
            
            Item* itemBase = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigoDoItemBase, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
            if (!itemBase) { std::cout << "\n[SISTEMA]: Item invalido!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }
            if (itemBase == jogadorAtual->obterArma() || itemBase == jogadorAtual->obterEscudo() || itemBase == jogadorAtual->obterArmadura()) { std::cout << "\n[Bjorn]: Voce precisa DESEQUIPAR o item antes de usa-lo na bigorna!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }
            if (itemBase->temPropriedade(Propriedade::Melhorado)) { std::cout << "\n[Bjorn]: Este item ja atingiu o limite de melhoria basica!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }
            TipoEquipamento tipo = itemBase->obterTipo();
            if (tipo != TipoEquipamento::ARMA && tipo != TipoEquipamento::ESCUDO && tipo != TipoEquipamento::ARMADURA) { std::cout << "\n[Bjorn]: Eu so posso melhorar Armas, Escudos e Armaduras!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }

            std::cout << "[Bjorn]: Agora, escolha o SEGUNDO item identico (copia) ou [0] CANCELAR: ";
            std::cin >> codigoDoItemCopia;
            if (codigoDoItemCopia == "0") continue;

            Item* itemCopia = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigoDoItemCopia, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
            if (!itemCopia) { std::cout << "\n[SISTEMA]: Item invalido!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }
            if (itemBase->obterNomeItem() != itemCopia->obterNomeItem()) { std::cout << "\n[Bjorn]: Os itens precisam ser EXATAMENTE iguais para serem fundidos!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }

            if (jogadorAtual->obterInventario()->contarItem(itemBase->obterNomeItem()) < 2) {
                std::cout << "\n[Bjorn]: Voce nao possui UMA COPIA deste item!\n"; SimplificacoesAparencia::aguardarEnter(); continue; 
            }

            if ((jogadorAtual->obterArma() && jogadorAtual->obterArma()->obterNomeItem() == itemBase->obterNomeItem()) ||
                (jogadorAtual->obterEscudo() && jogadorAtual->obterEscudo()->obterNomeItem() == itemBase->obterNomeItem()) ||
                (jogadorAtual->obterArmadura() && jogadorAtual->obterArmadura()->obterNomeItem() == itemBase->obterNomeItem())) {
                std::cout << "\n[Bjorn]: Voce possui uma copia deste item equipada! DESEQUIPE antes de fundir.\n";
                SimplificacoesAparencia::aguardarEnter(); continue;
            }

            std::unique_ptr<Item> novoItem = itemBase->gerarCopiaMelhorada();

             if (novoItem) {
                std::string nomeAntigo = itemBase->obterNomeItem();
                std::string novoNome = novoItem->obterNomeItem();
                jogadorAtual->obterInventario()->removerItem(itemBase);
                jogadorAtual->obterInventario()->removerItem(itemCopia);
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
        } while (codigoDoItemBase != "0");
    }

    void processarUpgradePorMaterial(SistemaPersonagem* jogadorAtual) {
        std::string codigoDaArmadura;
        std::string nomePedraUpgrade = FabricaItens::obterNomeDeID(ItemID::PedraUpgrade);
        do {
            int qtdPedraUpgrade = jogadorAtual->obterInventario()->contarItem(nomePedraUpgrade);
            if (qtdPedraUpgrade < 1) {
                SimplificacoesAparencia::limparTela();
                TelaMenu::exibirLogoDoJogo("FORJA - MELHORIA POR MATERIAL");
                std::cout << "\n[Bjorn]: Voce nao tem nenhuma " << nomePedraUpgrade << "!\n";
                SimplificacoesAparencia::aguardarEnter();
                return;
            }

            TelaInventario::exibir(jogadorAtual);
            std::cout << "\n[Bjorn]: Escolha a ARMADURA para melhorar (+3 Defesa/Resistencia) ou [0] VOLTAR: ";
            std::cin >> codigoDaArmadura;
            if (codigoDaArmadura == "0") break;

            Item* itemParaUpgrade = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigoDaArmadura, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
            if (!itemParaUpgrade) { std::cout << "\n[SISTEMA]: Item invalido!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }

            if (itemParaUpgrade == jogadorAtual->obterArmadura() || itemParaUpgrade == jogadorAtual->obterArma() || itemParaUpgrade == jogadorAtual->obterEscudo()) {
                std::cout << "\n[Bjorn]: Voce precisa DESEQUIPAR o item antes de usa-lo na bigorna!\n"; 
                SimplificacoesAparencia::aguardarEnter(); 
                continue; 
            }

            if (itemParaUpgrade->obterTipo() != TipoEquipamento::ARMADURA) {
                std::cout << "\n[Bjorn]: Esta pedra magica so pode ser usada em ARMADURAS!\n";
                SimplificacoesAparencia::aguardarEnter(); 
                continue;
            }

            EquipamentoArmadura* armadura = dynamic_cast<EquipamentoArmadura*>(itemParaUpgrade);
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

            jogadorAtual->obterInventario()->removerItem(nomePedraUpgrade);
            jogadorAtual->obterInventario()->removerItem(armadura);
            jogadorAtual->obterInventario()->adicionarItem(std::move(novaArmadura));

            SimplificacoesAparencia::limparTela();
                TelaMenu::exibirLogoDoJogo("FORJA - SUCESSO");
                TelaMenu::exibirLogoDoJogo("FORJA - SUCESSO");
            std::string equacao = "[" + nomeAntigo + "] + [Pedra magica] = [" + novoNome + "]";
            std::cout << "\n" << SimplificacoesAparencia::cor(Cor::AMARELO) << equacao << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
            std::cout << "\n[Bjorn]: Impressionante! Essa pedra e mesmo magica. A armadura agora possui mais +3 de resistencia (defesa)!\n";
            SimplificacoesAparencia::aguardarEnter();
        } while (codigoDaArmadura != "0");
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
