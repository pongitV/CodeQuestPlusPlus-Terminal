#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <algorithm>
#include <memory>

#include "NPCFerreiro.h"
#include "../../../Sistemas/Inventario/Item.h"
#include "../../../Sistemas/Inventario/FabricaItens.h"
#include "../../../Sistemas/Inventario/Equipamentos/EquipamentoArmadura.h"
#include "../../../Sistemas/Inventario/Equipamentos/EquipamentoEscudo.h"
#include "../../../Interface/Telas/Inventario/TelaInventario.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Core/Controladores/Loja.h"
#include "../../../Core/Utilidades/FuncoesDialogo.h"
#include "NPCFerreiroLayout.h"

namespace {
    // --- DADOS DO ESTOQUE ---
    std::map<int, ProdutoLoja> estoqueArmas = {
        {1, {ItemID::EspadaFerro, 40, -1}},
        {2, {ItemID::ArcoMadeira, 40, -1}},
        {3, {ItemID::CajadoCristal, 40, -1}},
        {4, {ItemID::ViolaoEncantado, 40, -1}}
    };
    
    std::map<int, ProdutoLoja> estoqueArmaduras = {
        {1, {ItemID::ArmaduraMalha, 40, -1}},
        {2, {ItemID::ArmaduraCouro, 40, -1}},
        {3, {ItemID::Tunica, 40, -1}},
        {4, {ItemID::TrajeNobre, 40, -1}}
    };
    
    void processarCompraDeEquipamento(Personagem* jogadorAtual, bool comprandoArmas);
    void processarMelhoriaNaBigorna(Personagem* jogadorAtual);
    void processarUpgradePorMaterial(Personagem* jogadorAtual);
    void processarConsertoDeEscudo(Personagem* jogadorAtual);

    // --- APARENCIA E DIALOGOS ---
    void dialogoBjorn(const std::vector<std::string>& linhas) {
        Aparencia::exibirPopup("BJORN", linhas, Cor::CIANO, NPCFerreiroLayouts::arteFerreiro);
    }
    
    void dialogoBjornUnico(const std::string& msg) {
        dialogoBjorn({msg});
    }
}

// --- INFORMACOES DO LUGAR ---
std::string NPCFerreiro::obterNomeDoLugar() const {
    return "FORJA DO BJORN";
}

Cor NPCFerreiro::obterCorDoCabecalho() const {
    return Cor::CIANO;
}

Cor NPCFerreiro::obterCorDaArte() const {
    return Cor::CIANO;
}

const std::vector<std::string>& NPCFerreiro::obterArteASCII() const {
    return NPCFerreiroLayouts::arteFerreiro;
}

// --- INTERACAO E MENU ---
void NPCFerreiro::interagir(Personagem* jogador) {
    ControleDeInput::executarLoopMenuPopup(
        [this, jogador]() { this->exibirDialogo(jogador); },
        [this, jogador]() { return this->obterOpcoesMenu(jogador, Aparencia::obterLarguraTerminal()); },
        [this, jogador](const std::string& op) { this->processarOpcao(jogador, op, Aparencia::obterLarguraTerminal()); return true; },
        obterNomeDoLugar(), obterCorDoCabecalho(), obterArteASCII()
    );
}

void NPCFerreiro::exibirDialogo(Personagem* /*jogador*/) {
    dialogoBjorn(std::vector<std::string>{
        "Bem-vindo a minha forja, salvador!",
        "O que vai ser hoje?"
    });
}

std::vector<std::string> NPCFerreiro::obterOpcoesMenu(Personagem* /*jogador*/, int /*larguraDoTerminal*/) {
    return {
        "COMPRAR Armas das Classes",
        "COMPRAR Armaduras das Classes",
        "MELHORAR POR FUSAO",
        "MELHORAR POR MATERIAL",
        "Missoes de Bjorn",
        "VOLTAR"
    };
}

void NPCFerreiro::processarOpcao(Personagem* jogador, const std::string& opcao, int /*larguraDoTerminal*/) {
    if (opcao == "COMPRAR Armas das Classes" || opcao == "COMPRAR Armaduras das Classes") {
        processarCompraDeEquipamento(jogador, opcao == "COMPRAR Armas das Classes");
    } else if (opcao == "MELHORAR POR FUSAO") {
        processarMelhoriaNaBigorna(jogador);
    } else if (opcao == "MELHORAR POR MATERIAL") {
        processarUpgradePorMaterial(jogador);
    } else if (opcao == "CONSERTAR Escudo") {
        processarConsertoDeEscudo(jogador);
    } else if (opcao == "Missoes de Bjorn") {
        InteracaoNPC::processarMenuMissoesVazio(jogador, "MISSOES DE BJORN", Cor::CIANO, "Bjorn", "Nao tenho nenhum trabalho especial para voce no momento.");
    }
}

