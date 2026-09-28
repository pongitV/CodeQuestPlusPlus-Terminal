#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <mutex>

struct SystemBestiaryEnemyInfo {
    std::string name;
    std::string map;
    std::string habitat;
    std::vector<std::string> appearance;
    std::string lore;
    std::string curiousFact;
    std::string factCurious;
    std::vector<std::string> attributesText;
    std::vector<std::string> activeSkills;
    std::vector<std::string> skillsActive;
    std::string skillPassive;
    std::vector<std::string> drops;
    // Dificuldade base para ordenacao no menu
    int difficulty;
};

class Bestiary {
public:
    static Bestiary& instance();

    void initializeEnemies();
    inline void bootEnemies() { initializeEnemies(); }

    void registerFirstView(const std::string& enemyName);
    void registerDefeat(const std::string& enemyName);
    void registerSkillView(const std::string& enemyName, const std::string& skill);
    void registerDrop(const std::string& enemyName, const std::string& drop);

    bool isDiscovered(const std::string& enemyName) const;
    inline bool thisDiscovered(const std::string& enemyName) const { return isDiscovered(enemyName); }

    bool isDefeated(const std::string& enemyName) const;
    inline bool alreadyDefeated(const std::string& enemyName) const { return isDefeated(enemyName); }

    int getDefeatCount(const std::string& enemyName) const;
    inline int getQuantityDefeats(const std::string& enemyName) const { return getDefeatCount(enemyName); }

    bool hasSeenSkill(const std::string& enemyName, const std::string& skill) const;
    inline bool jaSawSkill(const std::string& enemyName, const std::string& skill) const { return hasSeenSkill(enemyName, skill); }

    bool hasCollectedDrop(const std::string& enemyName, const std::string& drop) const;
    inline bool jaCollectedDrop(const std::string& enemyName, const std::string& drop) const { return hasCollectedDrop(enemyName, drop); }

    const SystemBestiaryEnemyInfo* getInfo(const std::string& enemyName) const;
    std::vector<std::string> getEnemiesOrderedByDifficulty() const;

    void save(std::ofstream& out) const;
    void load(std::ifstream& in);

private:
    Bestiary();
    std::map<std::string, SystemBestiaryEnemyInfo> baseEnemies;
    
    std::set<std::string> seenEnemies;
    std::set<std::string> defeated;
    std::map<std::string, int> defeatCounts;
    std::map<std::string, std::set<std::string>> seenSkills;
    std::map<std::string, std::set<std::string>> collectedDrops;
    
    mutable std::mutex mtx;
};
