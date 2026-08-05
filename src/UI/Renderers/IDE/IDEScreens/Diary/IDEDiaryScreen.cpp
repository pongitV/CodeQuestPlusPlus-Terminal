#include "UI/Renderers/IDE/IDEScreens/Diary/IDEDiaryScreen.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include <chrono>

#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "UI/Screens/Menu/MenuScreen.h"
#include "UI/Screens/Bestiary/BestiaryScreen.h"
#include "UI/Screens/BaseScreen.h"

#include "UI/Screens/Diary/DiaryScreenLayout.h"
#include "UI/Screens/Diary/DiaryScreenLogic.h"
#include "Domain/Items/ItemFactory.h"
#include "Domain/Items/Item.h"
#include "Systems/Progression/Diary.h"
#include "Domain/Characters/Character.h"
#include "Domain/Characters/Races/Dwarf.h"
#include "Domain/Characters/Races/Elf.h"
#include "Domain/Characters/Races/Human.h"
#include "Domain/Characters/Races/Orc.h"
#include "Domain/Characters/Classes/Archer.h"
#include "Domain/Characters/Classes/Bard.h"
#include "Domain/Characters/Classes/Warrior.h"
#include "Domain/Characters/Classes/Mage.h"
#include "Domain/Characters/Classes/Necromancer.h"
#include "Domain/NPCs/NPCBlacksmithLayout.h"
#include "Domain/NPCs/NPCMerchantLayout.h"
#include "Domain/NPCs/NPCMageLayout.h"
#include "Domain/NPCs/NPCGenericKnightLayout.h"
#include "UI/Screens/Inventory/InventoryScreen.h"
#include "Domain/Characters/Races/BaseRace.h"
#include "Domain/Characters/Classes/BaseClass.h"

namespace {

    void inspectItem(Character* currentPlayer, const std::string& itemName) {
        Appearance::clearScreen();
        Appearance::displayArtPanel(ArtsDiary::soonDiary, 75, Color::YELLOW, "INSPECAO DE ITEM", false);
        std::cout << "\n";
        auto item = ItemFactory::createItem(itemName);
        if (item) {
            ScreenInventory::displayInspectionItem(item.get(), currentPlayer);
        } else {
            Appearance::printCentralized("Detalhes indisponiveis para: " + itemName, Appearance::color(Color::RED));
        }
        InputControl::waitForEnter();
    }

    void inspectNPC(const std::string& nameNPC) {
        Appearance::clearScreen();
        Appearance::displayArtPanel(ArtsDiary::soonDiary, 75, Color::YELLOW, "INSPECAO DE NPC", false);
        std::cout << "\n";

        std::vector<std::string> art;
        std::string lore;

        if (nameNPC.find("Bjorn") != std::string::npos) {
            art = NPCBlacksmithLayouts::artBlacksmith;
            lore = "Bjorn, o Ferreiro da Vila.\nUm anao robusto de poucas palavras, deixa o ferro a forja falarem por ele.\nSempre disposto a melhorar seus equipamentos por materiais da regiao.";
        } else if (nameNPC.find("Franchesco") != std::string::npos) {
            art = NPCMerchantLayouts::artMerchant;
            lore = "Franchesco, o Mercador Ambulante.\nSempre com um sorriso no rosto, ainda mais se ver sua carteira cheia.\nGosta de moedas de ouro mais do que da propria vida.";
        } else if (nameNPC.find("Morgana") != std::string::npos) {
            art = NPCMageLayouts::artMage;
            lore = "Morgana, a Bruxa da Floresta.\nUm misterio, domina as artes da alquimia e encantamento.\nSeu labirinto guarda segredos que poucos ousam buscar.";
        } else if (nameNPC.find("Cavaleiro Real") != std::string::npos) {
            art = NPCKnightGenericLayouts::artKnight;
            lore = "Cavaleiro Real.\nProtetores leais do Reino e executores da vontade do Rei.\nFortemente blindados e treinados contra ameacas e monstros.";
        } else {
            lore = "Informacoes detalhadas sobre as intencoes e o passado dessa pessoa permanecem um misterio.";
        }

        if (!art.empty()) {
            Appearance::printBlockCentralized(art, Appearance::color(Color::GRAY));
            std::cout << "\n";
        }

        Appearance::printCentralized(Appearance::color(Color::CYAN) + " ═══ " + nameNPC + " ═══" + Appearance::color(Color::RESET));
        std::cout << "\n";

        std::vector<std::string> linesLore;
        size_t post = 0;
        std::string desc = lore;
        while ((post = desc.find('\n')) != std::string::npos) {
            linesLore.push_back(" > " + desc.substr(0, post));
            desc.erase(0, post + 1);
        }
        linesLore.push_back(" > " + desc);
        Appearance::printBlockCentralized(linesLore);

        std::cout << "\n";
        InputControl::waitForEnter();
    }

