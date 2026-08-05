#include "UI/Screens/Diary/DiaryScreenLogic.h"
#include "Systems/Progression/Progression.h"
#include "Systems/Progression/ProgressionFlags.h"
#include "Systems/Progression/Diary.h"
#include "Systems/Progression/Bestiary.h"
#include "Domain/Items/ItemFactory.h"
#include "Domain/Items/Item.h"
#include "Domain/Characters/Character.h"
#include "Core/Utils/Appearance.h"

#include <algorithm>
#include <map>
#include <functional>
#include <sstream>

namespace {
    enum class CategoryProgress { NPC, MONSTER, ITEM };

    struct ItemProgress {
        const char* flag;
        const char* name;
        const char* description;
        CategoryProgress category;
    };

    const std::vector<ItemProgress> itemsDeProgress = {
        {Flags::Village_KissRescued, "O Salvador da Forja", "Resgatou o ferreiro Bjorn encurralado por um Orc.", CategoryProgress::NPC},
        {Flags::Village_InvitationReal, "Passe Real", "Ajudou os cavaleiros a se livrarem dos Trolls e recebeu um convite para o Reino.", CategoryProgress::NPC},
        {Flags::Forest_MorganaQuest, "Pacto com a Bruxa", "Entregou os Coracoes da Floresta para Morgana e recebeu a chave para o Labirinto.", CategoryProgress::NPC},
        {Flags::Forest_MahoragaDefeated, "Ritual concluido", "Derrotou Mahoraga pela primeira vez.", CategoryProgress::MONSTER},
        {Flags::BridgeKingdom_TrollDefeated, "Pacificador do Reino", "Derrotou todos os Trolls que invadiram a entrada do Reino.", CategoryProgress::MONSTER}
    };

    struct MissionRegistration {
        std::string id;
        std::string name;
        std::function<bool(Character*)> checkRequirements;
    };

    const std::vector<MissionRegistration> registrationDeMissions = {
        {
            "morgana_coracoes",
            "Consiga 3x Coracoes da floresta (Morgana)",
            [](Character* p) { return p->getInventory()->countItem("Coracao da floresta") >= 3; }
        },
        {
            "cavaleiro_trolls",
            "Reportar Trolls derrotados (Cavaleiro Real)",
            [](Character*) { return Progression::instance().getFlag(Flags::BridgeKingdom_TrollDefeated); }
        }
    };
}

DataProgress ScreenDiaryLogic::getProgress() {
    DataProgress data;
    int achievementsDisplayed = 0;

    auto processCategory = [&](const std::string& title, CategoryProgress category) -> int {
        std::vector<std::string>* target = nullptr;
        if (category == CategoryProgress::NPC) target = &data.linesNPC;
        else if (category == CategoryProgress::MONSTER) target = &data.linesMonster;
        else if (category == CategoryProgress::ITEM) target = &data.linesItem;

        int count = 0;
        for (const auto& item : itemsDeProgress) {
            if (item.category == category && Progression::instance().getFlag(item.flag)) {
                if (target->empty()) {
                    target->push_back("  " + Appearance::color(Color::CYAN) + "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90 " + title + " \xe2\x95\x90\xe2\x95\x90\xe2\x95\x90" + Appearance::color(Color::RESET));
                    target->push_back("");
                }
                std::string colorName = Appearance::color(Color::YELLOW);
                std::string colorDesc = Appearance::color(Color::WHITE);
                std::string status = Appearance::color(Color::GREEN) + "[CONCLUIDO]";

                target->push_back("    " + status + " " + colorName + item.name + Appearance::color(Color::RESET));
                target->push_back("      " + colorDesc + "  " + item.description + Appearance::color(Color::RESET));
                target->push_back("");
                count++;
            }
        }
        return count;
    };

    achievementsDisplayed += processCategory("NPCs", CategoryProgress::NPC);
    achievementsDisplayed += processCategory("Monstros", CategoryProgress::MONSTER);
    achievementsDisplayed += processCategory("Itens", CategoryProgress::ITEM);

    data.totalAchievements = achievementsDisplayed;
    return data;
}

