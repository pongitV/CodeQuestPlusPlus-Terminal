#include "InteracaoNPC.h"
#include <iostream>
#include "../../Core/Utilidades/ControleDeInput.h"
#include "../../Visoes/TelasBase/Inventario/TelaInventario.h"
#include "../../Sistemas/Inventario/FabricaItens.h"
#include "../../Core/Utilidades/FuncoesDialogo.h"
#include "../../Visoes/TelasBase/TelaBase.h"

// --- INTERACAO PRINCIPAL ---
void InteracaoNPC::interagir(Personagem* jogadorAtual) {
    std::string opcao;
    
    do {
        Aparencia::limparTela();
        Aparencia::exibirPainelTexto(obterNomeDoLugar(), obterCorDoCabecalho());

        int larguraDoTerminal = Aparencia::obterLarguraTerminal();
        
        const std::vector<std::string>& arte = obterArteASCII();
        Aparencia::imprimirBlocoCentralizado(arte);
        std::cout << "\n";

        std::cout << Aparencia::cor(Cor::AMARELO) << " Seu Ouro: " << jogadorAtual->obterInventario()->obterOuro() << "G\n" << Aparencia::cor(Cor::RESET);

        exibirDialogo(jogadorAtual);
        std::cout << "\n";

        std::vector<std::string> opcoes = obterOpcoesMenu(jogadorAtual, larguraDoTerminal);
        
        int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoes);

        if (escolha >= 0 && escolha < opcoes.size()) {
            opcao = opcoes[escolha];
            if (opcao != "VOLTAR") {
                processarOpcao(jogadorAtual, opcao, larguraDoTerminal);
            }
        }

    } while (opcao != "VOLTAR");
}

void InteracaoNPC::processarMenuMissoesVazio(Personagem* jogadorAtual, const std::string& tituloMenu, Cor corCabecalho, const std::string& nomeNPC, const std::string& falaVazia) {
    std::string opcaoMissao;
    do {
        std::vector<std::string> missoes = {
            "(Nenhuma missao disponivel)",
            "VOLTAR"
        };
        
        int id = ControleDeInput::lerSelecaoMenuEmPopup(tituloMenu, {"Escolha uma missao:"}, missoes, corCabecalho);
        if (id == -1) break;
        opcaoMissao = missoes[id];

        if (opcaoMissao == "(Nenhuma missao disponivel)") {
            Aparencia::exibirPopup(nomeNPC, {falaVazia}, corCabecalho);
        }
    } while (opcaoMissao != "VOLTAR");
}

bool InteracaoNPC::verificarMaterialNoInventario(Personagem* jogadorAtual, const std::string& nomeMaterial, int quantidadeNecessaria, const std::string& nomeNPC, Cor corNPC, const std::string& mensagemPersonalizada) {
    int qtdAtual = jogadorAtual->obterInventario()->contarItem(nomeMaterial);
    if (qtdAtual < quantidadeNecessaria) {
        std::string msg = mensagemPersonalizada.empty() 
            ? "Voce nao tem " + nomeMaterial + " suficiente! (Possui: " + std::to_string(qtdAtual) + "/" + std::to_string(quantidadeNecessaria) + ")"
            : mensagemPersonalizada;
        FuncoesDialogo::imprimirDialogoNPC(nomeNPC, corNPC, msg);
        ControleDeInput::aguardarEnter();
        return false;
    }
    return true;
}

Item* InteracaoNPC::lerItemDoInventario(Personagem* jogadorAtual, const std::string& mensagemDialogo, const std::string& nomeNPC, Cor corNPC, std::string& codigoSaida, bool exibirPrecos) {
    Item* itemSelecionado = nullptr;

    TelaBase::executarLoop(
        [](bool animar) { TelaInventario::exibirCabecalhoInventario(animar); },
        [&]() {
            std::cout << "\n";
            Aparencia::imprimirCentralizado(Aparencia::cor(corNPC) + "[" + nomeNPC + "]: " + Aparencia::cor(Cor::BRANCO) + mensagemDialogo + Aparencia::cor(Cor::RESET));
            std::cout << "\n";
        },
        [jogadorAtual, exibirPrecos]() {
            std::vector<std::string> opcoes;
            opcoes.push_back("Arsenal de Equipamentos");
            opcoes.push_back("Itens Consumiveis");
            opcoes.push_back("Estoque e Materiais");
            opcoes.push_back("Itens de Missao");
            opcoes.push_back("VOLTAR");
            return opcoes;
        },
        [&](int escolhaCat) {
            if (escolhaCat < 0 || escolhaCat == 4) {
                codigoSaida = "0";
                return false;
            }
            
            TelaBase::executarLoop(
                [](bool animar) { TelaInventario::exibirCabecalhoInventario(animar); },
                [&]() {
                    std::cout << "\n";
                    Aparencia::imprimirCentralizado(Aparencia::cor(corNPC) + "[" + nomeNPC + "]: " + Aparencia::cor(Cor::BRANCO) + mensagemDialogo + Aparencia::cor(Cor::RESET));
                    std::cout << "\n";
                },
                [&]() {
                    auto itens = TelaInventario::obterListaCategoria(jogadorAtual, escolhaCat, exibirPrecos);
                    std::vector<std::string> opcoes;
                    for (auto& par : itens) opcoes.push_back(par.first);
                    opcoes.push_back("VOLTAR");
                    return opcoes;
                },
                [&](int escolhaItem) {
                    auto itens = TelaInventario::obterListaCategoria(jogadorAtual, escolhaCat, exibirPrecos);
                    if (escolhaItem < 0 || escolhaItem >= static_cast<int>(itens.size())) {
                        return false;
                    }
                    itemSelecionado = itens[escolhaItem].second;
                    codigoSaida = "selecionado";
                    return false;
                }
            );

            if (itemSelecionado) return false;
            return true;
        }
    );
    
    return itemSelecionado;
}

void InteracaoNPC::exibirTelaDeSucesso(const std::string& tituloCabecalho, Cor corCabecalho, const std::string& equacao, const std::vector<std::string>& arteAscii, const std::string& nomeNPC, const std::string& falaNPC) {
    Aparencia::limparTela();
    Aparencia::exibirPainelTexto(tituloCabecalho, corCabecalho);
    Aparencia::imprimirCentralizadoMultilinha({equacao, ""}, 0, Aparencia::cor(corCabecalho));
    if (!arteAscii.empty()) {
        Aparencia::imprimirCentralizadoMultilinha(arteAscii, 29, Aparencia::cor(corCabecalho));
    }
    FuncoesDialogo::imprimirDialogoNPC(nomeNPC, corCabecalho, falaNPC);
    ControleDeInput::aguardarEnter();
}

std::string InteracaoNPC::obterFormatadorStatusItem(ItemID id) {
    std::unique_ptr<Item> tempItem = FabricaItens::criarItem(id);
    return tempItem ? tempItem->obterInfoStatus() : "";
}

bool InteracaoNPC::verificarItemNaoEquipado(Personagem* jogadorAtual, Item* itemAvaliado, const std::string& nomeNPC, Cor corNPC, const std::string& msgErro) {
    if (jogadorAtual->isItemEquipado(itemAvaliado)) {
        FuncoesDialogo::imprimirDialogoNPC(nomeNPC, corNPC, msgErro);
        ControleDeInput::aguardarEnter();
        return false;
    }
    return true;
}
