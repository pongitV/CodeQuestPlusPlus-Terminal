#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <algorithm>
#include <memory>

#include "NPCBjorn.h"
#include "../Inventario/Item.h"
#include "../Inventario/FabricaItens.h"
#include "../Inventario/EquipamentoArmadura.h"
#include "../Telas/TelaInventario.h"
#include "../Utilidades/Aparencia.h"
#include "../Utilidades/ControleDeInput.h"
#include "NPCBjornLayouts.h"

namespace {
    // --- DADOS DO ESTOQUE ---
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
    
    void processarCompraDeEquipamento(SistemaPersonagem* jogadorAtual, bool comprandoArmas);
    void processarMelhoriaNaBigorna(SistemaPersonagem* jogadorAtual);
    void processarUpgradePorMaterial(SistemaPersonagem* jogadorAtual);

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
    static std::vector<std::string> arte = NPCBjornLayouts::obterArteBjorn();
    return arte;
}

// --- INTERACAO E MENU ---
void NPCBjorn::exibirDialogo(SistemaPersonagem* jogador) {
    dialogoBjorn(std::vector<std::string>{
        "Bem-vindo a minha forja, salvador!",
        "O que vai ser hoje?"
    });
}

std::vector<std::string> NPCBjorn::obterOpcoesMenu(SistemaPersonagem* jogador, int larguraDoTerminal) {
    return {
        "Seu Ouro: " + std::to_string(jogador->obterInventario()->obterOuro()) + "G",
        "",
        "[1] COMPRAR Armas das Classes",
        "[2] COMPRAR Armaduras das Classes",
        "[3] MELHORAR POR FUSAO",
        "[4] MELHORAR POR MATERIAL",
        "[0] VOLTAR",
        ""
    };
}

void NPCBjorn::processarOpcao(SistemaPersonagem* jogador, const std::string& opcao, int larguraDoTerminal) {
    if (opcao == "1" || opcao == "2") {
        processarCompraDeEquipamento(jogador, opcao == "1");
    } else if (opcao == "3") {
        processarMelhoriaNaBigorna(jogador);
    } else if (opcao == "4") {
        processarUpgradePorMaterial(jogador);
    }
}

