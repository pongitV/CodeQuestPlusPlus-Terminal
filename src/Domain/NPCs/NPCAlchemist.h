#pragma once

#include "Domain/Characters/Character.h"
#include <string>
#include <vector>
#include "Domain/NPCs/NPCInteraction.h"

class NPCAlchemist : public InteractionNPC
{
public:
    void interact(Character* player);

protected:
    std::string getPlaceName() const override;
    Color getHeaderColor() const override;
    Color getArtColor() const override;
    const std::vector<std::string>& getArtASCII() const override;

    void displayDialogue(Character* player) override;
    std::vector<std::string> getOptionsMenu(Character* player, int terminalWidth) override;
    void processOption(Character* player, const std::string& option, int terminalWidth) override;
};
