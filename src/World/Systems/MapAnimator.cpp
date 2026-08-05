#include "World/Systems/MapAnimator.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "World/Systems/MapRenderer.h"
#include "World/MapControl.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterWorld.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>

int MapAnimator::animateMapIntroduction(
    const std::string& mapTitle,
    const std::vector<std::string>& mapArt,
    int /*larguraArte*/,
    const std::vector<std::string>& transitionArt,
    int /*larguraTransicao*/,
    Color /*corTema*/,
    const std::vector<std::string>& mapMatrix,
    int playerPositionX,
    int playerPositionY,
    const std::function<std::string(char, int, int)>& cellFormatter,
    bool animate,
    bool useBannerAnimation,
    const std::function<void()>& actionAfterFadeInArt
) {
    if (MapControl::is3DExplorationActive()) {
        RaycasterWorld::updateMapHash(mapMatrix);
        return 0; 
    }

    RaycasterWorld::updateMapHash(mapMatrix);
    Appearance::hideCursor();

    int terminalWidth = Appearance::getTerminalWidth();
    int terminalHeight = Appearance::getTerminalHeight();

    if (!animate) {
        Appearance::displayTextPanel(mapTitle, Color::WHITE);
        int initialMapLine = Appearance::getCursorPostY();
        MapRenderer::renderMap(mapMatrix, playerPositionX, playerPositionY, terminalWidth, terminalHeight, initialMapLine, cellFormatter);
        return initialMapLine;
    }

    Appearance::clearScreen();

    if (actionAfterFadeInArt) {
        actionAfterFadeInArt();
        Appearance::clearScreen();
    }
    
    Appearance::displayTextPanel(mapTitle, Color::WHITE, true);
    int initialMapLine = Appearance::getCursorPostY();

    std::vector<std::string> baseBanner;
    if (useBannerAnimation) {
        if (!mapArt.empty()) {
            baseBanner = mapArt;
        } else if (!transitionArt.empty()) {
            baseBanner = transitionArt;
        }
    }

    if (baseBanner.empty() || !useBannerAnimation) {
        MapRenderer::renderMap(mapMatrix, playerPositionX, playerPositionY, terminalWidth, terminalHeight, initialMapLine, cellFormatter);
        return initialMapLine;
    }

    std::vector<std::string> banner;
    for (const auto& l : baseBanner) {
        banner.push_back("\033[1;37m" + Appearance::removeANSIColors(l) + "\033[0m");
    }
    
    int maxW = 0;
    for (const auto& l : banner) {
        int w = Appearance::getVisualLength(l);
        if (w > maxW) maxW = w;
    }
    
    int bannerHeight = banner.size();
    int startXBox = (terminalWidth - maxW) / 2;
    if (startXBox < 0) startXBox = 0;

    int startX, endX;
    MapRenderer::calculateCameraHorizontal(terminalWidth, playerPositionX, mapMatrix.empty() ? 0 : static_cast<int>(mapMatrix[0].length()), startX, endX);
    std::string mapLeftMargin = MapRenderer::calculateCenteredMargin(terminalWidth, endX - startX);
    
    std::string controlsText = "W,A,S,D: Mover | V: Visao | I: Inventario | C: Ficha | B: Diario | M: Mapa";
    std::string controlsLeftMargin = MapRenderer::calculateCenteredMargin(terminalWidth, controlsText.length());
    
    int realMapOffset = 2;
    
    int startY, endY;
    MapRenderer::calculateCameraVertical(terminalHeight, initialMapLine, playerPositionY, static_cast<int>(mapMatrix.size()), startY, endY);
    
    std::vector<std::string> mapCacheLines;
    for (int y = startY; y < endY; y++) {
        std::string lineStr = mapLeftMargin;
        lineStr.reserve(mapLeftMargin.size() + (endX - startX) * 10);
        for (int x = startX; x < endX; x++) {
            char c = (x < static_cast<int>(mapMatrix[y].length())) ? mapMatrix[y][x] : ' ';
            lineStr += cellFormatter(c, x, y);
        }
        mapCacheLines.push_back(lineStr);
    }

    std::ostringstream initialMap;
    initialMap << "\033[" << (initialMapLine + 1) << ";1H\033[K" << controlsLeftMargin << Appearance::color(Color::GRAY) << controlsText << Appearance::color(Color::RESET) << "\n\033[K\n";
    for (int i = 0; i < (int)mapCacheLines.size(); i++) {
        initialMap << "\033[" << (initialMapLine + 1 + realMapOffset + i) << ";1H" << mapCacheLines[i] << "\033[K";
    }
    std::cout << initialMap.str() << std::flush;

    int destinationY = 2;
    
    Appearance::animateFadeIn(15, 40, [&](int frame, int /*intensidade*/) {
        float opacity = frame / 15.0f;
        int c = (int)(255 * opacity);
        std::string fadedBannerColor = "\033[38;2;" + std::to_string(c) + ";" + std::to_string(c) + ";" + std::to_string(c) + "m";
        
        std::ostringstream frameScreen;
        
        for (int i = 0; i < bannerHeight; i++) {
            int drawY = initialMapLine + destinationY + i;
            if (drawY >= initialMapLine && drawY < terminalHeight) {
                std::string cleanLine = Appearance::removeANSIColors(baseBanner[i]);
                frameScreen << "\033[" << (drawY + 1) << ";" << (startXBox + 1) << "H" << fadedBannerColor << cleanLine << "\033[0m";
            }
        }
        std::cout << frameScreen.str() << std::flush;
    });

    if (!InputControl::pressedKey()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    InputControl::clearBuffer();
    
    MapRenderer::renderMap(mapMatrix, playerPositionX, playerPositionY, terminalWidth, terminalHeight, initialMapLine, cellFormatter);
    
    return initialMapLine;
}

void MapAnimator::animateFlashbang(int r, int g, int b) {
    int SCREEN_WIDTH = Appearance::getTerminalWidth();
    int SCREEN_HEIGHT = Appearance::getTerminalHeight();
    if (SCREEN_WIDTH <= 0) SCREEN_WIDTH = 120;
    if (SCREEN_HEIGHT <= 0) SCREEN_HEIGHT = 30;

    std::cout << "\033[?25l"; // Hide cursor
    std::string colorPrefix = "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
    
    std::vector<std::string> fadeChars = {"█", "▓", "▒", "░", " "};
    
    for (int step = 0; step < (int)fadeChars.size(); step++) {
        std::string buffer = "\033[H";
        buffer.reserve(SCREEN_WIDTH * SCREEN_HEIGHT * 20);
        
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                if (y == SCREEN_HEIGHT - 1 && x == SCREEN_WIDTH - 1) break; // prevent scroll
                if (fadeChars[step] == " ") {
                    buffer += "\033[40m \033[0m"; // Preto final
                } else {
                    buffer += colorPrefix + "\033[38;2;255;255;255m" + fadeChars[step] + "\033[0m";
                }
            }
            if (y < SCREEN_HEIGHT - 1) buffer += "\n";
        }
        std::cout << buffer << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
    Appearance::clearScreen();
}
