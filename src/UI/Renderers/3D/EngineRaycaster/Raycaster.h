#pragma once

#include <vector>
#include <string>
#include "UI/Renderers/3D/EngineRaycaster/RaycasterFrame.h"
#include "Domain/Characters/Character.h"
#include "Core/Utils/Appearance.h"

class Raycaster : public RaycasterFrame {
public:
    static float sensitivityX;
    static float sensitivityY;

    static char start3DExploration(const std::vector<std::string>& mapMatrix, float& playerX, float& playerY, float& angleVisa, const std::string& titleMap, Character* player, int& outHitX, int& outHitY, int typeAnimationEntry = 0);
    static void blinkScreenColor(Color color, int durationMs);
    static std::vector<std::string> drawFrameStatic3D(const std::vector<std::string>& mapMatrix, float playerX, float playerY, float angleVisa, const std::string& titleMap, Character* player, int heightOverride = -1);
};
