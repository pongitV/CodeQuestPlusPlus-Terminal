#pragma once

#include <string>
#include <vector>

#include "Domain/Characters/Character.h"

class ScreenAttributesGO {
public:
    static void display(Character* currentPlayer);
    static void displayDetailsAttributes(Character* currentPlayer);
};
