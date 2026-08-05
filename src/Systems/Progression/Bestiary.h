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
    std::string factCurious;
    std::vector<std::string> attributesText;
    std::vector<std::string> skillsActive;
    std::string skillPassive;
    std::vector<std::string> drops;
    int difficulty; // Dificuldade base para ordenar no menu
};

class Bestiary {
public:
    static Bestiary& instance();

    void bootEnemies();

    void registerFirstView(const std::string& nameEnemy);
    void registerDefeat(const std::string& nameEnemy);
    void registerSkillView(const std::string& nameEnemy, const std::string& skill);
    void registerDrop(const std::string& nameEnemy, const std::string& drop);

    bool thisDiscovered(const std::string& nameEnemy) const;
    bool jaDefeated(const std::string& nameEnemy) const;
    int getQuantityDefeats(const std::string& nameEnemy) const;
    bool jaSawSkill(const std::string& nameEnemy, const std::string& skill) const;
    bool jaCollectedDrop(const std::string& nameEnemy, const std::string& drop) const;

    const SystemBestiaryEnemyInfo* getInfo(const std::string& nameEnemy) const;
    std::vector<std::string> getEnemiesOrderedByDifficulty() const;

    void save(std::ofstream& out) const;
    void load(std::ifstream& in);

private:
    Bestiary();
    std::map<std::string, SystemBestiaryEnemyInfo> enemiesBase;
    
    std::set<std::string> visas;
    std::set<std::string> defeated;
    std::map<std::string, int> quantityDefeats;
    std::map<std::string, std::set<std::string>> skillsViews;
    std::map<std::string, std::set<std::string>> dropsCollected;
    
    mutable std::mutex mtx;
};
