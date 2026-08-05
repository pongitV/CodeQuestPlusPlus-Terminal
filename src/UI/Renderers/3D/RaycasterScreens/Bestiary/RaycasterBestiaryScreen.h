#pragma once

#include <string>
#include <vector>

class Character;

class ScreenBestiaryRaycaster {
public:
    static void display(const std::vector<Character*>& enemies);
    static void displayDetail(Character* enemy);
};
