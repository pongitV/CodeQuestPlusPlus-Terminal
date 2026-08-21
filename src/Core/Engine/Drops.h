// [PT-BR] Arquivo: Drops.h
// [PT-BR] Proposito: Gerencia a distribuicao e exibicao de recompensas (XP, Ouro e Itens) resultantes de combates.
// [EN-US] File: Drops.h
// [EN-US] Purpose: Manages distribution and display of rewards (XP, Gold, and Items) from combat.

#pragma once

#include <string>
#include <vector>

class Character;
enum class ItemID;

class Drops 
{
public:
    // [PT-BR] Processa e exibe as recompensas de XP e Ouro concedidas ao personagem
    // [EN-US] Processes and displays XP and Gold rewards granted to the character
    static void reportAndProcessXpGold(Character* player, int xpDrop, int goldDrop, int& totalGold, int& totalXp);
    
    // [PT-BR] Formata e exibe a mensagem de recebimento de um item dropado
    // [EN-US] Formats and displays the received message for a dropped item
    static void reportItemDrop(const std::string& itemName, int quantity);

    // [PT-BR] Tenta conceder um item ao player com base em uma chance de drop (0-100%)
    // [EN-US] Attempts to award an item to the player based on a drop chance (0-100%)
    static void giveAndProcessItem(Character* player, ItemID itemId, int quantity, std::vector<std::string>& obtainedItems, int dropChance = 100);
};
