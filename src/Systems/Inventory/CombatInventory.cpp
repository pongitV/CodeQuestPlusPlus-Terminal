#include "Systems/Inventory/CombatInventory.h"
#include "UI/PerspectiveManager.h"
#include "Domain/Characters/Character.h"
#include "Domain/Items/Item.h"
#include "Domain/Items/Equipment/WeaponEquipment.h"
#include "Domain/Items/Equipment/ShieldEquipment.h"
#include "Domain/Items/Equipment/ArmorEquipment.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "UI/Screens/BaseScreen.h"
#include "UI/Screens/Inventory/InventoryScreen.h"
#include "UI/Screens/Combat/CombatScreen.h"
#include "Core/Utils/DialogFunctions.h"
#include "Systems/Inventory/InventoryControl.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterFrame.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>

enum StateInventory { MAIN, ARSENAL, CONSUMABLES, STOCK, MISSION };

static int readSelectionPopupInventory(const std::string& title, const std::vector<std::string>& text, const std::vector<std::string>& options) {
    int selectionCurrent = 0;
    int totalOptions = options.size();
    bool is3D = PerspectiveManager::getInstance().is3DViewActive();
    
    InputControl::clearBuffer();

    while (true) {
        std::vector<std::string> lines;
        for (const auto& t : text) {
            lines.push_back(" " + t + " ");
        }
        lines.push_back("");
        for (int i = 0; i < totalOptions; ++i) {
            if (i == selectionCurrent) {
                lines.push_back(Appearance::color(Color::GREEN) + " > " + options[i] + Appearance::color(Color::WHITE) + (is3D ? "\033[48;2;25;25;25m" : ""));
            } else {
                lines.push_back("   " + options[i]);
            }
        }

        std::vector<std::string> boxEnd = BaseScreen::createBox(lines, title, 0, Color::YELLOW, is3D ? "\033[48;2;25;25;25m" : "");
        int outW = Appearance::getVisualLength(boxEnd[0]);
        int outH = boxEnd.size();
        int startX = (Appearance::getTerminalWidth() - outW) / 2;
        int startY = (Appearance::getTerminalHeight() - outH) / 2;
        
        if (startX < 0) startX = 0;
        if (is3D && startY < 8) startY = 8;
        
        std::cout << "\033[?25l";
        for (size_t i = 0; i < boxEnd.size(); ++i) {
            Appearance::moveCursor(startX, startY + i);
            std::cout << boxEnd[i];
        }
        std::cout << std::flush;

        char key = InputControl::readKey();
        if (key == 'w' || key == 'W') {
            selectionCurrent--;
            if (selectionCurrent < 0) selectionCurrent = totalOptions - 1;
        } else if (key == 's' || key == 'S') {
            selectionCurrent++;
            if (selectionCurrent >= totalOptions) selectionCurrent = 0;
        } else if (key == '\r' || key == '\n') {
            return selectionCurrent;
        }
    }
}

static void displayMessagePopupInventory(const std::string& title, const std::vector<std::string>& text) {
    readSelectionPopupInventory(title, text, {"[ VOLTAR ]"});
}

