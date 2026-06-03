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
        std::string nomeMenu;
        ItemID materialId;
        int qtd;
        ItemID armaRestritaId; 
        std::function<bool(EquipamentoArma*)> checarConflito;
        std::string msgConflito;
        std::function<std::string(Personagem*, EquipamentoArma*)> aplicar;
    };

    const std::vector<EncantoOperacao> operacoesDeEncantamento = {
        { "Sangramento (40x Dente de Goblin)", ItemID::DenteGoblin, 40, ItemID::Nenhum, 
          [](EquipamentoArma* a){ return a->possuiEfeitoSangramento(); }, "Esta arma ja esta encantada com Sangramento!",
          [](Personagem*, EquipamentoArma* a){ a->aplicarEfeitoSangramento(); a->alterarNome(a->obterNomeItem() + " (Sangrenta)"); return a->obterNomeItem(); } },
          
        { "Lentidao (5x Nucleo pegajoso)", ItemID::NucleoPegajoso, 5, ItemID::Nenhum,
          [](EquipamentoArma* a){ return a->possuiEfeitoLentidao(); }, "Esta arma ja esta encantada com Lentidao!",
          [](Personagem*, EquipamentoArma* a){ a->aplicarEfeitoLentidao(); a->alterarNome(a->obterNomeItem() + " (Viscosa)"); return a->obterNomeItem(); } },
          
        { "Quebra de Resistencia (25x Po magico)", ItemID::PoMagico, 25, ItemID::Nenhum,
          [](EquipamentoArma* a){ return a->temPropriedade(Propriedade::Penetrante); }, "Esta arma ja esta encantada com Reducao de Resistencia!",
          [](Personagem*, EquipamentoArma* a){ a->alterarNome(a->obterNomeItem() + " (Quebra-Defesas)"); a->adicionarPropriedade(Propriedade::Penetrante); return a->obterNomeItem(); } },
          
        { "Arco recurvo de madeira: Magia (1x Madeira enfeiticada)", ItemID::MadeiraEnfeiticada, 1, ItemID::ArcoMadeira,
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
          
        { "Cajado de cristal magico: Cipos (1x Coracao da floresta)", ItemID::CoracaoFloresta, 1, ItemID::CajadoCristal,
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
          
        { "Violao encantado: Raizes (1x Madeira enfeiticada)", ItemID::MadeiraEnfeiticada, 1, ItemID::ViolaoEncantado,
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

    void dialogoMorgana(const std::vector<std::string>& linhas) {
        Aparencia::exibirPopup("MORGANA", linhas, Cor::MAGENTA, NPCMorganaLayouts::arteMorgana);
    }
    
    void dialogoMorganaUnico(const std::string& msg) {
        dialogoMorgana({msg});
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
void NPCMorgana::interagir(Personagem* jogador) {
    ControleDeInput::executarLoopMenuPopup(
        [this, jogador]() { this->exibirDialogo(jogador); },
        [this, jogador]() { return this->obterOpcoesMenu(jogador, Aparencia::obterLarguraTerminal()); },
        [this, jogador](const std::string& op) { this->processarOpcao(jogador, op, Aparencia::obterLarguraTerminal()); return true; },
        obterNomeDoLugar(), obterCorDoCabecalho(), obterArteASCII()
    );
}

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
        std::vector<const EncantoOperacao*> opsAtuais;
        int inicio = isUniversal ? 0 : 3;
        int fim = isUniversal ? 3 : 6;
        for (int i = inicio; i < fim; ++i) {
            opsAtuais.push_back(&operacoesDeEncantamento[i]);
        }

        Aparencia::ordenarAlfabeticamente(opsAtuais, [](const EncantoOperacao* op) { return op->nomeMenu; });

        while (true) {
            std::vector<std::string> linhas;
            for (auto* op : opsAtuais) linhas.push_back(op->nomeMenu);
            linhas.push_back("VOLTAR");

            int id = ControleDeInput::lerSelecaoMenuEmPopup(
                isUniversal ? "CABANA - ENCANTOS UNIVERSAIS" : "CABANA - ENCANTOS ESPECIFICOS",
                {"Escolha um encantamento:"},
                linhas,
                Cor::MAGENTA,
                NPCMorganaLayouts::arteMorgana
            );

            if (id == static_cast<int>(opsAtuais.size()) || id == -1) {
                break;
            }

            const auto& op = *opsAtuais[id];
            
            std::string itemNecessario = FabricaItens::obterNomeDeID(op.materialId);
            int qtdAtual = jogadorAtual->obterInventario()->contarItem(itemNecessario);
            if (qtdAtual < op.qtd) {
                dialogoMorganaUnico("Voce nao tem " + itemNecessario + " suficiente! (Possui: " + std::to_string(qtdAtual) + "/" + std::to_string(op.qtd) + ")");
                continue;
            }
            
            std::vector<Item*> itensValidos;
            std::vector<std::string> opcoesItem;
            for (auto* item : jogadorAtual->obterInventario()->obterTodosOsItens()) {
                if (item->obterTipo() == TipoEquipamento::ARMA) {
                    itensValidos.push_back(item);
                    opcoesItem.push_back(item->obterNomeItem());
                }
            }
            if (opcoesItem.empty()) { dialogoMorganaUnico("Voce nao tem armas para encantar!"); continue; }
            opcoesItem.push_back("VOLTAR");
            
            int escolhaArma = ControleDeInput::lerSelecaoMenuEmPopup("ESCOLHA UMA ARMA", {"Qual arma deseja encantar?"}, opcoesItem, Cor::MAGENTA, NPCMorganaLayouts::arteCaldeirao);
            if (escolhaArma == -1 || escolhaArma == static_cast<int>(opcoesItem.size()) - 1) continue;
            
            EquipamentoArma* armaEscolhida = dynamic_cast<EquipamentoArma*>(itensValidos[escolhaArma]);
            
            if (op.armaRestritaId != ItemID::Nenhum) {
                std::string nomeRestrito = FabricaItens::obterNomeDeID(op.armaRestritaId);
                if (armaEscolhida->obterNomeItem().find(nomeRestrito) == std::string::npos) {
                    dialogoMorganaUnico("Este encantamento so funciona no " + nomeRestrito + "!");
                    continue;
                }
            }
            
            if (op.checarConflito(armaEscolhida)) {
                dialogoMorganaUnico(op.msgConflito);
                continue;
            }
            
            std::string nomeAntigoArma = armaEscolhida->obterNomeItem();
            for (int i = 0; i < op.qtd; ++i) jogadorAtual->obterInventario()->removerItem(itemNecessario);
            
            std::string novoNome = op.aplicar(jogadorAtual, armaEscolhida);
            
            std::string equacao = "[" + nomeAntigoArma + "] + " + std::to_string(op.qtd) + "x [" + itemNecessario + "] = [" + novoNome + "]";
            Aparencia::exibirPopup("ENCANTAMENTO SUCESSO", {equacao, "", "Feito! A magia flui pela sua arma..."}, Cor::MAGENTA, NPCMorganaLayouts::arteCaldeirao);
        }
    }

    void processarPocoes(Personagem* jogadorAtual, bool isBuff) {
        std::string titulo = isBuff ? "CABANA - POCOES DE BUFF" : "CABANA - FRASCOS DE DEBUFF";
        auto& estoqueAtual = isBuff ? estoquePocoesBuff : estoquePocoesDebuff;
        
        Loja::processarCompra(jogadorAtual, titulo, Cor::MAGENTA, estoqueAtual, 
            [](const std::string& msg) { dialogoMorganaUnico(msg); }, InteracaoNPC::obterFormatadorStatusItem);
    }

    void processarMissaoLabirinto(Personagem* jogadorAtual) {
        std::string nomeCoracao = FabricaItens::obterNomeDeID(ItemID::CoracaoFloresta);
        int qtdCoracoes = jogadorAtual->obterInventario()->contarItem(nomeCoracao);

        if (qtdCoracoes < 3) {
            dialogoMorgana({
                "Voce ainda nao possui os 3 Coracoes da floresta que eu pedi. (Possui: " + std::to_string(qtdCoracoes) + "/3)",
                "Eles sao dropados por Abominacoes no Coracao da Arvore."
            });
            return;
        }

        for (int i = 0; i < 3; ++i) jogadorAtual->obterInventario()->removerItem(nomeCoracao);
        jogadorAtual->desbloquearLabirinto();
        Progressao::instancia().definirFlag(Flags::Floresta_MissaoMorgana, true);
        
        std::vector<std::string> dialogo = {
            "Ah, perfeitos! Estes coracoes pulsam com uma magia ancestral.",
            "Como recompensa, revelarei um segredo... Atras de mim, ha uma passagem secreta.",
            "Use a entrada [^L] para explorar o meu Labirinto Subterraneo.",
            "E um lugar perigoso, mergulhado em uma nevoa de cor roxa, mas guarda grandes tesouros."
        };
        Aparencia::exibirPopup("MISSAO CONCLUIDA", dialogo, Cor::MAGENTA, NPCMorganaLayouts::arteMorgana);
    }

    void processarMenuMissoes(Personagem* jogadorAtual) {
        while (true) {
            std::vector<std::string> missoes;
            if (!jogadorAtual->obterLabirintoDesbloqueado()) {
                missoes.push_back("[M] Consiga 3x Coracoes da floresta");
            } else {
                missoes.push_back("(Nenhuma missao disponivel)");
            }
            missoes.push_back("VOLTAR");

            int id = ControleDeInput::lerSelecaoMenuEmPopup(
                "MISSOES DE MORGANA",
                {"Escolha uma missao:"},
                missoes,
                Cor::MAGENTA,
                NPCMorganaLayouts::arteMorgana
            );

            if (!jogadorAtual->obterLabirintoDesbloqueado() && id == 0) {
                processarMissaoLabirinto(jogadorAtual);
            } else if (jogadorAtual->obterLabirintoDesbloqueado() && id == 0) {
                dialogoMorganaUnico("Nao busco mais nada de voce no momento...");
            } else if (id == 1 || id == -1) {
                break;
            }
        }
    }
}
