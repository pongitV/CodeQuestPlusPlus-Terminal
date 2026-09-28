#pragma once

class LevelSystem {
private:
    int level;
    int currentXp;
    int xpToNextLevel;

public:
    LevelSystem(int initialLevel = 1, int initialXp = 0, int initialXpToNextLevel = 100) 
        : level(initialLevel), currentXp(initialXp), xpToNextLevel(initialXpToNextLevel) {}

    int getLevel() const { return level; }
    int getCurrentXp() const { return currentXp; }
    int getXpToNextLevel() const { return xpToNextLevel; }
    inline int getXpForRise() const { return getXpToNextLevel(); }

    void setLevel(int newLevel) { level = newLevel; }
    void setCurrentXp(int newXp) { currentXp = newXp; }
    void setXpToNextLevel(int newXpToNextLevel) { xpToNextLevel = newXpToNextLevel; }
    inline void setXpForRise(int newXpForRise) { setXpToNextLevel(newXpForRise); }

    void gainXp(int value) { currentXp += value; }
    bool canLevelUp() const { return currentXp >= xpToNextLevel; }
};
