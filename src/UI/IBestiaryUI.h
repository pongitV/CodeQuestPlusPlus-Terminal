#pragma once
#include "Domain/Characters/Character.h"
#include <vector>

class IBestiaryUI {
public:
    virtual ~IBestiaryUI() = default;
    virtual void display(const std::vector<Character*>& enemies) = 0;
    virtual void displayDetail(Character* enemy) = 0;
};
