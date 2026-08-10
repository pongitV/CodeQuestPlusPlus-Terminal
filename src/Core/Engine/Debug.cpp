/*
 * Arquivo: Debug.cpp
 * Proposito: Implementacao do menu de depuracao e trapacas do jogo.
 */

#include "Core/Engine/Debug.h"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <map>
#include <algorithm>

#include "Domain/Characters/Character.h"
#include "Domain/Items/ItemFactory.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Core/Engine/EnemyCreator.h"
#include "Systems/Progression/Diary.h"
#include "Core/Utils/DialogFunctions.h"
#include "UI/Screens/BaseScreen.h"
#include "Systems/Progression/Progression.h"
#include "Systems/Progression/ProgressionFlags.h"
#include "Systems/Combat/Combat.h"
#include "World/MapControl.h"

bool Debug::isGodModeActive = false;
bool Debug::isNoclipActive = false;

namespace {
    void activateGodMode(Character* player) {
        player->getFinalAttributes().health += 999999;
        player->getFinalAttributes().strength += 99999;
        player->getFinalAttributes().dexterity += 99999;
        player->getFinalAttributes().resistance += 99999;
        player->getFinalAttributes().constitution += 99999;
        player->getFinalAttributes().intelligence += 99999;
        player->getFinalAttributes().wisdom += 99999;
        player->forceCacheRecalculation();
        player->setHealth(player->getMaxHealth());
        
        std::cout << "\n";
        Appearance::printCentralized(DialogueFunctions::formatSystemMsg("God Mode ativado! Voce agora e um deus intocavel.", Color::YELLOW));
        std::cout << "\n";
        InputControl::waitForEnter();
    }

    void defineAttributesMenu(Character* player) {
        BaseScreen::executeDefaultLoop(
            "DEFINIR ATRIBUTOS (CHEAT)", Color::YELLOW,
            nullptr,
            [player]() {
                auto& attrs = player->getFinalAttributes();
                return std::vector<std::string>{
                    "Vida Maxima  : " + std::to_string(attrs.health), "Forca        : " + std::to_string(attrs.strength),
                    "Destreza     : " + std::to_string(attrs.dexterity), "Resistencia  : " + std::to_string(attrs.resistance),
                    "Constituicao : " + std::to_string(attrs.constitution), "Inteligencia : " + std::to_string(attrs.intelligence),
                    "Sabedoria    : " + std::to_string(attrs.wisdom), "Voltar"
                };
            },
            [player](int attrChoice) {
                if (attrChoice == 7 || attrChoice == -1) return false;
                
                auto& attrs = player->getFinalAttributes();
                std::string attrName; int* ptrAttr = nullptr;
                switch (attrChoice) {
                    case 0: attrName = "Vida Maxima"; ptrAttr = &attrs.health; break;
                    case 1: attrName = "Forca"; ptrAttr = &attrs.strength; break;
                    case 2: attrName = "Destreza"; ptrAttr = &attrs.dexterity; break;
                    case 3: attrName = "Resistencia"; ptrAttr = &attrs.resistance; break;
                    case 4: attrName = "Constituicao"; ptrAttr = &attrs.constitution; break;
                    case 5: attrName = "Inteligencia"; ptrAttr = &attrs.intelligence; break;
                    case 6: attrName = "Sabedoria"; ptrAttr = &attrs.wisdom; break;
                }
                
                std::cout << "\n";
                int newValue = InputControl::readIntegerWithLimits("Defina o novo valor para " + attrName + ": ", 0, 999999, true);
                *ptrAttr = newValue;
                player->forceCacheRecalculation();
                if (attrChoice == 0) player->setHealth(player->getMaxHealth());
                return true;
            }
        );
    }