    void inspectRacePlayable(const std::string& raceName) {
        Appearance::clearScreen();
        Appearance::displayArtPanel(ArtsDiary::soonDiary, 75, Color::YELLOW, "INSPECAO DE RACA", false);
        std::cout << "\n";

        std::unique_ptr<BaseRace> raceObj;
        if (raceName == "Humano") raceObj = std::make_unique<Human>();
        else if (raceName == "Dwarf") raceObj = std::make_unique<Dwarf>();
        else if (raceName == "Elfo") raceObj = std::make_unique<Elf>();
        else if (raceName == "Ork") raceObj = std::make_unique<Ork>();

        if (raceObj) {
            std::vector<std::string> art = raceObj->getAppearanceRace();
            std::vector<std::string> attributes = MenuScreen::composeFrameDeAttributes(
                raceObj->getAttributesRace(),
                "[ ATRIBUTOS BASE ]",
                "[ HABILIDADE DA RACA ]",
                raceObj->getNameSkillRace(),
                raceObj->getDescriptionSkillRace()
            );

            Appearance::printSideASide(attributes, art, 40, 6, Color::WHITE, Color::WHITE, 0);
        }
        std::cout << "\n";
        InputControl::waitForEnter();
    }

    void inspectClass(const std::string& className) {
        Appearance::clearScreen();
        Appearance::displayArtPanel(ArtsDiary::soonDiary, 75, Color::YELLOW, "INSPECAO DE CLASSE", false);
        std::cout << "\n";

        std::unique_ptr<BaseClass> classObj;
        if (className == "Guerreiro") classObj = std::make_unique<Warrior>();
        else if (className == "Mago") classObj = std::make_unique<Wizard>();
        else if (className == "Arqueiro") classObj = std::make_unique<Archer>();
        else if (className == "Bardo") classObj = std::make_unique<Bard>();
        else if (className == "Necromante") classObj = std::make_unique<Necromancer>();

        if (classObj) {
            std::vector<std::string> art = classObj->getAppearanceClassMenu();

            std::vector<std::string> attributes = MenuScreen::composeFrameDeAttributes(
                classObj->getAttributesClass(),
                "[ ATRIBUTOS BONUS ]",
                "[ PASSIVA DA CLASSE ]",
                classObj->getNamePassiveClass(),
                classObj->getDescriptionPassiveClass(),
                "[ HABILIDADE ATIVA ]",
                classObj->getNameSkillClass(),
                classObj->getDescriptionSkillClass()
            );

            Appearance::printSideASide(attributes, art, 40, 6, Color::WHITE, Color::WHITE, 0);
        }
        std::cout << "\n";
        InputControl::waitForEnter();
    }
}

