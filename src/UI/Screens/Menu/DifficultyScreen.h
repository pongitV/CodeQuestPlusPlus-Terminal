#pragma once
#include <string>

class DifficultyScreen {
public:
    struct Result {
        int index = 0;
        bool returned = false;
    };
    static Result display(const std::string& namePlayer, const std::string& raceName, const std::string& className);
};
