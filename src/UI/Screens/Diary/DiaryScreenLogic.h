#pragma once

#include <string>
#include <vector>
#include <map>

class Character;

struct DataProgress {
    std::vector<std::string> linesNPC;
    std::vector<std::string> linesMonster;
    std::vector<std::string> linesItem;
    int totalAchievements = 0;
};

struct ItemsCategorized {
    std::vector<std::string> weapons;
    std::vector<std::string> shields;
    std::vector<std::string> armor;
    std::vector<std::string> consumables;
    std::vector<std::string> materials;
    std::vector<std::string> missions;
    std::vector<std::string> others;
};

struct MissionsCategorized {
    std::vector<std::string> emTempo;
    std::vector<std::string> ready;
    std::vector<std::string> complete;
};

struct GroupCategorized {
    std::string name;
    std::vector<std::string> items;
};

class ScreenDiaryLogic {
public:
    static DataProgress getProgress();
    static ItemsCategorized categorizeItems(Character* player);
    static MissionsCategorized categorizeMissions(Character* player);
    static std::vector<GroupCategorized> categorizeBestiary();
    static std::vector<GroupCategorized> categorizeNPCs();
    static std::vector<GroupCategorized> categorizeBreeds(const std::vector<std::string>& breedsDiscoveries);
    static std::vector<std::string> getAllClasses();
    static std::vector<std::string> breakText(const std::string& text, int widthMax);
};