static void displayProgress(Character* /*jogador*/) {
    BaseScreen::executeDefaultLoop(
        "DIARIO - PROGRESSO",
        Color::YELLOW,
        [&]() {
            std::cout << "\n";
            Appearance::printCentralized("Seus maiores feitos nesta jornada:");
            std::cout << "\n\n";

            auto data = ScreenDiaryLogic::getProgress();

            if (data.totalAchievements == 0) {
                Appearance::printCentralized("Nenhum grande feito para registrar ainda...", Appearance::color(Color::GRAY));
                std::cout << "\n";
            } else {
                std::vector<std::string> linesForPrint;
                for (const auto& l : data.linesNPC) linesForPrint.push_back(l);
                for (const auto& l : data.linesMonster) linesForPrint.push_back(l);
                for (const auto& l : data.linesItem) linesForPrint.push_back(l);
                if (!linesForPrint.empty()) linesForPrint.pop_back();
                Appearance::printBlockCentralized(linesForPrint);
            }
        },
        []() { return std::vector<std::string>{"VOLTAR"}; },
        [&](int id) {
            return !(id == 0 || id == -1);
        }
    );
}

static void displayMenuItems(Character* currentPlayer) {
    bool keepRunning = true;
    while (keepRunning) {
        Appearance::clearScreen();
        Appearance::displayArtPanel(ArtsDiary::soonDiary, 75, Color::YELLOW, "ITENS DESCOBERTOS", false);
        std::cout << "\n";
        std::vector<std::string> items = Diary::instance().getItemsDiscovered();
        if (items.empty()) {
            Appearance::printCentralized("Nenhum item descoberto ainda.", Appearance::color(Color::GRAY));
            InputControl::waitForEnter();
            return;
        }

        auto cats = ScreenDiaryLogic::categorizeItems(currentPlayer);
        std::vector<std::string> weapons = cats.weapons;
        std::vector<std::string> shields = cats.shields;
        std::vector<std::string> armor = cats.armor;
        std::vector<std::string> consumables = cats.consumables;
        std::vector<std::string> materials = cats.materials;
        std::vector<std::string> missions = cats.missions;
        std::vector<std::string> others = cats.others;

        std::vector<std::string> categories = {
            "Armas (" + std::to_string(weapons.size()) + ")",
            "Escudos (" + std::to_string(shields.size()) + ")",
            "Armaduras (" + std::to_string(armor.size()) + ")",
            "Consumiveis (" + std::to_string(consumables.size()) + ")",
            "Materiais (" + std::to_string(materials.size()) + ")",
            "Itens de Missao (" + std::to_string(missions.size()) + ")",
            "Outros (" + std::to_string(others.size()) + ")",
            "Voltar"
        };

        int catChoice = InputControl::readSelectionMenuWithArrows(categories, true);
        if (catChoice == 7 || catChoice == -1) { keepRunning = false; break; }

        std::vector<std::string>* listSelected = nullptr;
        if (catChoice == 0) listSelected = &weapons;
        else if (catChoice == 1) listSelected = &shields;
        else if (catChoice == 2) listSelected = &armor;
        else if (catChoice == 3) listSelected = &consumables;
        else if (catChoice == 4) listSelected = &materials;
        else if (catChoice == 5) listSelected = &missions;
        else if (catChoice == 6) listSelected = &others;

        if (listSelected && !listSelected->empty()) {
            bool readingItems = true;
            while (readingItems) {
                Appearance::clearScreen();
                Appearance::displayArtPanel(ArtsDiary::soonDiary, 75, Color::YELLOW, categories[catChoice], false);
                std::cout << "\n";

                std::vector<std::string> menuItems = *listSelected;
                menuItems.push_back("Voltar");

                int itemChoice = InputControl::readSelectionMenuWithArrows(menuItems, true);
                if (static_cast<size_t>(itemChoice) == menuItems.size() - 1 || itemChoice == -1) {
                    readingItems = false;
                } else {
                    std::string nameClean = (*listSelected)[itemChoice].substr(3);
                    inspectItem(currentPlayer, nameClean);
                }
            }
        } else {
            Appearance::printCentralized("Nenhum item nesta categoria.", Appearance::color(Color::GRAY));
            InputControl::waitForEnter();
        }
    }
}

