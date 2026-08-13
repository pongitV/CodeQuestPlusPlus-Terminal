#pragma once

#include <vector>
#include <string>
#include "Domain/Characters/Character.h"

class RaycasterHUD {
public:
    static void draw(std::vector<std::string>& screen, int widthScreen, int heightScreen, float playerX, float playerY, float angleVisa, const std::vector<std::string>& mapMatrix, const std::string& titleMap, bool themeForest, Character* player);
    static void drawBarStatus(std::vector<std::string>& screen, int SCREEN_WIDTH, int SCREEN_HEIGHT, Character* player, float angleVisa, const std::string& titleEdge = "", int framesDamagePlayer = 0, int damageAmount = -1, bool isHealing = false);

private:
    static void drawMinimap(std::vector<std::string>& screen, int SCREEN_WIDTH, int SCREEN_HEIGHT, float playerX, float playerY, float angleVisa, const std::vector<std::string>& mapMatrix, const std::string& titleMap, bool themeForest, char playerIcon, const std::string& colorPlayerAnsi);
    static void drawControls(std::vector<std::string>& screen, int SCREEN_WIDTH, int SCREEN_HEIGHT);
};