#pragma once

#include "UI/Interfaces/IAttributesUI.h"

class IDEAttributesScreen : public IAttributesUI {
public:
    IDEAttributesScreen() = default;
    ~IDEAttributesScreen() override = default;

    void display(Character* player) override;
    void displayDetailsAttributes(Character* currentPlayer) override;
    void managePlayerCharacterSheet(Character* currentPlayer) override;

    static IDEAttributesScreen& instance() {
        static IDEAttributesScreen s_instance;
        return s_instance;
    }
};
