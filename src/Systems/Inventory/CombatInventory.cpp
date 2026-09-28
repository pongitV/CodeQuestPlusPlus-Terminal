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
#include "UI/Renderers/IDE/IDETheme.h"
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
        int termW = Appearance::getTerminalWidth();
        int termH = Appearance::getTerminalHeight();

        std::cout << "\033[?25l";
        if (!is3D) {
            std::vector<std::string> codePopup;
            codePopup.push_back(IDETheme::comment("// Chamada de directive na instance do item:"));
            for (const auto& t : text) {
                codePopup.push_back(IDETheme::comment("// " + t));
            }
            codePopup.push_back("");
            codePopup.push_back(IDETheme::keyword("auto ") + IDETheme::variable("executeItemDirective") + IDETheme::punctuation(" = [&]() {"));
            for (int i = 0; i < totalOptions; ++i) {
                std::string op = options[i];
                std::string stmt = "";
                if (op.find("Equipar") != std::string::npos) stmt = "hero->equipItem(selectedItem);";
                else if (op.find("Desequipar") != std::string::npos) stmt = "hero->unequipItem(selectedItem);";
                else if (op.find("Usar") != std::string::npos) stmt = "selectedItem->useFromInventory(hero);";
                else if (op.find("Descartar") != std::string::npos) stmt = "inventory.freeSlot(selectedItem);";
                else if (op.find("Inspecionar") != std::string::npos) stmt = "selectedItem->inspectMemoryLayout();";
                else if (op.find("Acesso Rapido") != std::string::npos) stmt = "hero->bindQuickSlot(selectedItem);";
                else if (op.find("VOLTAR") != std::string::npos || op.find("Voltar") != std::string::npos) stmt = "return;";
                else stmt = op + "();";

                std::string line = "    " + stmt + " // [" + op + "]";
                if (i == selectionCurrent) {
                    codePopup.push_back(std::string(IDETheme::COLOR_ACTIVE_TAB) + "  > " + line + " <" + std::string(IDETheme::COLOR_RESET));
                } else {
                    codePopup.push_back("    " + line);
                }
            }
            codePopup.push_back(IDETheme::punctuation("};"));

            std::vector<std::string> tabs = { "ItemDirective.cpp" };
            auto editorView = IDETheme::renderEditorView(tabs, 0, "// src/Systems/Inventory/ItemDirective.cpp", codePopup, termW, termH, "[W/S] Selecionar | [ENTER] Executar");

            Appearance::clearScreen();
            for (const auto& l : editorView) std::cout << l << "\n";
            std::cout << "\033[J" << std::flush;
        } else {
            std::vector<std::string> lines;
            for (const auto& t : text) {
                lines.push_back(" " + t + " ");
            }
            lines.push_back("");
            for (int i = 0; i < totalOptions; ++i) {
                if (i == selectionCurrent) {
                    lines.push_back(Appearance::color(Color::GREEN) + " > " + options[i] + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m");
                } else {
                    lines.push_back("   " + options[i]);
                }
            }

            std::vector<std::string> boxEnd = BaseScreen::createBox(lines, title, 0, Color::YELLOW, "\033[48;2;25;25;25m");
            int outW = Appearance::getVisualLength(boxEnd[0]);
            int outH = (int)boxEnd.size();
            int startX = std::max(0, (termW - outW) / 2);
            int startY = std::max(0, (termH - outH) / 2);
            if (startY + outH > termH) startY = std::max(0, termH - outH);
            if (startX + outW > termW) startX = std::max(0, termW - outW);

            for (size_t i = 0; i < boxEnd.size(); ++i) {
                if (startY + (int)i < termH) {
                    Appearance::moveCursor(startX, startY + i);
                    std::cout << boxEnd[i];
                }
            }
            std::cout << std::flush;
        }

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
            titleBox = is3D ? " MENU DE BOLSOS " : "Systems::Memory::InventoryHeap";
            std::string strPocket = is3D ? ("BOLSO: " + std::to_string(currentPlayer->getInventory()->getGold()) + " Moedas de Ouro [$$]")
                                         : ("uint32_t gold = " + std::to_string(currentPlayer->getInventory()->getGold()) + "; // Saldo de currency na heap");
            
            std::vector<std::string> optionsBase;
            if (currentPlayer->getConsumableQuickly()) {
                int qty = currentPlayer->getInventory()->countItem(currentPlayer->getConsumableQuickly()->getItemName());
                if (!is3D) {
                    optionsBase.push_back("hero->useQuickConsumable();          // &heap[\"" + currentPlayer->getConsumableQuickly()->getItemName() + "\"] (" + std::to_string(qty) + "x)");
                } else {
                    optionsBase.push_back(Appearance::color(Color::GREEN) + "[+] " + Appearance::color(Color::WHITE) + "Acesso Rapido: " + currentPlayer->getConsumableQuickly()->getItemName() + " (" + std::to_string(qty) + "x)");
                }
            }
            if (!is3D) {
                optionsBase.push_back("category = Category::EQUIPMENT_ARSENAL; // std::vector<Equipment*>");
                optionsBase.push_back("category = Category::CONSUMABLE_ITEMS;   // std::vector<Consumable*>");
                optionsBase.push_back("category = Category::STOCK_MATERIALS;    // std::vector<Material*>");
                optionsBase.push_back("category = Category::MISSION_ITEMS;      // std::vector<QuestItem*>");
                optionsBase.push_back("");
                optionsBase.push_back(strPocket);
                optionsBase.push_back("");
                optionsBase.push_back("return;                                  // [0] Retornar e fechar heap");
            } else {
                optionsBase.push_back("Arsenal de Equipamentos");
                optionsBase.push_back("Itens Consumiveis");
                optionsBase.push_back("Estoque e Materiais");
                optionsBase.push_back("Itens de Missao");
                optionsBase.push_back("");
                optionsBase.push_back(strPocket);
                optionsBase.push_back("");
                optionsBase.push_back("[<] VOLTAR");
            }
            
            for (size_t i = 0; i < optionsBase.size(); ++i) {
                if (optionsBase[i].empty() || optionsBase[i].find("BOLSO:") != std::string::npos || optionsBase[i].find("uint32_t gold") != std::string::npos || optionsBase[i].substr(0, 3) == "   ") {
                    lines.push_back("   " + optionsBase[i]);
                } else {
                    interactive.push_back(optionsBase[i]);
                    indicesReal.push_back(i);
                    if (static_cast<int>(interactive.size() - 1) == selectionCurrent) {
                        if (!is3D) {
                            lines.push_back(std::string(IDETheme::COLOR_ACTIVE_TAB) + "  > " + optionsBase[i] + " <" + std::string(IDETheme::COLOR_RESET));
                        } else {
                            lines.push_back(Appearance::color(Color::GREEN) + " > " + optionsBase[i] + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m");
                        }
                    } else {
                        lines.push_back("   " + optionsBase[i]);
                    }
                }
            }
        } else if (state == ARSENAL) {
            titleBox = is3D ? " ARSENAL DE EQUIPAMENTOS " : "Domain::Items::EquipmentArsenal";

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
                std::string itemDisplay = name;
                if (!is3D) {
                    itemDisplay = "heap.push_back(make_unique<Equipment>(\"" + name + "\"));";
                }
                if (idx == selectionSub) {
                    if (!is3D) {
                        lines.push_back(std::string(IDETheme::COLOR_ACTIVE_TAB) + "  > " + itemDisplay + " <" + std::string(IDETheme::COLOR_RESET));
                    } else {
                        lines.push_back(Appearance::color(Color::GREEN) + " > " + name + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m");
                    }
                } else {
                    lines.push_back("   " + itemDisplay);
                }
            };

            auto addGroup = [&](const std::string& label, std::vector<Item*>& group) {
                if (group.empty()) return;
                if (!is3D) {
                    lines.push_back(IDETheme::comment("   // Seção: " + label));
                } else {
                    lines.push_back(" " + colorDiv + "--- " + label + " ---" + colorReset);
                }
                for (auto* item : group) {
                    auto itemsGrouped = currentPlayer->getInventory()->countItem(item->getItemName());
                    std::string prefix = (itemsGrouped > 1) ? std::to_string(itemsGrouped) + "x " : "";
                    addItem(prefix + item->getItemName(), item);
                }
                lines.push_back("");
            };

            if (!is3D) {
                lines.push_back(IDETheme::comment("   // Seção: Hardware pointers (Equipados)"));
            } else {
                lines.push_back(" " + colorDiv + "--- Equipados ---" + colorReset);
            }
            bool hasEq = false;
            auto addEq = [&](const std::string& label, Item* item) {
                if (!item) return;
                hasEq = true;
                std::string name = item->getItemName();
                int idx = (int)interactive.size();
                interactive.push_back("(E) " + name);
                indicesReal.push_back((int)itemIndexMap.size());
                itemIndexMap.push_back(item);

                std::string eqDisplay = "[E] " + label + ": " + name;
                if (!is3D) {
                    eqDisplay = "slots.eq" + label + " = &heap[\"" + name + "\"]; // [E] Equipado";
                }
                if (idx == selectionSub) {
                    if (!is3D) {
                        lines.push_back(std::string(IDETheme::COLOR_ACTIVE_TAB) + "  > " + eqDisplay + " <" + std::string(IDETheme::COLOR_RESET));
                    } else {
                        lines.push_back(Appearance::color(Color::GREEN) + " > " + Appearance::color(Color::GREEN) + "[E] " + Appearance::color(Color::RESET) + label + ": " + name + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m");
                    }
                } else {
                    lines.push_back("   " + eqDisplay);
                }
            };
            addEq("Arma", weaponEq);
            addEq("Armadura", armorEq);
            addEq("Escudo", shieldEq);
            if (!hasEq) {
                if (!is3D) lines.push_back(IDETheme::comment("   // slots.empty() == true"));
                else lines.push_back("   " + Appearance::color(Color::GRAY) + "(Nada equipado)" + colorReset);
            }
            lines.push_back("");

            addGroup("Armas", weapons);
            addGroup("Armaduras", armor);
            addGroup("Escudos", shields);

            interactive.push_back("[<] VOLTAR");
            indicesReal.push_back(-1);
            if ((int)interactive.size() - 1 == selectionSub) {
                if (!is3D) {
                    lines.push_back(std::string(IDETheme::COLOR_ACTIVE_TAB) + "  > return; // [Voltar ao Menu Principal] <" + std::string(IDETheme::COLOR_RESET));
                } else {
                    lines.push_back(Appearance::color(Color::GREEN) + " > [<] VOLTAR" + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m");
                }
            } else {
                lines.push_back(is3D ? "   [<] VOLTAR" : "   return; // [Voltar]");
            }

        } else {
            int category = 0;
            if (state == CONSUMABLES) { titleBox = is3D ? " ITENS CONSUMIVEIS " : "Domain::Items::Consumables"; category = 1; }
            else if (state == STOCK) { titleBox = is3D ? " ESTOQUE E MATERIAIS " : "Domain::Items::StockMaterials"; category = 2; }
            else if (state == MISSION) { titleBox = is3D ? " ITENS DE MISSAO " : "Domain::Items::QuestItems"; category = 3; }

            auto items = ScreenInventory::getListCategory(currentPlayer, category, false);
            Appearance::sortAlphabetically(items, [](const auto& pair) { return pair.first; });

            itemIndexMap.clear();

            if (!items.empty()) {
                for (const auto& p : items) {
                    interactive.push_back(p.first);
                    indicesReal.push_back((int)itemIndexMap.size());
                    itemIndexMap.push_back(p.second);

                    std::string itemDisplay = p.first;
                    if (!is3D) {
                        itemDisplay = "heap.emplace_back<Item>(\"" + p.first + "\");";
                    }

                    if ((int)interactive.size() - 1 == selectionSub) {
                        if (!is3D) {
                            lines.push_back(std::string(IDETheme::COLOR_ACTIVE_TAB) + "  > " + itemDisplay + " <" + std::string(IDETheme::COLOR_RESET));
                        } else {
                            lines.push_back(Appearance::color(Color::GREEN) + " > " + p.first + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m");
                        }
                    } else {
                        lines.push_back("   " + itemDisplay);
                    }
                }
            } else {
                if (!is3D) {
                    lines.push_back(IDETheme::comment("   // Vector vazio na heap: std::vector<Item*> { size: 0 }"));
                } else {
                    lines.push_back("   " + Appearance::color(Color::GRAY) + "Nenhum item nesta categoria." + Appearance::color(Color::RESET));
                }
            }
            lines.push_back("");

            interactive.push_back("[<] VOLTAR");
            indicesReal.push_back(-1);
            if ((int)interactive.size() - 1 == selectionSub) {
                if (!is3D) {
                    lines.push_back(std::string(IDETheme::COLOR_ACTIVE_TAB) + "  > return; // [Retornar ao menu principal da heap] <" + std::string(IDETheme::COLOR_RESET));
                } else {
                    lines.push_back(Appearance::color(Color::GREEN) + " > [<] VOLTAR" + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m");
                }
            } else {
                lines.push_back(is3D ? "   [<] VOLTAR" : "   return; // [Retornar]");
            }
        }
        
        int totalOptions = interactive.size();
        int* selRef = (state == MAIN) ? &selectionCurrent : &selectionSub;
        if (*selRef >= totalOptions && totalOptions > 0) *selRef = totalOptions - 1;
        
        if (is3D) {
            std::vector<std::string> boxEnd = BaseScreen::createBox(lines, titleBox, 0, Color::YELLOW, "\033[48;2;25;25;25m");
            int outW = Appearance::getVisualLength(boxEnd[0]);
            int outH = boxEnd.size();
            if (redesignCompleteInv) RaycasterFrame::restoreLastFrame();
            int termW = Appearance::getTerminalWidth();
            int termH = Appearance::getTerminalHeight();
            
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
            int termW = Appearance::getTerminalWidth();
            int termH = Appearance::getTerminalHeight();

            std::vector<std::string> codeBlock;
            codeBlock.push_back(IDETheme::preprocessor("#pragma once"));
            codeBlock.push_back(IDETheme::keyword("namespace ") + IDETheme::type("Systems::Memory") + IDETheme::punctuation(" {"));
            codeBlock.push_back("");
            codeBlock.insert(codeBlock.end(), lines.begin(), lines.end());
            codeBlock.push_back("");
            codeBlock.push_back(IDETheme::punctuation("} // namespace Systems::Memory"));

            std::vector<std::string> tabs = {
                "InventoryHeap.hpp",
                "HardwareSlots.sys"
            };
            auto editorView = IDETheme::renderEditorView(tabs, 0, "// src/Systems/Inventory/InventoryHeap.hpp", codeBlock, termW, termH, "[W/S] Navegar | [ENTER] Inspecionar / Executar | [ESC] Fechar");

            for (const auto& l : editorView) {
                std::cout << l << "\n";
            }
            std::cout << "\033[J";
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
                        // CONSUMIVEL RAPIDO
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
                                    // Equipar ou Desequipar Acesso Rapido
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
                                    // Inspecionar
                                    std::vector<std::string> details = foundItem->getDetailsInspection(currentPlayer);
                                    std::vector<std::string> inspectionLines;
                                    inspectionLines.push_back(Appearance::color(Color::YELLOW) + " >> " + foundItem->getItemName() + " <<" + Appearance::color(Color::RESET));
                                    inspectionLines.push_back("");
                                    inspectionLines.insert(inspectionLines.end(), details.begin(), details.end());
                                    displayMessagePopupInventory("INSPECAO DE ITEM", inspectionLines);
                                    if (is3D) RaycasterFrame::restoreLastFrame();
                                } else if (subOption == 0) {
                                    // Usar consumivel
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
                                // Equipavel / Material / Missao
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


