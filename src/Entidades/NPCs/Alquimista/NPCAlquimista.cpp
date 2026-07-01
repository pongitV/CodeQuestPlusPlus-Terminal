#include "NPCAlquimista.h"
#include "NPCAlquimistaLayout.h"
#include "../../../Core/Controladores/MenuJogo.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Sistemas/Inventario/FabricaItens.h"
#include <iostream>

void NPCAlquimista::interagir(Personagem* jogador) {
    ControleDeInput::executarLoopMenuPopup(
        [this, jogador]() { this->exibirDialogo(jogador); },
        [this, jogador]() { return this->obterOpcoesMenu(jogador, Aparencia::obterLarguraTerminal()); },
        [this, jogador](const std::string& op) { this->processarOpcao(jogador, op, Aparencia::obterLarguraTerminal()); return true; },
        obterNomeDoLugar(), obterCorDoCabecalho(), obterArteASCII()
    );
}

std::string NPCAlquimista::obterNomeDoLugar() const {
    return "LABORATORIO DE ALQUIMIA";
}

Cor NPCAlquimista::obterCorDoCabecalho() const {
    return Cor::VERDE_ESCURO;
}

Cor NPCAlquimista::obterCorDaArte() const {
    return Cor::VERDE_ESCURO;
}

const std::vector<std::string>& NPCAlquimista::obterArteASCII() const {
    return NPCAlquimistaLayouts::arteAlquimista;
}

void NPCAlquimista::exibirDialogo(Personagem* jogador) {
    std::vector<std::string> linhas = {
        "Seja bem-vindo ao laboratorio de transmutacao!",
        "Eu sou Quintus, o Alquimista Real. Se voce me trouxer ingredientes de monstros",
        "e alimentos terrestres, posso transmutar elixires poderosos!",
        "Minhas criacoes podem curar sua alma ou devastar as defesas inimigas."
    };
    Aparencia::exibirPopup("QUINTUS - ALQUIMISTA REAL", linhas, Cor::VERDE_ESCURO, obterArteASCII());
}

std::vector<std::string> NPCAlquimista::obterOpcoesMenu(Personagem* jogador, int /*larguraDoTerminal*/) {
    return {
        "Pocao de Cura Grande (50%VM) [1x Maca + 1x Po magico]",
        "Pocao de Forca Alquimica [1x Pao + 1x Dente de goblin]",
        "Pocao de Veneno Alquimica [1x Carne Seca + 1x Gosma acida]",
        "Pocao de Lentidao Alquimica [1x Queijo + 1x Nucleo pegajoso]",
        "Voltar"
    };
}

void NPCAlquimista::processarOpcao(Personagem* jogador, const std::string& opcao, int /*larguraDoTerminal*/) {
    std::string comidaReq = "";
    std::string dropReq = "";
    ItemID produtoId = ItemID::Nenhum;

    if (opcao.find("Cura Grande") != std::string::npos) {
        comidaReq = "Maca";
        dropReq = "Po magico";
        produtoId = ItemID::PocaoCuraGrande;
    }
    else if (opcao.find("Forca Alquimica") != std::string::npos) {
        comidaReq = "Pao";
        dropReq = "Dente de goblin";
        produtoId = ItemID::PocaoForcaAlquimica;
    }
    else if (opcao.find("Veneno Alquimica") != std::string::npos) {
        comidaReq = "Carne Seca";
        dropReq = "Gosma acida";
        produtoId = ItemID::PocaoVenenoAlquimica;
    }
    else if (opcao.find("Lentidao Alquimica") != std::string::npos) {
        comidaReq = "Queijo";
        dropReq = "Nucleo pegajoso";
        produtoId = ItemID::PocaoLentidaoAlquimica;
    }

    if (produtoId != ItemID::Nenhum) {
        auto* mochila = jogador->obterInventario();
        int qtdComida = mochila->contarItem(comidaReq);
        int qtdDrop = mochila->contarItem(dropReq);

        if (qtdComida >= 1 && qtdDrop >= 1) {
            mochila->removerItem(comidaReq);
            mochila->removerItem(dropReq);

            auto itemNovo = FabricaItens::criarItem(produtoId);
            if (itemNovo) {
                std::string nomeProduto = itemNovo->obterNomeItem();
                mochila->adicionarItem(std::move(itemNovo));

                std::vector<std::string> msgSucesso = {
                    "Mistura fervilhando... Vapor borbulhando...",
                    "Sucesso! VocÃª obteve: " + nomeProduto
                };
                Aparencia::exibirPopup("TRANSMUTACAO COMPLETA", msgSucesso, Cor::VERDE_ESCURO, obterArteASCII());
            }
        } else {
            std::vector<std::string> msgErro = {
                "Ingredientes insuficientes!",
                "VocÃª precisa de:",
                " -> 1x " + comidaReq + " (Possui: " + std::to_string(qtdComida) + ")",
                " -> 1x " + dropReq + " (Possui: " + std::to_string(qtdDrop) + ")"
            };
            Aparencia::exibirPopup("FALHA NA TRANSMUTACAO", msgErro, Cor::VERMELHO, obterArteASCII());
        }
    }
}

