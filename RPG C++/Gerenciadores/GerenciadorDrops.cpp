#include "GerenciadorDrops.h"
#include "../Sistemas/SistemaPersonagem.h"
#include "../Inventario/FabricaItens.h"
#include "../Utilidades/Aparencia.h"
#include "../Telas/TelaCombate.h"

void GerenciadorDrops::relatarEProcessarXpOuro(SistemaPersonagem* jogador, int xpDrop, int ouroDrop, int& ouroTotal, int& xpTotal) 
{
    jogador->ganharXp(xpDrop);
    jogador->ganharOuro(ouroDrop);
    xpTotal += xpDrop;
    ouroTotal += ouroDrop;

    std::string msgOuro = Aparencia::margemCombate() + Aparencia::cor(Cor::FUNDO_AMARELO) + "+" + std::to_string(ouroDrop) + "G" + Aparencia::cor(Cor::RESET) + "\n";
    std::string msgXp = Aparencia::margemCombate() + Aparencia::cor(Cor::FUNDO_AZUL) + "+" + std::to_string(xpDrop) + " XP" + Aparencia::cor(Cor::RESET) + "\n";
    
    TelaCombate::adicionarMensagemFixa(msgOuro);
    TelaCombate::adicionarMensagemFixa(msgXp);
}

void GerenciadorDrops::relatarDropItem(const std::string& nomeItem, int quantidade) 
{
    std::string msg = Aparencia::margemCombate() + Aparencia::cor(Cor::BRANCO) + "+" + std::to_string(quantidade) + "x " + nomeItem + Aparencia::cor(Cor::RESET) + "\n";
    TelaCombate::adicionarMensagemFixa(msg);
}

void GerenciadorDrops::darEProcessarItem(SistemaPersonagem* jogador, ItemID idItem, int quantidade, std::vector<std::string>& itensObtidos)
{
    if (quantidade <= 0) return;
    std::string nomeItem = FabricaItens::obterNomeDeID(idItem);
    for (int i = 0; i < quantidade; ++i) {
        jogador->obterInventario()->adicionarItem(FabricaItens::criarItem(idItem));
        itensObtidos.push_back(nomeItem);
    }
    relatarDropItem(nomeItem, quantidade);
}