    std::map<std::string, std::vector<ItemID>> getAllCategorizedItems() {
        std::map<std::string, std::vector<ItemID>> categories;
        
        // Realiza varredura de IDs de itens cadastrados no sistema
        for (int i = 1; i <= 200; ++i) {
            ItemID id = static_cast<ItemID>(i);
            std::string name = ItemFactory::getNameFromID(id);
            if (!name.empty() && name != "Desconhecido") {
                auto tempItem = ItemFactory::createItem(id);
                if (tempItem) {
                    EquipmentType type = tempItem->getType();
                    if (type == EquipmentType::WEAPONS) categories["Armas"].push_back(id);
                    else if (type == EquipmentType::ARMOR) categories["Armaduras"].push_back(id);
                    else if (type == EquipmentType::SHIELD) categories["Escudos"].push_back(id);
                    else {
                        // Classifica itens de utilidade por nome ou tipo base
                        if (name.find("Talisma") != std::string::npos || name == "Convite" || name == "Dispositivo") {
                            categories["Missoes"].push_back(id);
                        } else if (name.find("Pocao") != std::string::npos || name.find("Frasco") != std::string::npos || name.find("Elixir") != std::string::npos || name.find("Regenerador") != std::string::npos) {
                            categories["Consumiveis"].push_back(id);
                        } else {
                            categories["Materiais"].push_back(id);
                        }
                    }
                }
            }
        }
        return categories;
    }

    void getSoulMenu(Character* player) {
        struct SoulOption {
            std::string name;
            std::vector<std::unique_ptr<Character>> (*creator)(int);
        };

        std::vector<SoulOption> soulOptions = {
            {"Goblin", EnemyCreator::createGoblinEnemy},
            {"Slime", EnemyCreator::createSlimeEnemy},
            {"Fada", EnemyCreator::createFairyEnemy},
            {"Orc Exilado", EnemyCreator::createExiledOrcEnemy},
            {"Abominacao da Floresta", EnemyCreator::createForestAbominationEnemy},
            {"Troll", EnemyCreator::createTrollEnemy},
            {"Mimico", EnemyCreator::createMimicEnemy},
            {"Mahoraga", EnemyCreator::createMahoragaEnemy}
        };

        Appearance::sortAlphabetically(soulOptions, [](const SoulOption& op) { return op.name; });

        BaseScreen::executeDefaultLoop(
            "OBTER ALMAS (NECROMANTE)", Color::YELLOW,
            nullptr,
            [&soulOptions]() {
                std::vector<std::string> names;
                for (const auto& op : soulOptions) names.push_back(op.name);
                names.push_back("Voltar");
                return names;
            },
            [player, &soulOptions](int soulChoice) {
                if (soulChoice == static_cast<int>(soulOptions.size()) || soulChoice == -1) return false;
                
                std::string createdName = soulOptions[soulChoice].name;
                
                std::cout << "\n";
                int quantity = InputControl::readIntegerWithLimits("Quantidade (1 a 99): ", 1, 99);
                auto created = soulOptions[soulChoice].creator(quantity);
                
                int added = 0;
                for (auto& soul : created) {
                    if (soul) { player->addSoul(std::move(soul)); added++; }
                }
                
                if (added > 0) {
                    std::cout << "\n";
                    std::string plural = added > 1 ? "s" : "";
                    Appearance::printCentralized(DialogueFunctions::formatInteractionMsg(std::to_string(added) + "x Alma" + plural + " de " + createdName + " adicionada" + plural + " com sucesso!"));
                    InputControl::waitForEnter();
                }
                return true;
            }
        );
    }

