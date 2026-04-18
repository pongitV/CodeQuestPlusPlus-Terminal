#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

#include "NPCMorgana.h"
#include "../Sistema/Menu.h"
#include "../Inventario/Item.h"
#include "../Inventario/Arma.h"
#include "../Inventario/ItemConsumivel.h"
#include "../Inventario/Material.h"
#include "../Interfaces/TelaInventario.h"

void NPCMorgana::interagir(Personagem* jogadorAtual)
{
    int larguraDoTerminal = Menu::obterLarguraDoTerminalEmColunas();
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
        Menu::limparTelaDoTerminal();
        Menu::exibirLogoDoJogo("CABANA DA BRUXA");
        
        int espacosMsg = (larguraDoTerminal - 55) / 2;
        std::string margemMsg(espacosMsg > 0 ? espacosMsg : 0, ' ');

        std::vector<std::string> menuEsquerda = 
        {
            "[Morgana]: Hmmm... sinto cheiro de poder no ar.",
            "O que voce busca, viajante?",
            "",
            "[1] ENCANTAR Armas",
            "[2] COMPRAR Pocoes de Buff",
            "[3] COMPRAR Frascos de Debuff",
            "[0] VOLTAR",
            ""
        };

        int maxLinhas = std::max(menuEsquerda.size(), arteMorgana.size());
        int larguraInfo = 45;
        int recuo = (larguraDoTerminal - (larguraInfo + 60)) / 2;
        if (recuo < 0) recuo = 0;

        std::cout << "\n";
        for (size_t i = 0; i < maxLinhas; ++i) {
            std::cout << std::string(recuo, ' ');
            if (i < menuEsquerda.size()) 
            {
                std::cout << std::left << std::setw(larguraInfo) << menuEsquerda[i];
            } 
            else 
            {
                std::cout << std::string(larguraInfo, ' ');
            }
            if (i < arteMorgana.size()) 
            {
                std::cout << "\033[0m" << arteMorgana[i] << "\033[0m"; // Magenta
            }
            std::cout << "\n";
        }
        
        std::cout << "\n" << std::string(recuo, ' ') << "Escolha: ";
        std::cin >> opcaoMorgana;

        if (opcaoMorgana == "1") 
        {
            std::string opcaoEncantar;
            do {
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo("CABANA - ENCANTAMENTOS");
                std::cout << "\n" << margemMsg << "Escolha um encantamento:\n\n";
                std::cout << margemMsg << "[1] ENCANTAR Arma: Sangramento (40x Dente de Goblin)\n";
                std::cout << margemMsg << "[2] ENCANTAR Arma: Lentidao (5x Nucleo pegajoso)\n";
                std::cout << margemMsg << "[3] ENCANTAR Arma: Reducao Resist. (25x Po magico)\n";
                std::cout << margemMsg << "[4] ENCANTAR Arco: Magia (1x Madeira enfeiticada)\n";
                std::cout << margemMsg << "[5] ENCANTAR Cajado: Cipos (1x Coracao da floresta)\n";
                std::cout << margemMsg << "[6] ENCANTAR Violao: Raizes (1x Madeira enfeiticada)\n";
                std::cout << "\n" << margemMsg << "[0] VOLTAR\n\n" << margemMsg << "Escolha: ";
                std::cin >> opcaoEncantar;

                if (opcaoEncantar >= "1" && opcaoEncantar <= "6") 
                {
                    bool isSangramento = (opcaoEncantar == "1");
                    bool isLentidao = (opcaoEncantar == "2");
                    bool isResistencia = (opcaoEncantar == "3");
                    bool isMagia = (opcaoEncantar == "4");
                    bool isCipos = (opcaoEncantar == "5");
                    bool isRaizes = (opcaoEncantar == "6");
                    
                    std::string itemNecessario;
                    int qtdNecessaria = 0;
                    if (isSangramento) { itemNecessario = "Dente de goblin"; qtdNecessaria = 40; }
                    else if (isLentidao) { itemNecessario = "Nucleo pegajoso"; qtdNecessaria = 5; }
                    else if (isResistencia) { itemNecessario = "Po magico"; qtdNecessaria = 25; }
                    else if (isMagia) { itemNecessario = "Madeira enfeiticada"; qtdNecessaria = 1; }
                    else if (isCipos) { itemNecessario = "Coracao da floresta"; qtdNecessaria = 1; }
                    else if (isRaizes) { itemNecessario = "Madeira enfeiticada"; qtdNecessaria = 1; }
                    
                    int qtdAtual = jogadorAtual->obterInventario()->contarItem(itemNecessario);
                    if (qtdAtual < qtdNecessaria) 
                    {
                        std::cout << "\n[Morgana]: Voce nao tem " << itemNecessario << " suficiente! (Possui: " << qtdAtual << "/" << qtdNecessaria << ")\n";
                        Menu::aguardarPressionamentoDeEnter();
                        continue;
                    }
                    
                    std::string codigoArma;
                    TelaInventario::exibir(jogadorAtual);
                    std::cout << "\n[Morgana]: Escolha a ARMA para encantar ou [0] VOLTAR: ";
                    std::cin >> codigoArma;
                    if (codigoArma == "0") continue;
                    
                    Item* itemEscolhido = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigoArma, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
                    if (!itemEscolhido) { std::cout << "\n[SISTEMA]: Item invalido!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                    
                    Arma* armaEscolhida = dynamic_cast<Arma*>(itemEscolhido);
                    if (!armaEscolhida) { std::cout << "\n[Morgana]: Eu so posso encantar ARMAS com isso!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                    
                    if (isMagia && armaEscolhida->obterNomeItem().find("Arco recurvo de madeira") == std::string::npos) { std::cout << "\n[Morgana]: Este encantamento so funciona no Arco recurvo de madeira!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                    if (isCipos && armaEscolhida->obterNomeItem().find("Cajado") == std::string::npos) { std::cout << "\n[Morgana]: Este encantamento so funciona no Cajado!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                    if (isRaizes && armaEscolhida->obterNomeItem().find("Violao") == std::string::npos) { std::cout << "\n[Morgana]: Este encantamento so funciona no Violao!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                    
                    if (isSangramento && armaEscolhida->possuiEfeitoSangramento()) { std::cout << "\n[Morgana]: Esta arma ja esta encantada com Sangramento!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                    if (isLentidao && armaEscolhida->possuiEfeitoLentidao()) { std::cout << "\n[Morgana]: Esta arma ja esta encantada com Lentidao!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                    if (isResistencia && armaEscolhida->temPropriedade(Propriedade::Penetrante)) { std::cout << "\n[Morgana]: Esta arma ja esta encantada com Reducao de Resistencia!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                    if (isMagia && armaEscolhida->temPropriedade(Propriedade::Magica)) { std::cout << "\n[Morgana]: Esta arma ja esta encantada com Magia!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                    if (isCipos && armaEscolhida->temPropriedade(Propriedade::CipoPrisao)) { std::cout << "\n[Morgana]: Esta arma ja esta encantada com Cipos!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                    if (isRaizes && armaEscolhida->temPropriedade(Propriedade::ViolaoMagico)) { std::cout << "\n[Morgana]: Esta arma ja esta encantada com Raizes!\n"; Menu::aguardarPressionamentoDeEnter(); continue; }
                    
                    std::string nomeAntigoArma = armaEscolhida->obterNomeItem();

                    for (int i = 0; i < qtdNecessaria; ++i) jogadorAtual->obterInventario()->removerItem(itemNecessario);
                    
                    if (isSangramento) { armaEscolhida->aplicarEfeitoSangramento(); armaEscolhida->alterarNome(armaEscolhida->obterNomeItem() + " (Sangrenta)"); }
                    else if (isLentidao) { armaEscolhida->aplicarEfeitoLentidao(); armaEscolhida->alterarNome(armaEscolhida->obterNomeItem() + " (Viscosa)"); }
                    else if (isResistencia) { armaEscolhida->alterarNome(armaEscolhida->obterNomeItem() + " (Penetrante)"); armaEscolhida->adicionarPropriedade(Propriedade::Penetrante); }
                    else if (isMagia) { 
                        std::string nome = armaEscolhida->obterNomeItem(); 
                        size_t pos = nome.find("Arco recurvo de madeira"); 
                        if (pos != std::string::npos) nome.replace(pos, 23, "Arco recurvo de madeira enfeiticada"); 
                        
                        int novoDanoMagico = armaEscolhida->obterDanoMagico() + (armaEscolhida->obterDanoFisico() / 2);
                        auto novoArcoObj = std::make_unique<Arma>(nome, armaEscolhida->obterDanoFisico(), novoDanoMagico);
                        Arma* novoArco = novoArcoObj.get();
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
                    else if (isCipos) { std::string nome = armaEscolhida->obterNomeItem(); size_t pos = nome.find("Cajado"); if (pos != std::string::npos) nome.replace(pos, 6, "Cajado de cipos"); armaEscolhida->alterarNome(nome); armaEscolhida->adicionarPropriedade(Propriedade::CipoPrisao); }
                    else if (isRaizes) { std::string nome = armaEscolhida->obterNomeItem(); size_t pos = nome.find("Violao encantado"); if (pos != std::string::npos) nome.replace(pos, 16, "Violao enfeiticado"); else nome += " enfeiticado"; armaEscolhida->alterarNome(nome); armaEscolhida->adicionarPropriedade(Propriedade::ViolaoMagico); }
                    
                    Menu::limparTelaDoTerminal();
                    Menu::exibirLogoDoJogo("ENCANTAMENTO SUCESSO");
                    std::vector<std::string> arteCaldeirao = 
                    { 
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
                    Menu::imprimirLinhasCentralizadasNaTela({equacao, ""}, 0, "\033[33m");
                    Menu::imprimirLinhasCentralizadasNaTela(arteCaldeirao, 46, "\033[0m");
                    
                    std::cout << "\n[Morgana]: Feito! A magia flui pela sua arma...\n";
                    Menu::aguardarPressionamentoDeEnter();
                }
            } while (opcaoEncantar != "0");
        }
        else if (opcaoMorgana == "2" || opcaoMorgana == "3") 
        {
            bool isBuff = (opcaoMorgana == "2");
            std::string titulo = isBuff ? "CABANA - POCOES DE BUFF" : "CABANA - FRASCOS DE DEBUFF";
            std::string opcaoCompra;
            do {
                Menu::limparTelaDoTerminal();
                Menu::exibirLogoDoJogo(titulo);
                std::cout << "\n" << margemMsg << "Seu Ouro: " << jogadorAtual->obterInventario()->obterOuro() << "G\n\n";

                if (isBuff) 
                {
                    std::cout << margemMsg << "[1] Pocao de Furia (Buff x1.5 Atributos | 2 Turnos) - 25G\n";
                    std::cout << margemMsg << "[2] Elixir Arcano (Buff x1.5 Atributos | 2 Turnos)  - 25G\n";
                } 
                else 
                {
                    std::cout << margemMsg << "[1] Frasco de Gosma (Debuff Lentidao | 3 Turnos)    - 30G\n";
                    std::cout << margemMsg << "[2] Frasco de Fraqueza (Debuff Fraqueza | 3 Turnos) - 30G\n";
                }
                std::cout << "\n" << margemMsg << "[0] VOLTAR\n\n" << margemMsg << "Escolha: ";
                std::cin >> opcaoCompra;

                if (opcaoCompra >= "1" && opcaoCompra <= "2") 
                {
                    int preco = (isBuff ? 25 : 30);
                    if (jogadorAtual->obterInventario()->obterOuro() >= preco) 
                    {
                        jogadorAtual->obterInventario()->adicionarOuro(-preco);
                        std::unique_ptr<Item> novoItem = nullptr;
                        if (isBuff) 
                        {
                                if (opcaoCompra == "1") {
                                    novoItem = std::make_unique<ItemConsumivel>("Pocao de Furia (Buff)");
                                    novoItem->adicionarPropriedade(Propriedade::ConsumivelBuff);
                                } else if (opcaoCompra == "2") {
                                    novoItem = std::make_unique<ItemConsumivel>("Elixir Arcano (Buff)");
                                    novoItem->adicionarPropriedade(Propriedade::ConsumivelBuff);
                                }
                        } 
                        else 
                        {
                                if (opcaoCompra == "1") {
                                    novoItem = std::make_unique<ItemConsumivel>("Frasco de Gosma (Debuff)");
                                    novoItem->adicionarPropriedade(Propriedade::ConsumivelDebuffLentidao);
                                }
                                else if (opcaoCompra == "2") {
                                    novoItem = std::make_unique<ItemConsumivel>("Frasco de Fraqueza (Debuff)");
                                    novoItem->adicionarPropriedade(Propriedade::ConsumivelDebuffFraqueza);
                                }
                        }
                        if (novoItem) 
                        {
                            std::string nomeDoNovoItem = novoItem->obterNomeItem();
                            jogadorAtual->obterInventario()->adicionarItem(std::move(novoItem));
                            std::cout << "\n" << margemMsg << "[Morgana]: Heehee... Use com sabedoria! " << nomeDoNovoItem << " adicionado.\n";
                        }
                    } 
                    else 
                    {

                        std::cout << "\n" << margemMsg << "[Morgana]: Voce nao tem ouro suficiente para as minhas preparacoes!\n";
                    }
                    Menu::aguardarPressionamentoDeEnter();
                }
            } while (opcaoCompra != "0");
        }
    } while (opcaoMorgana != "0");
}

std::vector<std::string> NPCMorgana::obterMapaCabana()
{
    return {
        " ######################",
        "##..........;<>......##",
        "##..[^S]..........M..##",
        "##..........;<>......##",
        " ######################"
    };
}
