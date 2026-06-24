#include "Drops.h"
#include "../../Entidades/Personagem.h"
#include "../../Sistemas/Inventario/FabricaItens.h"
#include "../Utilidades/Aparencia.h"
#include "../../Interface/Telas/Combate/TelaCombate.h"
#include "../Utilidades/GeradorAleatorio.h"
#include "../../Sistemas/Progresso/Diario.h"
#include "../Utilidades/FuncoesDialogo.h"

void Drops::relatarEProcessarXpOuro(Personagem* jogador, int xpDrop, int ouroDrop, int& ouroTotal, int& xpTotal) 
{
    jogador->ganharXp(xpDrop);
    jogador->ganharOuro(ouroDrop);
    xpTotal += xpDrop;
    ouroTotal += ouroDrop;

    if (ouroDrop > 0) Aparencia::registrarLogBatalha(FuncoesDialogo::formatarMsgDrop("+" + std::to_string(ouroDrop) + "G", Cor::AMARELO));
    if (xpDrop > 0) Aparencia::registrarLogBatalha(FuncoesDialogo::formatarMsgDrop("+" + std::to_string(xpDrop) + " XP", Cor::CIANO));
}

void Drops::relatarDropItem(const std::string& nomeItem, int quantidade) 
{
    Aparencia::registrarLogBatalha(FuncoesDialogo::formatarMsgDrop("+" + std::to_string(quantidade) + "x " + nomeItem, Cor::BRANCO));
}

void Drops::darEProcessarItem(Personagem* jogador, ItemID idItem, int quantidade, std::vector<std::string>& itensObtidos, int chanceDeDrop)
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
    Diario::instancia().registrarItem(Aparencia::removerCoresANSI(nomeItem));
    relatarDropItem(nomeItem, quantidade);
}
