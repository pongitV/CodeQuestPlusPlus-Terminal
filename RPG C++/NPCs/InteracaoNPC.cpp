#include "InteracaoNPC.h"
#include <iostream>
#include "../Utilidades/ControleDeInput.h"
#include "../Telas/TelaInventario.h"
#include "../Inventario/FabricaItens.h"

// --- INTERACAO PRINCIPAL ---
void InteracaoNPC::interagir(SistemaPersonagem* jogadorAtual) {
    std::string opcao;
    
    do {
        Aparencia::limparTela();
        Aparencia::exibirCabecalho(obterNomeDoLugar(), obterCorDoCabecalho());

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

void InteracaoNPC::processarMenuMissoesVazio(SistemaPersonagem* jogadorAtual, const std::string& tituloMenu, Cor corCabecalho, const std::string& nomeNPC, const std::string& falaVazia) {
    std::string opcaoMissao;
    do {
        Aparencia::limparTela();
        Aparencia::exibirCabecalho(tituloMenu, corCabecalho);
        std::vector<std::string> missoes = {
            "(Nenhuma missao disponivel)",
            "VOLTAR"
        };
        
        std::cout << "\n";
        Aparencia::imprimirCentralizado("Escolha uma missao:");
        std::cout << "\n";
        
        int id = ControleDeInput::lerSelecaoMenuComSetas(missoes);
        opcaoMissao = missoes[id];

        if (opcaoMissao == "(Nenhuma missao disponivel)") {
            Aparencia::imprimirDialogoNPC(nomeNPC, corCabecalho, falaVazia);
            ControleDeInput::aguardarEnter();
        }
    } while (opcaoMissao != "VOLTAR");
}

bool InteracaoNPC::verificarMaterialNoInventario(SistemaPersonagem* jogadorAtual, const std::string& nomeMaterial, int quantidadeNecessaria, const std::string& nomeNPC, Cor corNPC, const std::string& mensagemPersonalizada) {
    int qtdAtual = jogadorAtual->obterInventario()->contarItem(nomeMaterial);
    if (qtdAtual < quantidadeNecessaria) {
        std::string msg = mensagemPersonalizada.empty() 
            ? "Voce nao tem " + nomeMaterial + " suficiente! (Possui: " + std::to_string(qtdAtual) + "/" + std::to_string(quantidadeNecessaria) + ")"
            : mensagemPersonalizada;
        Aparencia::imprimirDialogoNPC(nomeNPC, corNPC, msg);
        ControleDeInput::aguardarEnter();
        return false;
    }
    return true;
}

Item* InteracaoNPC::lerItemDoInventario(SistemaPersonagem* jogadorAtual, const std::string& mensagemDialogo, const std::string& nomeNPC, Cor corNPC, std::string& codigoSaida, bool exibirPrecos) {
    TelaInventario::exibir(jogadorAtual, exibirPrecos);
    Aparencia::imprimirDialogoNPC(nomeNPC, corNPC, mensagemDialogo, false, false);
    std::cout << "\033[s";
    return TelaInventario::lerSelecaoDeItem(jogadorAtual, codigoSaida);
}

void InteracaoNPC::exibirTelaDeSucesso(const std::string& tituloCabecalho, Cor corCabecalho, const std::string& equacao, const std::vector<std::string>& arteAscii, const std::string& nomeNPC, const std::string& falaNPC) {
    Aparencia::limparTela();
    Aparencia::exibirCabecalho(tituloCabecalho, corCabecalho);
    Aparencia::imprimirCentralizadoMultilinha({equacao, ""}, 0, Aparencia::cor(corCabecalho));
    if (!arteAscii.empty()) {
        Aparencia::imprimirCentralizadoMultilinha(arteAscii, 29, Aparencia::cor(corCabecalho));
    }
    Aparencia::imprimirDialogoNPC(nomeNPC, corCabecalho, falaNPC);
    ControleDeInput::aguardarEnter();
}

std::string InteracaoNPC::obterFormatadorStatusItem(ItemID id) {
    std::unique_ptr<Item> tempItem = FabricaItens::criarItem(id);
    return tempItem ? tempItem->obterInfoStatus() : "";
}

bool InteracaoNPC::verificarItemNaoEquipado(SistemaPersonagem* jogadorAtual, Item* itemAvaliado, const std::string& nomeNPC, Cor corNPC, const std::string& msgErro) {
    if (jogadorAtual->isItemEquipado(itemAvaliado)) {
        Aparencia::imprimirDialogoNPC(nomeNPC, corNPC, msgErro);
        ControleDeInput::aguardarEnter();
        return false;
    }
    return true;
}
