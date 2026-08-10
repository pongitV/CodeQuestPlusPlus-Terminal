#pragma once

class LevelSystem {
private:
    int level;
    int xpCurrent;
    int xpForRise;

public:
    LevelSystem(int initialLevel = 1, int initialXp = 0, int initialXpForRise = 100) 
        : level(initialLevel), xpCurrent(initialXp), xpForRise(initialXpForRise) {}

    int getLevel() const { return level; }
    int getCurrentXp() const { return xpCurrent; }
    int getXpForRise() const { return xpForRise; }

    void setLevel(int newLevel) { level = newLevel; }
    void setCurrentXp(int newXp) { xpCurrent = newXp; }
    void setXpForRise(int newXpForRise) { xpForRise = newXpForRise; }

    void gainXp(int value) { xpCurrent += value; }
    bool canLevelUp() const { return xpCurrent >= xpForRise; }
};
