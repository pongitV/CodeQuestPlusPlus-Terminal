#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

#include "NPCMorgana.h"
#include "../Telas/TelaMenu.h"
#include "../Inventario/Item.h"
#include "../Inventario/FabricaItens.h"
#include "../Telas/TelaInventario.h"
#include "../Utilidades/SimplificacoesAparencia.h"
#include "../Inventario/EquipamentoArma.h"

namespace {
    void processarEncantamentos(SistemaPersonagem* jogadorAtual, bool isUniversal, const std::string& margemMsg);
    void processarPocoes(SistemaPersonagem* jogadorAtual, bool isBuff, const std::string& margemMsg);
    void processarMissaoLabirinto(SistemaPersonagem* jogadorAtual, const std::string& margemMsg);
}

void NPCMorgana::interagir(SistemaPersonagem* jogadorAtual)
{
    int larguraDoTerminal = SimplificacoesAparencia::obterLarguraTerminal();
    std::string opcaoMorgana;
    
    std::vector<std::string> arteMorgana = 
    {
        "                                %*                          ",
        "                               #%%                          ",
        "                              =%%%                          ",
        "                              %%%#                          ",
        "                             %%%%%                          ",
        "                            #%%%%%.                         ",
        "                      .    .%%%%%%-                         ",
        "                      .####%%%%%%%@:+*-         -:-         ",
        "                           *+*=*+#-@%%%@       ::: .        ",
        "                           -==---:+           :-:           ",
        "           :  *            . ---+-   +:       @%%           ",
        "           -               +%%@%%@%%@%%@      %%%           ",
        "          *            #%%%%%%%%%%%%%@%%%%%% %%%-           ",
        "          *           -%%%%%%%@%@%%%%%%%%%%%%%%%            ",
        "          #-          %%%@%%%%%%%%%%%%%%%%%%%%%=            ",
        "           =+        -%@@%%%%%%%%%%%%%   .                  ",
        "            #:       .%@%%%%%%%#%%%%%                       ",
        "             #        .@%%%%%%%*%%%%                        ",
        "             -%.   :%%@%@%%%%%%%%%%%#                       ",
        "             :-%-%%%%@@#%%%%%%%%%%%%%                       ",
        "              #:       %%%%%%%@%%%%%%                       ",
        "              #:      #%%%%%%@@%%%%%%#                      ",
        "              #-      %%%%%%%%@%@%%%%%                      ",
        "              *-     :%%%%%%%%%%@%#%%%                      ",
        "              #-     =%%%%%%%@%%%%@%%%.                     ",
        "              #.     +%%%%%%%%%%%%@%%%+                     ",
        "             .#      +%%%%%%%%%%%%@%%%#                     ",
        "             +#      +%%%%%%%%%%%%@@%%%                     ",
        "             @%      *%%%%%%%%%%%%@@%%%                     ",
        "             %%      %%%%%%%%%%%%%@@%%%-                    ",
        "           .%%%:     %%%%%%%%%%%%%%@%%%@                    ",
        "          .#%%%#.    #%%%%%%%%%%%%%@%%%%                    ",
        "          -%%%%%     *%%%%%%%%%%%%%@%%%%=                   ",
        "          +%#%%#:    *%%%%%%%%%%%%%%%%%%@                   ",
        "         .*#%%%#     +%%%%%%%%%%%%%%%%%%%                   ",
        "         .-*#+*-     =%%%%%%%%%%%%%%%%%%%%                  ",
        "          ::=:=:.    -%%%%%%%%%%%%%%%%%%%%                  ",
        "                       %%%%%%%%%%     +@%%@                 ",
        "                      .%%*             @.@#%                ",
        "                      +#%=                *#*:              "
    };

    do {
        SimplificacoesAparencia::limparTela();
        TelaMenu::exibirLogoDoJogo("CABANA DA BRUXA");
        
        int espacosMsg = (larguraDoTerminal - 55) / 2;
        std::string margemMsg(espacosMsg > 0 ? espacosMsg : 0, ' ');

        std::vector<std::string> menuEsquerda = 
        {
            "[Morgana]: Hmmm... sinto cheiro de poder no ar.",
            "O que voce busca, viajante?",
            "",
            "[1] ENCANTAR Armas (Universais)",
            "[2] ENCANTAR Armas (Especificas)",
            "[3] COMPRAR Pocoes de Buff",
            "[4] COMPRAR Frascos de Debuff"
        };
        
        if (!jogadorAtual->obterLabirintoDesbloqueado()) {
            menuEsquerda.push_back("[5] [M] Consiga 3x Coracoes da floresta");
        }
        
        menuEsquerda.push_back("[0] VOLTAR");
        menuEsquerda.push_back("");

        int maxLinhas = std::max(menuEsquerda.size(), arteMorgana.size());
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
            if (i < arteMorgana.size()) {
                std::cout << SimplificacoesAparencia::cor(Cor::MAGENTA) << arteMorgana[i] << SimplificacoesAparencia::cor(Cor::RESET); // Magenta
            }
            std::cout << "\n";
        }
        
        std::cout << "\n" << std::string(recuo, ' ') << "Escolha: ";
        std::cin >> opcaoMorgana;

        if (opcaoMorgana == "1") {
            processarEncantamentos(jogadorAtual, true, margemMsg);
        }
        else if (opcaoMorgana == "2") {
            processarEncantamentos(jogadorAtual, false, margemMsg);
        }
        else if (opcaoMorgana == "3" || opcaoMorgana == "4") {
            processarPocoes(jogadorAtual, opcaoMorgana == "3", margemMsg);
        }
        else if (opcaoMorgana == "5" && !jogadorAtual->obterLabirintoDesbloqueado()) {
            processarMissaoLabirinto(jogadorAtual, margemMsg);
        }
    } while (opcaoMorgana != "0");
}

