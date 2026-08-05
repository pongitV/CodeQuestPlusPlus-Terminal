/*
 * Arquivo: Drops.h
 * Propósito: Gerencia a distribuição e exibição de recompensas (XP, Ouro e Itens) resultantes de combates.
 */

#pragma once

#include <string>
#include <vector>

class Character;
enum class ItemID;

class Drops 
{
public:
    /*
     * Processa e exibe as recompensas de XP e Ouro concedidas ao personagem.
     */
    static void reportAndProcessXpGold(Character* player, int xpDrop, int goldDrop, int& totalGold, int& totalXp);
    
    /*
     * Formata e exibe a mensagem de recebimento de um item dropado.
     */
    static void reportItemDrop(const std::string& itemName, int quantity);

    /*
     * Tenta conceder um item ao jogador com base em uma chance de drop (0-100%).
     */
    static void giveAndProcessItem(Character* player, ItemID itemId, int quantity, std::vector<std::string>& obtainedItems, int dropChance = 100);
};

