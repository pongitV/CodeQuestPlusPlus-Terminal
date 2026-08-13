#pragma once

#include <string>
#include <vector>

class Character;

class IDEBestiaryScreen {
public:
    static void displayList(Character* currentPlayer);
    static void displaySheet(Character* currentPlayer, const std::string& nameEnemy, int indexDiscovered, const std::vector<std::string>& discovered);
};