static void displayResultItem(const UseItemInfo& info, Item* item, bool* shiftWasConsumed) {
    switch (info.result) {
        case ResultItem::Error_ShiftJaUsed:
            displayMessagePopupInventory("SISTEMA", {"Voce ja usou um item neste turno!"});
            break;
        case ResultItem::Error_ShieldBroken: {
            std::string msg = DialogueFunctions::formatSystemMsg("O escudo [" + info.itemName + "] esta quebrado e nao pode ser equipado!", Color::RED);
            displayMessagePopupInventory("SISTEMA", {msg});
            break;
        }
        case ResultItem::Error_Requirements:
            displayMessagePopupInventory("SISTEMA", {info.messageExtra});
            break;
        case ResultItem::Unequipped:
            displayMessagePopupInventory("SISTEMA", {info.itemName + " desequipado(a)!"});
            if (shiftWasConsumed) *shiftWasConsumed = true;
            displayMessagePopupInventory("SISTEMA", {"Turno gasto alterando um equipamento..."});
            break;
        case ResultItem::Equipped:
            displayMessagePopupInventory("SISTEMA", {info.itemName + " equipado(a)!"});
            if (shiftWasConsumed) *shiftWasConsumed = true;
            displayMessagePopupInventory("SISTEMA", {"Turno gasto alterando um equipamento..."});
            break;
        case ResultItem::Used_Shift:
            if (shiftWasConsumed) *shiftWasConsumed = true;
            break;
        case ResultItem::Used_WithoutShift:
            break;
        case ResultItem::Error_NoCanUse:
            displayMessagePopupInventory("SISTEMA", {ControlInventory::getMessageError(item, shiftWasConsumed != nullptr)});
            break;
        default: break;
    }
}

static int readWholePopupInventory(const std::string& title, const std::string& message, int min, int max) {
    bool is3D = PerspectiveManager::getInstance().is3DViewActive();
    std::string currentInput = "";

    while (true) {
        std::vector<std::string> lines;
        lines.push_back(" " + message + " ");
        lines.push_back("");
        lines.push_back(Appearance::color(Color::YELLOW) + " >> " + currentInput + "_" + Appearance::color(Color::WHITE) + (is3D ? "\033[48;2;25;25;25m" : ""));
        lines.push_back("");
        lines.push_back(" [ENTER para confirmar]");

        std::vector<std::string> boxEnd = BaseScreen::createBox(lines, title, 0, Color::YELLOW, is3D ? "\033[48;2;25;25;25m" : "");
        int outW = Appearance::getVisualLength(boxEnd[0]);
        int outH = boxEnd.size();
        int startX = (Appearance::getTerminalWidth() - outW) / 2;
        int startY = (Appearance::getTerminalHeight() - outH) / 2;
        
        if (startX < 0) startX = 0;
        if (is3D && startY < 8) startY = 8;
        
        std::cout << "\033[?25l";
        for (size_t i = 0; i < boxEnd.size(); ++i) {
            Appearance::moveCursor(startX, startY + i);
            std::cout << boxEnd[i];
        }
        std::cout << std::flush;

        char key = InputControl::readKey();
        if (key >= '0' && key <= '9') {
            currentInput += key;
        } else if (key == '\b' && !currentInput.empty()) {
            currentInput.pop_back();
            if (is3D) RaycasterFrame::restoreLastFrame();
            else Appearance::clearScreen();
        } else if (key == '\r' || key == '\n') {
            if (currentInput.empty()) return min;
            int val = std::stoi(currentInput);
            if (val < min) return min;
            if (val > max) return max;
            return val;
        }
    }
}

