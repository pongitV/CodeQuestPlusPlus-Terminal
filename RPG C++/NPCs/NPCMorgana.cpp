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
#include "../Gerenciadores/GerenciadorLoja.h"
#include "NPCMorganaLayouts.h"

namespace {
    std::map<int, ProdutoLoja> estoquePocoesBuff = {
        {1, {ItemID::PocaoFuria, 25, -1}},
        {2, {ItemID::ElixirArcano, 25, -1}}
    };

    std::map<int, ProdutoLoja> estoquePocoesDebuff = {
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
                struct EncantoOperacao {
                    ItemID materialId;
                    int qtd;
                    ItemID armaRestritaId; 
                    std::function<bool(EquipamentoArma*)> checarConflito;
                    std::string msgConflito;
                    std::function<std::string(SistemaPersonagem*, EquipamentoArma*)> aplicar;
                };

                std::vector<EncantoOperacao> operacoes = {
                    { ItemID::DenteGoblin, 40, ItemID::Nenhum, 
                      [](EquipamentoArma* a){ return a->possuiEfeitoSangramento(); }, "Esta arma ja esta encantada com Sangramento!",
                      [](SistemaPersonagem*, EquipamentoArma* a){ a->aplicarEfeitoSangramento(); a->alterarNome(a->obterNomeItem() + " (Sangrenta)"); return a->obterNomeItem(); } },
                      
                    { ItemID::NucleoPegajoso, 5, ItemID::Nenhum,
                      [](EquipamentoArma* a){ return a->possuiEfeitoLentidao(); }, "Esta arma ja esta encantada com Lentidao!",
                      [](SistemaPersonagem*, EquipamentoArma* a){ a->aplicarEfeitoLentidao(); a->alterarNome(a->obterNomeItem() + " (Viscosa)"); return a->obterNomeItem(); } },
                      
                    { ItemID::PoMagico, 25, ItemID::Nenhum,
                      [](EquipamentoArma* a){ return a->temPropriedade(Propriedade::Penetrante); }, "Esta arma ja esta encantada com Reducao de Resistencia!",
                      [](SistemaPersonagem*, EquipamentoArma* a){ a->alterarNome(a->obterNomeItem() + " (Quebra-Defesas)"); a->adicionarPropriedade(Propriedade::Penetrante); return a->obterNomeItem(); } },
                      
                    { ItemID::MadeiraEnfeiticada, 1, ItemID::ArcoMadeira,
                      [](EquipamentoArma* a){ return a->temPropriedade(Propriedade::Magica); }, "Esta arma ja esta encantada com Magia!",
                      [](SistemaPersonagem* jogadorAtual, EquipamentoArma* armaEscolhida) {
                          std::string nomeArco = FabricaItens::obterNomeDeID(ItemID::ArcoMadeira);
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
                          return novoArco->obterNomeItem();
                      } },
                      
                    { ItemID::CoracaoFloresta, 1, ItemID::CajadoCristal,
                      [](EquipamentoArma* a){ return a->temPropriedade(Propriedade::CipoPrisao); }, "Esta arma ja esta encantada com Cipos!",
                      [](SistemaPersonagem*, EquipamentoArma* a){
                          std::string nomeCajado = FabricaItens::obterNomeDeID(ItemID::CajadoCristal);
                          std::string nome = a->obterNomeItem();
                          size_t pos = nome.find(nomeCajado);
                          if (pos != std::string::npos) nome.replace(pos, 24, "Cajado de cipos");
                          a->alterarNome(nome);
                          a->adicionarPropriedade(Propriedade::CipoPrisao);
                          return a->obterNomeItem();
                      } },
                      
                    { ItemID::MadeiraEnfeiticada, 1, ItemID::ViolaoEncantado,
                      [](EquipamentoArma* a){ return a->temPropriedade(Propriedade::ViolaoMagico); }, "Esta arma ja esta encantada com Raizes!",
                      [](SistemaPersonagem*, EquipamentoArma* a){
                          std::string nomeViolao = FabricaItens::obterNomeDeID(ItemID::ViolaoEncantado);
                          std::string nome = a->obterNomeItem();
                          size_t pos = nome.find(nomeViolao);
                          if (pos != std::string::npos) nome.replace(pos, 16, "Violao enfeiticado");
                          else nome += " enfeiticado";
                          a->alterarNome(nome);
                          a->adicionarPropriedade(Propriedade::ViolaoMagico);
                          return a->obterNomeItem();
                      } }
                };

                int index = (isUniversal ? 0 : 3) + (opcaoEncantar[0] - '1');
                auto& op = operacoes[index];
                
                std::string itemNecessario = FabricaItens::obterNomeDeID(op.materialId);
                int qtdAtual = jogadorAtual->obterInventario()->contarItem(itemNecessario);
                if (qtdAtual < op.qtd) {
                    dialogoMorgana("Voce nao tem " + itemNecessario + " suficiente! (Possui: " + std::to_string(qtdAtual) + "/" + std::to_string(op.qtd) + ")");
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
                
                if (op.armaRestritaId != ItemID::Nenhum) {
                    std::string nomeRestrito = FabricaItens::obterNomeDeID(op.armaRestritaId);
                    if (armaEscolhida->obterNomeItem().find(nomeRestrito) == std::string::npos) {
                        dialogoMorgana("Este encantamento so funciona no " + nomeRestrito + "!");
                        Aparencia::aguardarEnter();
                        continue;
                    }
                }
                
                if (op.checarConflito(armaEscolhida)) {
                    dialogoMorgana(op.msgConflito);
                    Aparencia::aguardarEnter();
                    continue;
                }
                
                std::string nomeAntigoArma = armaEscolhida->obterNomeItem();
                for (int i = 0; i < op.qtd; ++i) jogadorAtual->obterInventario()->removerItem(itemNecessario);
                
                std::string novoNome = op.aplicar(jogadorAtual, armaEscolhida);
                
                Aparencia::limparTela();
                Aparencia::exibirCabecalho("ENCANTAMENTO SUCESSO", Cor::MAGENTA);
                std::vector<std::string> arteCaldeirao = NPCMorganaLayouts::obterArteCaldeirao();
                
                std::string equacao = "[" + nomeAntigoArma + "] + " + std::to_string(op.qtd) + "x [" + itemNecessario + "] = [" + novoNome + "]";
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
        
        auto formatador = [](ItemID id) {
            if (id == ItemID::PocaoFuria || id == ItemID::ElixirArcano) return std::string(" (Buff x1.5 Atributos | 2 Turnos)");
            if (id == ItemID::FrascoGosma) return std::string(" (Debuff Lentidao | 3 Turnos)");
            if (id == ItemID::FrascoFraqueza) return std::string(" (Debuff Fraqueza | 3 Turnos)");
            return std::string("");
        };

        GerenciadorLoja::processarCompra(jogadorAtual, titulo, Cor::MAGENTA, estoqueAtual, 
            [](const std::string& msg) { dialogoMorgana(msg); }, formatador);
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
