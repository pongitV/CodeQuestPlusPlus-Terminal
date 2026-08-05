/*
 * Arquivo: NPCInteraction.h
 * Propósito: Interface abstrata para interações de menu e diálogos com NPCs (Ferreiro, Alquimista, Mercador).
 */

#pragma once

#include <string>
#include <vector>
#include "Core/Utils/Appearance.h"
#include "Domain/Items/Item.h"

class Character;

/*
 * Interface de contrato para comportamentos de diálogo e transações com NPCs.
 */
class InteractionNPC {
public:
    virtual ~InteractionNPC() = default;

    virtual std::string getNameDoPlace() const = 0;
    virtual Color getColorDoHeader() const = 0;
    virtual void displayDialogue(Character* currentPlayer) = 0;
    virtual std::vector<std::string> getOptionsMenu(Character* currentPlayer, int terminalWidth) = 0;
    virtual void processOption(Character* currentPlayer, const std::string& option, int terminalWidth) = 0;
    virtual Color getColorDaArt() const = 0;
    virtual const std::vector<std::string>& getArtASCII() const = 0;

    void interact(Character* currentPlayer);

    static void processMenuMissionsEmpty(Character* currentPlayer, const std::string& titleMenu, Color colorHeader, const std::string& nameNPC, const std::string& speakEmpty);
    static bool checkMaterialNoInventory(Character* currentPlayer, const std::string& nameMaterial, int quantityNecessary, const std::string& nameNPC, Color colorNPC, const std::string& messagePersonalized = "");
    static bool checkItemNoEquipped(Character* currentPlayer, Item* itemEvaluated, const std::string& nameNPC, Color colorNPC, const std::string& msgError);
    static Item* readItemDoInventory(Character* currentPlayer, const std::string& messageDialogue, const std::string& nameNPC, Color colorNPC, std::string& codeExit, bool displayPrices = false);
    static void displayScreenDeSuccess(const std::string& titleHeader, Color colorHeader, const std::string& equation, const std::vector<std::string>& asciiArt, const std::string& nameNPC, const std::string& speakNPC);
    static std::string getFormatterStatusItem(ItemID id);
};