void CombatInventory::manageInventory(Character* currentPlayer, bool* shiftWasConsumed)
{
    if (currentPlayer == nullptr) return;
    
    StateInventory state = MAIN;
    int selectionCurrent = 0;
    int selectionSub = 0;
    bool running = true;
    
    std::vector<Item*> mapIndexForItem;

    bool redesignCompleteInv = true;

    while (running) {
        std::cout << "\033[?25l";
        bool is3D = PerspectiveManager::getInstance().is3DViewActive();
        
        std::vector<std::string> lines;
        std::string titleBox = "";
        std::vector<std::string> interactive;
        std::vector<int> indicesReal;
        
        if (state == MAIN) {
            titleBox = " MENU DE BOLSOS ";
            std::string strPocket = "BOLSO: " + std::to_string(currentPlayer->getInventory()->getGold()) + " Moedas de Ouro [$$]";
            
            std::vector<std::string> optionsBase;
            if (currentPlayer->getConsumableQuickly()) {
                int qty = currentPlayer->getInventory()->countItem(currentPlayer->getConsumableQuickly()->getItemName());
                optionsBase.push_back(Appearance::color(Color::GREEN) + "[+] " + Appearance::color(Color::WHITE) + "Acesso Rapido: " + currentPlayer->getConsumableQuickly()->getItemName() + " (" + std::to_string(qty) + "x)");
            }
            optionsBase.push_back("Arsenal de Equipamentos");
            optionsBase.push_back("Itens Consumiveis");
            optionsBase.push_back("Estoque e Materiais");
            optionsBase.push_back("Itens de Missao");
            optionsBase.push_back("");
            optionsBase.push_back(strPocket);
            optionsBase.push_back("");
            optionsBase.push_back("[<] VOLTAR");
            
            for (size_t i = 0; i < optionsBase.size(); ++i) {
                if (optionsBase[i].empty() || optionsBase[i].find("BOLSO:") != std::string::npos || optionsBase[i].substr(0, 3) == "   ") {
                    lines.push_back("   " + optionsBase[i]);
                } else {
                    interactive.push_back(optionsBase[i]);
                    indicesReal.push_back(i);
                    if (interactive.size() - 1 == selectionCurrent) {
                        lines.push_back(Appearance::color(Color::GREEN) + " > " + optionsBase[i] + Appearance::color(Color::WHITE) + (is3D ? "\033[48;2;25;25;25m" : ""));
                    } else {
                        lines.push_back("   " + optionsBase[i]);
                    }
                }
            }
        } else if (state == ARSENAL) {
            titleBox = " ARSENAL DE EQUIPAMENTOS ";

            mapIndexForItem.clear();

            Item* weaponEq = currentPlayer->getWeapons();
            Item* armorEq = currentPlayer->getArmor();
            Item* shieldEq = currentPlayer->getShield();

            auto everyoneItems = currentPlayer->getInventory()->getAllItems();
            std::vector<Item*> weapons, armor, shields;
            for (auto* item : everyoneItems) {
                if (item == weaponEq || item == armorEq || item == shieldEq) continue;
                EquipmentType type = item->getType();
                if (type == EquipmentType::WEAPONS) weapons.push_back(item);
                else if (type == EquipmentType::ARMOR) armor.push_back(item);
                else if (type == EquipmentType::SHIELD) shields.push_back(item);
            }

            Appearance::sortAlphabetically(weapons, [](Item* a) { return a->getItemName(); });
            Appearance::sortAlphabetically(armor, [](Item* a) { return a->getItemName(); });
            Appearance::sortAlphabetically(shields, [](Item* a) { return a->getItemName(); });

            std::string colorDiv = Appearance::color(Color::YELLOW);
            std::string colorReset = Appearance::color(Color::RESET);

            auto addItem = [&](const std::string& name, Item* item) {
                int idx = (int)interactive.size();
                interactive.push_back(name);
                indicesReal.push_back((int)mapIndexForItem.size());
                mapIndexForItem.push_back(item);
                if (idx == selectionSub)
                    lines.push_back(Appearance::color(Color::GREEN) + " > " + name + Appearance::color(Color::WHITE) + (is3D ? "\033[48;2;25;25;25m" : ""));
                else
                    lines.push_back("   " + name);
            };

            auto addGroup = [&](const std::string& label, std::vector<Item*>& group) {
                if (group.empty()) return;
                lines.push_back(" " + colorDiv + "--- " + label + " ---" + colorReset);
                for (auto* item : group) {
                    auto itemsGrouped = currentPlayer->getInventory()->countItem(item->getItemName());
                    std::string prefix = (itemsGrouped > 1) ? std::to_string(itemsGrouped) + "x " : "";
                    addItem(prefix + item->getItemName(), item);
                }
                lines.push_back("");
            };

            // Equipados (non-interactive)
            lines.push_back(" " + colorDiv + "--- Equipados ---" + colorReset);
            bool hasEq = false;
            auto addEq = [&](const std::string& label, Item* item) {
                if (!item) return;
                hasEq = true;
                std::string name = item->getItemName();
                // also add to interativos so it can be selected
                int idx = (int)interactive.size();
                interactive.push_back("(E) " + name);
                indicesReal.push_back((int)mapIndexForItem.size());
                mapIndexForItem.push_back(item);
                if (idx == selectionSub)
                    lines.push_back(Appearance::color(Color::GREEN) + " > " + Appearance::color(Color::GREEN) + "[E] " + Appearance::color(Color::RESET) + label + ": " + name + Appearance::color(Color::WHITE) + (is3D ? "\033[48;2;25;25;25m" : ""));
                else
                    lines.push_back("   " + Appearance::color(Color::GREEN) + "[E] " + Appearance::color(Color::RESET) + label + ": " + name);
            };
            addEq("Arma", weaponEq);
            addEq("Armadura", armorEq);
            addEq("Escudo", shieldEq);
            if (!hasEq) lines.push_back("   " + Appearance::color(Color::GRAY) + "(Nada equipado)" + colorReset);
            lines.push_back("");

            addGroup("Armas", weapons);
            addGroup("Armaduras", armor);
            addGroup("Escudos", shields);

            interactive.push_back("[<] VOLTAR");
            indicesReal.push_back(-1);
            if ((int)interactive.size() - 1 == selectionSub)
                lines.push_back(Appearance::color(Color::GREEN) + " > [<] VOLTAR" + Appearance::color(Color::WHITE) + (is3D ? "\033[48;2;25;25;25m" : ""));
            else
                lines.push_back("   [<] VOLTAR");

        } else {
            int category = 0;
            if (state == CONSUMABLES) { titleBox = " ITENS CONSUMIVEIS "; category = 1; }
            else if (state == STOCK) { titleBox = " ESTOQUE E MATERIAIS "; category = 2; }
            else if (state == MISSION) { titleBox = " ITENS DE MISSAO "; category = 3; }

            auto items = ScreenInventory::getListCategory(currentPlayer, category, false);
            Appearance::sortAlphabetically(items, [](const auto& pair) { return pair.first; });

            mapIndexForItem.clear();

            if (!items.empty()) {
                for (const auto& p : items) {
                    interactive.push_back(p.first);
                    indicesReal.push_back((int)mapIndexForItem.size());
                    mapIndexForItem.push_back(p.second);

                    if ((int)interactive.size() - 1 == selectionSub) {
                        lines.push_back(Appearance::color(Color::GREEN) + " > " + p.first + Appearance::color(Color::WHITE) + (is3D ? "\033[48;2;25;25;25m" : ""));
                    } else {
                        lines.push_back("   " + p.first);
                    }
                }
            } else {
                lines.push_back("   " + Appearance::color(Color::GRAY) + "Nenhum item nesta categoria." + Appearance::color(Color::RESET));
            }
            lines.push_back("");

            interactive.push_back("[<] VOLTAR");
            indicesReal.push_back(-1);
            if ((int)interactive.size() - 1 == selectionSub) {
                lines.push_back(Appearance::color(Color::GREEN) + " > [<] VOLTAR" + Appearance::color(Color::WHITE) + (is3D ? "\033[48;2;25;25;25m" : ""));
            } else {
                lines.push_back("   [<] VOLTAR");
            }
        }
        
        int totalOptions = interactive.size();
        int* selRef = (state == MAIN) ? &selectionCurrent : &selectionSub;
        if (*selRef >= totalOptions && totalOptions > 0) *selRef = totalOptions - 1;
        
        std::vector<std::string> boxEnd = BaseScreen::createBox(lines, titleBox, 0, Color::YELLOW, is3D ? "\033[48;2;25;25;25m" : "");
        int outW = Appearance::getVisualLength(boxEnd[0]);
        int outH = boxEnd.size();
        
        if (is3D) {
            if (redesignCompleteInv) RaycasterFrame::restoreLastFrame();
            int startX = (Appearance::getTerminalWidth() - outW) / 2;
            int startY = (Appearance::getTerminalHeight() - outH) / 2;
            if (startX < 0) startX = 0;
            if (startY < 8) startY = 8;
            
            PerspectiveManager::getInventoryUI().displayHeader(false, startY);
            
            for (size_t i = 0; i < boxEnd.size(); ++i) {
                Appearance::moveCursor(startX, startY + i);
                std::cout << boxEnd[i];
            }
        } else {
            Appearance::clearScreen();
            ScreenInventory::displayHeaderInventory(false);
            ScreenInventory::displayBoxEquipped(currentPlayer);
            std::cout << "\n";
            for (const auto& l : boxEnd) {
                std::cout << Appearance::spacesToCenter(Appearance::getVisualLength(l)) << l << "\n";
            }
        }
        std::cout << std::flush;
        
        redesignCompleteInv = false;
        char key = InputControl::readKey();
        if (key == 'w' || key == 'W') {
            (*selRef)--;
            if (*selRef < 0) *selRef = totalOptions - 1;
        } else if (key == 's' || key == 'S') {
            (*selRef)++;
            if (*selRef >= totalOptions) *selRef = 0;
        } else if (key == '\n' || key == '\r') {
            redesignCompleteInv = true;
            if (totalOptions > 0) {
                if (state == MAIN) {
                    int offset = currentPlayer->getConsumableQuickly() ? 1 : 0;
                    int escLogic = indicesReal[*selRef];
                    
                    if (escLogic == 7 + offset) {
                        running = false;
                    } else if (offset == 1 && escLogic == 0) {
                        // Consumivel rapido
                        Item* quickly = currentPlayer->getConsumableQuickly();
                        std::string nameQuickly = quickly->getItemName();
                        int countBefore = currentPlayer->getInventory()->countItem(nameQuickly);
                        if (countBefore > 0) {
                            bool shiftJaUsed = shiftWasConsumed && *shiftWasConsumed;
                            UseItemInfo info = ControlInventory::useOuEquip(currentPlayer, quickly, shiftJaUsed);
                            if (shiftWasConsumed && info.consumiuShift) *shiftWasConsumed = true;
                            if (currentPlayer->getItemSelectedForUse() != nullptr) {
                                running = false;
                            }
                            if (currentPlayer->getInventory()->countItem(nameQuickly) == 0) {
                                currentPlayer->unequipConsumable();
                            }
                        } else {
                            currentPlayer->unequipConsumable();
                        }
                        if (shiftWasConsumed && *shiftWasConsumed) running = false;
                        
                        if (is3D) RaycasterFrame::restoreLastFrame();
                    } else {
                        int cat = escLogic - offset;
                        if (cat == 0) state = ARSENAL;
                        else if (cat == 1) state = CONSUMABLES;
                        else if (cat == 2) state = STOCK;
                        else if (cat == 3) state = MISSION;
                        selectionSub = 0;
                        if (is3D) RaycasterFrame::restoreLastFrame();
                    }
                } else {
                    int idx = indicesReal[*selRef];
                    if (idx == -1) {
                        state = MAIN;
                        if (is3D) RaycasterFrame::restoreLastFrame();
                    } else {
                        Item* itemFound = mapIndexForItem[idx];
                        bool ehEquipable = itemFound->isEquipable();
                        
                        bool submenuOpen = true;
                        while(submenuOpen) {
                            int subOption = readSelectionPopupInventory(
                                "OPCOES DE ITEM", 
                                {"O que deseja fazer com:", Appearance::color(Color::YELLOW) + ">> " + itemFound->getItemName() + " <<" + Appearance::color(Color::RESET)}, 
                                {"Usar / Equipar", "Inspecionar", "[ VOLTAR ]"}
                            );
                            
                            if (subOption == 2) { // VOLTAR
                                submenuOpen = false;
                                if (is3D) RaycasterFrame::restoreLastFrame();
                                break;
                            } else if (subOption == 0) { // Usar / Equipar
                                bool shiftJaUsed = shiftWasConsumed && *shiftWasConsumed;
                                if (ehEquipable) {
                                    UseItemInfo info = ControlInventory::useOuEquip(currentPlayer, itemFound, shiftJaUsed);
                                    displayResultItem(info, itemFound, shiftWasConsumed);
                                } else {
                                    int qtyAvailable = currentPlayer->getInventory()->countItem(itemFound->getItemName());
                                    int quantityForUse = 1;
                                    
                                    if (qtyAvailable > 1) {
                                        int qtyChoice = readSelectionPopupInventory(
                                            "QUANTIDADE: " + itemFound->getItemName(),
                                            {"Voce possui " + std::to_string(qtyAvailable) + " unidades deste item."},
                                            {"Usar UMA unidade", "Usar TODAS as unidades", "Usar quantidade ESPECIFICA", "[ CANCELAR ]"}
                                        );
                                        
                                        if (qtyChoice == 0) {
                                            quantityForUse = 1;
                                        } else if (qtyChoice == 1) {
                                            quantityForUse = qtyAvailable;
                                        } else if (qtyChoice == 2) {
                                            std::string qtyMsg = "Quantidade (1 a " + std::to_string(qtyAvailable) + ", 0 cancelar): ";
                                            quantityForUse = readWholePopupInventory("QUANTIDADE", qtyMsg, 0, qtyAvailable);
                                        } else {
                                            if (is3D) RaycasterFrame::restoreLastFrame();
                                            continue; 
                                        }
                                    }
                                    
                                    if (quantityForUse <= 0) {
                                        if (is3D) RaycasterFrame::restoreLastFrame();
                                        continue;
                                    }
                                    
                                    std::string itemName = itemFound->getItemName();
                                    int countBefore = currentPlayer->getInventory()->countItem(itemName);
                                    bool consumiuSomeShift = false;
                                    
                                    for (int i = 0; i < quantityForUse; ++i) {
                                        shiftJaUsed = shiftWasConsumed && *shiftWasConsumed;
                                        UseItemInfo info = ControlInventory::useOuEquip(currentPlayer, itemFound, shiftJaUsed);
                                        if (info.consumiuShift) consumiuSomeShift = true;
                                        
                                        if (currentPlayer->getItemSelectedForUse() != nullptr) {
                                            if (quantityForUse > 1) {
                                                displayMessagePopupInventory("SISTEMA", {"Este item requer selecao de alvo e", "sera usado apenas uma vez."});
                                            }
                                            break;
                                        }
                                        
                                        int countAfter = currentPlayer->getInventory()->countItem(itemName);
                                        if (countAfter == countBefore && !ehEquipable) {
                                            break;
                                        }
                                    }
                                    
                                    if (currentPlayer->getConsumableQuickly() && currentPlayer->getInventory()->countItem(currentPlayer->getConsumableQuickly()->getItemName()) == 0) {
                                        currentPlayer->unequipConsumable();
                                    }

                                    if (shiftWasConsumed && consumiuSomeShift) {
                                        *shiftWasConsumed = true;
                                    }
                                }
                                submenuOpen = false; 
                                if (is3D) RaycasterFrame::restoreLastFrame();
                            } else if (subOption == 1) { // Inspecionar
                                std::vector<std::string> details = itemFound->getDetailsInspection(currentPlayer);
                                std::vector<std::string> linesInsp;
                                linesInsp.push_back(Appearance::color(Color::YELLOW) + " >> " + itemFound->getItemName() + " <<" + Appearance::color(Color::RESET));
                                linesInsp.push_back("");
                                linesInsp.insert(linesInsp.end(), details.begin(), details.end());
                                
                                displayMessagePopupInventory("INSPECAO DE ITEM", linesInsp);
                                if (is3D) RaycasterFrame::restoreLastFrame();
                            }
                        }
                        
                        if (shiftWasConsumed && *shiftWasConsumed) running = false;
                        if (is3D) RaycasterFrame::restoreLastFrame();
                    }
                }
            }
        }
    }
}


