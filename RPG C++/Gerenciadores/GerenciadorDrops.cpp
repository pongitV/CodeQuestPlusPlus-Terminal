#include "GerenciadorDrops.h"
#include "../Sistemas/SistemaPersonagem.h"
#include "../Utilidades/SimplificacoesAparencia.h"
#include <iostream>

void GerenciadorDrops::relatarEProcessarXpOuro(SistemaPersonagem* jogador, int xpDrop, int ouroDrop, int& ouroTotal, int& xpTotal) 
{
    jogador->ganharXp(xpDrop);
    jogador->ganharOuro(ouroDrop);
    xpTotal += xpDrop;
    ouroTotal += ouroDrop;
    std::cout << SimplificacoesAparencia::margemCombate() << SimplificacoesAparencia::cor(Cor::FUNDO_AMARELO) << "+" << ouroDrop << "G" << SimplificacoesAparencia::cor(Cor::RESET) << " " 
              << SimplificacoesAparencia::cor(Cor::FUNDO_AZUL) << "+" << xpDrop << " XP" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
}

void GerenciadorDrops::relatarDropItem(const std::string& nomeItem, int quantidade) 
{
    std::cout << SimplificacoesAparencia::margemCombate() << SimplificacoesAparencia::cor(Cor::BRANCO) << "+" << quantidade << "x " << nomeItem << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
}
