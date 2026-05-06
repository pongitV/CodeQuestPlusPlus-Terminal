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
#include "../Utilidades/Aparencia.h"
#include "../Inventario/EquipamentoArma.h"

namespace {
    void processarEncantamentos(SistemaPersonagem* jogadorAtual, bool isUniversal);
    void processarPocoes(SistemaPersonagem* jogadorAtual, bool isBuff);
    void processarMissaoLabirinto(SistemaPersonagem* jogadorAtual);
}

void NPCMorgana::interagir(SistemaPersonagem* jogadorAtual)
{
    int larguraDoTerminal = Aparencia::obterLarguraTerminal();
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
        Aparencia::limparTela();
        Aparencia::exibirCabecalho("CABANA DA BRUXA", Cor::VERDE);

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

        int recuo = Aparencia::imprimirLadoALado(menuEsquerda, arteMorgana, 45, 0, Cor::RESET, Cor::MAGENTA);
        std::cout << "\n" << std::string(recuo, ' ') << "Escolha: ";
        std::cin >> opcaoMorgana;

        if (opcaoMorgana == "1") {
            processarEncantamentos(jogadorAtual, true);
        }
        else if (opcaoMorgana == "2") {
            processarEncantamentos(jogadorAtual, false);
        }
        else if (opcaoMorgana == "3" || opcaoMorgana == "4") {
            processarPocoes(jogadorAtual, opcaoMorgana == "3");
        }
        else if (opcaoMorgana == "5" && !jogadorAtual->obterLabirintoDesbloqueado()) {
            processarMissaoLabirinto(jogadorAtual);
        }
    } while (opcaoMorgana != "0");
}

