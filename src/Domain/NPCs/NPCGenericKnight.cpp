#include "Domain/NPCs/NPCGenericKnight.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

#include "Domain/Characters/Classes/Warrior.h"
#include "Domain/Characters/Races/Human.h"
#include "Domain/Items/ItemFactory.h"
#include "Domain/Items/Items/MaterialItem.h"
#include "Systems/Combat/Combat.h"
#include "Core/Engine/EnemyCreator.h"
#include "UI/Screens/Menu/MenuScreen.h"
#include "Core/Utils/RendererProvider.h"
#include "Core/Utils/DialogFunctions.h"
#include "Systems/Progression/Diary.h"
#include "Systems/Progression/Progression.h"
#include "Systems/Progression/ProgressionFlags.h"
#include "Core/Utils/InputControl.h"
#include "World/MapControl.h"
#include "Domain/NPCs/NPCGenericKnightLayout.h"

namespace {
    // --- CLASSES E FUNCOES AUXILIARES ---
    Item* searchByName(Inventory* inv, const std::string& name) {
        for (auto* item : inv->getAllItems()) {
            if (item->getItemName() == name) return item;
        }
        return nullptr;
    }

    class ClassKnight : public Warrior {
    public:
        std::string getClassName() const override { return "Cavaleiro Real"; }
        
        std::vector<std::unique_ptr<Item>> getEquipmentClass() const override {
            std::vector<std::unique_ptr<Item>> equipment;
            Warrior base;
            for (auto& item : base.getEquipmentClass()) {
                if (item->getType() == EquipmentType::SHIELD || item->getType() == EquipmentType::CONSUMABLE) {
                    equipment.push_back(std::move(item));
                }
            }
            equipment.push_back(ItemFactory::createItem(ItemID::SwordKnight));
            return equipment;
        }
    };

    class RaceKnight : public Human {
    public:
        const std::vector<std::string>& getAppearanceRace() const override {
            return NPCKnightGenericLayouts::artKnight;
        }
    };

    /*
     * Comentario adicionado para forcar a recompilacao e resolver erros de linkagem do Guerreiro
     * --- APARENCIA E DIALOGOS ---
     */
    void dialogueKnight(const std::vector<std::string>& lines) {
        DialogueFunctions::printDialogueNPC("Cavaleiro Real", Color::GRAY, lines);
    }

