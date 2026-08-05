#pragma once

#include <string>

class Character;

class ScreenAttributesRaycaster {
public:
    static void display(Character* player);
    static void displayDetailsAttributes(Character* currentPlayer);
    static void managePlayerCharacterSheet(Character* currentPlayer);
};
