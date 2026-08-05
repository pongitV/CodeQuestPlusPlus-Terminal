#pragma once
#include <string>

class ParryScreen {
public:
    struct Result {
        enum class Mode { Movement, Typing, Off };
        Mode mode = Mode::Off;
        bool returned = false;
    };
    static Result display(const std::string& namePlayer, const std::string& raceName, const std::string& className);
};
