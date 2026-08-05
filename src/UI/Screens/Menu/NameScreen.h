#pragma once
#include <string>

class NameScreen {
public:
    struct Result {
        std::string name;
        bool returned = false;
    };
    static Result display();
};
