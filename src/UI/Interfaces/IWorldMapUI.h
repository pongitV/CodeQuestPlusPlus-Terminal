#pragma once
#include <string>
#include <vector>

class IWorldMapUI {
public:
    virtual ~IWorldMapUI() = default;
    virtual void renderPopup(const std::vector<std::string>& art, const std::vector<std::string>& places, int selection, bool redesignComplete = true) = 0;
};
