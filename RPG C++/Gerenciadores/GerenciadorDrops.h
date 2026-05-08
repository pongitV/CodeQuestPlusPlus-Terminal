#pragma once

#include <string>
#include <vector>

class SistemaPersonagem;
enum class ItemID;

class GerenciadorDrops 
{
public:
    // Centraliza o calculo e as mensagens na tela para as recompensas dos monstros
    static void relatarEProcessarXpOuro(SistemaPersonagem* jogador, int xpDrop, int ouroDrop, int& ouroTotal, int& xpTotal);
    
    // Padroniza a mensagem verde ou branca do recebimento de um item 
    static void relatarDropItem(const std::string& nomeItem, int quantidade);

    // Delega a responsabilidade de dar o item e processar a string no array (Aplicando DRY)
    static void darEProcessarItem(SistemaPersonagem* jogador, ItemID idItem, int quantidade, std::vector<std::string>& itensObtidos);
};