namespace {
    void processarEncantamentos(SistemaPersonagem* jogadorAtual, bool isUniversal, const std::string& margemMsg) {
        std::string opcaoEncantar;
        do {
            SimplificacoesAparencia::limparTela();
            TelaMenu::exibirLogoDoJogo(isUniversal ? "CABANA - ENCANTOS UNIVERSAIS" : "CABANA - ENCANTOS ESPECIFICOS");
            std::cout << "\n" << margemMsg << "Escolha um encantamento:\n\n";
            
            if (isUniversal) {
                std::cout << margemMsg << "[1] Sangramento (40x Dente de Goblin)\n";
                std::cout << margemMsg << "[2] Lentidao (5x Nucleo pegajoso)\n";
                std::cout << margemMsg << "[3] Quebra de Resistencia (25x Po magico)\n";
            } else {
                std::cout << margemMsg << "[1] Arco recurvo de madeira: Magia (1x Madeira enfeiticada)\n";
                std::cout << margemMsg << "[2] Cajado de cristal magico: Cipos (1x Coracao da floresta)\n";
                std::cout << margemMsg << "[3] Violao encantado: Raizes (1x Madeira enfeiticada)\n";
            }
            std::cout << "\n" << margemMsg << "[0] VOLTAR\n\n" << margemMsg << "Escolha: ";
            std::cin >> opcaoEncantar;

            if (opcaoEncantar == "1" || opcaoEncantar == "2" || opcaoEncantar == "3") {
                struct EncantoInfo { std::string nome; int qtd; };
                EncantoInfo encantos[] = {
                    {"Dente de goblin", 40}, {"Nucleo pegajoso", 5}, {"Po magico", 25},
                    {"Madeira enfeiticada", 1}, {"Coracao da floresta", 1}, {"Madeira enfeiticada", 1}
                };
                
                int index = (isUniversal ? 0 : 3) + (opcaoEncantar[0] - '1');
                bool isSangramento = (index == 0), isLentidao = (index == 1), isResistencia = (index == 2);
                bool isMagia = (index == 3), isCipos = (index == 4), isRaizes = (index == 5);

                auto [itemNecessario, qtdNecessaria] = encantos[index];
                
                int qtdAtual = jogadorAtual->obterInventario()->contarItem(itemNecessario);
                if (qtdAtual < qtdNecessaria) {
                    std::cout << "\n[Morgana]: Voce nao tem " << itemNecessario << " suficiente! (Possui: " << qtdAtual << "/" << qtdNecessaria << ")\n";
                    SimplificacoesAparencia::aguardarEnter();
                    continue;
                }
                
                std::string codigoArma;
                TelaInventario::exibir(jogadorAtual);
                std::cout << "\n[Morgana]: Escolha a ARMA para encantar ou [0] VOLTAR: ";
                std::cin >> codigoArma;
                if (codigoArma == "0") continue;
                
                Item* itemEscolhido = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigoArma, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
                if (!itemEscolhido) { std::cout << "\n[SISTEMA]: Item invalido!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }
                
                EquipamentoArma* armaEscolhida = dynamic_cast<EquipamentoArma*>(itemEscolhido);
                if (!armaEscolhida) { std::cout << "\n[Morgana]: Eu so posso encantar ARMAS com isso!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }
                
                if (isMagia && armaEscolhida->obterNomeItem().find("Arco recurvo de madeira") == std::string::npos) { std::cout << "\n[Morgana]: Este encantamento so funciona no Arco recurvo de madeira!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }
                if (isCipos && armaEscolhida->obterNomeItem().find("Cajado de cristal magico") == std::string::npos) { std::cout << "\n[Morgana]: Este encantamento so funciona no Cajado de cristal magico!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }
                if (isRaizes && armaEscolhida->obterNomeItem().find("Violao encantado") == std::string::npos) { std::cout << "\n[Morgana]: Este encantamento so funciona no Violao!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }
                
                if (isSangramento && armaEscolhida->possuiEfeitoSangramento()) { std::cout << "\n[Morgana]: Esta arma ja esta encantada com Sangramento!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }
                if (isLentidao && armaEscolhida->possuiEfeitoLentidao()) { std::cout << "\n[Morgana]: Esta arma ja esta encantada com Lentidao!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }
                if (isResistencia && armaEscolhida->temPropriedade(Propriedade::Penetrante)) { std::cout << "\n[Morgana]: Esta arma ja esta encantada com Reducao de Resistencia!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }
                if (isMagia && armaEscolhida->temPropriedade(Propriedade::Magica)) { std::cout << "\n[Morgana]: Esta arma ja esta encantada com Magia!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }
                if (isCipos && armaEscolhida->temPropriedade(Propriedade::CipoPrisao)) { std::cout << "\n[Morgana]: Esta arma ja esta encantada com Cipos!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }
                if (isRaizes && armaEscolhida->temPropriedade(Propriedade::ViolaoMagico)) { std::cout << "\n[Morgana]: Esta arma ja esta encantada com Raizes!\n"; SimplificacoesAparencia::aguardarEnter(); continue; }
                
                std::string nomeAntigoArma = armaEscolhida->obterNomeItem();

                for (int i = 0; i < qtdNecessaria; ++i) jogadorAtual->obterInventario()->removerItem(itemNecessario);
                
                if (isSangramento) { armaEscolhida->aplicarEfeitoSangramento(); armaEscolhida->alterarNome(armaEscolhida->obterNomeItem() + " (Sangrenta)"); }
                else if (isLentidao) { armaEscolhida->aplicarEfeitoLentidao(); armaEscolhida->alterarNome(armaEscolhida->obterNomeItem() + " (Viscosa)"); }
                else if (isResistencia) { armaEscolhida->alterarNome(armaEscolhida->obterNomeItem() + " (Quebra-Defesas)"); armaEscolhida->adicionarPropriedade(Propriedade::Penetrante); }
                else if (isMagia) {
                    std::string nome = armaEscolhida->obterNomeItem();
                    size_t pos = nome.find("Arco recurvo de madeira");
                    if (pos != std::string::npos) nome.replace(pos, 23, "Arco recurvo de madeira enfeiticada");
                    int novoDanoMagico = armaEscolhida->obterDanoMagico() + (armaEscolhida->obterDanoFisico() / 2);
                    auto novoArcoObj = std::make_unique<EquipamentoArma>(nome, armaEscolhida->obterDanoFisico(), novoDanoMagico, armaEscolhida->obterReqForca(), armaEscolhida->obterReqDestreza(), armaEscolhida->obterReqInteligencia(), armaEscolhida->obterReqSabedoria(), 0);
                    EquipamentoArma* novoArco = novoArcoObj.get();
                    if (armaEscolhida->possuiEfeitoSangramento()) novoArco->aplicarEfeitoSangramento();
                    if (armaEscolhida->possuiEfeitoLentidao()) novoArco->aplicarEfeitoLentidao();
                    if (armaEscolhida->temPropriedade(Propriedade::Penetrante)) novoArco->adicionarPropriedade(Propriedade::Penetrante);
                    novoArco->adicionarPropriedade(Propriedade::Magica);

                    bool estavaEquipado = (jogadorAtual->obterArma() == armaEscolhida);
                    if (estavaEquipado) jogadorAtual->desequiparArma();
                    jogadorAtual->obterInventario()->removerItem(nomeAntigoArma);
                    jogadorAtual->obterInventario()->adicionarItem(std::move(novoArcoObj));
                    if (estavaEquipado) jogadorAtual->equiparItem(novoArco);
                    armaEscolhida = novoArco;
                }
                else if (isCipos) {
                    std::string nome = armaEscolhida->obterNomeItem();
                    size_t pos = nome.find("Cajado de cristal magico");
                    if (pos != std::string::npos) nome.replace(pos, 24, "Cajado de cipos");
                    armaEscolhida->alterarNome(nome);
                    armaEscolhida->adicionarPropriedade(Propriedade::CipoPrisao);
                }
                else if (isRaizes) {
                    std::string nome = armaEscolhida->obterNomeItem();
                    size_t pos = nome.find("Violao encantado");
                    if (pos != std::string::npos) nome.replace(pos, 16, "Violao enfeiticado");
                    else nome += " enfeiticado";
                    armaEscolhida->alterarNome(nome);
                    armaEscolhida->adicionarPropriedade(Propriedade::ViolaoMagico);
                }
                
                SimplificacoesAparencia::limparTela();
                TelaMenu::exibirLogoDoJogo("ENCANTAMENTO SUCESSO");
                std::vector<std::string> arteCaldeirao = { 
                    "                                         ",
                    "⠀⠀⣤⣤⣤⣀⣀⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣀⣠⣤⣤⡀⠀   ",
                    "⠀⠀⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠁⠀",
                    "⠀⠀⣼⣿⣿⣦⠉⠉⠉⠉⠉⠉⠉⠙⠛⠛⠛⠛⠛⣛⣉⣩⣭⣽⣿⣿⣿⣧⠀⠀",
                    "⠀⢰⣿⣿⣿⣿⡇⢰⣿⣷⠀⣿⣿⣿⣧⠀⢸⡆⠸⣿⣿⣿⣿⣿⣿⣿⣿⣿⡆⠀",
                    "⠀⣼⣿⣿⣿⣿⡀⣸⣿⣿⣾⣿⣿⣿⣿⡇⢸⡇⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀",
                    "⠀⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⢸⣷⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠀",
                    "⠀⠸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠇⠀",
                    "⠀⠀⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠀⠀ ",
                    "⠀⠀⠀⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠀⠀⠀ ",
                    "⠀⠀⠀⠀⠙⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠋⠀⠀⠀⠀ ",
                    "⠀⠀⠀⠀⠀⣰⣿⣿⡿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⣿⣿⣇⠀⠀⠀⠀⠀  ",
                    "⠀⠀⠀⠀⢰⣿⣿⡟⠀⠀⠉⠙⠛⠛⠛⠛⠛⠛⠋⠉⠀⠀⢻⣿⣿⣆⠀⠀⠀⠀  ",
                    "⠀⠀⠀⠀⠈⠛⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠛⠁⠀⠀⠀⠀    ",
                    "                                         "
                };
                
                std::string equacao = "[" + nomeAntigoArma + "] + " + std::to_string(qtdNecessaria) + "x [" + itemNecessario + "] = [" + armaEscolhida->obterNomeItem() + "]";
                SimplificacoesAparencia::imprimirCentralizadoMultilinha({equacao, ""}, 0, SimplificacoesAparencia::cor(Cor::AMARELO));
                SimplificacoesAparencia::imprimirCentralizadoMultilinha(arteCaldeirao, 46, SimplificacoesAparencia::cor(Cor::RESET));
                
                std::cout << "\n[Morgana]: Feito! A magia flui pela sua arma...\n";
                SimplificacoesAparencia::aguardarEnter();
            }
        } while (opcaoEncantar != "0");
    }

    void processarPocoes(SistemaPersonagem* jogadorAtual, bool isBuff, const std::string& margemMsg) {
        std::string titulo = isBuff ? "CABANA - POCOES DE BUFF" : "CABANA - FRASCOS DE DEBUFF";
        std::string opcaoCompra;
        do {
            SimplificacoesAparencia::limparTela();
            TelaMenu::exibirLogoDoJogo(titulo);
            std::cout << "\n" << margemMsg << "Seu Ouro: " << jogadorAtual->obterInventario()->obterOuro() << "G\n\n";

            if (isBuff) {
                std::cout << margemMsg << "[1] Pocao de Furia (Buff x1.5 Atributos | 2 Turnos) - 25G\n";
                std::cout << margemMsg << "[2] Elixir Arcano (Buff x1.5 Atributos | 2 Turnos)  - 25G\n";
            } else {
                std::cout << margemMsg << "[1] Frasco de Gosma (Debuff Lentidao | 3 Turnos)    - 30G\n";
                std::cout << margemMsg << "[2] Frasco de Fraqueza (Debuff Fraqueza | 3 Turnos) - 30G\n";
            }
            std::cout << "\n" << margemMsg << "[0] VOLTAR\n\n" << margemMsg << "Escolha: ";
            std::cin >> opcaoCompra;

            if (opcaoCompra >= "1" && opcaoCompra <= "2") {
                int preco = (isBuff ? 25 : 30);
                if (jogadorAtual->obterInventario()->obterOuro() >= preco) {
                    jogadorAtual->obterInventario()->adicionarOuro(-preco);
                    std::string nomeItem = "";
                    if (isBuff) {
                        if (opcaoCompra == "1") nomeItem = "Pocao de Furia (Buff)";
                        else if (opcaoCompra == "2") nomeItem = "Elixir Arcano (Buff)";
                    } else {
                        if (opcaoCompra == "1") nomeItem = "Frasco de Gosma (Debuff)";
                        else if (opcaoCompra == "2") nomeItem = "Frasco de Fraqueza (Debuff)";
                    }
                    auto novoItem = FabricaItens::criarItem(nomeItem);
                    if (novoItem) {
                        std::string nomeDoNovoItem = novoItem->obterNomeItem();
                        jogadorAtual->obterInventario()->adicionarItem(std::move(novoItem));
                        std::cout << "\n" << margemMsg << "[Morgana]: Heehee... Use com sabedoria! " << nomeDoNovoItem << " adicionado.\n";
                    }
                } else {
                    std::cout << "\n" << margemMsg << "[Morgana]: Voce nao tem ouro suficiente para as minhas preparacoes!\n";
                }
                SimplificacoesAparencia::aguardarEnter();
            }
        } while (opcaoCompra != "0");
    }

    void processarMissaoLabirinto(SistemaPersonagem* jogadorAtual, const std::string& margemMsg) {
        int qtdCoracoes = jogadorAtual->obterInventario()->contarItem("Coracao da floresta");
        if (qtdCoracoes >= 3) {
            for (int i = 0; i < 3; ++i) jogadorAtual->obterInventario()->removerItem("Coracao da floresta");
            jogadorAtual->desbloquearLabirinto();
            
            SimplificacoesAparencia::limparTela();
            TelaMenu::exibirLogoDoJogo("MISSAO CONCLUIDA");
            std::cout << "\n" << margemMsg << "[Morgana]: Ah, perfeitos! Estes coracoes pulsam com uma magia ancestral.\n";
            std::cout << margemMsg << "[Morgana]: Como recompensa, revelarei um segredo... Atrás de mim, ha uma passagem secreta.\n";
            std::cout << margemMsg << "[Morgana]: Use a entrada [^L] para explorar o meu Labirinto Subterraneo.\n";
            std::cout << margemMsg << "[Morgana]: E um lugar perigoso, mergulhado em uma nevoa de cor roxa, mas guarda grandes tesouros.\n";
        }
        else {
            SimplificacoesAparencia::limparTela();
            TelaMenu::exibirLogoDoJogo("MISSAO");
            std::cout << "\n" << margemMsg << "[Morgana]: Voce ainda nao possui os 3 Coracoes da floresta que eu pedi.\n";
            std::cout << margemMsg << "[Morgana]: (Voce possui: " << qtdCoracoes << "/3)\n";
            std::cout << margemMsg << "[Morgana]: Eles sao dropados por Abominacoes no Coracao da Arvore.\n";
        }
        SimplificacoesAparencia::aguardarEnter();
    }
}

std::vector<std::string> NPCMorgana::obterMapaCabana()
{
    return {
        " ######################",
        "##..........;<>......##",
        "##..[^S]..........M..^L#",
        "##..........;<>......##",
        " ######################"
    };
}
