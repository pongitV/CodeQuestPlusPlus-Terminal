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
        int outH = (int)boxEnd.size();
        int termW = Appearance::getTerminalWidth();
        int termH = Appearance::getTerminalHeight();
        int startX = std::max(0, (termW - outW) / 2);
        int startY = std::max(0, (termH - outH) / 2);
        
        if (startY + outH > termH) startY = std::max(0, termH - outH);
        if (startX + outW > termW) startX = std::max(0, termW - outW);
        
        std::cout << "\033[?25l";
        for (size_t i = 0; i < boxEnd.size(); ++i) {
            if (startY + (int)i < termH) {
                Appearance::moveCursor(startX, startY + i);
                std::cout << boxEnd[i];
            }
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
        case ResultItem::Error_TurnAlreadyUsed:
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
            if (shiftWasConsumed) {
                *shiftWasConsumed = true;
                displayMessagePopupInventory("SISTEMA", {"Turno gasto alterando um equipamento..."});
            }
            break;
        case ResultItem::Equipped:
            displayMessagePopupInventory("SISTEMA", {info.itemName + " equipado(a)!"});
            if (shiftWasConsumed) {
                *shiftWasConsumed = true;
                displayMessagePopupInventory("SISTEMA", {"Turno gasto alterando um equipamento..."});
            }
            break;
        case ResultItem::Used_Shift:
            if (shiftWasConsumed) *shiftWasConsumed = true;
            break;
        case ResultItem::Used_WithoutShift:
            break;
        case ResultItem::Error_CannotUse:
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
        int outH = (int)boxEnd.size();
        int termW = Appearance::getTerminalWidth();
        int termH = Appearance::getTerminalHeight();
        int startX = std::max(0, (termW - outW) / 2);
        int startY = std::max(0, (termH - outH) / 2);
        
        if (startY + outH > termH) startY = std::max(0, termH - outH);
        if (startX + outW > termW) startX = std::max(0, termW - outW);
        
        std::cout << "\033[?25l";
        for (size_t i = 0; i < boxEnd.size(); ++i) {
            if (startY + (int)i < termH) {
                Appearance::moveCursor(startX, startY + i);
                std::cout << boxEnd[i];
            }
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
    
    std::vector<Item*> itemIndexMap;

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

            itemIndexMap.clear();

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
                indicesReal.push_back((int)itemIndexMap.size());
                itemIndexMap.push_back(item);
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

            // [PT-BR] Equipados (nao interativos)
            // [EN-US] Equipped items (non-interactive)
            lines.push_back(" " + colorDiv + "--- Equipados ---" + colorReset);
            bool hasEq = false;
            auto addEq = [&](const std::string& label, Item* item) {
                if (!item) return;
                hasEq = true;
                std::string name = item->getItemName();
                // [PT-BR] Adiciona aos interativos para permitir selecao
                // [EN-US] Adds to interactives to enable selection
                int idx = (int)interactive.size();
                interactive.push_back("(E) " + name);
                indicesReal.push_back((int)itemIndexMap.size());
                itemIndexMap.push_back(item);
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

            itemIndexMap.clear();

            if (!items.empty()) {
                for (const auto& p : items) {
                    interactive.push_back(p.first);
                    indicesReal.push_back((int)itemIndexMap.size());
                    itemIndexMap.push_back(p.second);

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
            int termW = Appearance::getTerminalWidth();
            int termH = Appearance::getTerminalHeight();
            
            // [PT-BR] Altura aproximada do cabecalho do inventario
            // [EN-US] Approximate height of inventory header
            int soonHeight = 8;
            int totalH = outH + soonHeight + 1;
            int startY = 0;
            if (termH > totalH) {
                startY = (termH - totalH) / 2 + soonHeight + 1;
            } else {
                startY = std::max(0, (termH - outH) / 2);
            }
            if (startY + outH > termH) startY = std::max(0, termH - outH);
            
            int startX = std::max(0, (termW - outW) / 2);
            if (startX + outW > termW) startX = std::max(0, termW - outW);
            
            PerspectiveManager::getInventoryUI().displayHeader(false, startY);
            
            for (size_t i = 0; i < boxEnd.size(); ++i) {
                if (startY + (int)i < termH) {
                    Appearance::moveCursor(startX, startY + i);
                    std::cout << boxEnd[i];
                }
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
                        // [PT-BR] --- CONSUMIVEL RAPIDO ---
                        // [EN-US] --- QUICK CONSUMABLE ---
                        Item* quickly = currentPlayer->getConsumableQuickly();
                        std::string quickItemName = quickly->getItemName();
                        int countBefore = currentPlayer->getInventory()->countItem(quickItemName);
                        if (countBefore > 0) {
                            int quickChoice = readSelectionPopupInventory(
                                "ACESSO RAPIDO",
                                {"Item em Acesso Rapido:", Appearance::color(Color::YELLOW) + ">> " + quickItemName + " (" + std::to_string(countBefore) + "x) <<" + Appearance::color(Color::RESET)},
                                {"Usar 1 unidade", "Desequipar do Acesso Rapido", "Inspecionar", "[ VOLTAR ]"}
                            );

                            if (quickChoice == 0) {
                                bool turnAlreadyUsed = shiftWasConsumed && *shiftWasConsumed;
                                UseItemInfo info = ControlInventory::useOrEquip(currentPlayer, quickly, turnAlreadyUsed);
                                if (shiftWasConsumed && info.consumedTurn) *shiftWasConsumed = true;
                                if (currentPlayer->getItemSelectedForUse() != nullptr) {
                                    running = false;
                                }
                                if (currentPlayer->getInventory()->countItem(quickItemName) == 0) {
                                    currentPlayer->unequipConsumable();
                                }
                                if (shiftWasConsumed && *shiftWasConsumed) running = false;
                            } else if (quickChoice == 1) {
                                currentPlayer->unequipConsumable();
                                displayMessagePopupInventory("SISTEMA", {quickItemName + " desequipado(a) do Acesso Rapido!"});
                            } else if (quickChoice == 2) {
                                std::vector<std::string> details = quickly->getDetailsInspection(currentPlayer);
                                std::vector<std::string> inspectionLines;
                                inspectionLines.push_back(Appearance::color(Color::YELLOW) + " >> " + quickItemName + " <<" + Appearance::color(Color::RESET));
                                inspectionLines.push_back("");
                                inspectionLines.insert(inspectionLines.end(), details.begin(), details.end());
                                displayMessagePopupInventory("INSPECAO DE ITEM", inspectionLines);
                            }
                        } else {
                            currentPlayer->unequipConsumable();
                        }
                        
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
                        Item* foundItem = itemIndexMap[idx];
                        bool isEquipable = foundItem->isEquipable();
                        bool isConsumable = (foundItem->getType() == EquipmentType::CONSUMABLE);
                        
                        bool submenuOpen = true;
                        while(submenuOpen) {
                            bool isQuickEquipped = (currentPlayer->getConsumableQuickly() && 
                                                   currentPlayer->getConsumableQuickly()->getItemName() == foundItem->getItemName());
                            
                            std::vector<std::string> optionsMenu;
                            if (isConsumable) {
                                optionsMenu = {
                                    "Usar",
                                    isQuickEquipped ? "Desequipar do Acesso Rapido" : "Equipar no Acesso Rapido",
                                    "Inspecionar",
                                    "[ VOLTAR ]"
                                };
                            } else {
                                optionsMenu = {"Usar / Equipar", "Inspecionar", "[ VOLTAR ]"};
                            }

                            int subOption = readSelectionPopupInventory(
                                "OPCOES DE ITEM", 
                                {"O que deseja fazer com:", Appearance::color(Color::YELLOW) + ">> " + foundItem->getItemName() + " <<" + Appearance::color(Color::RESET)}, 
                                optionsMenu
                            );
                            
                            if (isConsumable) {
                                if (subOption == 3) {
                                    submenuOpen = false;
                                    if (is3D) RaycasterFrame::restoreLastFrame();
                                    break;
                                } else if (subOption == 1) {
                                    // [PT-BR] Equipar ou Desequipar Acesso Rapido
                                    // [EN-US] Equip or Unequip Quick Access
                                    if (isQuickEquipped) {
                                        currentPlayer->unequipConsumable();
                                        displayMessagePopupInventory("SISTEMA", {foundItem->getItemName() + " desequipado(a) do Acesso Rapido!"});
                                    } else {
                                        currentPlayer->equipItem(foundItem);
                                        displayMessagePopupInventory("SISTEMA", {foundItem->getItemName() + " equipado(a) no Acesso Rapido!"});
                                    }
                                    submenuOpen = false;
                                    if (is3D) RaycasterFrame::restoreLastFrame();
                                } else if (subOption == 2) {
                                    // [PT-BR] Inspecionar
                                    // [EN-US] Inspect
                                    std::vector<std::string> details = foundItem->getDetailsInspection(currentPlayer);
                                    std::vector<std::string> inspectionLines;
                                    inspectionLines.push_back(Appearance::color(Color::YELLOW) + " >> " + foundItem->getItemName() + " <<" + Appearance::color(Color::RESET));
                                    inspectionLines.push_back("");
                                    inspectionLines.insert(inspectionLines.end(), details.begin(), details.end());
                                    displayMessagePopupInventory("INSPECAO DE ITEM", inspectionLines);
                                    if (is3D) RaycasterFrame::restoreLastFrame();
                                } else if (subOption == 0) {
                                    // [PT-BR] Usar consumivel
                                    // [EN-US] Use consumable
                                    int qtyAvailable = currentPlayer->getInventory()->countItem(foundItem->getItemName());
                                    int quantityForUse = 1;
                                    
                                    if (qtyAvailable > 1) {
                                        int qtyChoice = readSelectionPopupInventory(
                                            "QUANTIDADE: " + foundItem->getItemName(),
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
                                    
                                    std::string itemName = foundItem->getItemName();
                                    int countBefore = currentPlayer->getInventory()->countItem(itemName);
                                    bool consumedSomeTurn = false;
                                    
                                    for (int i = 0; i < quantityForUse; ++i) {
                                        bool turnAlreadyUsed = shiftWasConsumed && *shiftWasConsumed;
                                        UseItemInfo info = ControlInventory::useOrEquip(currentPlayer, foundItem, turnAlreadyUsed);
                                        if (info.consumedTurn) consumedSomeTurn = true;
                                        
                                        if (currentPlayer->getItemSelectedForUse() != nullptr) {
                                            if (quantityForUse > 1) {
                                                displayMessagePopupInventory("SISTEMA", {"Este item requer selecao de alvo e", "sera usado apenas uma vez."});
                                            }
                                            break;
                                        }
                                        
                                        int countAfter = currentPlayer->getInventory()->countItem(itemName);
                                        if (countAfter == countBefore) {
                                            break;
                                        }
                                    }
                                    
                                    if (currentPlayer->getConsumableQuickly() && currentPlayer->getInventory()->countItem(currentPlayer->getConsumableQuickly()->getItemName()) == 0) {
                                        currentPlayer->unequipConsumable();
                                    }

                                    if (shiftWasConsumed && consumedSomeTurn) {
                                        *shiftWasConsumed = true;
                                    }
                                    submenuOpen = false; 
                                    if (is3D) RaycasterFrame::restoreLastFrame();
                                }
                            } else {
                                // [PT-BR] Equipavel / Material / Missao
                                // [EN-US] Equipable / Material / Mission
                                if (subOption == 2) {
                                    submenuOpen = false;
                                    if (is3D) RaycasterFrame::restoreLastFrame();
                                    break;
                                } else if (subOption == 0) {
                                    bool turnAlreadyUsed = shiftWasConsumed && *shiftWasConsumed;
                                    UseItemInfo info = ControlInventory::useOrEquip(currentPlayer, foundItem, turnAlreadyUsed);
                                    displayResultItem(info, foundItem, shiftWasConsumed);
                                    submenuOpen = false;
                                    if (is3D) RaycasterFrame::restoreLastFrame();
                                } else if (subOption == 1) {
                                    std::vector<std::string> details = foundItem->getDetailsInspection(currentPlayer);
                                    std::vector<std::string> inspectionLines;
                                    inspectionLines.push_back(Appearance::color(Color::YELLOW) + " >> " + foundItem->getItemName() + " <<" + Appearance::color(Color::RESET));
                                    inspectionLines.push_back("");
                                    inspectionLines.insert(inspectionLines.end(), details.begin(), details.end());
                                    displayMessagePopupInventory("INSPECAO DE ITEM", inspectionLines);
                                    if (is3D) RaycasterFrame::restoreLastFrame();
                                }
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