static void displayMenuNPCs(Character* /*jogadorAtual*/) {
    bool keepRunning = true;
    while (keepRunning) {
        Appearance::clearScreen();
        Appearance::displayArtPanel(ArtsDiary::soonDiary, 75, Color::YELLOW, "NPCs CONHECIDOS", false);
        std::cout << "\n";
        std::vector<std::string> npcs = Diary::instance().getNPCsDiscovered();
        if (npcs.empty()) {
            Appearance::printCentralized("Nenhum NPC encontrado ainda.", Appearance::color(Color::GRAY));
            InputControl::waitForEnter();
            return;
        }

        Appearance::sortAlphabetically(npcs);

        std::vector<std::string> options = npcs;
        options.push_back("Voltar");

        int choice = InputControl::readSelectionMenuWithArrows(options, true);
        if (static_cast<size_t>(choice) == options.size() - 1 || choice == -1) {
            keepRunning = false;
        } else {
            inspectNPC(npcs[choice]);
        }
    }
}

static void displayMenuBreeds(Character* currentPlayer) {
    bool keepRunning = true;
    while (keepRunning) {
        Appearance::clearScreen();
        Appearance::displayArtPanel(ArtsDiary::soonDiary, 75, Color::YELLOW, "RACAS DESCOBERTAS", false);
        std::cout << "\n";
        std::vector<std::string> breeds = Diary::instance().getBreedsDiscoveries();
        if (breeds.empty()) {
            Appearance::printCentralized("Nenhuma raca registrada ainda.", Appearance::color(Color::GRAY));
            InputControl::waitForEnter();
            return;
        }

        std::vector<std::string> playable, monsters;
        for (const auto& race : breeds) {
            if (race == "Humano" || race == "Dwarf" || race == "Elfo" || race == "Ork") {
                playable.push_back(race);
            } else {
                monsters.push_back(race);
            }
        }

        Appearance::sortAlphabetically(playable);
        Appearance::sortAlphabetically(monsters);

        std::vector<std::string> categories = {
            "Racas Jogaveis (" + std::to_string(playable.size()) + ")",
            "Monstros e Inimigos (" + std::to_string(monsters.size()) + ")",
            "Voltar"
        };

        int catChoice = InputControl::readSelectionMenuWithArrows(categories, true);
        if (catChoice == 2 || catChoice == -1) { keepRunning = false; break; }

        std::vector<std::string>* listSelected = (catChoice == 0) ? &playable : &monsters;

        if (!listSelected->empty()) {
            bool readingBreeds = true;
            while (readingBreeds) {
                Appearance::clearScreen();
                Appearance::displayArtPanel(ArtsDiary::soonDiary, 75, Color::YELLOW, (catChoice == 0 ? "RACAS JOGAVEIS" : "MONSTROS E INIMIGOS"), false);
                std::cout << "\n";

                std::vector<std::string> menuBreeds = *listSelected;
                menuBreeds.push_back("Voltar");

                int chooseRace = InputControl::readSelectionMenuWithArrows(menuBreeds, true);
                if (static_cast<size_t>(chooseRace) == menuBreeds.size() - 1 || chooseRace == -1) {
                    readingBreeds = false;
                } else {
                    if (catChoice == 0) {
                        inspectRacePlayable((*listSelected)[chooseRace]);
                    } else {
                        ScreenBestiary::displaySheet(currentPlayer, (*listSelected)[chooseRace], 0, {});
                    }
                }
            }
        } else {
            Appearance::printCentralized("Nenhuma raca nesta categoria.", Appearance::color(Color::GRAY));
            InputControl::waitForEnter();
        }
    }
}

