#include "Systems/Progression/Diary.h"

Diary& Diary::instance() {
    static Diary inst;
    return inst;
}

Diary::Diary() {}

void Diary::registerItem(const std::string& itemName) {
    std::lock_guard<std::mutex> lock(mtx);
    itemsDiscovered.insert(itemName);
}

void Diary::registerNPC(const std::string& nameNPC) {
    std::lock_guard<std::mutex> lock(mtx);
    npcsDiscovered.insert(nameNPC);
}

void Diary::registerRace(const std::string& raceName) {
    std::lock_guard<std::mutex> lock(mtx);
    breedsDiscoveries.insert(raceName);
}

void Diary::registerClass(const std::string& className) {
    std::lock_guard<std::mutex> lock(mtx);
    classesDiscoveries.insert(className);
}

void Diary::registerMissionAccept(const std::string& idMission) {
    std::lock_guard<std::mutex> lock(mtx);
    missionsAccepted.insert(idMission);
}

void Diary::registerMissionCompleted(const std::string& idMission) {
    std::lock_guard<std::mutex> lock(mtx);
    missionsCompleted.insert(idMission);
}

bool Diary::itemDiscovered(const std::string& itemName) const {
    std::lock_guard<std::mutex> lock(mtx);
    return itemsDiscovered.count(itemName) > 0;
}

bool Diary::npcDiscovered(const std::string& nameNPC) const {
    std::lock_guard<std::mutex> lock(mtx);
    return npcsDiscovered.count(nameNPC) > 0;
}

bool Diary::raceDiscovery(const std::string& raceName) const {
    std::lock_guard<std::mutex> lock(mtx);
    return breedsDiscoveries.count(raceName) > 0;
}

bool Diary::classDiscovery(const std::string& className) const {
    std::lock_guard<std::mutex> lock(mtx);
    return classesDiscoveries.count(className) > 0;
}

bool Diary::missionAccept(const std::string& idMission) const {
    std::lock_guard<std::mutex> lock(mtx);
    return missionsAccepted.count(idMission) > 0;
}

bool Diary::missionCompleted(const std::string& idMission) const {
    std::lock_guard<std::mutex> lock(mtx);
    return missionsCompleted.count(idMission) > 0;
}

std::vector<std::string> Diary::getItemsDiscovered() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(itemsDiscovered.begin(), itemsDiscovered.end());
}

std::vector<std::string> Diary::getNPCsDiscovered() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(npcsDiscovered.begin(), npcsDiscovered.end());
}

std::vector<std::string> Diary::getBreedsDiscoveries() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(breedsDiscoveries.begin(), breedsDiscoveries.end());
}

std::vector<std::string> Diary::getClassesDiscoveries() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(classesDiscoveries.begin(), classesDiscoveries.end());
}

std::vector<std::string> Diary::getMissionsAccepted() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(missionsAccepted.begin(), missionsAccepted.end());
}

std::vector<std::string> Diary::getMissionsCompleted() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(missionsCompleted.begin(), missionsCompleted.end());
}

void Diary::save(std::ofstream& out) const {
    std::lock_guard<std::mutex> lock(mtx);
    
    auto writeSet = [&](const std::set<std::string>& set) {
        out << set.size() << "\n";
        for (const auto& item : set) out << item << "\n";
    };

    writeSet(itemsDiscovered);
    writeSet(npcsDiscovered);
    writeSet(breedsDiscoveries);
    writeSet(classesDiscoveries);
    writeSet(missionsAccepted);
    writeSet(missionsCompleted);
}

void Diary::load(std::ifstream& in) {
    std::lock_guard<std::mutex> lock(mtx);
    
    itemsDiscovered.clear();
    npcsDiscovered.clear();
    breedsDiscoveries.clear();
    classesDiscoveries.clear();
    missionsAccepted.clear();
    missionsCompleted.clear();

    auto readSet = [&](std::set<std::string>& set) {
        size_t size;
        if (!(in >> size)) return false;
        std::string line; std::getline(in, line);
        for (size_t i = 0; i < size; ++i) {
            std::getline(in, line);
            set.insert(line);
        }
        return true;
    };
    
    readSet(itemsDiscovered);
    readSet(npcsDiscovered);
    readSet(breedsDiscoveries);
    readSet(classesDiscoveries);
    readSet(missionsAccepted);
    readSet(missionsCompleted);
}
