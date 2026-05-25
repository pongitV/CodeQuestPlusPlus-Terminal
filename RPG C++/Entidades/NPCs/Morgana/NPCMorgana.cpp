#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <map>

#include "NPCMorgana.h"
#include "../../../Interface/Telas/Menu/TelaMenu.h"
#include "../../../Sistemas/Inventario/Item.h"
#include "../../../Sistemas/Inventario/FabricaItens.h"
#include "../../../Interface/Telas/Inventario/TelaInventario.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Sistemas/Inventario/Equipamentos/EquipamentoArma.h"
#include "../../../Core/Controladores/Loja.h"
#include "../../../Core/Utilidades/FuncoesDialogo.h"
#include "../../../Sistemas/Progresso/Progressao.h"
#include "../../../Sistemas/Progresso/ProgressaoFlags.h"
#include "../../../Interface/Telas/TelaBase.h"
#include "NPCMorganaLayout.h"

namespace {
    std::map<int, ProdutoLoja> estoquePocoesBuff = {
        {1, {ItemID::PocaoFuria, 25, -1}},
        {2, {ItemID::ElixirArcano, 25, -1}}
    };

    std::map<int, ProdutoLoja> estoquePocoesDebuff = {
        {1, {ItemID::FrascoGosma, 30, -1}},
        {2, {ItemID::FrascoFraqueza, 30, -1}}
    };

    struct EncantoOperacao {
        ItemID materialId;
        int qtd;
        ItemID armaRestritaId; 
        std::function<bool(EquipamentoArma*)> checarConflito;
        std::string msgConflito;
        std::function<std::string(Personagem*, EquipamentoArma*)> aplicar;
    };