static void displayMenuMissions(Character* currentPlayer) {
    bool keepRunning = true;
    while (keepRunning) {
        Appearance::clearScreen();
        Appearance::displayArtPanel(ArtsDiary::soonDiary, 75, Color::YELLOW, "DIARIO DE MISSOES", false);
        std::cout << "\n";

        auto cats = ScreenDiaryLogic::categorizeMissions(currentPlayer);

        std::vector<std::string> lines;

        lines.push_back(Appearance::color(Color::YELLOW) + "Em andamento" + Appearance::color(Color::RESET));
        if (cats.emTempo.empty()) lines.push_back("  (Nenhuma)");
        for (const auto& m : cats.emTempo) lines.push_back("  " + Appearance::color(Color::WHITE) + m + Appearance::color(Color::RESET));
        lines.push_back("");

        lines.push_back(Appearance::color(Color::YELLOW) + "Prontas" + Appearance::color(Color::RESET));
        if (cats.ready.empty()) lines.push_back("  (Nenhuma)");
        for (const auto& m : cats.ready) lines.push_back("  " + Appearance::color(Color::GREEN) + m + Appearance::color(Color::RESET));
        lines.push_back("");

        lines.push_back(Appearance::color(Color::YELLOW) + "Completas" + Appearance::color(Color::RESET));
        if (cats.complete.empty()) lines.push_back("  (Nenhuma)");
        for (const auto& m : cats.complete) lines.push_back("  " + Appearance::color(Color::GRAY) + m + Appearance::color(Color::RESET));

        Appearance::printBlockCentralized(lines);
        std::cout << "\n";

        std::vector<std::string> options = {"Voltar"};
        int choice = InputControl::readSelectionMenuWithArrows(options, true);
        if (choice == 0 || choice == -1) keepRunning = false;
    }
}

static void displayMenuClasses(Character* /*jogadorAtual*/) {
    bool keepRunning = true;
    while (keepRunning) {
        Appearance::clearScreen();
        Appearance::displayArtPanel(ArtsDiary::soonDiary, 75, Color::YELLOW, "CLASSES DESCOBERTAS", false);
        std::cout << "\n";
        std::vector<std::string> classes = Diary::instance().getClassesDiscoveries();
        if (classes.empty()) {
            Appearance::printCentralized("Nenhuma classe registrada ainda.", Appearance::color(Color::GRAY));
            InputControl::waitForEnter();
            return;
        }

        Appearance::sortAlphabetically(classes);

        std::vector<std::string> options = classes;
        options.push_back("Voltar");

        int choice = InputControl::readSelectionMenuWithArrows(options, true);
        if (static_cast<size_t>(choice) == options.size() - 1 || choice == -1) {
            keepRunning = false;
        } else {
            inspectClass(classes[choice]);
        }
    }
}

void ScreenDiaryGO::display(Character* currentPlayer) {
    if (currentPlayer == nullptr) return;

    static auto lastAccess = std::chrono::steady_clock::now() - std::chrono::hours(1);
    bool animateEntrance = BaseScreen::mustAnimateScreenEntry(lastAccess, 300);

    bool keepRunning = true;
    while (keepRunning) {
        Appearance::clearScreen();
        Appearance::displayArtPanel(ArtsDiary::soonDiary, 75, Color::YELLOW, "DIARIO DE JORNADA", animateEntrance);
        animateEntrance = false;

        std::vector<std::string> options = {
            "Bestiario (Inimigos)",
            "Itens (Equipamentos, Consumiveis, Materiais, Missoes)",
            "NPCs Conhecidos",
            "Racas do Mundo",
            "Classes Jogaveis",
            "Missoes (Em andamento, Prontas, Completas)",
            "Progresso",
            "Voltar"
        };

        std::cout << "\n";
        Appearance::printCentralized("O que voce deseja consultar?");
        std::cout << "\n";

        int choice = InputControl::readSelectionMenuWithArrows(options, true);

        switch (choice) {
            case 0:
                ScreenBestiary::displayList(currentPlayer);
                break;
            case 1:
                displayMenuItems(currentPlayer);
                break;
            case 2:
                displayMenuNPCs(currentPlayer);
                break;
            case 3:
                displayMenuBreeds(currentPlayer);
                break;
            case 4:
                displayMenuClasses(currentPlayer);
                break;
            case 5:
                displayMenuMissions(currentPlayer);
                break;
            case 6:
                displayProgress(currentPlayer);
                break;
            case 7:
            case -1:
                keepRunning = false;
                break;
        }
    }
}