ItemsCategorized ScreenDiaryLogic::categorizeItems(Character* player) {
    (void)player;
    ItemsCategorized result;
    std::vector<std::string> items = Diary::instance().getItemsDiscovered();

    for (const auto& itemName : items) {
        auto tempItem = ItemFactory::createItem(itemName);
        if (tempItem) {
            EquipmentType type = tempItem->getType();
            std::string prefix = " - " + itemName;
            if (type == EquipmentType::WEAPONS) result.weapons.push_back(prefix);
            else if (type == EquipmentType::SHIELD) result.shields.push_back(prefix);
            else if (type == EquipmentType::ARMOR) result.armor.push_back(prefix);
            else if (type == EquipmentType::CONSUMABLE) result.consumables.push_back(prefix);
            else if (type == EquipmentType::MATERIAL) result.materials.push_back(prefix);
            else if (type == EquipmentType::MISSION) result.missions.push_back(prefix);
            else result.others.push_back(prefix);
        } else {
            result.others.push_back(" - " + itemName);
        }
    }

    Appearance::sortAlphabetically(result.weapons);
    Appearance::sortAlphabetically(result.shields);
    Appearance::sortAlphabetically(result.armor);
    Appearance::sortAlphabetically(result.consumables);
    Appearance::sortAlphabetically(result.materials);
    Appearance::sortAlphabetically(result.missions);
    Appearance::sortAlphabetically(result.others);

    return result;
}

MissionsCategorized ScreenDiaryLogic::categorizeMissions(Character* player) {
    MissionsCategorized result;

    for (const auto& mission : registrationDeMissions) {
        if (Diary::instance().missionCompleted(mission.id)) {
            result.complete.push_back("[V] " + mission.name);
        } else if (Diary::instance().missionAccept(mission.id)) {
            if (mission.checkRequirements(player)) {
                result.ready.push_back("[X] " + mission.name);
            } else {
                result.emTempo.push_back("[ ] " + mission.name);
            }
        }
    }

    return result;
}

std::vector<GroupCategorized> ScreenDiaryLogic::categorizeBestiary() {
    std::map<std::string, std::vector<std::string>> groups;
    auto order = Bestiary::instance().getEnemiesOrderedByDifficulty();
    for (const auto& name : order) {
        if (!Bestiary::instance().thisDiscovered(name)) continue;
        const auto* info = Bestiary::instance().getInfo(name);
        std::string map = info ? info->map : "Desconhecido";
        groups[map].push_back(name);
    }

    std::vector<GroupCategorized> result;
    for (auto& pair : groups) {
        Appearance::sortAlphabetically(pair.second);
        result.push_back({pair.first, pair.second});
    }
    return result;
}

std::vector<GroupCategorized> ScreenDiaryLogic::categorizeNPCs() {
    std::map<std::string, std::vector<std::string>> groups;
    auto npcs = Diary::instance().getNPCsDiscovered();
    for (const auto& name : npcs) {
        std::string area = "Viajante";
        if (name.find("Bjorn") != std::string::npos || name.find("Cavaleiro Real") != std::string::npos) {
            area = "Vila/Reino";
        } else if (name.find("Morgana") != std::string::npos) {
            area = "Floresta";
        } else if (name.find("Franchesco") != std::string::npos) {
            area = "Viajante";
        }
        groups[area].push_back(name);
    }

    std::vector<GroupCategorized> result;
    for (auto& pair : groups) {
        Appearance::sortAlphabetically(pair.second);
        result.push_back({pair.first, pair.second});
    }
    return result;
}

std::vector<GroupCategorized> ScreenDiaryLogic::categorizeBreeds(const std::vector<std::string>& breedsDiscoveries) {
    std::vector<std::string> playable, monsters;
    for (const auto& race : breedsDiscoveries) {
        if (race == "Humano" || race == "Dwarf" || race == "Elfo" || race == "Ork") {
            playable.push_back(race);
        } else {
            monsters.push_back(race);
        }
    }
    Appearance::sortAlphabetically(playable);
    Appearance::sortAlphabetically(monsters);

    std::vector<GroupCategorized> result;
    if (!playable.empty()) result.push_back({"Racas Jogaveis", playable});
    if (!monsters.empty()) result.push_back({"Monstros e Inimigos", monsters});
    return result;
}

std::vector<std::string> ScreenDiaryLogic::getAllClasses() {
    return {"Guerreiro", "Mago", "Arqueiro", "Bardo", "Necromante"};
}

std::vector<std::string> ScreenDiaryLogic::breakText(const std::string& text, int widthMax) {
    std::vector<std::string> result;
    std::istringstream stream(text);
    std::string lineCurrent;
    std::string word;
    while (stream >> word) {
        if (lineCurrent.length() + word.length() + (lineCurrent.empty() ? 0 : 1) > (size_t)widthMax) {
            if (!lineCurrent.empty()) result.push_back(lineCurrent);
            lineCurrent = word;
        } else {
            if (!lineCurrent.empty()) lineCurrent += ' ';
            lineCurrent += word;
        }
    }
    if (!lineCurrent.empty()) result.push_back(lineCurrent);
    if (result.empty() && !text.empty()) result.push_back(text);
    return result;
}
