#include "GerenciadorDrops.h"
#include "../Sistemas/SistemaPersonagem.h"
#include "../Inventario/FabricaItens.h"
#include "../Utilidades/Aparencia.h"
#include "../Telas/TelaCombate.h"
#include "../Utilidades/GeradorAleatorio.h"

void GerenciadorDrops::relatarEProcessarXpOuro(SistemaPersonagem* jogador, int xpDrop, int ouroDrop, int& ouroTotal, int& xpTotal) 
{
    jogador->ganharXp(xpDrop);
    jogador->ganharOuro(ouroDrop);
    xpTotal += xpDrop;
    ouroTotal += ouroDrop;

    Aparencia::registrarLogBatalha(Aparencia::cor(Cor::AMARELO) + "+" + std::to_string(ouroDrop) + "G" + Aparencia::cor(Cor::RESET));
    Aparencia::registrarLogBatalha(Aparencia::cor(Cor::CIANO) + "+" + std::to_string(xpDrop) + " XP" + Aparencia::cor(Cor::RESET));
}

void GerenciadorDrops::relatarDropItem(const std::string& nomeItem, int quantidade) 
{
    Aparencia::registrarLogBatalha(Aparencia::cor(Cor::BRANCO) + "+" + std::to_string(quantidade) + "x " + nomeItem + Aparencia::cor(Cor::RESET));
}

void GerenciadorDrops::darEProcessarItem(SistemaPersonagem* jogador, ItemID idItem, int quantidade, std::vector<std::string>& itensObtidos, int chanceDeDrop)
{
    if (quantidade <= 0) return;
    if (chanceDeDrop < 100 && !GeradorAleatorio::rolarChance(chanceDeDrop)) return;

    std::string nomeItem = FabricaItens::obterNomeDeID(idItem);
    if (nomeItem.empty() || nomeItem == "Desconhecido") {
        auto temp = FabricaItens::criarItem(idItem);
        if (temp) nomeItem = temp->obterNomeItem();
    }
    for (int i = 0; i < quantidade; ++i) {
        auto itemCriado = FabricaItens::criarItem(idItem);
        if (itemCriado && i == 0) nomeItem = itemCriado->obterNomeItem(); // Pega nome com cores/degrade caso tenha
        jogador->obterInventario()->adicionarItem(std::move(itemCriado));
        itensObtidos.push_back(nomeItem);
    }
    relatarDropItem(nomeItem, quantidade);
}
