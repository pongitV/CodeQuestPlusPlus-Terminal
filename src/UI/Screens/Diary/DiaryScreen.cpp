#include "UI/Screens/Diary/DiaryScreen.h"
#include "UI/Screens/Diary/DiaryScreenLayout.h"
#include "UI/Screens/Diary/DiaryScreenLogic.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include <map>
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "UI/Screens/BaseScreen.h"
#include "UI/Screens/Menu/MenuScreen.h"

#include "Domain/Items/ItemFactory.h"
#include "Domain/Items/Item.h"
#include "Systems/Progression/Diary.h"
#include "Systems/Progression/Bestiary.h"

#include "Domain/Characters/Character.h"
#include "Domain/Characters/Races/Dwarf.h"
#include "Domain/Characters/Races/Elf.h"
#include "Domain/Characters/Races/Human.h"
#include "Domain/Characters/Races/Orc.h"
#include "Domain/Characters/Races/BaseRace.h"
#include "Domain/Characters/Classes/Archer.h"
#include "Domain/Characters/Classes/Bard.h"
#include "Domain/Characters/Classes/Warrior.h"
#include "Domain/Characters/Classes/Mage.h"
#include "Domain/Characters/Classes/Necromancer.h"
#include "Domain/Characters/Classes/BaseClass.h"
#include "Domain/NPCs/NPCBlacksmithLayout.h"
#include "Domain/NPCs/NPCMerchantLayout.h"
#include "Domain/NPCs/NPCMageLayout.h"
#include "Domain/NPCs/NPCGenericKnightLayout.h"

#include "UI/PerspectiveManager.h"