    bool searchTrollNext(const std::vector<std::string>& map, int startX, int startY, int& outX, int& outY) {
        for (int dy = -2; dy <= 2; ++dy) {
            for (int dx = -5; dx <= 5; ++dx) {
                int y = startY + dy;
                int x = startX + dx;
                if (y >= 0 && y < static_cast<int>(map.size()) && x >= 0 && x < static_cast<int>(map[y].size())) {
                    if (map[y][x] == 'T') {
                        outX = x;
                        outY = y;
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void displayScreenKnight(const std::string& titleHeader, const std::vector<std::string>& lines) {
        if (RendererProvider::get()) RendererProvider::get()->displayPopup(titleHeader, lines, Color::GRAY, NPCKnightGenericLayouts::artKnight);
    }
}

// --- CRIACAO DO NPC ---
std::unique_ptr<Character> NPCGenericKnight::createKnight(const std::string& name) {
    auto knight = std::make_unique<Character>(name, std::make_unique<RaceKnight>(), std::make_unique<ClassKnight>());
    std::string nameArmor = ItemFactory::getNameFromID(ItemID::ArmorKnight);
    std::string nameSword = ItemFactory::getNameFromID(ItemID::SwordKnight);
    knight->getInventory()->addItem(ItemFactory::createItem(ItemID::ArmorKnight));
    knight->equipItem(searchByName(knight->getInventory(), nameArmor));
    knight->equipItem(searchByName(knight->getInventory(), nameSword));
    knight->calculateAttributes();
    knight->modifyHealth(knight->getMaxHealth());
    return knight;
}

// --- INTERACAO ---
void NPCGenericKnight::interact(Character* currentPlayer, bool& trollDefeated, bool& invitationReceived, int /*terminalWidth*/, std::vector<std::string>& currentMapMatrix, bool isExplorationActive, const std::function<void()>& restoreScreen, char destinationCell, int nextPositionX, int nextPositionY) {
    Diary::instance().registerNPC("Cavaleiro Real");
    if (!trollDefeated && (destinationCell == 'T' || destinationCell == 'C')) {
        int positionTrollX = -1, positionTrollY = -1;
        
        if (destinationCell == 'T') {
            positionTrollX = nextPositionX;
            positionTrollY = nextPositionY;
        } else if (destinationCell == 'C') {
            searchTrollNext(currentMapMatrix, nextPositionX, nextPositionY, positionTrollX, positionTrollY);
        }

        if (positionTrollX == -1) {
            if (RendererProvider::get()) RendererProvider::get()->startPopupInteraction();
            std::vector<std::string> lines = {
                "Ainda temos invasores no reino!",
                "voce precisa de permissao se nao quiser ser",
                "tratado como invasor tambem...",
                "Nos ajude a derrotar todos e podemos",
                "garantir sua entrada no reino!"
            };
            if (RendererProvider::get()) RendererProvider::get()->displayPopup("CAVALEIROS REAIS", lines, Color::GRAY, NPCKnightGenericLayouts::artKnight);
            return;
        }

        if (RendererProvider::get()) RendererProvider::get()->startPopupInteraction();
        std::vector<std::string> lines = {
            "Viajante! Este Troll bloqueia a passagem.",
            "Nossas forcas estao se esgotando!",
            "Nos ajude a derrota-lo e o recompensaremos!"
        };
        
        int choice = 1; // Default
        InputControl::executeLoopMenuPopup(
            [&]() { Appearance::displayPopup("PEDIDO DE AJUDA", lines, Color::GRAY, NPCKnightGenericLayouts::artKnight); },
            [&]() { return std::vector<std::string>{"Ajudar os Cavaleiros", "Recuar"}; },
            [&](const std::string& op) {
                if (op == "Ajudar os Cavaleiros") choice = 0;
                else choice = 1;
                return false; // Exit loop after choice
            },
            "PEDIDO DE AJUDA", Color::GRAY, NPCKnightGenericLayouts::artKnight
        );

        if (choice == 0) {
            Diary::instance().registerMissionAccept("cavaleiro_trolls");
            std::vector<std::unique_ptr<Character>> allies;
            allies.push_back(createKnight("Cavaleiro Real 1"));
            allies.push_back(createKnight("Cavaleiro Real 2"));
            
            std::vector<std::unique_ptr<Character>> enemies;
            auto trolls = EnemyCreator::createTrollEnemy(1);
            if (!trolls.empty()) enemies.push_back(std::move(trolls[0])); 
            
            Combat combat(currentPlayer, std::move(enemies));
            combat.addAllies(std::move(allies));
            if (MapControl::is3DExplorationActive()) {
                combat.set3DContext(
                    true, 
                    currentMapMatrix, 
                    MapControl::getCameraPostX3D(), 
                    MapControl::getCameraPostY3D(), 
                    MapControl::getCameraAngle3D(), 
                    MapControl::getCurrentMapTitle()
                );
            }
            combat.startCombat();
            
            if (currentPlayer->getHealth() > 0) {
                currentMapMatrix[positionTrollY][positionTrollX] = '.';
                
                int trollsRemaining = 0;
                for (const auto& line : currentMapMatrix) {
                    trollsRemaining += std::count(line.begin(), line.end(), 'T');
                }
                if (trollsRemaining == 0) {
                    trollDefeated = true;
                    Progression::instance().setFlag(Flags::BridgeKingdom_TrollDefeated, true);
                }
            }
        }
        if (isExplorationActive && !MapControl::is3DExplorationActive()) restoreScreen();
    } else if (destinationCell == 'C') {
        InputControl::executeLoopMenuPopup(
            [&]() {
                if (!invitationReceived) {
                    Appearance::displayPopup("CAVALEIRO REAL", {"Saudacoes, viajante. O que deseja?"}, Color::GRAY, NPCKnightGenericLayouts::artKnight);
                } else {
                    Appearance::displayPopup("CAVALEIRO REAL", {"O Rei o aguarda no Reino. Siga em frente!"}, Color::GRAY, NPCKnightGenericLayouts::artKnight);
                }
            },
            [&]() { return std::vector<std::string>{"Conversar", "Missoes do Cavaleiro", "VOLTAR"}; },
            [&](const std::string& op) {
                if (op == "Conversar") {
                    if (!invitationReceived) {
                        Appearance::displayPopup("CAVALEIRO REAL", { "Obrigado por nos ajudar com os Trolls!" }, Color::GRAY, NPCKnightGenericLayouts::artKnight);
                    } else {
                        Appearance::displayPopup("CAVALEIRO REAL", { "O Rei o aguarda no Reino. Siga em frente!" }, Color::GRAY, NPCKnightGenericLayouts::artKnight);
                    }
                    return true;
                } else if (op == "Missoes do Cavaleiro") {
                    if (!invitationReceived) {
                        int escMission = -1;
                        InputControl::executeLoopMenuPopup(
                            [&]() { Appearance::displayPopup("MISSOES - CAVALEIRO", {"Escolha uma missao:"}, Color::GRAY, NPCKnightGenericLayouts::artKnight); },
                            [&]() { return std::vector<std::string>{"[M] Reportar Trolls derrotados", "VOLTAR"}; },
                            [&](const std::string& subOp) {
                                if (subOp == "[M] Reportar Trolls derrotados") { escMission = 0; return false; }
                                return false;
                            },
                            "MISSOES - CAVALEIRO", Color::GRAY, NPCKnightGenericLayouts::artKnight
                        );

                        if (escMission == 0) {
                            std::vector<std::string> rewardYouspeak = {
                                "Voce lutou bravamente e limpou o reino dos Trolls!",
                                "Como prometido, aqui esta a sua recompensa.",
                                "",
                                "Voce recebeu o [Convite Real]!"
                            };
                            Appearance::displayPopup("RECOMPENSA", rewardYouspeak, Color::YELLOW, NPCKnightGenericLayouts::artKnight);
                            currentPlayer->getInventory()->addItem(ItemFactory::createItem(ItemID::RoyalInvitation));
                            Diary::instance().registerItem("Convite Real");
                            Diary::instance().registerMissionCompleted("cavaleiro_trolls");
                            Progression::instance().setFlag(Flags::Village_RoyalInvitation, true);
                            invitationReceived = true;
                        }
                    } else {
                        InputControl::executeLoopMenuPopup(
                            [&]() { Appearance::displayPopup("MISSOES - CAVALEIRO", {"Escolha uma missao:"}, Color::GRAY, NPCKnightGenericLayouts::artKnight); },
                            [&]() { return std::vector<std::string>{"(Nenhuma missao disponivel)", "VOLTAR"}; },
                            [&](const std::string& subOp) {
                                if (subOp == "(Nenhuma missao disponivel)") {
                                    Appearance::displayPopup("CAVALEIRO REAL", { "Nao precisamos de ajuda no momento." }, Color::GRAY, NPCKnightGenericLayouts::artKnight);
                                }
                                return false;
                            },
                            "MISSOES - CAVALEIRO", Color::GRAY, NPCKnightGenericLayouts::artKnight
                        );
                    }
                    return true;
                } else if (op == "VOLTAR" || op == "Sair") {
                    return false;
                }
                return true;
            },
            "CAVALEIRO REAL", Color::GRAY, NPCKnightGenericLayouts::artKnight
        );
        if (isExplorationActive && !MapControl::is3DExplorationActive()) restoreScreen();
    }
}