namespace {
    // --- PROCESSAMENTO DE OPCOES ---
    void processarCompraDeEquipamento(Personagem* jogadorAtual, bool comprandoArmas) {
        auto& estoqueAtual = comprandoArmas ? estoqueArmas : estoqueArmaduras;
        std::string tituloLoja = comprandoArmas ? "FORJA - ARMAS" : "FORJA - ARMADURAS";

        Loja::processarCompra(jogadorAtual, tituloLoja, Cor::CIANO, estoqueAtual, 
            [](const std::string& msg) { dialogoBjornUnico(msg); }, InteracaoNPC::obterFormatadorStatusItem, NPCFerreiroLayouts::arteFerreiro);
    }

    void processarMelhoriaNaBigorna(Personagem* jogadorAtual) {
        do {
            std::vector<Item*> itensValidos;
            std::vector<std::string> opcoesItem;
            for (auto* item : jogadorAtual->obterInventario()->obterTodosOsItens()) {
                TipoEquipamento tipo = item->obterTipo();
                if ((tipo == TipoEquipamento::ARMA || tipo == TipoEquipamento::ESCUDO || tipo == TipoEquipamento::ARMADURA) && !item->temPropriedade(Propriedade::Melhorado)) {
                    itensValidos.push_back(item);
                    opcoesItem.push_back(item->obterNomeItem());
                }
            }
            if (opcoesItem.empty()) { dialogoBjornUnico("Voce nao tem nenhum equipamento que eu possa melhorar!"); break; }
            opcoesItem.push_back("VOLTAR");
            
            int escolha = ControleDeInput::lerSelecaoMenuEmPopup("FUSAO DE EQUIPAMENTO", {"Qual item deseja fundir? (Requer copia no inventario)"}, opcoesItem, Cor::CIANO, NPCFerreiroLayouts::arteBigorna);
            if (escolha == -1 || escolha == static_cast<int>(opcoesItem.size()) - 1) break;
            
            Item* itemBase = itensValidos[escolha];
            if (!InteracaoNPC::verificarItemNaoEquipado(jogadorAtual, itemBase, "Bjorn", Cor::CIANO, "Voce precisa DESEQUIPAR o item antes de usa-lo na bigorna!")) continue;

            if (!InteracaoNPC::verificarMaterialNoInventario(jogadorAtual, itemBase->obterNomeItem(), 2, "Bjorn", Cor::CIANO)) continue;

            if ((jogadorAtual->obterArma() && jogadorAtual->obterArma()->obterNomeItem() == itemBase->obterNomeItem()) ||
                (jogadorAtual->obterEscudo() && jogadorAtual->obterEscudo()->obterNomeItem() == itemBase->obterNomeItem()) ||
                (jogadorAtual->obterArmadura() && jogadorAtual->obterArmadura()->obterNomeItem() == itemBase->obterNomeItem())) {
                dialogoBjornUnico("Voce possui uma copia deste item equipada! DESEQUIPE antes de fundir."); continue;
            }

            std::unique_ptr<Item> novoItem = itemBase->gerarCopiaMelhorada();

             if (novoItem) {
                std::string nomeAntigo = itemBase->obterNomeItem();
                std::string novoNome = novoItem->obterNomeItem();
                jogadorAtual->obterInventario()->removerItem(itemBase);
                jogadorAtual->obterInventario()->removerItem(nomeAntigo);
                jogadorAtual->obterInventario()->adicionarItem(std::move(novoItem));

                std::string equacao = "[" + nomeAntigo + "] + [" + nomeAntigo + "] = [" + novoNome + "]";
            Aparencia::exibirPopup("FORJA - SUCESSO", {equacao, "", "Ha! Trabalho feito! Seu equipamento esta mais forte do que nunca!"}, Cor::CIANO, NPCFerreiroLayouts::arteBigorna);
            }
        } while (true);
    }