namespace {
    void processarEncantamentos(SistemaPersonagem* jogadorAtual, bool isUniversal) {
        std::string opcaoEncantar;
        do {
            Aparencia::limparTela();
            Aparencia::exibirCabecalho(isUniversal ? "CABANA - ENCANTOS UNIVERSAIS" : "CABANA - ENCANTOS ESPECIFICOS", Cor::VERDE);
            
            std::vector<std::string> linhas = { "Escolha um encantamento:", "" };
            if (isUniversal) {
                linhas.push_back("[1] Sangramento (40x Dente de Goblin)");
                linhas.push_back("[2] Lentidao (5x Nucleo pegajoso)");
                linhas.push_back("[3] Quebra de Resistencia (25x Po magico)");
            } else {
                linhas.push_back("[1] Arco recurvo de madeira: Magia (1x Madeira enfeiticada)");
                linhas.push_back("[2] Cajado de cristal magico: Cipos (1x Coracao da floresta)");
                linhas.push_back("[3] Violao encantado: Raizes (1x Madeira enfeiticada)");
            }
            linhas.push_back("");
            linhas.push_back("[0] VOLTAR");
            
            std::cout << "\n";
            Aparencia::imprimirBlocoCentralizado(linhas);
            std::cout << "\n";
            Aparencia::exibirPrompt("Escolha: ");
            std::cin >> opcaoEncantar;

            if (opcaoEncantar == "1" || opcaoEncantar == "2" || opcaoEncantar == "3") {
                struct EncantoInfo { ItemID idItem; int qtd; };
                EncantoInfo encantos[] = {
                    {ItemID::DenteGoblin, 40}, {ItemID::NucleoPegajoso, 5}, {ItemID::PoMagico, 25},
                    {ItemID::MadeiraEnfeiticada, 1}, {ItemID::CoracaoFloresta, 1}, {ItemID::MadeiraEnfeiticada, 1}
                };
                
                int index = (isUniversal ? 0 : 3) + (opcaoEncantar[0] - '1');
                bool isSangramento = (index == 0), isLentidao = (index == 1), isResistencia = (index == 2);
                bool isMagia = (index == 3), isCipos = (index == 4), isRaizes = (index == 5);

                auto [itemIdNecessario, qtdNecessaria] = encantos[index];
                std::string itemNecessario = FabricaItens::obterNomeDeID(itemIdNecessario);
                
                int qtdAtual = jogadorAtual->obterInventario()->contarItem(itemNecessario);
                if (qtdAtual < qtdNecessaria) {
                    std::cout << "\n[Morgana]: Voce nao tem " << itemNecessario << " suficiente! (Possui: " << qtdAtual << "/" << qtdNecessaria << ")\n";
                    Aparencia::aguardarEnter();
                    continue;
                }
                
                std::string codigoArma;
                TelaInventario::exibir(jogadorAtual);
            Aparencia::exibirPrompt("[Morgana]: Escolha a ARMA para encantar ou [0] VOLTAR: ");
                std::cin >> codigoArma;
                if (codigoArma == "0") continue;
                
                Item* itemEscolhido = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigoArma, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
                if (!itemEscolhido) { std::cout << "\n[SISTEMA]: Item invalido!\n"; Aparencia::aguardarEnter(); continue; }
                
                EquipamentoArma* armaEscolhida = dynamic_cast<EquipamentoArma*>(itemEscolhido);
                if (!armaEscolhida) { std::cout << "\n[Morgana]: Eu so posso encantar ARMAS com isso!\n"; Aparencia::aguardarEnter(); continue; }
                
                std::string nomeArco = FabricaItens::obterNomeDeID(ItemID::ArcoMadeira);
                std::string nomeCajado = FabricaItens::obterNomeDeID(ItemID::CajadoCristal);
                std::string nomeViolao = FabricaItens::obterNomeDeID(ItemID::ViolaoEncantado);

                if (isMagia && armaEscolhida->obterNomeItem().find(nomeArco) == std::string::npos) { std::cout << "\n[Morgana]: Este encantamento so funciona no Arco recurvo de madeira!\n"; Aparencia::aguardarEnter(); continue; }
                if (isCipos && armaEscolhida->obterNomeItem().find(nomeCajado) == std::string::npos) { std::cout << "\n[Morgana]: Este encantamento so funciona no Cajado de cristal magico!\n"; Aparencia::aguardarEnter(); continue; }
                if (isRaizes && armaEscolhida->obterNomeItem().find(nomeViolao) == std::string::npos) { std::cout << "\n[Morgana]: Este encantamento so funciona no Violao!\n"; Aparencia::aguardarEnter(); continue; }
                
                if (isSangramento && armaEscolhida->possuiEfeitoSangramento()) { std::cout << "\n[Morgana]: Esta arma ja esta encantada com Sangramento!\n"; Aparencia::aguardarEnter(); continue; }
                if (isLentidao && armaEscolhida->possuiEfeitoLentidao()) { std::cout << "\n[Morgana]: Esta arma ja esta encantada com Lentidao!\n"; Aparencia::aguardarEnter(); continue; }
                if (isResistencia && armaEscolhida->temPropriedade(Propriedade::Penetrante)) { std::cout << "\n[Morgana]: Esta arma ja esta encantada com Reducao de Resistencia!\n"; Aparencia::aguardarEnter(); continue; }
                if (isMagia && armaEscolhida->temPropriedade(Propriedade::Magica)) { std::cout << "\n[Morgana]: Esta arma ja esta encantada com Magia!\n"; Aparencia::aguardarEnter(); continue; }
                if (isCipos && armaEscolhida->temPropriedade(Propriedade::CipoPrisao)) { std::cout << "\n[Morgana]: Esta arma ja esta encantada com Cipos!\n"; Aparencia::aguardarEnter(); continue; }
                if (isRaizes && armaEscolhida->temPropriedade(Propriedade::ViolaoMagico)) { std::cout << "\n[Morgana]: Esta arma ja esta encantada com Raizes!\n"; Aparencia::aguardarEnter(); continue; }
                
                std::string nomeAntigoArma = armaEscolhida->obterNomeItem();

                for (int i = 0; i < qtdNecessaria; ++i) jogadorAtual->obterInventario()->removerItem(itemNecessario);
                
                if (isSangramento) { armaEscolhida->aplicarEfeitoSangramento(); armaEscolhida->alterarNome(armaEscolhida->obterNomeItem() + " (Sangrenta)"); }
                else if (isLentidao) { armaEscolhida->aplicarEfeitoLentidao(); armaEscolhida->alterarNome(armaEscolhida->obterNomeItem() + " (Viscosa)"); }
                else if (isResistencia) { armaEscolhida->alterarNome(armaEscolhida->obterNomeItem() + " (Quebra-Defesas)"); armaEscolhida->adicionarPropriedade(Propriedade::Penetrante); }
                else if (isMagia) {
                    std::string nome = armaEscolhida->obterNomeItem();
                    size_t pos = nome.find(nomeArco);
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
                    jogadorAtual->obterInventario()->removerItem(armaEscolhida);
                    jogadorAtual->obterInventario()->adicionarItem(std::move(novoArcoObj));
                    if (estavaEquipado) jogadorAtual->equiparItem(novoArco);
                    armaEscolhida = novoArco;
                }
                else if (isCipos) {
                    std::string nome = armaEscolhida->obterNomeItem();
                    size_t pos = nome.find(nomeCajado);
                    if (pos != std::string::npos) nome.replace(pos, 24, "Cajado de cipos");
                    armaEscolhida->alterarNome(nome);
                    armaEscolhida->adicionarPropriedade(Propriedade::CipoPrisao);
                }
                else if (isRaizes) {
                    std::string nome = armaEscolhida->obterNomeItem();
                    size_t pos = nome.find(nomeViolao);
                    if (pos != std::string::npos) nome.replace(pos, 16, "Violao enfeiticado");
                    else nome += " enfeiticado";
                    armaEscolhida->alterarNome(nome);
                    armaEscolhida->adicionarPropriedade(Propriedade::ViolaoMagico);
                }
                
                Aparencia::limparTela();
                Aparencia::exibirCabecalho("ENCANTAMENTO SUCESSO", Cor::VERDE);
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
                Aparencia::imprimirCentralizadoMultilinha({equacao, ""}, 0, Aparencia::cor(Cor::AMARELO));
                Aparencia::imprimirCentralizadoMultilinha(arteCaldeirao, 46, Aparencia::cor(Cor::RESET));
                
                std::cout << "\n[Morgana]: Feito! A magia flui pela sua arma...\n";
                Aparencia::aguardarEnter();
            }
        } while (opcaoEncantar != "0");
    }