namespace {

const int SOON_HEIGHT = 8;
const int MIN_Y = 11;

enum Dry { MAIN, BESTIARY, ITEMS, NPCS, BREEDS, CLASSES, MISSIONS, PROGRESS };

void addOption(std::vector<std::string>& lines, const std::string& text, bool selected) {
    if (selected) {
        lines.push_back(Appearance::color(Color::GREEN) + " > " + text + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m");
    } else {
        lines.push_back("   " + text);
    }
}

void displayPopupEWait(const std::string& title, const std::vector<std::string>& lines) {
    PerspectiveManager::getDiaryUI().renderPopupMessage(title, lines);
    InputControl::readKey();
}

void displayPopupWithArtEWait(const std::string& title, const std::vector<std::string>& art, const std::vector<std::string>& info, const std::string& subtitle) {
    PerspectiveManager::getDiaryUI().renderPopupInspectionWithArt(title, art, info, subtitle);
    InputControl::readKey();
}

void inspectItem(Character* currentPlayer, const std::string& itemName) {
    auto item = ItemFactory::createItem(itemName);
    if (item) {
        std::vector<std::string> details = item->getDetailsInspection(currentPlayer);
        std::vector<std::string> linesInsp;
        linesInsp.push_back(Appearance::color(Color::YELLOW) + " >> " + item->getItemName() + " <<" + Appearance::color(Color::RESET));
        linesInsp.push_back("");
        linesInsp.insert(linesInsp.end(), details.begin(), details.end());
        displayPopupEWait("INSPECAO DE ITEM", linesInsp);
    }
}

void inspectNPC(const std::string& nameNPC) {
    std::vector<std::string> art;
    std::string lore;
    if (nameNPC.find("Bjorn") != std::string::npos) {
        art = NPCBlacksmithLayouts::artBlacksmith;
        lore = "Bjorn, o Ferreiro da Vila.\nUm anao robusto de poucas palavras.\nSempre disposto a melhorar equipamentos.";
    } else if (nameNPC.find("Franchesco") != std::string::npos) {
        art = NPCMerchantLayouts::artMerchant;
        lore = "Franchesco, o Mercador Ambulante.\nSempre com um sorriso no rosto.\nGosta de moedas de ouro mais do que de viver.";
    } else if (nameNPC.find("Morgana") != std::string::npos) {
        art = NPCMageLayouts::artMage;
        lore = "Morgana, a Bruxa da Floresta.\nUm misterio, domina alquimia e encantamentos.\nSeu labirinto guarda segredos profundos.";
    } else if (nameNPC.find("Cavaleiro Real") != std::string::npos) {
        art = NPCKnightGenericLayouts::artKnight;
        lore = "Cavaleiro Real.\nProtetores leais do Reino.\nFortemente blindados e treinados.";
    } else {
        lore = "Informacoes sobre essa pessoa permanecem um misterio.";
    }
    std::vector<std::string> linesLore;
    size_t post = 0;
    std::string temp = lore;
    while ((post = temp.find('\n')) != std::string::npos) {
        linesLore.push_back(" > " + temp.substr(0, post));
        temp.erase(0, post + 1);
    }
    linesLore.push_back(" > " + temp);
    displayPopupWithArtEWait("INSPECAO DE NPC", art, linesLore, nameNPC);
}

void inspectRace(const std::string& raceName) {
    std::unique_ptr<BaseRace> raceObj;
    if (raceName == "Humano") raceObj = std::make_unique<Human>();
    else if (raceName == "Dwarf") raceObj = std::make_unique<Dwarf>();
    else if (raceName == "Elfo") raceObj = std::make_unique<Elf>();
    else if (raceName == "Ork") raceObj = std::make_unique<Ork>();

    if (raceObj) {
        std::vector<std::string> art = raceObj->getAppearanceRace();
        std::vector<std::string> attributes = MenuScreen::composeAttributesFrame(
            raceObj->getAttributesRace(),
            "[ ATRIBUTOS BASE ]",
            "[ HABILIDADE DA RACA ]",
            raceObj->getNameSkillRace(),
            raceObj->getDescriptionSkillRace()
        );
        displayPopupWithArtEWait("INSPECAO DE RACA", art, attributes, raceName);
    }
}

void inspectClass(const std::string& className) {
    std::unique_ptr<BaseClass> classObj;
    if (className == "Guerreiro") classObj = std::make_unique<Warrior>();
    else if (className == "Mago") classObj = std::make_unique<Wizard>();
    else if (className == "Arqueiro") classObj = std::make_unique<Archer>();
    else if (className == "Bardo") classObj = std::make_unique<Bard>();
    else if (className == "Necromante") classObj = std::make_unique<Necromancer>();

    if (classObj) {
        std::vector<std::string> art = classObj->getAppearanceClassMenu();
        std::vector<std::string> attributes = MenuScreen::composeAttributesFrame(
            classObj->getAttributesClass(),
            "[ ATRIBUTOS BONUS ]",
            "[ PASSIVA DA CLASSE ]",
            classObj->getNamePassiveClass(),
            classObj->getDescriptionPassiveClass(),
            "[ HABILIDADE ATIVA ]",
            classObj->getNameSkillClass(),
            classObj->getDescriptionSkillClass()
        );
        displayPopupWithArtEWait("INSPECAO DE CLASSE", art, attributes, className);
    }
}

void inspectBestiary(const std::string& nameEnemy) {
    const auto* info = Bestiary::instance().getInfo(nameEnemy);
    if (info) {
        std::vector<std::string> details;
        if (Bestiary::instance().alreadyDefeated(nameEnemy)) {
            details.push_back(Appearance::color(Color::RED) + "Derrotas: " + std::to_string(Bestiary::instance().getQuantityDefeats(nameEnemy)) + Appearance::color(Color::RESET));
            details.push_back("");
            auto loreLines = ScreenDiaryLogic::breakText(info->lore, 50);
            for (const auto& l : loreLines)
                details.push_back(Appearance::color(Color::GRAY) + l + Appearance::color(Color::RESET));
            auto factLines = ScreenDiaryLogic::breakText(info->factCurious, 50);
            for (const auto& f : factLines)
                details.push_back(Appearance::color(Color::YELLOW) + f + Appearance::color(Color::RESET));
        } else {
            details.push_back(Appearance::color(Color::GRAY) + "Ainda nao derrotado. Pouco se sabe sobre seus costumes." + Appearance::color(Color::RESET));
        }
        displayPopupWithArtEWait("BESTIARIO", info->appearance, details, nameEnemy);
    }
}

void displayRaycaster(Character* currentPlayer) {
    Dry dry = MAIN;
    int sel = 0;
    int selSub = 0;
    bool running = true;

    bool emList = false;
    int idxGroup = 0;
    std::vector<GroupCategorized> groups;
    std::vector<std::string> listCurrent;

    bool redesignComplete = true;

    while (running) {
        if (redesignComplete) {
            PerspectiveManager::getDiaryUI().renderBackground();
        }
        std::cout << "\033[?25l";

        std::vector<std::string> lines;
        std::string titleBox;
        std::vector<std::string> interactive;

        bool isGroupView = !emList && (dry == BESTIARY || dry == ITEMS || dry == NPCS || dry == BREEDS);

        if (dry == MAIN) {
            titleBox = " DIARIO DE JORNADA ";
            interactive = {"Bestiario (Inimigos)", "Itens (Descobertos)", "NPCs Conhecidos",
                           "Racas do Mundo", "Classes Jogaveis", "Missoes (Diario)",
                           "Progresso e Feitos", "Voltar"};
            for (size_t i = 0; i < interactive.size(); ++i)
                addOption(lines, interactive[i], (int)i == sel);
        } else if (isGroupView) {
            if (dry == BESTIARY) {
                titleBox = " BESTIARIO ";
                groups = ScreenDiaryLogic::categorizeBestiary();
            } else if (dry == ITEMS) {
                titleBox = " ITENS DESCOBERTOS ";
            } else if (dry == NPCS) {
                titleBox = " NPCS CONHECIDOS ";
                groups = ScreenDiaryLogic::categorizeNPCs();
            } else if (dry == BREEDS) {
                titleBox = " RACAS DESCOBERTAS ";
                auto allBreeds = Diary::instance().getBreedsDiscoveries();
                groups = ScreenDiaryLogic::categorizeBreeds(allBreeds);
            }

            if (dry == ITEMS) {
                auto cats = ScreenDiaryLogic::categorizeItems(currentPlayer);
                struct CatInfo { const char* name; std::vector<std::string>* list; };
                CatInfo allCats[] = {
                    {"Armas", &cats.weapons}, {"Escudos", &cats.shields},
                    {"Armaduras", &cats.armor}, {"Consumiveis", &cats.consumables},
                    {"Materiais", &cats.materials}, {"Missoes", &cats.missions},
                    {"Outros", &cats.others}
                };
                groups.clear();
                for (auto& ci : allCats) {
                    if (!ci.list->empty())
                        groups.push_back({ci.name, *ci.list});
                }
            }

            if (groups.empty()) {
                lines.push_back("   " + Appearance::color(Color::GRAY) + "Nenhum registro encontrado ainda." + Appearance::color(Color::RESET));
                lines.push_back("");
                interactive.push_back("Voltar");
                addOption(lines, "Voltar", selSub == 0);
            } else {
                for (size_t i = 0; i < groups.size(); ++i) {
                    interactive.push_back(groups[i].name);
                    std::string txt = groups[i].name + " (" + std::to_string(groups[i].items.size()) + ")";
                    addOption(lines, txt, (int)i == selSub);
                }
                lines.push_back("");
                interactive.push_back("Voltar");
                addOption(lines, "Voltar", selSub == (int)groups.size());
            }
        } else {
            if (dry == MISSIONS) titleBox = " DIARIO DE MISSOES ";
            else if (dry == PROGRESS) titleBox = " PROGRESSO E FEITOS ";
            else if (dry == CLASSES) titleBox = " CLASSES JOGAVEIS ";
            else if (dry == BESTIARY) titleBox = groups.empty() ? " BESTIARIO " : (" " + groups[idxGroup].name + " ");
            else if (dry == ITEMS) titleBox = groups.empty() ? " ITENS " : (" " + groups[idxGroup].name + " ");
            else if (dry == NPCS) titleBox = groups.empty() ? " NPCS " : (" " + groups[idxGroup].name + " ");
            else if (dry == BREEDS) titleBox = groups.empty() ? " RACAS " : (" " + groups[idxGroup].name + " ");

            if (dry == PROGRESS) {
                auto data = ScreenDiaryLogic::getProgress();
                if (data.totalAchievements == 0) {
                    lines.push_back("   " + Appearance::color(Color::GRAY) + "Nenhum grande feito para registrar ainda..." + Appearance::color(Color::RESET));
                } else {
                    for (const auto& l : data.linesNPC) lines.push_back(l);
                    for (const auto& l : data.linesMonster) lines.push_back(l);
                    for (const auto& l : data.linesItem) lines.push_back(l);
                    if (!lines.empty()) lines.pop_back();
                }
                lines.push_back("");
                interactive.push_back("Voltar");
                addOption(lines, "Voltar", selSub == 0);
            } else if (dry == MISSIONS) {
                auto cats = ScreenDiaryLogic::categorizeMissions(currentPlayer);
                lines.push_back(Appearance::color(Color::YELLOW) + "Em andamento" + Appearance::color(Color::RESET));
                if (cats.inProgress.empty()) lines.push_back("  (Nenhuma)");
                for (const auto& m : cats.inProgress) lines.push_back("  " + Appearance::color(Color::WHITE) + m + Appearance::color(Color::RESET));
                lines.push_back("");
                lines.push_back(Appearance::color(Color::YELLOW) + "Prontas" + Appearance::color(Color::RESET));
                if (cats.ready.empty()) lines.push_back("  (Nenhuma)");
                for (const auto& m : cats.ready) lines.push_back("  " + Appearance::color(Color::GREEN) + m + Appearance::color(Color::RESET));
                lines.push_back("");
                lines.push_back(Appearance::color(Color::YELLOW) + "Completas" + Appearance::color(Color::RESET));
                if (cats.complete.empty()) lines.push_back("  (Nenhuma)");
                for (const auto& m : cats.complete) lines.push_back("  " + Appearance::color(Color::GRAY) + m + Appearance::color(Color::RESET));
                lines.push_back("");
                interactive.push_back("Voltar");
                addOption(lines, "Voltar", selSub == 0);
            } else {
                if (dry == CLASSES) {
                    listCurrent = ScreenDiaryLogic::getAllClasses();
                } else if (!groups.empty() && idxGroup < (int)groups.size()) {
                    listCurrent = groups[idxGroup].items;
                }

                if (listCurrent.empty()) {
                    lines.push_back("   " + Appearance::color(Color::GRAY) + "Nenhum registro encontrado ainda." + Appearance::color(Color::RESET));
                } else {
                    for (size_t i = 0; i < listCurrent.size(); ++i) {
                        interactive.push_back(listCurrent[i]);
                        addOption(lines, listCurrent[i], (int)i == selSub);
                    }
                }
                lines.push_back("");
                interactive.push_back("Voltar");
                addOption(lines, "Voltar", selSub == (int)listCurrent.size());
            }
        }

        int totalOptions = interactive.size();

        if (dry == MAIN) {
            if (sel >= totalOptions && totalOptions > 0) sel = totalOptions - 1;
        } else {
            if (selSub >= totalOptions && totalOptions > 0) selSub = totalOptions - 1;
        }

        std::vector<std::string> boxPreview = BaseScreen::createBox(lines, titleBox, 0, Color::YELLOW, "\033[48;2;25;25;25m");
        int termH = Appearance::getTerminalHeight();
        int outH = (int)boxPreview.size();
        
        int soonHeight = 8; // Altura aproximada do logo do diario
        int totalH = outH + soonHeight + 1;
        int startYBox = 0;
        if (termH > totalH) {
            startYBox = (termH - totalH) / 2 + soonHeight + 1;
        } else {
            startYBox = std::max(0, (termH - outH) / 2);
        }
        if (startYBox + outH > termH) startYBox = std::max(0, termH - outH);

        PerspectiveManager::getDiaryUI().displayHeader(startYBox);
        PerspectiveManager::getDiaryUI().renderBox(lines, titleBox, Color::YELLOW, 0, startYBox);

        redesignComplete = false;
        char key = InputControl::readKey();

        auto moveCursor = [&](int say) {
            int& s = (dry == MAIN) ? sel : selSub;
            s += say;
            if (s < 0) s = totalOptions - 1;
            if (s >= totalOptions) s = 0;
        };

        if (key == 'w' || key == 'W') moveCursor(-1);
        else if (key == 's' || key == 'S') moveCursor(1);
        else if (key == '\n' || key == '\r') {
            redesignComplete = true;
            if (dry == MAIN) {
                switch (sel) {
                    case 0: dry = BESTIARY; break;
                    case 1: dry = ITEMS; break;
                    case 2: dry = NPCS; break;
                    case 3: dry = BREEDS; break;
                    case 4: dry = CLASSES; break;
                    case 5: dry = MISSIONS; break;
                    case 6: dry = PROGRESS; break;
                    case 7: running = false; break;
                }
                emList = false;
                selSub = 0;
                idxGroup = 0;
                groups.clear();
                listCurrent.clear();
            } else if (isGroupView) {
                if (selSub == (int)groups.size()) {
                    dry = MAIN;
                    emList = false;
                } else if (selSub >= 0 && selSub < (int)groups.size()) {
                    idxGroup = selSub;
                    emList = true;
                    selSub = 0;
                }
    } else {
            int lastIdx = (int)listCurrent.size();
            if (selSub == lastIdx || listCurrent.empty()) {
                if (dry == MISSIONS || dry == PROGRESS || dry == CLASSES) {
                    dry = MAIN;
                } else {
                    emList = false;
                    selSub = 0;
                }
                } else if (selSub >= 0 && selSub < lastIdx) {
                    std::string itemSel = listCurrent[selSub];
                    if (dry == ITEMS) {
                        std::string itemName = (itemSel.size() > 3) ? itemSel.substr(3) : itemSel;
                        inspectItem(currentPlayer, itemName);
                    } else if (dry == NPCS) {
                        inspectNPC(itemSel);
                    } else if (dry == BREEDS) {
                        inspectRace(itemSel);
                    } else if (dry == CLASSES) {
                        inspectClass(itemSel);
                    } else if (dry == BESTIARY) {
                        inspectBestiary(itemSel);
                    }
                }
            }
        }
    }
}

} // anonymous namespace

void DiaryScreen::display(Character* currentPlayer) {
    if (!currentPlayer) return;

    displayRaycaster(currentPlayer);
}
