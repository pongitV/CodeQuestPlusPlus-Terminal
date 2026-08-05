#pragma once
#include <string>
#include <vector>

class IMapWorldUI {
public:
    virtual ~IMapWorldUI() = default;
    virtual void renderPopup(const std::vector<std::string>& art, const std::vector<std::string>& places, int selection, bool redesignComplete = true) = 0;
};