namespace {
    // --- PROCESSAMENTO DE OPCOES ---
    void processarCompraDeEquipamento(SistemaPersonagem* jogadorAtual, bool comprandoArmas) {
        auto& estoqueAtual = comprandoArmas ? estoqueArmas : estoqueArmaduras;
        std::string tituloLoja = comprandoArmas ? "FORJA - ARMAS" : "FORJA - ARMADURAS";

        std::string opcaoCompra;
        do {
            Aparencia::limparTela();
            Aparencia::exibirCabecalho(tituloLoja, Cor::CIANO);
            
            std::vector<std::string> linhas;
            linhas.push_back("Seu Ouro: " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G");
            linhas.push_back("");

            for (auto const& [id, idItem] : estoqueAtual) {
                std::string preco = "40G";
                std::unique_ptr<Item> tempItem = FabricaItens::criarItem(idItem);
                std::string infoStatus = tempItem ? tempItem->obterInfoStatus() : "";
                linhas.push_back("[" + std::to_string(id) + "] " + (tempItem ? tempItem->obterNomeItem() : "???") + infoStatus + " (" + preco + ")");
            }
            linhas.push_back("");
            linhas.push_back("[0] VOLTAR");

            std::cout << "\n";
            Aparencia::imprimirBlocoCentralizado(linhas);
            std::cout << "\n";
            int idCompra = ControleDeInput::lerInteiroComLimites("Escolha: ", 0, 4, true);
            opcaoCompra = std::to_string(idCompra);
            
            if (opcaoCompra != "0") {
                int idCompra = std::stoi(opcaoCompra);
                if (jogadorAtual->obterInventario()->obterOuro() >= 40) {
                    jogadorAtual->obterInventario()->adicionarOuro(-40);
                    std::unique_ptr<Item> novoItem = FabricaItens::criarItem(estoqueAtual[idCompra]);
                    std::string nomeNovo = novoItem->obterNomeItem();
                    jogadorAtual->obterInventario()->adicionarItem(std::move(novoItem));
                    dialogoBjorn("Otima escolha! Voce comprou " + nomeNovo + ".");
                } else {
                    dialogoBjorn("Voce nao tem ouro suficiente para isso!");
                }
                Aparencia::aguardarEnter();
            }
        } while (opcaoCompra != "0");
    }

    void processarMelhoriaNaBigorna(SistemaPersonagem* jogadorAtual) {
        std::string codigoDoItemBase, codigoDoItemCopia;
        do {
            TelaInventario::exibir(jogadorAtual);
            dialogoBjorn("Escolha a ARMA, ESCUDO ou ARMADURA para melhorar (requer copia) ou [0] VOLTAR: ", true, false);
            std::cout << "\033[s";
            
            Item* itemBase = nullptr;
            while (true) {
                codigoDoItemBase = ControleDeInput::lerEntradaProtegida();
                if (codigoDoItemBase == "0") break;
                itemBase = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigoDoItemBase, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
                if (itemBase) break;
                std::cout << "\033[u\033[J";
            }
            if (codigoDoItemBase == "0") break;
            
            if (itemBase == jogadorAtual->obterArma() || itemBase == jogadorAtual->obterEscudo() || itemBase == jogadorAtual->obterArmadura()) { dialogoBjorn("Voce precisa DESEQUIPAR o item antes de usa-lo na bigorna!"); Aparencia::aguardarEnter(); continue; }
            if (itemBase->temPropriedade(Propriedade::Melhorado)) { dialogoBjorn("Este item ja atingiu o limite de melhoria basica!"); Aparencia::aguardarEnter(); continue; }
            TipoEquipamento tipo = itemBase->obterTipo();
            if (tipo != TipoEquipamento::ARMA && tipo != TipoEquipamento::ESCUDO && tipo != TipoEquipamento::ARMADURA) { dialogoBjorn("Eu so posso melhorar Armas, Escudos e Armaduras!"); Aparencia::aguardarEnter(); continue; }

            dialogoBjorn("Agora, escolha o SEGUNDO item identico (copia) ou [0] CANCELAR: ", true, false);
            std::cout << "\033[s";

            Item* itemCopia = nullptr;
            while (true) {
                codigoDoItemCopia = ControleDeInput::lerEntradaProtegida();
                if (codigoDoItemCopia == "0") break;
                itemCopia = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigoDoItemCopia, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
                if (itemCopia) break;
                std::cout << "\033[u\033[J";
            }
            if (codigoDoItemCopia == "0") continue;
            
            if (itemBase->obterNomeItem() != itemCopia->obterNomeItem()) { dialogoBjorn("Os itens precisam ser EXATAMENTE iguais para serem fundidos!"); Aparencia::aguardarEnter(); continue; }

            if (jogadorAtual->obterInventario()->contarItem(itemBase->obterNomeItem()) < 2) { dialogoBjorn("Voce nao possui UMA COPIA deste item!"); Aparencia::aguardarEnter(); continue; }

            if ((jogadorAtual->obterArma() && jogadorAtual->obterArma()->obterNomeItem() == itemBase->obterNomeItem()) ||
                (jogadorAtual->obterEscudo() && jogadorAtual->obterEscudo()->obterNomeItem() == itemBase->obterNomeItem()) ||
                (jogadorAtual->obterArmadura() && jogadorAtual->obterArmadura()->obterNomeItem() == itemBase->obterNomeItem())) {
                dialogoBjorn("Voce possui uma copia deste item equipada! DESEQUIPE antes de fundir.");
                Aparencia::aguardarEnter(); continue;
            }

            std::unique_ptr<Item> novoItem = itemBase->gerarCopiaMelhorada();

             if (novoItem) {
                std::string nomeAntigo = itemBase->obterNomeItem();
                std::string novoNome = novoItem->obterNomeItem();
                jogadorAtual->obterInventario()->removerItem(itemBase);
                jogadorAtual->obterInventario()->removerItem(itemCopia);
                jogadorAtual->obterInventario()->adicionarItem(std::move(novoItem));

                Aparencia::limparTela();
                Aparencia::exibirCabecalho("FORJA - SUCESSO", Cor::CIANO);
                std::vector<std::string> arteBigorna = NPCBjornLayouts::obterArteBigorna();
                std::string equacao = "[" + nomeAntigo + "] + [" + nomeAntigo + "] = [" + novoNome + "]";
                Aparencia::imprimirCentralizadoMultilinha({equacao, ""}, 0, Aparencia::cor(Cor::CIANO));
                Aparencia::imprimirCentralizadoMultilinha(arteBigorna, 29, Aparencia::cor(Cor::CIANO));

                dialogoBjorn("Ha! Trabalho feito! Seu equipamento esta mais forte do que nunca!");
                Aparencia::aguardarEnter();
            }
        } while (codigoDoItemBase != "0");
    }

    void processarUpgradePorMaterial(SistemaPersonagem* jogadorAtual) {
        std::string codigoDaArmadura;
        std::string nomePedraUpgrade = FabricaItens::obterNomeDeID(ItemID::PedraUpgrade);
        do {
            int qtdPedraUpgrade = jogadorAtual->obterInventario()->contarItem(nomePedraUpgrade);
            if (qtdPedraUpgrade < 1) {
                Aparencia::limparTela();
                Aparencia::exibirCabecalho("FORJA - MELHORIA POR MATERIAL", Cor::CIANO);
            dialogoBjorn("Voce nao tem nenhuma " + nomePedraUpgrade + "!");
                Aparencia::aguardarEnter();
                return;
            }

            TelaInventario::exibir(jogadorAtual);
        dialogoBjorn("Escolha a ARMADURA para melhorar (+3 Defesa/Resistencia) ou [0] VOLTAR: ", false, false);
            std::cout << "\033[s";

            Item* itemParaUpgrade = nullptr;
            while (true) {
                codigoDaArmadura = ControleDeInput::lerEntradaProtegida();
                if (codigoDaArmadura == "0") break;
                itemParaUpgrade = jogadorAtual->obterInventario()->buscarItemPorCodigo(codigoDaArmadura, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura());
                if (itemParaUpgrade) break;
                std::cout << "\033[u\033[J";
            }
            if (codigoDaArmadura == "0") break;

            if (itemParaUpgrade == jogadorAtual->obterArmadura() || itemParaUpgrade == jogadorAtual->obterArma() || itemParaUpgrade == jogadorAtual->obterEscudo()) {
            dialogoBjorn("Voce precisa DESEQUIPAR o item antes de usa-lo na bigorna!"); 
                Aparencia::aguardarEnter(); 
                continue; 
            }

            if (itemParaUpgrade->obterTipo() != TipoEquipamento::ARMADURA) {
            dialogoBjorn("Esta pedra magica so pode ser usada em ARMADURAS!");
                Aparencia::aguardarEnter(); 
                continue;
            }

            EquipamentoArmadura* armadura = dynamic_cast<EquipamentoArmadura*>(itemParaUpgrade);
            if (!armadura) continue;

            if (armadura->temPropriedade(Propriedade::MelhoradoMaterial)) {
            dialogoBjorn("Esta armadura ja foi imbuida com a pedra magica!");
                Aparencia::aguardarEnter();
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

            Aparencia::limparTela();
            Aparencia::exibirCabecalho("FORJA - SUCESSO", Cor::CIANO);
            std::string equacao = "[" + nomeAntigo + "] + [Pedra magica] = [" + novoNome + "]";
            std::cout << "\n" << Aparencia::cor(Cor::CIANO) << equacao << Aparencia::cor(Cor::RESET) << "\n";
        dialogoBjorn("Impressionante! Essa pedra e mesmo magica. A armadura agora possui mais +3 de resistencia (defesa)!");
            Aparencia::aguardarEnter();
        } while (codigoDaArmadura != "0");
    }
}
