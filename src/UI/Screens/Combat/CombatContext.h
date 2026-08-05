#pragma once

#include <vector>
#include <string>
#include "Domain/Characters/Character.h"

struct ContextCombat {
    bool isMode3D = false;
    bool isTerminalView = false;
    std::vector<std::string> currentMapMatrix;
    float playerPostX = 0;
    float playerPostY = 0;
    float playerAngle = 0;
    std::string titleMapCurrent;

    int shiftCurrentVisible = 0;
    std::string nameShiftVisible;
    int selectionActionCurrent = 0;
    int selectionTargetCurrent = -1;
    bool blinkSelection = false;

    Character* enemyDeadWithDrops = nullptr;
    std::vector<std::string> dropsAssets;
    std::vector<std::string> optionsMenuCurrent;
    Character* characterHUD = nullptr;

    void configure(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title) {
        isMode3D = mode3D;
        currentMapMatrix = matrix;
        playerPostX = postX;
        playerPostY = postY;
        playerAngle = angle;
        titleMapCurrent = title;
    }

    void setShiftVisible(int shift, const std::string& name) {
        shiftCurrentVisible = shift;
        nameShiftVisible = name;
    }
};
