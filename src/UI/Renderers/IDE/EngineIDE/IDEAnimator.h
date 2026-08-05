#pragma once

#include <string>
#include <vector>
#include <functional>

class GOAnimator {
public:
    static void effectType(const std::vector<std::string>& lines, int speedMs = 30);
    static void scrollExcited(const std::vector<std::string>& content, int linesByFrame = 1, int intervalMs = 50);
    static void transitionScreens(const std::function<void()>& renderNewScreen, int durationMs = 300);
    static void blinkCursor(bool active);
};