    const std::vector<EncantoOperacao> operacoesDeEncantamento = {
        { ItemID::DenteGoblin, 40, ItemID::Nenhum, 
          [](EquipamentoArma* a){ return a->possuiEfeitoSangramento(); }, "Esta arma ja esta encantada com Sangramento!",
          [](Personagem*, EquipamentoArma* a){ a->aplicarEfeitoSangramento(); a->alterarNome(a->obterNomeItem() + " (Sangrenta)"); return a->obterNomeItem(); } },
          
        { ItemID::NucleoPegajoso, 5, ItemID::Nenhum,
          [](EquipamentoArma* a){ return a->possuiEfeitoLentidao(); }, "Esta arma ja esta encantada com Lentidao!",
          [](Personagem*, EquipamentoArma* a){ a->aplicarEfeitoLentidao(); a->alterarNome(a->obterNomeItem() + " (Viscosa)"); return a->obterNomeItem(); } },
          
        { ItemID::PoMagico, 25, ItemID::Nenhum,
          [](EquipamentoArma* a){ return a->temPropriedade(Propriedade::Penetrante); }, "Esta arma ja esta encantada com Reducao de Resistencia!",
          [](Personagem*, EquipamentoArma* a){ a->alterarNome(a->obterNomeItem() + " (Quebra-Defesas)"); a->adicionarPropriedade(Propriedade::Penetrante); return a->obterNomeItem(); } },
          
        { ItemID::MadeiraEnfeiticada, 1, ItemID::ArcoMadeira,
          [](EquipamentoArma* a){ return a->temPropriedade(Propriedade::Magica); }, "Esta arma ja esta encantada com Magia!",
          [](Personagem* jogadorAtual, EquipamentoArma* armaEscolhida) {
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
          [](Personagem*, EquipamentoArma* a){
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
          [](Personagem*, EquipamentoArma* a){
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

    // --- APARENCIA E DIALOGOS ---
    void processarEncantamentos(Personagem* jogadorAtual, bool isUniversal);
    void processarPocoes(Personagem* jogadorAtual, bool isBuff);
    void processarMissaoLabirinto(Personagem* jogadorAtual);
    void processarMenuMissoes(Personagem* jogadorAtual);

    void dialogoMorgana(const std::string& texto, bool novaLinhaAntes = true, bool novaLinhaDepois = true) {
        FuncoesDialogo::imprimirDialogoNPC("Morgana", Cor::MAGENTA, texto, novaLinhaAntes, novaLinhaDepois);
    }

    void dialogoMorgana(const std::vector<std::string>& linhas) {
        FuncoesDialogo::imprimirDialogoNPC("Morgana", Cor::MAGENTA, linhas);
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
    return NPCMorganaLayouts::arteMorgana;
}

// --- INTERACAO E MENU ---
void NPCMorgana::exibirDialogo(Personagem* /*jogador*/) {
    if (Progressao::instancia().obterFlag(Flags::Floresta_MissaoMorgana)) {
        dialogoMorgana(std::vector<std::string>{
            "O Labirinto o aguarda..."
        });
    } else {
        dialogoMorgana(std::vector<std::string>{
            "Hmmm... sinto cheiro de poder no ar.",
            "O que voce busca, viajante?"
        });
    }
}

std::vector<std::string> NPCMorgana::obterOpcoesMenu(Personagem* /*jogador*/, int /*larguraDoTerminal*/) {
    return {
        "ENCANTAR Armas (Universais)",
        "ENCANTAR Armas (Especificas)",
        "COMPRAR Pocoes de Buff",
        "COMPRAR Frascos de Debuff",
        "Missoes de Morgana",
        "VOLTAR"
    };
}

void NPCMorgana::processarOpcao(Personagem* jogador, const std::string& opcao, int /*larguraDoTerminal*/) {
    if (opcao == "ENCANTAR Armas (Universais)") {
        processarEncantamentos(jogador, true);
    }
    else if (opcao == "ENCANTAR Armas (Especificas)") {
        processarEncantamentos(jogador, false);
    }
    else if (opcao == "COMPRAR Pocoes de Buff" || opcao == "COMPRAR Frascos de Debuff") {
        processarPocoes(jogador, opcao == "COMPRAR Pocoes de Buff");
    }
    else if (opcao == "Missoes de Morgana") {
        processarMenuMissoes(jogador);
    }
}

namespace {
    // --- PROCESSAMENTO DE OPCOES ---
    void processarEncantamentos(Personagem* jogadorAtual, bool isUniversal) {
        TelaBase::executarLoopPadrao(
            isUniversal ? "CABANA - ENCANTOS UNIVERSAIS" : "CABANA - ENCANTOS ESPECIFICOS", Cor::MAGENTA,
            nullptr,
            [isUniversal]() {
                std::vector<std::string> linhas;
                if (isUniversal) {
                    linhas.push_back("Sangramento (40x Dente de Goblin)");
                    linhas.push_back("Lentidao (5x Nucleo pegajoso)");
                    linhas.push_back("Quebra de Resistencia (25x Po magico)");
                } else {
                    linhas.push_back("Arco recurvo de madeira: Magia (1x Madeira enfeiticada)");
                    linhas.push_back("Cajado de cristal magico: Cipos (1x Coracao da floresta)");
                    linhas.push_back("Violao encantado: Raizes (1x Madeira enfeiticada)");
                }
                linhas.push_back("VOLTAR");
                return linhas;
            },
            [&](int id) {
                if (id == 3 || id == -1) return false;

                int index = (isUniversal ? 0 : 3) + id;
                const auto& op = operacoesDeEncantamento[index];
                
                std::string itemNecessario = FabricaItens::obterNomeDeID(op.materialId);
                if (!InteracaoNPC::verificarMaterialNoInventario(jogadorAtual, itemNecessario, op.qtd, "Morgana", Cor::MAGENTA)) {
                    return true;
                }
                
                std::string codigoArma;
                Item* itemEscolhido = InteracaoNPC::lerItemDoInventario(jogadorAtual, "Escolha a ARMA para encantar ou [0] VOLTAR: ", "Morgana", Cor::MAGENTA, codigoArma);
                if (codigoArma == "0") return true;
                if (!itemEscolhido) return true;
                
                EquipamentoArma* armaEscolhida = dynamic_cast<EquipamentoArma*>(itemEscolhido);
                if (!armaEscolhida) { dialogoMorgana("Eu so posso encantar ARMAS com isso!"); ControleDeInput::aguardarEnter(); return true; }
                
                if (op.armaRestritaId != ItemID::Nenhum) {
                    std::string nomeRestrito = FabricaItens::obterNomeDeID(op.armaRestritaId);
                    if (armaEscolhida->obterNomeItem().find(nomeRestrito) == std::string::npos) {
                        dialogoMorgana("Este encantamento so funciona no " + nomeRestrito + "!");
                        ControleDeInput::aguardarEnter();
                        return true;
                    }
                }
                
                if (op.checarConflito(armaEscolhida)) {
                    dialogoMorgana(op.msgConflito);
                    ControleDeInput::aguardarEnter();
                    return true;
                }
                
                std::string nomeAntigoArma = armaEscolhida->obterNomeItem();
                for (int i = 0; i < op.qtd; ++i) jogadorAtual->obterInventario()->removerItem(itemNecessario);
                
                std::string novoNome = op.aplicar(jogadorAtual, armaEscolhida);
                
                std::string equacao = "[" + nomeAntigoArma + "] + " + std::to_string(op.qtd) + "x [" + itemNecessario + "] = [" + novoNome + "]";
                InteracaoNPC::exibirTelaDeSucesso("ENCANTAMENTO SUCESSO", Cor::MAGENTA, equacao, NPCMorganaLayouts::arteCaldeirao, "Morgana", "Feito! A magia flui pela sua arma...");
                return true;
            }
        );
    }

    void processarPocoes(Personagem* jogadorAtual, bool isBuff) {
        std::string titulo = isBuff ? "CABANA - POCOES DE BUFF" : "CABANA - FRASCOS DE DEBUFF";
        auto& estoqueAtual = isBuff ? estoquePocoesBuff : estoquePocoesDebuff;
        
        Loja::processarCompra(jogadorAtual, titulo, Cor::MAGENTA, estoqueAtual, 
            [](const std::string& msg) { dialogoMorgana(msg); }, InteracaoNPC::obterFormatadorStatusItem);
    }

    void processarMissaoLabirinto(Personagem* jogadorAtual) {
        std::string nomeCoracao = FabricaItens::obterNomeDeID(ItemID::CoracaoFloresta);
        int qtdCoracoes = jogadorAtual->obterInventario()->contarItem(nomeCoracao);
        std::string msgErro = "Voce ainda nao possui os 3 Coracoes da floresta que eu pedi. (Possui: " + std::to_string(qtdCoracoes) + "/3)\nEles sao dropados por Abominacoes no Coracao da Arvore.";
        
        if (!InteracaoNPC::verificarMaterialNoInventario(jogadorAtual, nomeCoracao, 3, "Morgana", Cor::MAGENTA, msgErro)) {
            return;
        }

        for (int i = 0; i < 3; ++i) jogadorAtual->obterInventario()->removerItem(nomeCoracao);
        jogadorAtual->desbloquearLabirinto();
        Progressao::instancia().definirFlag(Flags::Floresta_MissaoMorgana, true);
        
        Aparencia::limparTela();
        Aparencia::exibirPainelTexto("MISSAO CONCLUIDA", Cor::VERDE);
        dialogoMorgana(std::vector<std::string>{
            "Ah, perfeitos! Estes coracoes pulsam com uma magia ancestral.",
            "Como recompensa, revelarei um segredo... Atras de mim, ha uma passagem secreta.",
            "Use a entrada [^L] para explorar o meu Labirinto Subterraneo.",
            "E um lugar perigoso, mergulhado em uma nevoa de cor roxa, mas guarda grandes tesouros."
        });
        ControleDeInput::aguardarEnter();
    }

    void processarMenuMissoes(Personagem* jogadorAtual) {
        TelaBase::executarLoopPadrao(
            "MISSOES DE MORGANA", Cor::MAGENTA,
            []() {
                std::cout << "\n";
                Aparencia::imprimirCentralizado("Escolha uma missao:");
                std::cout << "\n";
            },
            [jogadorAtual]() {
                std::vector<std::string> missoes;
                if (!jogadorAtual->obterLabirintoDesbloqueado()) {
                    missoes.push_back("[M] Consiga 3x Coracoes da floresta");
                } else {
                    missoes.push_back("(Nenhuma missao disponivel)");
                }
                missoes.push_back("VOLTAR");
                return missoes;
            },
            [&](int id) {
                if (!jogadorAtual->obterLabirintoDesbloqueado() && id == 0) {
                    processarMissaoLabirinto(jogadorAtual);
                } else if (jogadorAtual->obterLabirintoDesbloqueado() && id == 0) {
                    dialogoMorgana("Nao busco mais nada de voce no momento...");
                    ControleDeInput::aguardarEnter();
                } else if (id == 1 || id == -1) { return false; }
                return true;
            }
        );
    }
}
