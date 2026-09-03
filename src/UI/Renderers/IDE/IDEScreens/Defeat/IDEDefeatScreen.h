#pragma once

#include "UI/Interfaces/IDefeatUI.h"

class IDEDefeatScreen : public IDefeatUI {
public:
    IDEDefeatScreen() = default;
    ~IDEDefeatScreen() override = default;

    void display(
        Character* currentPlayer,
        int obtainedGoldQuantity,
        int obtainedXpQuantity,
        int totalDamageCaused,
        int totalDamageReceived,
        int totalHealingReceived,
        int combatTurns
    ) override;

    static IDEDefeatScreen& instance() {
        static IDEDefeatScreen s_instance;
        return s_instance;
    }
};