    void processarPocoes(SistemaPersonagem* jogadorAtual, bool isBuff) {
        std::string titulo = isBuff ? "CABANA - POCOES DE BUFF" : "CABANA - FRASCOS DE DEBUFF";
        std::string opcaoCompra;
        do {
            Aparencia::limparTela();
            Aparencia::exibirCabecalho(titulo, Cor::VERDE);

            std::vector<std::string> linhas = { "Seu Ouro: " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G", "" };

            if (isBuff) {
                linhas.push_back("[1] Pocao de Furia (Buff x1.5 Atributos | 2 Turnos) - 25G");
                linhas.push_back("[2] Elixir Arcano (Buff x1.5 Atributos | 2 Turnos)  - 25G");
            } else {
                linhas.push_back("[1] Frasco de Gosma (Debuff Lentidao | 3 Turnos)    - 30G");
                linhas.push_back("[2] Frasco de Fraqueza (Debuff Fraqueza | 3 Turnos) - 30G");
            }
            linhas.push_back("");
            linhas.push_back("[0] VOLTAR");

            std::cout << "\n";
            Aparencia::imprimirBlocoCentralizado(linhas);
            std::cout << "\n";
            Aparencia::exibirPrompt("Escolha: ");
            std::cin >> opcaoCompra;

            if (opcaoCompra >= "1" && opcaoCompra <= "2") {
                int preco = (isBuff ? 25 : 30);
                if (jogadorAtual->obterInventario()->obterOuro() >= preco) {
                    jogadorAtual->obterInventario()->adicionarOuro(-preco);
                    ItemID idPot = ItemID::Nenhum;
                    if (isBuff) {
                        if (opcaoCompra == "1") idPot = ItemID::PocaoFuria;
                        else if (opcaoCompra == "2") idPot = ItemID::ElixirArcano;
                    } else {
                        if (opcaoCompra == "1") idPot = ItemID::FrascoGosma;
                        else if (opcaoCompra == "2") idPot = ItemID::FrascoFraqueza;
                    }
                    auto novoItem = FabricaItens::criarItem(idPot);
                    if (novoItem) {
                        std::string nomeDoNovoItem = novoItem->obterNomeItem();
                        jogadorAtual->obterInventario()->adicionarItem(std::move(novoItem));
                        std::cout << "\n[Morgana]: Heehee... Use com sabedoria! " << nomeDoNovoItem << " adicionado.\n";
                    }
                } else {
                    std::cout << "\n[Morgana]: Voce nao tem ouro suficiente para as minhas preparacoes!\n";
                }
                Aparencia::aguardarEnter();
            }
        } while (opcaoCompra != "0");
    }

    void processarMissaoLabirinto(SistemaPersonagem* jogadorAtual) {
        std::string nomeCoracao = FabricaItens::obterNomeDeID(ItemID::CoracaoFloresta);
        int qtdCoracoes = jogadorAtual->obterInventario()->contarItem(nomeCoracao);
        if (qtdCoracoes >= 3) {
            for (int i = 0; i < 3; ++i) jogadorAtual->obterInventario()->removerItem(nomeCoracao);
            jogadorAtual->desbloquearLabirinto();
            
            Aparencia::limparTela();
            Aparencia::exibirCabecalho("MISSAO CONCLUIDA", Cor::VERDE);
            std::vector<std::string> texto = {
                "[Morgana]: Ah, perfeitos! Estes coracoes pulsam com uma magia ancestral.",
                "[Morgana]: Como recompensa, revelarei um segredo... Atras de mim, ha uma passagem secreta.",
                "[Morgana]: Use a entrada [^L] para explorar o meu Labirinto Subterraneo.",
                "[Morgana]: E um lugar perigoso, mergulhado em uma nevoa de cor roxa, mas guarda grandes tesouros."
            };
            std::cout << "\n";
            Aparencia::imprimirBlocoCentralizado(texto);
        }
        else {
            Aparencia::limparTela();
            Aparencia::exibirCabecalho("MISSAO", Cor::VERDE);
            std::vector<std::string> texto = {
                "[Morgana]: Voce ainda nao possui os 3 Coracoes da floresta que eu pedi. (Possui: " + std::to_string(qtdCoracoes) + "/3)",
                "[Morgana]: Eles sao dropados por Abominacoes no Coracao da Arvore."
            };
            std::cout << "\n";
            Aparencia::imprimirBlocoCentralizado(texto);
        }
        Aparencia::aguardarEnter();
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
