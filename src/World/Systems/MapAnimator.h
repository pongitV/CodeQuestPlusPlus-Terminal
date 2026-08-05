#pragma once
#include <vector>
#include <string>
#include <functional>

#include "Core/Utils/Appearance.h"

class MapAnimator {
public:
    static int animateMapIntroduction(
        const std::string& mapTitle,
        const std::vector<std::string>& mapArt,
        int artWidth,
        const std::vector<std::string>& transitionArt,
        int transitionWidth,
        Color themeColor,
        const std::vector<std::string>& mapMatrix,
        int playerPositionX,
        int playerPositionY,
        const std::function<std::string(char, int, int)>& cellFormatter,
        bool animate,
        bool useBannerAnimation,
        const std::function<void()>& actionAfterFadeInArt
    );

    static void animateFlashbang(int r, int g, int b);
};
