#include "UI/Renderers/IDE/EngineIDE/IDEAnimator.h"
#include <iostream>
#include <thread>
#include <chrono>

void GOAnimator::effectType(const std::vector<std::string>& lines, int speedMs) {
    for (const auto& line : lines) {
        for (char c : line) {
            std::cout << c << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(speedMs));
        }
        std::cout << "\n";
    }
}

void GOAnimator::scrollExcited(const std::vector<std::string>& content, int linesByFrame, int intervalMs) {
    for (size_t i = 0; i < content.size(); i += linesByFrame) {
        std::cout << "\033[H\033[J";
        for (size_t j = i; j < i + 20 && j < content.size(); ++j) {
            std::cout << content[j] << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
    }
}

void GOAnimator::transitionScreens(const std::function<void()>& renderNewScreen, int durationMs) {
    int steps = 10;
    for (int i = 0; i < steps; ++i) {
        std::cout << "\033[H\033[J";
        renderNewScreen();
        std::this_thread::sleep_for(std::chrono::milliseconds(durationMs / steps));
    }
}

void GOAnimator::blinkCursor(bool active) {
    if (active) {
        std::cout << "\033[?25h";
    } else {
        std::cout << "\033[?25l";
    }
}
