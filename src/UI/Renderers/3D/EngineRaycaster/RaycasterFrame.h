#pragma once
#include <string>

class RaycasterFrame {
public:
    static std::string s_lastFrameRendered;
    static void restoreLastFrame();
};
