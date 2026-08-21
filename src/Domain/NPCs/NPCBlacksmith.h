#pragma once

#include "Domain/Characters/Character.h"
#include "Domain/NPCs/NPCInteraction.h"
#include <string>
#include <vector>

class NPCBlacksmith : public InteractionNPC
{
public:
    void interact(Character* player);

protected:
    // [PT-BR] --- INFORMACOES DO LUGAR E APARENCIA ---
    // [EN-US] --- LOCATION INFORMATION AND APPEARANCE ---
    std::string getPlaceName() const override;
    Color getHeaderColor() const override;
    Color getArtColor() const override;
    const std::vector<std::string>& getArtASCII() const override;

    // [PT-BR] --- INTERACAO E MENU ---
    // [EN-US] --- INTERACTION AND MENU ---
    void displayDialogue(Character* player) override;
    std::vector<std::string> getOptionsMenu(Character* player, int terminalWidth) override;
    void processOption(Character* player, const std::string& option, int terminalWidth) override;
};
