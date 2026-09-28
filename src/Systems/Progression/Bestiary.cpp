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
    initializeEnemies();
}

namespace {
    template<typename T>
    void registerInBestiary(std::map<std::string, SystemBestiaryEnemyInfo>& baseEnemies) {
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

        std::vector<std::string> skills = {classPattern.getNameSkillClass() + " | " + classPattern.getDescriptionSkillClass()};

        baseEnemies[race.getRaceName()] = {
            race.getRaceName(), info.map, info.habitat,
            race.getAppearanceRace(),
            info.lore,
            info.factCurious,
            info.factCurious,
            attrText,
            skills,
            skills,
            race.getNameSkillRace() + " | " + race.getDescriptionSkillRace(),
            info.drops,
            info.difficulty
        };
    }
}

void Bestiary::initializeEnemies() {
    registerInBestiary<Goblin>(baseEnemies);
    registerInBestiary<Slime>(baseEnemies);
    registerInBestiary<Fairy>(baseEnemies);
    registerInBestiary<ExiledOrc>(baseEnemies);
    registerInBestiary<ForestAbomination>(baseEnemies);
    registerInBestiary<Troll>(baseEnemies);
    registerInBestiary<Mimic>(baseEnemies);
    registerInBestiary<Mahoraga>(baseEnemies);
}

void Bestiary::registerFirstView(const std::string& enemyName) {
    std::lock_guard<std::mutex> lock(mtx);
    if (baseEnemies.count(enemyName)) seenEnemies.insert(enemyName);
}

void Bestiary::registerDefeat(const std::string& enemyName) {
    std::lock_guard<std::mutex> lock(mtx);
    if (baseEnemies.count(enemyName)) {
        seenEnemies.insert(enemyName);
        defeated.insert(enemyName);
        defeatCounts[enemyName]++;
    }
}

void Bestiary::registerSkillView(const std::string& enemyName, const std::string& skill) {
    std::lock_guard<std::mutex> lock(mtx);
    if (baseEnemies.count(enemyName)) seenSkills[enemyName].insert(skill);
}

void Bestiary::registerDrop(const std::string& enemyName, const std::string& drop) {
    std::lock_guard<std::mutex> lock(mtx);
    if (baseEnemies.count(enemyName)) collectedDrops[enemyName].insert(drop);
}

bool Bestiary::isDiscovered(const std::string& enemyName) const {
    std::lock_guard<std::mutex> lock(mtx);
    return seenEnemies.count(enemyName) > 0;
}

bool Bestiary::isDefeated(const std::string& enemyName) const {
    std::lock_guard<std::mutex> lock(mtx);
    return defeated.count(enemyName) > 0;
}

int Bestiary::getDefeatCount(const std::string& enemyName) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = defeatCounts.find(enemyName);
    if (it != defeatCounts.end()) {
        return it->second;
    }
    return 0;
}

bool Bestiary::hasSeenSkill(const std::string& enemyName, const std::string& skill) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = seenSkills.find(enemyName);
    if (it != seenSkills.end()) return it->second.count(skill) > 0;
    return false;
}

bool Bestiary::hasCollectedDrop(const std::string& enemyName, const std::string& drop) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = collectedDrops.find(enemyName);
    if (it != collectedDrops.end()) return it->second.count(drop) > 0;
    return false;
}

const SystemBestiaryEnemyInfo* Bestiary::getInfo(const std::string& enemyName) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = baseEnemies.find(enemyName);
    if (it != baseEnemies.end()) return &it->second;
    return nullptr;
}

std::vector<std::string> Bestiary::getEnemiesOrderedByDifficulty() const {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::string> names;
    names.reserve(baseEnemies.size());
    for (const auto& pair : baseEnemies) names.push_back(pair.first);
    
    std::sort(names.begin(), names.end(), [this](const std::string& a, const std::string& b) {
        return baseEnemies.at(a).difficulty < baseEnemies.at(b).difficulty;
    });
    
    return names;
}

void Bestiary::save(std::ofstream& out) const {
    std::lock_guard<std::mutex> lock(mtx);
    
    auto writeSet = [&](const auto& set) {
        out << set.size() << "\n";
        for (const auto& item : set) out << item << "\n";
    };

    writeSet(seenEnemies);
    writeSet(defeated);

    out << defeatCounts.size() << "\n";
    for (const auto& [name, qty] : defeatCounts) out << name << "\n" << qty << "\n";

    auto writeMapSets = [&](const auto& map) {
        out << map.size() << "\n";
        for (const auto& [name, set] : map) {
            out << name << "\n";
            writeSet(set);
        }
    };

    writeMapSets(seenSkills);
    writeMapSets(collectedDrops);
}

void Bestiary::load(std::ifstream& in) {
    std::lock_guard<std::mutex> lock(mtx);
    seenEnemies.clear();
    defeated.clear();
    defeatCounts.clear();
    seenSkills.clear();
    collectedDrops.clear();

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
    
    // Failsafe para compatibilidade com arquivos de save antigos
    if (!readSet(seenEnemies)) return;
    readSet(defeated);
    
    size_t qtyDefeatsSize;
    if (in >> qtyDefeatsSize) {
        std::string line; std::getline(in, line);
        for (size_t i = 0; i < qtyDefeatsSize; ++i) {
            std::string name; std::getline(in, name);
            int qty; in >> qty; std::getline(in, line);
            defeatCounts[name] = qty;
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

    readMap(seenSkills);
    readMap(collectedDrops);
}