    void processarUpgradePorMaterial(Personagem* jogadorAtual) {
        std::string nomePedraUpgrade = FabricaItens::obterNomeDeID(ItemID::PedraUpgrade);
        do {
            if (!InteracaoNPC::verificarMaterialNoInventario(jogadorAtual, nomePedraUpgrade, 1, "Bjorn", Cor::CIANO)) {
                return;
            }
            
            std::vector<Item*> itensValidos;
            std::vector<std::string> opcoesItem;
            for (auto* item : jogadorAtual->obterInventario()->obterTodosOsItens()) {
                if (item->obterTipo() == TipoEquipamento::ARMADURA && !item->temPropriedade(Propriedade::MelhoradoMaterial)) {
                    itensValidos.push_back(item);
                    opcoesItem.push_back(item->obterNomeItem());
                }
            }
            if (opcoesItem.empty()) { dialogoBjornUnico("Voce nao tem armaduras validas para imbuir!"); break; }
            opcoesItem.push_back("VOLTAR");
            
            int escolha = ControleDeInput::lerSelecaoMenuEmPopup("IMBUIR ARMADURA", {"Qual armadura imbuir com a Pedra? (+3 Defesa)"}, opcoesItem, Cor::CIANO, NPCFerreiroLayouts::arteBigorna);
            if (escolha == -1 || escolha == static_cast<int>(opcoesItem.size()) - 1) break;

            Item* itemParaUpgrade = itensValidos[escolha];
            if (!InteracaoNPC::verificarItemNaoEquipado(jogadorAtual, itemParaUpgrade, "Bjorn", Cor::CIANO, "Voce precisa DESEQUIPAR o item antes de usa-lo na bigorna!")) continue;

            EquipamentoArmadura* armadura = dynamic_cast<EquipamentoArmadura*>(itemParaUpgrade);
            if (!armadura) continue;

            if (armadura->temPropriedade(Propriedade::MelhoradoMaterial)) {
                dialogoBjornUnico("Esta armadura ja foi imbuida com a pedra magica!");
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

            std::string equacao = "[" + nomeAntigo + "] + [Pedra magica] = [" + novoNome + "]";
            Aparencia::exibirPopup("FORJA - SUCESSO", {equacao, "", "Impressionante! A armadura agora possui +3 de defesa!"}, Cor::CIANO, NPCFerreiroLayouts::arteBigorna);
        } while (true);
    }

    void processarConsertoDeEscudo(Personagem* jogadorAtual) {
        do {
            std::vector<EquipamentoEscudo*> escudosDanificados;
            std::vector<std::string> opcoesEscudo;

            for (auto* item : jogadorAtual->obterInventario()->obterTodosOsItens()) {
                EquipamentoEscudo* escudo = dynamic_cast<EquipamentoEscudo*>(item);
                if (escudo && escudo->obterDurabilidadeAtualEscudo() < escudo->obterDurabilidadeMaxima()) {
                    escudosDanificados.push_back(escudo);
                    int custo = (escudo->obterDurabilidadeMaxima() - escudo->obterDurabilidadeAtualEscudo()) * 5;
                    opcoesEscudo.push_back(escudo->obterNomeItem() + " (" + std::to_string(escudo->obterDurabilidadeAtualEscudo()) + "/" + std::to_string(escudo->obterDurabilidadeMaxima()) + ") - " + std::to_string(custo) + "g");
                }
            }

            if (escudosDanificados.empty()) {
                dialogoBjornUnico("Voce nao tem nenhum escudo danificado que eu possa consertar!");
                break;
            }
            
            opcoesEscudo.push_back("VOLTAR");
            
            int escolha = ControleDeInput::lerSelecaoMenuEmPopup("CONSERTAR ESCUDO", {"Qual escudo deseja consertar? (5g por ponto perdido)"}, opcoesEscudo, Cor::CIANO, NPCFerreiroLayouts::arteBigorna);
            if (escolha == -1 || escolha == static_cast<int>(opcoesEscudo.size()) - 1) break;

            EquipamentoEscudo* escudoParaConsertar = escudosDanificados[escolha];
            if (!InteracaoNPC::verificarItemNaoEquipado(jogadorAtual, escudoParaConsertar, "Bjorn", Cor::CIANO, "Voce precisa DESEQUIPAR o escudo antes de conserta-lo!")) continue;

            int durabilidadePerdida = escudoParaConsertar->obterDurabilidadeMaxima() - escudoParaConsertar->obterDurabilidadeAtualEscudo();
            int custoReparo = durabilidadePerdida * 5; // Exemplo: 5 de ouro por ponto de durabilidade perdida

            if (jogadorAtual->obterInventario()->obterOuro() >= custoReparo) {
                jogadorAtual->obterInventario()->adicionarOuro(-custoReparo);
                escudoParaConsertar->definirDurabilidade(escudoParaConsertar->obterDurabilidadeMaxima());
                dialogoBjornUnico("Hmph! Seu escudo esta como novo! (-" + std::to_string(custoReparo) + "g)");
            } else {
                dialogoBjornUnico("Voce nao tem ouro suficiente para consertar este escudo. Eu preciso de " + std::to_string(custoReparo) + "g.");
            }
        } while (true);
    }
}
