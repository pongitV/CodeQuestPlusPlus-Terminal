#pragma once
#include <functional>

class Character;

class MapInputController {
public:
    static bool processInputAndCommands(char key, Character* player, int& nextPositionX, int& nextPositionY, const std::function<void()>& restoreScreen);
};
