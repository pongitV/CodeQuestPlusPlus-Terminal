#include "ControleDeDrops.h"
#include "../Sistema/Personagem.h"
#include "../Sistema/SimplificacoesAparencia.h"
#include <iostream>

void ControleDeDrops::relatarEProcessarXpOuro(Personagem* jogador, int xpDrop, int ouroDrop, int& ouroTotal, int& xpTotal) 
{
    jogador->ganharXp(xpDrop);
    jogador->ganharOuro(ouroDrop);
    xpTotal += xpDrop;
    ouroTotal += ouroDrop;
    std::cout << SimplificacoesAparencia::cor(Cor::FUNDO_AMARELO) << "+" << ouroDrop << "G" << SimplificacoesAparencia::cor(Cor::RESET) << " " 
              << SimplificacoesAparencia::cor(Cor::FUNDO_AZUL) << "+" << xpDrop << " XP" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
}

void ControleDeDrops::relatarDropItem(const std::string& nomeItem, int quantidade) 
{
    std::cout << SimplificacoesAparencia::cor(Cor::BRANCO) << "+" << quantidade << "x " << nomeItem << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
}
