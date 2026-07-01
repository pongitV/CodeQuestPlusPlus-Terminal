#pragma once

#include <string>
#include <vector>

class Personagem;
enum class ItemID;

class Drops 
{
public:
    // Centraliza o calculo e as mensagens na tela para as recompensas dos monstros
    static void relatarEProcessarXpOuro(Personagem* jogador, int xpDrop, int ouroDrop, int& ouroTotal, int& xpTotal);
    
    // Padroniza a mensagem verde ou branca do recebimento de um item 
    static void relatarDropItem(const std::string& nomeItem, int quantidade);

    // Delega a responsabilidade de dar o item e processar a string no array (Aplicando DRY)
    static void darEProcessarItem(Personagem* jogador, ItemID idItem, int quantidade, std::vector<std::string>& itensObtidos, int chanceDeDrop = 100);
};