    void getItemMenu(Character* player) {
        BaseScreen::executeDefaultLoop(
            "OBTER ITEM (CATEGORIAS)", Color::YELLOW,
            nullptr,
            []() {
                return std::vector<std::string>{"Armas", "Armaduras", "Escudos", "Materiais", "Consumiveis", "Missoes", "Almas de Classes/Inimigos (Necromante)", "Voltar"};
            },
            [player](int catChoice) {
                if (catChoice == 7 || catChoice == -1) return false;
                if (catChoice == 6) { getSoulMenu(player); return true; }
                
                std::string categoryNames[] = { "Armas", "Armaduras", "Escudos", "Materiais", "Consumiveis", "Missoes" };
                std::string selectedCategory = categoryNames[catChoice];
                auto allCat = getAllCategorizedItems();
                auto itemsFromCat = allCat[selectedCategory];
                
                if (itemsFromCat.empty()) {
                    std::cout << "\n";
                    Appearance::printCentralized(DialogueFunctions::formatSystemMsg("Nenhum item encontrado nesta categoria.", Color::RED));
                    InputControl::waitForEnter();
                    return true;
                }

                Appearance::sortAlphabetically(itemsFromCat, [](ItemID id) { return ItemFactory::getNameFromID(id); });

                BaseScreen::executeDefaultLoop(
                    "OBTER ITEM - " + selectedCategory, Color::YELLOW,
                    nullptr,
                    [&itemsFromCat]() {
                        std::vector<std::string> options;
                        for (auto id : itemsFromCat) options.push_back(ItemFactory::getNameFromID(id));
                        options.push_back("Voltar");
                        return options;
                    },
                    [player, &itemsFromCat](int itemChoice) {
                        if (itemChoice == static_cast<int>(itemsFromCat.size()) || itemChoice == -1) return false;
                        ItemID chosenId = itemsFromCat[itemChoice];
                        std::cout << "\n";
                        int quantity = InputControl::readIntegerWithLimits("Quantidade (1 a 99): ", 1, 99);
                        for (int q = 0; q < quantity; ++q) player->getInventory()->addItem(ItemFactory::createItem(chosenId));
                        Diary::instance().registerItem(Appearance::removeANSIColors(ItemFactory::getNameFromID(chosenId)));
                        std::cout << "\n";
                        std::string plural = quantity > 1 ? "s" : "";
                        Appearance::printCentralized(DialogueFunctions::formatInteractionMsg(std::to_string(quantity) + "x '" + ItemFactory::getNameFromID(chosenId) + "' adicionado" + plural + " ao inventario!"));
                        InputControl::waitForEnter();
                        return true;
                    }
                );
                return true;
            }
        );
    }
    void startCombatMenu(Character* player) {
        BaseScreen::executeDefaultLoop(
            "INICIAR COMBATE (SELECIONAR INIMIGO)", Color::YELLOW,
            nullptr,
            []() {
                return std::vector<std::string>{
                    "Goblin",
                    "Slime",
                    "Fada",
                    "Ork Exilado",
                    "Abominacao da Floresta",
                    "Troll",
                    "Mimico",
                    "Mahoraga",
                    "Voltar"
                };
            },
            [player](int enemyChoice) {
                if (enemyChoice == 8 || enemyChoice == -1) return false;
                
                std::string enemyNames[] = {
                    "Goblin", "Slime", "Fada", "Ork Exilado", 
                    "Abominacao da Floresta", "Troll", "Mimico", "Mahoraga"
                };
                std::string enemyName = enemyNames[enemyChoice];
                
                std::cout << "\n";
                int quantity = InputControl::readIntegerWithLimits("Quantidade (1 a 5): ", 1, 5);
                
                std::vector<std::unique_ptr<Character>> enemies;
                switch (enemyChoice) {
                    case 0: enemies = EnemyCreator::createGoblinEnemy(quantity); break;
                    case 1: enemies = EnemyCreator::createSlimeEnemy(quantity); break;
                    case 2: enemies = EnemyCreator::createFairyEnemy(quantity); break;
                    case 3: enemies = EnemyCreator::createExiledOrcEnemy(quantity); break;
                    case 4: enemies = EnemyCreator::createForestAbominationEnemy(quantity); break;
                    case 5: enemies = EnemyCreator::createTrollEnemy(quantity); break;
                    case 6: enemies = EnemyCreator::createMimicEnemy(quantity); break;
                    case 7: enemies = EnemyCreator::createMahoragaEnemy(quantity); break;
                }
                
                if (enemies.empty()) {
                    std::cout << "\n";
                    Appearance::printCentralized(DialogueFunctions::formatSystemMsg("Erro ao instanciar os inimigos.", Color::RED));
                    InputControl::waitForEnter();
                    return true;
                }
                
                std::cout << "\n";
                Appearance::printCentralized(DialogueFunctions::formatSystemMsg("Iniciando combate com " + std::to_string(quantity) + "x " + enemyName + "...", Color::YELLOW));
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                
                Combat combat(player, std::move(enemies));
                if (MapControl::is3DExplorationActive()) {
                    combat.set3DContext(
                        true, 
                        MapControl::getCurrentMapMatrix(), 
                        MapControl::getCameraPostX3D(), 
                        MapControl::getCameraPostY3D(), 
                        MapControl::getCameraAngle3D(), 
                        MapControl::getCurrentMapTitle()
                    );
                }
                combat.startCombat();
                
                return false;
            }
        );
    }
}

