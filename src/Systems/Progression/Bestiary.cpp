#include "Systems/Progression/Bestiary.h"
#include <algorithm>
#include "Domain/Characters/Character.h"
#include "Domain/Enemies/Goblin.h"
#include "Domain/Enemies/Slime.h"
#include "Domain/Enemies/Fairy.h"
#include "Domain/Enemies/ExiledOrc.h"
#include "Domain/Enemies/ForestAbomination.h"
#include "Domain/Enemies/Troll.h"
#include "Domain/Enemies/Mimic.h"
#include "Domain/Enemies/Mahoraga.h"
#include "Domain/Enemies/BaseEnemyClass.h"

Bestiary& Bestiary::instance() {
    static Bestiary inst;
    return inst;
}

Bestiary::Bestiary() {
    bootEnemies();
}

namespace {
    template<typename T>
    void registerNoBestiary(std::map<std::string, SystemBestiaryEnemyInfo>& enemiesBase) {
        T race;
        BaseEnemyClass classPattern;
        Attributes attr = race.getAttributesRace();
        InfoBestiary info = race.getInfoBestiary();
        
        std::vector<std::string> attrText = {
            " > Vida           : " + std::to_string(attr.health),
            " > Forca          : " + std::to_string(attr.strength),
            " > Destreza       : " + std::to_string(attr.dexterity),
            " > Resistencia    : " + std::to_string(attr.resistance),
            " > Constituicao   : " + std::to_string(attr.constitution),
            " > Inteligencia   : " + std::to_string(attr.intelligence),
            " > Sabedoria      : " + std::to_string(attr.wisdom)
        };

        enemiesBase[race.getRaceName()] = {
            race.getRaceName(), info.map, info.habitat,
            race.getAppearanceRace(),
            info.lore,
            info.factCurious,
            attrText,
            {classPattern.getNameSkillClass() + " | " + classPattern.getDescriptionSkillClass()},
            race.getNameSkillRace() + " | " + race.getDescriptionSkillRace(),
            info.drops,
            info.difficulty
        };
    }
}

void Bestiary::bootEnemies() {
    registerNoBestiary<Goblin>(enemiesBase);
    registerNoBestiary<Slime>(enemiesBase);
    registerNoBestiary<Fairy>(enemiesBase);
    registerNoBestiary<ExiledOrc>(enemiesBase);
    registerNoBestiary<ForestAbomination>(enemiesBase);
    registerNoBestiary<Troll>(enemiesBase);
    registerNoBestiary<Mimic>(enemiesBase);
    registerNoBestiary<Mahoraga>(enemiesBase);
}

void Bestiary::registerFirstView(const std::string& nameEnemy) {
    std::lock_guard<std::mutex> lock(mtx);
    if (enemiesBase.count(nameEnemy)) visas.insert(nameEnemy);
}

void Bestiary::registerDefeat(const std::string& nameEnemy) {
    std::lock_guard<std::mutex> lock(mtx);
    if (enemiesBase.count(nameEnemy)) {
        visas.insert(nameEnemy);
        defeated.insert(nameEnemy);
        quantityDefeats[nameEnemy]++;
    }
}

void Bestiary::registerSkillView(const std::string& nameEnemy, const std::string& skill) {
    std::lock_guard<std::mutex> lock(mtx);
    if (enemiesBase.count(nameEnemy)) skillsViews[nameEnemy].insert(skill);
}

void Bestiary::registerDrop(const std::string& nameEnemy, const std::string& drop) {
    std::lock_guard<std::mutex> lock(mtx);
    if (enemiesBase.count(nameEnemy)) dropsCollected[nameEnemy].insert(drop);
}

bool Bestiary::thisDiscovered(const std::string& nameEnemy) const {
    std::lock_guard<std::mutex> lock(mtx);
    return visas.count(nameEnemy) > 0;
}

bool Bestiary::jaDefeated(const std::string& nameEnemy) const {
    std::lock_guard<std::mutex> lock(mtx);
    return defeated.count(nameEnemy) > 0;
}

int Bestiary::getQuantityDefeats(const std::string& nameEnemy) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = quantityDefeats.find(nameEnemy);
    if (it != quantityDefeats.end()) {
        return it->second;
    }
    return 0;
}

bool Bestiary::jaSawSkill(const std::string& nameEnemy, const std::string& skill) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = skillsViews.find(nameEnemy);
    if (it != skillsViews.end()) return it->second.count(skill) > 0;
    return false;
}

bool Bestiary::jaCollectedDrop(const std::string& nameEnemy, const std::string& drop) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = dropsCollected.find(nameEnemy);
    if (it != dropsCollected.end()) return it->second.count(drop) > 0;
    return false;
}

const SystemBestiaryEnemyInfo* Bestiary::getInfo(const std::string& nameEnemy) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = enemiesBase.find(nameEnemy);
    if (it != enemiesBase.end()) return &it->second;
    return nullptr;
}

std::vector<std::string> Bestiary::getEnemiesOrderedByDifficulty() const {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::string> names;
    names.reserve(enemiesBase.size());
    for (const auto& pair : enemiesBase) names.push_back(pair.first);
    
    std::sort(names.begin(), names.end(), [this](const std::string& a, const std::string& b) {
        return enemiesBase.at(a).difficulty < enemiesBase.at(b).difficulty;
    });
    
    return names;
}

void Bestiary::save(std::ofstream& out) const {
    std::lock_guard<std::mutex> lock(mtx);
    
    auto writeSet = [&](const auto& set) {
        out << set.size() << "\n";
        for (const auto& item : set) out << item << "\n";
    };

    writeSet(visas);
    writeSet(defeated);

    out << quantityDefeats.size() << "\n";
    for (const auto& [name, qty] : quantityDefeats) out << name << "\n" << qty << "\n";

    auto writeMapSets = [&](const auto& map) {
        out << map.size() << "\n";
        for (const auto& [name, set] : map) {
            out << name << "\n";
            writeSet(set);
        }
    };

    writeMapSets(skillsViews);
    writeMapSets(dropsCollected);
}

void Bestiary::load(std::ifstream& in) {
    std::lock_guard<std::mutex> lock(mtx);
    visas.clear();
    defeated.clear();
    quantityDefeats.clear();
    skillsViews.clear();
    dropsCollected.clear();

    auto readSet = [&](auto& set) {
        size_t size;
        if (!(in >> size)) return false;
        std::string line; std::getline(in, line);
        for (size_t i = 0; i < size; ++i) {
            std::getline(in, line);
            set.insert(line);
        }
        return true;
    };
    
    if (!readSet(visas)) return; // Failsafe para saves antigos
    readSet(defeated);
    
    size_t qtyDefeatsSize;
    if (in >> qtyDefeatsSize) {
        std::string line; std::getline(in, line);
        for (size_t i = 0; i < qtyDefeatsSize; ++i) {
            std::string name; std::getline(in, name);
            int qty; in >> qty; std::getline(in, line);
            quantityDefeats[name] = qty;
        }
    }

    auto readMap = [&](auto& map) {
        size_t size;
        if (!(in >> size)) return;
        std::string line; std::getline(in, line);
        for (size_t i = 0; i < size; ++i) {
            std::string key; std::getline(in, key);
            readSet(map[key]);
        }
    };

    readMap(skillsViews);
    readMap(dropsCollected);
}






