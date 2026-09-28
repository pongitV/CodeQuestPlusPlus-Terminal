#pragma once
#include <vector>
#include <string>
#include <map>
#include <cstdint>

struct Pixel3D {
    uint8_t r = 0, g = 0, b = 0;
    uint8_t fgR = 0, fgG = 0, fgB = 0;
    char ch = ' ';
    bool hasFg = false;
    bool isBackground = false;
};

struct SpritePixel {
    uint8_t r = 0, g = 0, b = 0;
    uint8_t fgR = 0, fgG = 0, fgB = 0;
    char ch = ' ';
    bool hasBg = false;
    bool hasFg = false;
    bool isTransparent = true;
};

struct SpriteCache {
    int width = 0, height = 0;
    std::vector<SpritePixel> pixels;
};

class RaycasterSprites {
public:
    static SpriteCache parseArt(const std::vector<std::string>& raw);
    static SpriteCache parseSprite(const std::vector<std::string>& raw, int r, int g, int b, bool isMahoraga = false);
    static inline SpriteCache parsiArt(const std::vector<std::string>& raw) { return parseArt(raw); }
    static inline SpriteCache parsiSprite(const std::vector<std::string>& raw, int r, int g, int b, bool isMahoraga = false) { return parseSprite(raw, r, g, b, isMahoraga); }
    static std::vector<std::string> colorArt(const std::vector<std::string>& art, const std::string& colorAnsi);
};