void Debug::displayDebugMenu(Character* player) {
    BaseScreen::executeDefaultLoop(
        "MENU DE DEBUG (CHEAT)", Color::YELLOW,
        nullptr,
        [player]() {
            return std::vector<std::string>{
                "God Mode (Max Atributos - Instakill/Imortal)",
                "Definir Atributos",
                "Obter Qualquer Item",
                "Definir Ouro e XP",
                "Liberar Todos os Mapas (Fast Travel)",
                std::string("Noclip (Atravessar paredes): ") + (Debug::isNoclipActive ? Appearance::color(Color::GREEN) + "LIGADO" + Appearance::color(Color::RESET) : Appearance::color(Color::RED) + "DESLIGADO" + Appearance::color(Color::RESET)),
                "Iniciar Combate com Qualquer Inimigo",
                "Fechar Debug Menu"
            };
        },
        [player](int debugChoice) {
            switch (debugChoice) {
                case 0:
                    activateGodMode(player);
                    break;
                case 1:
                    defineAttributesMenu(player);
                    break;
                case 2:
                    getItemMenu(player);
                    break;
                case 3: {
                    std::cout << "\n";
                    int qty = InputControl::readIntegerWithLimits("Digite a quantidade de Ouro e XP desejada (1 a 999999): ", 1, 999999, true);
                    player->gainGold(qty);
                    player->gainXp(qty);
                    std::cout << "\n";
                    Appearance::printCentralized(DialogueFunctions::formatSystemMsg("+" + std::to_string(qty) + " Ouro e +" + std::to_string(qty) + " XP adicionados!", Color::YELLOW));
                    std::cout << "\n";
                    InputControl::waitForEnter();
                    break;
                }
                case 4:
                    player->unlockMaze();
                    Progression::instance().setFlag(Flags::Village_KissRescued, true);
                    Progression::instance().setFlag(Flags::Forest_MorganaQuest, true);
                    Progression::instance().setFlag(Flags::Forest_MahoragaDefeated, true);
                    Progression::instance().setFlag(Flags::Visited_Forest, true);
                    Progression::instance().setFlag(Flags::Visited_KingdomBridge, true);
                    Progression::instance().setFlag(Flags::Visited_Kingdom, true);
                    Progression::instance().setFlag(Flags::Discovered_Maps, true);
                    std::cout << "\n";
                    Appearance::printCentralized(DialogueFunctions::formatSystemMsg("Todos os mapas e submapas liberados para Viagem Rapida (Tecla M)!", Color::YELLOW));
                    std::cout << "\n";
                    InputControl::waitForEnter();
                    break;
                case 5:
                    Debug::isNoclipActive = !Debug::isNoclipActive;
                    break;
                case 6:
                    startCombatMenu(player);
                    break;
                case 7:
                case -1:
                    return false;
            }
            return true;
        }
    );
}
