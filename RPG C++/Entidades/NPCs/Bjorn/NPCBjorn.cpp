#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <algorithm>
#include <memory>

#include "NPCBjorn.h"
#include "../../../Sistemas/Inventario/Item.h"
#include "../../../Sistemas/Inventario/FabricaItens.h"
#include "../../../Sistemas/Inventario/Equipamentos/EquipamentoArmadura.h"
#include "../../../Interface/Telas/Inventario/TelaInventario.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Core/Controladores/Loja.h"
#include "NPCBjornLayout.h"

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

    // --- APARENCIA E DIALOGOS ---
    void dialogoBjorn(const std::string& texto, bool novaLinhaAntes = true, bool novaLinhaDepois = true) {
        Aparencia::imprimirDialogoNPC("Bjorn", Cor::CIANO, texto, novaLinhaAntes, novaLinhaDepois);
    }

    void dialogoBjorn(const std::vector<std::string>& linhas) {
        Aparencia::imprimirDialogoNPC("Bjorn", Cor::CIANO, linhas);
    }
}

// --- INFORMACOES DO LUGAR ---
std::string NPCBjorn::obterNomeDoLugar() const {
    return "FORJA DO BJORN";
}

Cor NPCBjorn::obterCorDoCabecalho() const {
    return Cor::CIANO;
}

Cor NPCBjorn::obterCorDaArte() const {
    return Cor::CIANO;
}

const std::vector<std::string>& NPCBjorn::obterArteASCII() const {
    return NPCBjornLayouts::arteBjorn;
}

// --- INTERACAO E MENU ---
void NPCBjorn::exibirDialogo(Personagem* jogador) {
    dialogoBjorn(std::vector<std::string>{
        "Bem-vindo a minha forja, salvador!",
        "O que vai ser hoje?"
    });
}

std::vector<std::string> NPCBjorn::obterOpcoesMenu(Personagem* jogador, int larguraDoTerminal) {
    return {
        "COMPRAR Armas das Classes",
        "COMPRAR Armaduras das Classes",
        "MELHORAR POR FUSAO",
        "MELHORAR POR MATERIAL",
        "Missoes de Bjorn",
        "VOLTAR"
    };
}

