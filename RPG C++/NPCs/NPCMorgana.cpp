#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <map>

#include "NPCMorgana.h"
#include "../Telas/TelaMenu.h"
#include "../Inventario/Item.h"
#include "../Inventario/FabricaItens.h"
#include "../Telas/TelaInventario.h"
#include "../Utilidades/Aparencia.h"
#include "../Utilidades/ControleDeInput.h"
#include "../Inventario/EquipamentoArma.h"
#include "NPCMorganaLayouts.h"

namespace {
     struct Produto {
        ItemID idItem;
        int preco;
        int quantidade; // -1 para infinito
    };

    std::map<int, Produto> estoquePocoesBuff = {
        {1, {ItemID::PocaoFuria, 25, -1}},
        {2, {ItemID::ElixirArcano, 25, -1}}
    };

    std::map<int, Produto> estoquePocoesDebuff = {
        {1, {ItemID::FrascoGosma, 30, -1}},
        {2, {ItemID::FrascoFraqueza, 30, -1}}
    };

    // --- APARENCIA E DIALOGOS ---
    void processarEncantamentos(SistemaPersonagem* jogadorAtual, bool isUniversal);
    void processarPocoes(SistemaPersonagem* jogadorAtual, bool isBuff);
    void processarMissaoLabirinto(SistemaPersonagem* jogadorAtual);

    void dialogoMorgana(const std::string& texto, bool novaLinhaAntes = true, bool novaLinhaDepois = true) {
        Aparencia::imprimirDialogoNPC("Morgana", Cor::MAGENTA, texto, novaLinhaAntes, novaLinhaDepois);
    }

    void dialogoMorgana(const std::vector<std::string>& linhas) {
        Aparencia::imprimirDialogoNPC("Morgana", Cor::MAGENTA, linhas);
    }
}

// --- INFORMACOES DO LUGAR ---
std::string NPCMorgana::obterNomeDoLugar() const {
    return "CABANA DA BRUXA";
}

Cor NPCMorgana::obterCorDoCabecalho() const {
    return Cor::MAGENTA;
}

Cor NPCMorgana::obterCorDaArte() const {
    return Cor::MAGENTA;
}

const std::vector<std::string>& NPCMorgana::obterArteASCII() const {
    static std::vector<std::string> arte = NPCMorganaLayouts::obterArteMorgana();
    return arte;
}

// --- INTERACAO E MENU ---
void NPCMorgana::exibirDialogo(SistemaPersonagem* jogador) {
    dialogoMorgana(std::vector<std::string>{
        "Hmmm... sinto cheiro de poder no ar.",
        "O que voce busca, viajante?"
    });
}

std::vector<std::string> NPCMorgana::obterOpcoesMenu(SistemaPersonagem* jogador, int larguraDoTerminal) {
    std::vector<std::string> menuEsquerda = 
    {
        "[1] ENCANTAR Armas (Universais)",
        "[2] ENCANTAR Armas (Especificas)",
        "[3] COMPRAR Pocoes de Buff",
        "[4] COMPRAR Frascos de Debuff"
    };
    
    if (!jogador->obterLabirintoDesbloqueado()) {
        menuEsquerda.push_back("[5] [M] Consiga 3x Coracoes da floresta");
    }
    
    menuEsquerda.push_back("[0] VOLTAR");
    menuEsquerda.push_back("");
    return menuEsquerda;
}

void NPCMorgana::processarOpcao(SistemaPersonagem* jogador, const std::string& opcao, int larguraDoTerminal) {
    if (opcao == "1") {
        processarEncantamentos(jogador, true);
    }
    else if (opcao == "2") {
        processarEncantamentos(jogador, false);
    }
    else if (opcao == "3" || opcao == "4") {
        processarPocoes(jogador, opcao == "3");
    }
    else if (opcao == "5" && !jogador->obterLabirintoDesbloqueado()) {
        processarMissaoLabirinto(jogador);
    }
}

