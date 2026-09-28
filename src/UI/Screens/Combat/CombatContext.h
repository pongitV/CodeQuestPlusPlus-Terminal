#pragma once

#include <vector>
#include <string>
#include "Domain/Characters/Character.h"

struct CombatContext {
    bool isMode3D = false;
    bool isTerminalView = false;
    std::vector<std::string> currentMapMatrix;
    float playerPosX = 0;
    float playerPosY = 0;
    float playerAngle = 0;
    std::string currentMapTitle;

    int currentTurnVisible = 0;
    std::string turnVisibleName;
    int currentActionSelection = 0;
    int currentTargetSelection = -1;
    bool blinkSelection = false;

    Character* enemyDeadWithDrops = nullptr;
    std::vector<std::string> activeDrops;
    std::vector<std::string> currentMenuOptions;
    Character* characterHUD = nullptr;

    // Referencias para compatibilidade retroativa
    float& playerPostX;
    float& playerPostY;
    std::string& titleMapCurrent;
    int& shiftCurrentVisible;
    std::string& nameShiftVisible;
    int& selectionActionCurrent;
    int& selectionTargetCurrent;
    std::vector<std::string>& dropsAssets;
    std::vector<std::string>& optionsMenuCurrent;

    CombatContext() 
        : playerPostX(playerPosX), playerPostY(playerPosY), titleMapCurrent(currentMapTitle),
          shiftCurrentVisible(currentTurnVisible), nameShiftVisible(turnVisibleName),
          selectionActionCurrent(currentActionSelection), selectionTargetCurrent(currentTargetSelection),
          dropsAssets(activeDrops), optionsMenuCurrent(currentMenuOptions) {}

    CombatContext(const CombatContext& o)
        : isMode3D(o.isMode3D), isTerminalView(o.isTerminalView), currentMapMatrix(o.currentMapMatrix),
          playerPosX(o.playerPosX), playerPosY(o.playerPosY), playerAngle(o.playerAngle),
          currentMapTitle(o.currentMapTitle), currentTurnVisible(o.currentTurnVisible),
          turnVisibleName(o.turnVisibleName), currentActionSelection(o.currentActionSelection),
          currentTargetSelection(o.currentTargetSelection), blinkSelection(o.blinkSelection),
          enemyDeadWithDrops(o.enemyDeadWithDrops), activeDrops(o.activeDrops),
          currentMenuOptions(o.currentMenuOptions), characterHUD(o.characterHUD),
          playerPostX(playerPosX), playerPostY(playerPosY), titleMapCurrent(currentMapTitle),
          shiftCurrentVisible(currentTurnVisible), nameShiftVisible(turnVisibleName),
          selectionActionCurrent(currentActionSelection), selectionTargetCurrent(currentTargetSelection),
          dropsAssets(activeDrops), optionsMenuCurrent(currentMenuOptions) {}

    CombatContext& operator=(const CombatContext& o) {
        if (this != &o) {
            isMode3D = o.isMode3D;
            isTerminalView = o.isTerminalView;
            currentMapMatrix = o.currentMapMatrix;
            playerPosX = o.playerPosX;
            playerPosY = o.playerPosY;
            playerAngle = o.playerAngle;
            currentMapTitle = o.currentMapTitle;
            currentTurnVisible = o.currentTurnVisible;
            turnVisibleName = o.turnVisibleName;
            currentActionSelection = o.currentActionSelection;
            currentTargetSelection = o.currentTargetSelection;
            blinkSelection = o.blinkSelection;
            enemyDeadWithDrops = o.enemyDeadWithDrops;
            activeDrops = o.activeDrops;
            currentMenuOptions = o.currentMenuOptions;
            characterHUD = o.characterHUD;
        }
        return *this;
    }

    void configure(bool mode3D, const std::vector<std::string>& matrix, float posX, float posY, float angle, const std::string& title) {
        isMode3D = mode3D;
        currentMapMatrix = matrix;
        playerPosX = posX;
        playerPosY = posY;
        playerAngle = angle;
        currentMapTitle = title;
    }

    void setTurnVisible(int turn, const std::string& name) {
        currentTurnVisible = turn;
        turnVisibleName = name;
    }
    inline void setShiftVisible(int shift, const std::string& name) {
        setTurnVisible(shift, name);
    }
};

// Apelido para compatibilidade retroativa
using ContextCombat = CombatContext;
