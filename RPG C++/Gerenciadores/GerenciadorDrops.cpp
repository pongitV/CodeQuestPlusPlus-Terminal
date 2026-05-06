#include "GerenciadorDrops.h"
#include "../Sistemas/SistemaPersonagem.h"
#include "../Utilidades/Aparencia.h"
#include <iostream>

void GerenciadorDrops::relatarEProcessarXpOuro(SistemaPersonagem* jogador, int xpDrop, int ouroDrop, int& ouroTotal, int& xpTotal) 
{
    jogador->ganharXp(xpDrop);
    jogador->ganharOuro(ouroDrop);
    xpTotal += xpDrop;
    ouroTotal += ouroDrop;
    std::cout << Aparencia::margemCombate() << Aparencia::cor(Cor::FUNDO_AMARELO) << "+" << ouroDrop << "G" << Aparencia::cor(Cor::RESET) << " " 
              << Aparencia::cor(Cor::FUNDO_AZUL) << "+" << xpDrop << " XP" << Aparencia::cor(Cor::RESET) << "\n";
}

void GerenciadorDrops::relatarDropItem(const std::string& nomeItem, int quantidade) 
{
    std::cout << Aparencia::margemCombate() << Aparencia::cor(Cor::BRANCO) << "+" << quantidade << "x " << nomeItem << Aparencia::cor(Cor::RESET) << "\n";
}
