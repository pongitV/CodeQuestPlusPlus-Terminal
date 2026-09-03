#pragma once

#include "UI/Interfaces/IWorldMapUI.h"

class IDEMapScreen : public IWorldMapUI {
public:
    IDEMapScreen() = default;
    ~IDEMapScreen() override = default;

    void renderPopup(const std::vector<std::string>& art, const std::vector<std::string>& places, int selection, bool redesignComplete = true) override;

    static IDEMapScreen& instance() {
        static IDEMapScreen s_instance;
        return s_instance;
    }
};
