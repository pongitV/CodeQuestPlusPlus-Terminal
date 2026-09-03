#pragma once

#include "UI/Interfaces/IBestiaryUI.h"

class IDEBestiaryScreen : public IBestiaryUI {
public:
    IDEBestiaryScreen() = default;
    ~IDEBestiaryScreen() override = default;

    void display(const std::vector<Character*>& enemies) override;
    void displayDetail(Character* enemy) override;

    static IDEBestiaryScreen& instance() {
        static IDEBestiaryScreen s_instance;
        return s_instance;
    }
};
