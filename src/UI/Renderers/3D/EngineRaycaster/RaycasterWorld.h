#pragma once
#include <string>
#include <vector>
#include <tuple>
#include "UI/Renderers/3D/EngineRaycaster/RaycasterSprites.h"
#include "UI/Renderers/3D/EngineRaycaster/Illuminator.h"

class RaycasterWorld {
public:
    static bool isThemeForest(const std::string& titleMap);
    static bool isEntity(char c);
    static bool isTeleport(char c);
    static bool isWalkable(int mapX, int mapY, const std::vector<std::string>& mapMatrix);
    static bool isMapLabel(int mapX, int mapY, const std::vector<std::string>& mapMatrix);
    static void updateMapHash(const std::vector<std::string>& mapMatrix);
    static size_t getMapHash();
    
    static char getNPCNext(const std::string& titleMap, int mapX, int mapY, const std::vector<std::string>* mapMatrix = nullptr);
    
    static Pixel3D getInternalWallPixel(const std::string& titleMap, bool themeForest, float distanceUntilWall, float depthMaximum, char charWall, int y, int ceiling, int floor, float texX, float timeAnimation, const std::vector<std::tuple<int, int, int>>& lights, float hitX, float hitY, bool isSideWall = false, char npcFound = ' ', float nx = 0.0f, float ny = 0.0f);
    static Pixel3D getPixelWall(const std::string& titleMap, bool themeForest, float distanceUntilWall, float depthMaximum, char charWall, int y, int ceiling, int floor, float texX, float timeAnimation = 0.0f, bool isSideWall = false, const std::vector<std::tuple<int, int, int>>& lights = {}, float hitX = 0.0f, float hitY = 0.0f, char npcFound = ' ', float nx = 0.0f, float ny = 0.0f);

    static Pixel3D getInternalWallPixel(const std::string& titleMap, bool themeForest, float distanceUntilWall, float depthMaximum, char charWall, int y, int ceiling, int floor, float texX, float timeAnimation, const Highlighter::InfoLight& infoLight, float hitX, float hitY, bool isSideWall = false, char npcFound = ' ', float nx = 0.0f, float ny = 0.0f);
    static Pixel3D getPixelWall(const std::string& titleMap, bool themeForest, float distanceUntilWall, float depthMaximum, char charWall, int y, int ceiling, int floor, float texX, float timeAnimation, bool isSideWall, const Highlighter::InfoLight& infoLight, float hitX, float hitY, char npcFound = ' ', float nx = 0.0f, float ny = 0.0f);
    static Pixel3D getFloorPixel(const std::string& titleMap, float currentX, float currentY, float currentDist, float depthMaximum, const std::vector<std::tuple<int, int, int>>& lights = {}, const std::vector<std::string>* mapMatrix = nullptr, float timeAnimation = 0.0f);
    static Pixel3D getFloorPixel(const std::string& titleMap, float currentX, float currentY, float currentDist, float depthMaximum, const Highlighter::InfoLight& infoLight);
    static Pixel3D getPixelWater(float currentX, float currentY, float currentDist, float depthMaximum, float radiusAngle = 0.0f, float timeAnimation = 0.0f, int themeSky = 0);
    static int getThemeSky(const std::string& titleMap);
    static Pixel3D getPixelCeiling(int themeSky, float radiusAngle, float angleSunMoon, int y, int heightScreen, float timeAnimation = 0.0f, bool isMenu = false);

    static char getSpriteChar(int mapX, int mapY, char c, const std::string& titleMap);
    static std::string getColorMinimapEntity(char c, const std::string& titleMap);
};