void NPCBjorn::processarOpcao(Personagem* jogador, const std::string& opcao, int larguraDoTerminal) {
    if (opcao == "COMPRAR Armas das Classes" || opcao == "COMPRAR Armaduras das Classes") {
        processarCompraDeEquipamento(jogador, opcao == "COMPRAR Armas das Classes");
    } else if (opcao == "MELHORAR POR FUSAO") {
        processarMelhoriaNaBigorna(jogador);
    } else if (opcao == "MELHORAR POR MATERIAL") {
        processarUpgradePorMaterial(jogador);
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
            [](const std::string& msg) { dialogoBjorn(msg); }, InteracaoNPC::obterFormatadorStatusItem);
    }

    void processarMelhoriaNaBigorna(Personagem* jogadorAtual) {
        std::string codigoDoItemBase;
        do {
            Item* itemBase = InteracaoNPC::lerItemDoInventario(jogadorAtual, "Escolha a ARMA, ESCUDO ou ARMADURA para melhorar (requer copia) ou [0] VOLTAR: ", "Bjorn", Cor::CIANO, codigoDoItemBase);
            if (codigoDoItemBase == "0") break;
            if (!itemBase) continue;
            
            if (!InteracaoNPC::verificarItemNaoEquipado(jogadorAtual, itemBase, "Bjorn", Cor::CIANO, "Voce precisa DESEQUIPAR o item antes de usa-lo na bigorna!")) continue;
            if (itemBase->temPropriedade(Propriedade::Melhorado)) { dialogoBjorn("Este item ja atingiu o limite de melhoria basica!"); ControleDeInput::aguardarEnter(); continue; }
            TipoEquipamento tipo = itemBase->obterTipo();
            if (tipo != TipoEquipamento::ARMA && tipo != TipoEquipamento::ESCUDO && tipo != TipoEquipamento::ARMADURA) { dialogoBjorn("Eu so posso melhorar Armas, Escudos e Armaduras!"); ControleDeInput::aguardarEnter(); continue; }

            if (!InteracaoNPC::verificarMaterialNoInventario(jogadorAtual, itemBase->obterNomeItem(), 2, "Bjorn", Cor::CIANO)) continue;

            if ((jogadorAtual->obterArma() && jogadorAtual->obterArma()->obterNomeItem() == itemBase->obterNomeItem()) ||
                (jogadorAtual->obterEscudo() && jogadorAtual->obterEscudo()->obterNomeItem() == itemBase->obterNomeItem()) ||
                (jogadorAtual->obterArmadura() && jogadorAtual->obterArmadura()->obterNomeItem() == itemBase->obterNomeItem())) {
                dialogoBjorn("Voce possui uma copia deste item equipada! DESEQUIPE antes de fundir.");
                ControleDeInput::aguardarEnter(); continue;
            }

            std::unique_ptr<Item> novoItem = itemBase->gerarCopiaMelhorada();

             if (novoItem) {
                std::string nomeAntigo = itemBase->obterNomeItem();
                std::string novoNome = novoItem->obterNomeItem();
                jogadorAtual->obterInventario()->removerItem(itemBase);
                jogadorAtual->obterInventario()->removerItem(nomeAntigo);
                jogadorAtual->obterInventario()->adicionarItem(std::move(novoItem));

                std::string equacao = "[" + nomeAntigo + "] + [" + nomeAntigo + "] = [" + novoNome + "]";
                InteracaoNPC::exibirTelaDeSucesso("FORJA - SUCESSO", Cor::CIANO, equacao, NPCBjornLayouts::arteBigorna, "Bjorn", "Ha! Trabalho feito! Seu equipamento esta mais forte do que nunca!");
            }
        } while (codigoDoItemBase != "0");
    }

    void processarUpgradePorMaterial(Personagem* jogadorAtual) {
        std::string codigoDaArmadura;
        std::string nomePedraUpgrade = FabricaItens::obterNomeDeID(ItemID::PedraUpgrade);
        do {
            if (!InteracaoNPC::verificarMaterialNoInventario(jogadorAtual, nomePedraUpgrade, 1, "Bjorn", Cor::CIANO)) {
                return;
            }

            Item* itemParaUpgrade = InteracaoNPC::lerItemDoInventario(jogadorAtual, "Escolha a ARMADURA para melhorar (+3 Defesa/Resistencia) ou [0] VOLTAR: ", "Bjorn", Cor::CIANO, codigoDaArmadura);
            if (codigoDaArmadura == "0") break;
            if (!itemParaUpgrade) continue;

            if (!InteracaoNPC::verificarItemNaoEquipado(jogadorAtual, itemParaUpgrade, "Bjorn", Cor::CIANO, "Voce precisa DESEQUIPAR o item antes de usa-lo na bigorna!")) continue;

            if (itemParaUpgrade->obterTipo() != TipoEquipamento::ARMADURA) {
                dialogoBjorn("Esta pedra magica so pode ser usada em ARMADURAS!");
                ControleDeInput::aguardarEnter(); 
                continue;
            }

            EquipamentoArmadura* armadura = dynamic_cast<EquipamentoArmadura*>(itemParaUpgrade);
            if (!armadura) continue;

            if (armadura->temPropriedade(Propriedade::MelhoradoMaterial)) {
                dialogoBjorn("Esta armadura ja foi imbuida com a pedra magica!");
                ControleDeInput::aguardarEnter();
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
            InteracaoNPC::exibirTelaDeSucesso("FORJA - SUCESSO", Cor::CIANO, equacao, {}, "Bjorn", "Impressionante! Essa pedra e mesmo magica. A armadura agora possui mais +3 de resistencia (defesa)!");
        } while (codigoDaArmadura != "0");
    }
}






