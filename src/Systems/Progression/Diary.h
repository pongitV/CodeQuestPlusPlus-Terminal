#pragma once

#include <string>
#include <set>
#include <mutex>
#include <fstream>
#include <vector>

class Diary {
public:
    static Diary& instance();

    void registerItem(const std::string& itemName);
    void registerNPC(const std::string& nameNPC);
    void registerRace(const std::string& raceName);
    void registerClass(const std::string& className);
    void registerMissionAccept(const std::string& idMission);
    void registerMissionCompleted(const std::string& idMission);

    bool itemDiscovered(const std::string& itemName) const;
    bool npcDiscovered(const std::string& nameNPC) const;
    bool raceDiscovery(const std::string& raceName) const;
    bool classDiscovery(const std::string& className) const;
    bool missionAccept(const std::string& idMission) const;
    bool missionCompleted(const std::string& idMission) const;

    std::vector<std::string> getItemsDiscovered() const;
    std::vector<std::string> getNPCsDiscovered() const;
    std::vector<std::string> getBreedsDiscoveries() const;
    std::vector<std::string> getClassesDiscoveries() const;
    std::vector<std::string> getMissionsAccepted() const;
    std::vector<std::string> getMissionsCompleted() const;

    void save(std::ofstream& out) const;
    void load(std::ifstream& in);

private:
    Diary();
    
    std::set<std::string> itemsDiscovered;
    std::set<std::string> npcsDiscovered;
    std::set<std::string> breedsDiscoveries;
    std::set<std::string> classesDiscoveries;
    std::set<std::string> missionsAccepted;
    std::set<std::string> missionsCompleted;

    mutable std::mutex mtx;
};
