#pragma once

#include <string>
#include <vector>

class Personagem;

class ControleDeDrops 
{
public:
    // Centraliza o calculo e as mensagens na tela para as recompensas dos monstros
    static void relatarEProcessarXpOuro(Personagem* jogador, int xpDrop, int ouroDrop, int& ouroTotal, int& xpTotal);
    
    // Padroniza a mensagem verde ou branca do recebimento de um item 
    static void relatarDropItem(const std::string& nomeItem, int quantidade);
};
