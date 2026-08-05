#include "Domain/NPCs/NPCPriest.h"
#include "Domain/NPCs/NPCPriestLayout.h"
#include "Core/Engine/GameMenu.h"
#include "Core/Utils/InputControl.h"
#include <iostream>

void NPCPriest::interact(Character* player) {
    Appearance::startPopupInteraction();
    
    std::vector<std::string> lines = {
        "Que a paz e a luz dos deuses guiem seus passos, meu filho.",
        "Nesta sagrada igreja do reino, oferecemos refugio e cura para as almas fatigadas.",
        "Sinto uma aura extremamente sombria emanando do palacio real no norte...",
        "Prepare-se bem antes de desafiar o que quer que resida la."
    };
    Appearance::displayPopup("PADRE BENEDITO", lines, Color::CYAN, getArtASCII());

    while (true) {
        std::vector<std::string> options = getOptionsMenu(player, 80);
        int choice = InputControl::readMenuSelectionInPopup("PADRE BENEDITO", {"O que deseja fazer?"}, options, Color::CYAN);
        
        if (choice >= 0 && choice < (int)options.size()) {
            std::string option = options[choice];
            if (option == "Voltar") {
                break;
            }
            processOption(player, option, 80);
        } else {
            break;
        }
    }
}

std::string NPCPriest::getNameDoPlace() const {
    return "ALTAR DA IGREJA";
}

Color NPCPriest::getColorDoHeader() const {
    return Color::CYAN;
}

Color NPCPriest::getColorDaArt() const {
    return Color::CYAN;
}

const std::vector<std::string>& NPCPriest::getArtASCII() const {
    return NPCPriestLayouts::artPriest;
}

void NPCPriest::displayDialogue(Character* /*jogador*/) {
    // Agora chamado em interagir
}

std::vector<std::string> NPCPriest::getOptionsMenu(Character* player, int /*larguraDoTerminal*/) {
    return {
        "Pedir Bencao (Restaurar HP)",
        "Conversar sobre o Palacio",
        "Voltar"
    };
}

void NPCPriest::processOption(Character* player, const std::string& option, int /*larguraDoTerminal*/) {
    if (option == "Pedir Bencao (Restaurar HP)") {
        if (player->getHealth() >= player->getMaxHealth()) {
            Appearance::displayPopup("PADRE BENEDITO", {"Sua saúde já está plena! Guarde a bênção para quando precisar."}, Color::CYAN, getArtASCII());
        } else {
            player->modifyHealth(player->getMaxHealth());
            Appearance::displayPopup("BENCAO CONCEDIDA", {"O Padre recita preces sagradas. Uma luz quente envolve seu corpo!", "Seu HP foi totalmente restaurado!"}, Color::LIGHT_GREEN, getArtASCII());
        }
    }
    else if (option == "Conversar sobre o Palacio") {
        std::vector<std::string> lore = {
            "O palácio real costumava ser o farol de esperança do Reino.",
            "Contudo, há algumas semanas, o rei trancou-se em seus aposentos",
            "e ordenou que guardas mágicos selassem a entrada.",
            "Ninguém entra ou sai. Barulhos aterrorizantes são ouvidos à noite.",
            "Temo que o pior tenha acontecido com a realeza..."
        };
        Appearance::displayPopup("LORE DO PALACIO", lore, Color::CYAN, getArtASCII());
    }
}
