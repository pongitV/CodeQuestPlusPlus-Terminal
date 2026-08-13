#pragma once
#include <string>
#include <vector>

class RaycasterWorldMapScreen {
public:
    static void renderPopup(const std::vector<std::string>& art, const std::vector<std::string>& places, int selection, bool redesignComplete = true);
};
