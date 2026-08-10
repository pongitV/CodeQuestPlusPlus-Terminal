#include "Domain/NPCs/NPCInteraction.h"
#include <iostream>
#include "Core/Utils/InputControl.h"
#include "UI/Screens/Inventory/InventoryScreen.h"
#include "Domain/Items/ItemFactory.h"
#include "Core/Utils/DialogFunctions.h"
#include "UI/Screens/BaseScreen.h"

// --- INTERACAO PRINCIPAL ---
void InteractionNPC::interact(Character* currentPlayer) {
    std::string option;
    
    do {
        Appearance::clearScreen();
        Appearance::displayTextPanel(getPlaceName(), getHeaderColor());

        int terminalWidth = Appearance::getTerminalWidth();
        
        const std::vector<std::string>& art = getArtASCII();
        Appearance::printBlockCentralized(art);
        std::cout << "\n";

        std::cout << Appearance::color(Color::YELLOW) << " Seu Ouro: " << currentPlayer->getInventory()->getGold() << "G\n" << Appearance::color(Color::RESET);

        displayDialogue(currentPlayer);
        std::cout << "\n";

        std::vector<std::string> options = getOptionsMenu(currentPlayer, terminalWidth);
        
        int choice = InputControl::readSelectionMenuWithArrows(options);

        if (choice >= 0 && choice < options.size()) {
            option = options[choice];
            if (option != "VOLTAR") {
                processOption(currentPlayer, option, terminalWidth);
            }
        }

    } while (option != "VOLTAR");
}

void InteractionNPC::processMenuMissionsEmpty(Character* currentPlayer, const std::string& titleMenu, Color colorHeader, const std::string& nameNPC, const std::string& speakEmpty) {
    std::string optionMission;
    do {
        std::vector<std::string> missions = {
            "(Nenhuma missao disponivel)",
            "VOLTAR"
        };
        
        int id = InputControl::readMenuSelectionInPopup(titleMenu, {"Escolha uma missao:"}, missions, colorHeader);
        if (id == -1) break;
        optionMission = missions[id];

        if (optionMission == "(Nenhuma missao disponivel)") {
            Appearance::displayPopup(nameNPC, {speakEmpty}, colorHeader);
        }
    } while (optionMission != "VOLTAR");
}

bool InteractionNPC::checkMaterialNoInventory(Character* currentPlayer, const std::string& nameMaterial, int quantityNecessary, const std::string& nameNPC, Color colorNPC, const std::string& messagePersonalized) {
    int qtyCurrent = currentPlayer->getInventory()->countItem(nameMaterial);
    if (qtyCurrent < quantityNecessary) {
        std::string msg = messagePersonalized.empty() 
            ? "Voce nao tem " + nameMaterial + " suficiente! (Possui: " + std::to_string(qtyCurrent) + "/" + std::to_string(quantityNecessary) + ")"
            : messagePersonalized;
        DialogueFunctions::printDialogueNPC(nameNPC, colorNPC, msg);
        InputControl::waitForEnter();
        return false;
    }
    return true;
}

Item* InteractionNPC::readItemFromInventory(Character* currentPlayer, const std::string& messageDialogue, const std::string& nameNPC, Color colorNPC, std::string& codeExit, bool displayPrices) {
    Item* itemSelected = nullptr;

    BaseScreen::executeLoop(
        [](bool animate) { ScreenInventory::displayHeaderInventory(animate); },
        [&]() {
            std::cout << "\n";
            Appearance::printCentralized(Appearance::color(colorNPC) + "[" + nameNPC + "]: " + Appearance::color(Color::WHITE) + messageDialogue + Appearance::color(Color::RESET));
            std::cout << "\n";
        },
        [currentPlayer, displayPrices]() {
            std::vector<std::string> options;
            options.push_back("Arsenal de Equipamentos");
            options.push_back("Itens Consumiveis");
            options.push_back("Estoque e Materiais");
            options.push_back("Itens de Missao");
            options.push_back("VOLTAR");
            return options;
        },
        [&](int catChoice) {
            if (catChoice < 0 || catChoice == 4) {
                codeExit = "0";
                return false;
            }
            
            BaseScreen::executeLoop(
                [](bool animate) { ScreenInventory::displayHeaderInventory(animate); },
                [&]() {
                    std::cout << "\n";
                    Appearance::printCentralized(Appearance::color(colorNPC) + "[" + nameNPC + "]: " + Appearance::color(Color::WHITE) + messageDialogue + Appearance::color(Color::RESET));
                    std::cout << "\n";
                },
                [&]() {
                    auto items = ScreenInventory::getListCategory(currentPlayer, catChoice, displayPrices);
                    std::vector<std::string> options;
                    for (auto& pair : items) options.push_back(pair.first);
                    options.push_back("VOLTAR");
                    return options;
                },
                [&](int itemChoice) {
                    auto items = ScreenInventory::getListCategory(currentPlayer, catChoice, displayPrices);
                    if (itemChoice < 0 || itemChoice >= static_cast<int>(items.size())) {
                        return false;
                    }
                    itemSelected = items[itemChoice].second;
                    codeExit = "selecionado";
                    return false;
                }
            );

            if (itemSelected) return false;
            return true;
        }
    );
    
    return itemSelected;
}

void InteractionNPC::displaySuccessScreen(const std::string& titleHeader, Color colorHeader, const std::string& equation, const std::vector<std::string>& asciiArt, const std::string& nameNPC, const std::string& speakNPC) {
    Appearance::clearScreen();
    Appearance::displayTextPanel(titleHeader, colorHeader);
    Appearance::printCentralizedMultiline({equation, ""}, 0, Appearance::color(colorHeader));
    if (!asciiArt.empty()) {
        Appearance::printCentralizedMultiline(asciiArt, 29, Appearance::color(colorHeader));
    }
    DialogueFunctions::printDialogueNPC(nameNPC, colorHeader, speakNPC);
    InputControl::waitForEnter();
}

std::string InteractionNPC::getFormatterStatusItem(ItemID id) {
    std::unique_ptr<Item> tempItem = ItemFactory::createItem(id);
    return tempItem ? tempItem->getInfoStatus() : "";
}

bool InteractionNPC::checkItemNoEquipped(Character* currentPlayer, Item* itemEvaluated, const std::string& nameNPC, Color colorNPC, const std::string& msgError) {
    if (currentPlayer->isItemEquipped(itemEvaluated)) {
        DialogueFunctions::printDialogueNPC(nameNPC, colorNPC, msgError);
        InputControl::waitForEnter();
        return false;
    }
    return true;
}