namespace {
    // --- PROCESSAMENTO DE OPCOES ---
    void processarEncantamentos(SistemaPersonagem* jogadorAtual, bool isUniversal) {
        std::string opcaoEncantar;
        do {
            Aparencia::limparTela();
            Aparencia::exibirCabecalho(isUniversal ? "CABANA - ENCANTOS UNIVERSAIS" : "CABANA - ENCANTOS ESPECIFICOS", Cor::MAGENTA);
            
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
            int id = ControleDeInput::lerInteiroComLimites("Escolha: ", 0, 3, true);
            opcaoEncantar = std::to_string(id);

            if (opcaoEncantar != "0") {
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
                    dialogoMorgana("Voce nao tem " + itemNecessario + " suficiente! (Possui: " + std::to_string(qtdAtual) + "/" + std::to_string(qtdNecessaria) + ")");
                    Aparencia::aguardarEnter();
                    continue;
                }
                
                std::string codigoArma;
                TelaInventario::exibir(jogadorAtual);
                dialogoMorgana("Escolha a ARMA para encantar ou [0] VOLTAR: ", false, false);
                std::cout << "\033[s";
                
                Item* itemEscolhido = nullptr;
                while (true) {
                    codigoArma = ControleDeInput::lerEntradaProtegida();
                    if (codigoArma == "0") break;
                    itemEscolhido = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigoArma, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
                    if (itemEscolhido) break;
                    std::cout << "\033[u\033[J";
                }
                if (codigoArma == "0") continue;
                
                EquipamentoArma* armaEscolhida = dynamic_cast<EquipamentoArma*>(itemEscolhido);
                if (!armaEscolhida) { dialogoMorgana("Eu so posso encantar ARMAS com isso!"); Aparencia::aguardarEnter(); continue; }
                
                std::string nomeArco = FabricaItens::obterNomeDeID(ItemID::ArcoMadeira);
                std::string nomeCajado = FabricaItens::obterNomeDeID(ItemID::CajadoCristal);
                std::string nomeViolao = FabricaItens::obterNomeDeID(ItemID::ViolaoEncantado);

                if (isMagia && armaEscolhida->obterNomeItem().find(nomeArco) == std::string::npos) { dialogoMorgana("Este encantamento so funciona no Arco recurvo de madeira!"); Aparencia::aguardarEnter(); continue; }
                if (isCipos && armaEscolhida->obterNomeItem().find(nomeCajado) == std::string::npos) { dialogoMorgana("Este encantamento so funciona no Cajado de cristal magico!"); Aparencia::aguardarEnter(); continue; }
                if (isRaizes && armaEscolhida->obterNomeItem().find(nomeViolao) == std::string::npos) { dialogoMorgana("Este encantamento so funciona no Violao!"); Aparencia::aguardarEnter(); continue; }
                
                if (isSangramento && armaEscolhida->possuiEfeitoSangramento()) { dialogoMorgana("Esta arma ja esta encantada com Sangramento!"); Aparencia::aguardarEnter(); continue; }
                if (isLentidao && armaEscolhida->possuiEfeitoLentidao()) { dialogoMorgana("Esta arma ja esta encantada com Lentidao!"); Aparencia::aguardarEnter(); continue; }
                if (isResistencia && armaEscolhida->temPropriedade(Propriedade::Penetrante)) { dialogoMorgana("Esta arma ja esta encantada com Reducao de Resistencia!"); Aparencia::aguardarEnter(); continue; }
                if (isMagia && armaEscolhida->temPropriedade(Propriedade::Magica)) { dialogoMorgana("Esta arma ja esta encantada com Magia!"); Aparencia::aguardarEnter(); continue; }
                if (isCipos && armaEscolhida->temPropriedade(Propriedade::CipoPrisao)) { dialogoMorgana("Esta arma ja esta encantada com Cipos!"); Aparencia::aguardarEnter(); continue; }
                if (isRaizes && armaEscolhida->temPropriedade(Propriedade::ViolaoMagico)) { dialogoMorgana("Esta arma ja esta encantada com Raizes!"); Aparencia::aguardarEnter(); continue; }
                
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
                Aparencia::exibirCabecalho("ENCANTAMENTO SUCESSO", Cor::MAGENTA);
                std::vector<std::string> arteCaldeirao = NPCMorganaLayouts::obterArteCaldeirao();
                
                std::string equacao = "[" + nomeAntigoArma + "] + " + std::to_string(qtdNecessaria) + "x [" + itemNecessario + "] = [" + armaEscolhida->obterNomeItem() + "]";
                Aparencia::imprimirCentralizadoMultilinha({equacao, ""}, 0, Aparencia::cor(Cor::MAGENTA));
                Aparencia::imprimirCentralizadoMultilinha(arteCaldeirao, 29, Aparencia::cor(Cor::MAGENTA));
                
                dialogoMorgana("Feito! A magia flui pela sua arma...");
                Aparencia::aguardarEnter();
            }
        } while (opcaoEncantar != "0");
    }

    void processarPocoes(SistemaPersonagem* jogadorAtual, bool isBuff) {
        std::string titulo = isBuff ? "CABANA - POCOES DE BUFF" : "CABANA - FRASCOS DE DEBUFF";
        auto& estoqueAtual = isBuff ? estoquePocoesBuff : estoquePocoesDebuff;
        std::string opcaoCompra;
        do {
            Aparencia::limparTela();
            Aparencia::exibirCabecalho(titulo, Cor::MAGENTA);

            std::vector<std::string> linhas = { "Seu Ouro: " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G", "" };

            int maxId = 0;
            for (auto const& [id, produto] : estoqueAtual) {
                if (id > maxId) maxId = id;
                std::string nomeItem = FabricaItens::obterNomeDeID(produto.idItem);
                if (produto.idItem == ItemID::PocaoFuria || produto.idItem == ItemID::ElixirArcano) nomeItem += " (Buff x1.5 Atributos | 2 Turnos)";
                else if (produto.idItem == ItemID::FrascoGosma) nomeItem += " (Debuff Lentidao | 3 Turnos)";
                else if (produto.idItem == ItemID::FrascoFraqueza) nomeItem += " (Debuff Fraqueza | 3 Turnos)";
                
                std::string preco = std::to_string(produto.preco) + "G";
                std::string estoqueInfo = (produto.quantidade == -1) ? "" : (produto.quantidade == 0 ? " (Esgotado)" : " (Estoque: " + std::to_string(produto.quantidade) + ")");
                
                linhas.push_back("[" + std::to_string(id) + "] " + nomeItem + " - " + preco + estoqueInfo);
            }
            linhas.push_back("");
            linhas.push_back("[0] VOLTAR");

            std::cout << "\n";
            Aparencia::imprimirBlocoCentralizado(linhas);
            std::cout << "\n";
            int idCompra = ControleDeInput::lerInteiroComLimites("Escolha: ", 0, maxId, true);
            opcaoCompra = std::to_string(idCompra);

            if (opcaoCompra != "0" && estoqueAtual.find(idCompra) != estoqueAtual.end()) {
                auto& produto = estoqueAtual[idCompra];
                if (produto.quantidade == 0) {
                    dialogoMorgana("Acabou, volte mais tarde!");
                } else {
                    int maxComprador = jogadorAtual->obterInventario()->obterOuro() / produto.preco;
                    if (maxComprador == 0) {
                        dialogoMorgana("Voce nao tem ouro suficiente para as minhas preparacoes!");
                    } else {
                        int maxPossivel = (produto.quantidade == -1) ? maxComprador : std::min(maxComprador, produto.quantidade);
                        int qtdComprar = 1;
                        if (maxPossivel > 1) {
                            std::string msgQtd = "Quantidade para comprar (1 a " + std::to_string(maxPossivel) + ", ou 0 para cancelar): ";
                            qtdComprar = ControleDeInput::lerInteiroComLimites(msgQtd, 0, maxPossivel, true);
                        }
                        
                        if (qtdComprar > 0) {
                            jogadorAtual->obterInventario()->adicionarOuro(-(produto.preco * qtdComprar));
                            if (produto.quantidade != -1) produto.quantidade -= qtdComprar;
                            
                            std::string nomeNovo = FabricaItens::obterNomeDeID(produto.idItem);
                            for (int i = 0; i < qtdComprar; ++i) {
                                jogadorAtual->obterInventario()->adicionarItem(FabricaItens::criarItem(produto.idItem));
                            }
                            dialogoMorgana("Heehee... Use com sabedoria! " + std::to_string(qtdComprar) + "x " + nomeNovo + " adicionado.");
                        }
                    }
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
            dialogoMorgana(std::vector<std::string>{
                "Ah, perfeitos! Estes coracoes pulsam com uma magia ancestral.",
                "Como recompensa, revelarei um segredo... Atras de mim, ha uma passagem secreta.",
                "Use a entrada [^L] para explorar o meu Labirinto Subterraneo.",
                "E um lugar perigoso, mergulhado em uma nevoa de cor roxa, mas guarda grandes tesouros."
            });
        }
        else {
            Aparencia::limparTela();
            Aparencia::exibirCabecalho("MISSAO", Cor::VERDE);
            dialogoMorgana(std::vector<std::string>{
                "Voce ainda nao possui os 3 Coracoes da floresta que eu pedi. (Possui: " + std::to_string(qtdCoracoes) + "/3)",
                "Eles sao dropados por Abominacoes no Coracao da Arvore."
            });
        }
        Aparencia::aguardarEnter();
    }
}
