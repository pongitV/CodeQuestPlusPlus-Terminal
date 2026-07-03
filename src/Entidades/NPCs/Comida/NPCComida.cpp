#include "NPCComida.h"
#include "NPCComidaLayout.h"
#include "../../../Core/Controladores/MenuJogo.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Sistemas/Inventario/FabricaItens.h"
#include <iostream>

void NPCComida::interagir(Personagem* jogador) {
    InteracaoNPC::interagir(jogador);
}

std::string NPCComida::obterNomeDoLugar() const {
    return "FEIRA DO REINO";
}

Cor NPCComida::obterCorDoCabecalho() const {
    return Cor::VERDE_CLARO;
}

Cor NPCComida::obterCorDaArte() const {
    return Cor::VERDE_CLARO;
}

const std::vector<std::string>& NPCComida::obterArteASCII() const {
    return NPCComidaLayouts::arteComida;
}

void NPCComida::exibirDialogo(Personagem* jogador) {
    std::vector<std::string> linhas = {
        "Olá, combatente! Sente fome? A jornada deve ser cansativa.",
        "Tenho as melhores e mais frescas provisões do reino!",
        "Nossos alimentos curam sua vida instantaneamente ao serem consumidos na mochila."
    };
    Aparencia::exibirPopup("VENDEDOR DE COMIDAS", linhas, Cor::VERDE_CLARO, obterArteASCII());
}

std::vector<std::string> NPCComida::obterOpcoesMenu(Personagem* jogador, int /*larguraDoTerminal*/) {
    return {
        "Maca (Cura 15 HP) - 5G",
        "Pao (Cura 25 HP) - 10G",
        "Queijo (Cura 40 HP) - 18G",
        "Carne Seca (Cura 60 HP) - 30G",
        "Voltar"
    };
}

void NPCComida::processarOpcao(Personagem* jogador, const std::string& opcao, int /*larguraDoTerminal*/) {
    ItemID idCompra = ItemID::Nenhum;
    int custo = 0;

    if (opcao.find("Maca") != std::string::npos) {
        idCompra = ItemID::Maca;
        custo = 5;
    }
    else if (opcao.find("Pao") != std::string::npos) {
        idCompra = ItemID::Pao;
        custo = 10;
    }
    else if (opcao.find("Queijo") != std::string::npos) {
        idCompra = ItemID::Queijo;
        custo = 18;
    }
    else if (opcao.find("Carne Seca") != std::string::npos) {
        idCompra = ItemID::CarneSeca;
        custo = 30;
    }

    if (idCompra != ItemID::Nenhum) {
        if (jogador->obterInventario()->obterOuro() >= custo) {
            auto item = FabricaItens::criarItem(idCompra);
            if (item) {
                jogador->obterInventario()->adicionarOuro(-custo);
                std::string nomeItem = item->obterNomeItem();
                jogador->obterInventario()->adicionarItem(std::move(item));
                Aparencia::exibirPopup("COMPRA REALIZADA", {"Voce comprou: " + nomeItem}, Cor::VERDE_CLARO, obterArteASCII());
            }
        } else {
            Aparencia::exibirPopup("OURO INSUFICIENTE", {"Voce nao tem ouro suficiente para comprar este alimento!"}, Cor::VERMELHO, obterArteASCII());
        }
    }
}
