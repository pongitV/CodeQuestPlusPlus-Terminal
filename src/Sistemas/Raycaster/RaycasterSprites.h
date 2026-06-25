#pragma once
#include <vector>
#include <string>
#include <map>

struct SpriteCache {
    int width = 0, height = 0;
    std::vector<std::vector<std::string>> pixels;
};

class RaycasterSprites {
public:
    static SpriteCache parseArte(const std::vector<std::string>& raw);
    static SpriteCache parseSprite(const std::vector<std::string>& raw, int r, int g, int b, bool isMahoraga = false);
    static std::vector<std::string> colorirArte(const std::vector<std::string>& arte, const std::string& corAnsi);
};
