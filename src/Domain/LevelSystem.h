#pragma once

class SystemDeLevel {
private:
    int level;
    int xpCurrent;
    int xpForRise;

public:
    SystemDeLevel(int levelInitial = 1, int xpInitial = 0, int xpRiseInitial = 100) 
        : level(levelInitial), xpCurrent(xpInitial), xpForRise(xpRiseInitial) {}

    int getLevel() const { return level; }
    int getCurrentXp() const { return xpCurrent; }
    int getXpForRise() const { return xpForRise; }

    void setLevel(int newLevel) { level = newLevel; }
    void setCurrentXp(int newXp) { xpCurrent = newXp; }
    void setXpForRise(int newXpForRise) { xpForRise = newXpForRise; }

    void gainXp(int value) { xpCurrent += value; }
    bool canRiseDeLevel() const { return xpCurrent >= xpForRise; }
};
