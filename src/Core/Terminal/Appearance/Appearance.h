/*
 * Arquivo: Appearance.h
 * Propósito: Utilitários de estética do terminal (formatadores ANSI, movimentação de cursor e suporte a cores).
 */

#pragma once

#include "Core/Terminal/Appearance/Color.h"
#include "Core/Utils/AnsiColor.h"
#include "Core/Utils/TerminalUtils.h"
#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include <algorithm>

/*
 * Fornece utilitários de formação, cores e posicionamento para o console/terminal.
 */
class Appearance {
public:
    static Color colorBackgroundActive;
    static char customPlayerIcon;
    static Color customPlayerColor;

    static void bootConsole();
    static std::string convertVectorForStringGross(const std::vector<std::string>& vector);
    static std::string superimposePanelNaLineAnsi(const std::string& backgroundLine, const std::string& panelLine, int startX);
    static std::string color(Color code);
    static std::vector<std::string> convertRawStringToVector(const std::string& textGross);
    static void standardizeVectorSize(std::vector<std::string>& lines);
    static std::vector<std::string> loadArt(const std::string& pathFile);
    static std::string color(Color style, Color code);
    static std::string colorRGB(uint8_t r, uint8_t g, uint8_t b, bool bold = false);
    static std::string bgRGB(uint8_t r, uint8_t g, uint8_t b);

    static void maximizeWindowTerminal();
    static void adjustSourceForResolution();
    static void hideCursor();
    static void concertCursor();
    static void setColorBackgroundTerminal(const std::string& hexColor);
    static void clearScreen();
    static int getTerminalWidth();
    static int getTerminalHeight();
    static void moveCursor(int x, int y);
    static int getCursorPostY();
    
    static std::string removeANSIColors(const std::string& text);
    static int getVisualLength(const std::string& text);
    static std::string spacesToCenter(int textLength);
    static std::string centerText(const std::string& text);
    
    static void printCentralized(const std::string& text, const std::string& colorAnsi = "");
    static void printCentralizedMultiline(const std::vector<std::string>& lines, int widthVisual = 0, const std::string& colorAnsi = "", int delayLineMs = 0);
    static void printBlockCentralized(const std::vector<std::string>& lines, const std::string& colorAnsi = "", int delayLineMs = 0);
    static int printSideASide(const std::vector<std::string>& columnLeft, const std::vector<std::string>& columnRight, int minWidthLeft = 0, int spacing = 0, Color colorLeft = Color::RESET, Color colorRight = Color::RESET, int delayLineMs = 0);
    
    static std::string getColorRGBFade(Color themeColor, int intensity);
    static void animateFadeIn(int framesTotals, int timeByFrameMs, const std::function<void(int frame, int intensity)>& renderFrame);
    static void displayScreenIntro(const std::vector<std::string>& artSoon, const std::vector<std::string>& textNarration, Color themeColor);

    static int typingDelayMS;
    static void printBlockCentralizedTyping(const std::vector<std::string>& lines, int delayMs = typingDelayMS);
    static void printCentralizedTyping(const std::string& text, int delayMs = typingDelayMS);
    static void printTyping(const std::string& text, int delayMs = typingDelayMS, bool addNewline = true);
    static void printVectorExcited(const std::vector<std::string>& lines, int delayMs = 30);
    
    static void displayPanel(const std::string& title, Color colorMain = Color::WHITE, const std::vector<std::string>& artMain = {}, int artWidth = 0, const std::vector<std::string>& artSecondary = {}, Color colorSecondary = Color::RESET, bool animateFadeIn = false);
    static void displayTextPanel(const std::string& title, Color colorDoHeader = Color::WHITE, bool animateFadeIn = false);
    static void displayArtPanel(const std::vector<std::string>& asciiArt, int widthVisual, Color colorDaArt, const std::string& titleSecondary = "", bool animateFadeIn = false);
    static void displayTitlePattern(const std::string& title, Color themeColor = Color::WHITE);
    static void displayPrompt(const std::string& message);
    static void displayPopup(const std::string& title, const std::vector<std::string>& text, Color themeColor = Color::WHITE, const std::vector<std::string>& asciiArt = {});
    static void renderBoxPopupLively(const std::vector<std::string>& box, int startX, int startY, bool animate);
    static void removeBoxPopupLively(int endBoxWidth, int endBoxHeight, int startX, int startY, bool animate = true);
    static void startPopupInteraction();
    static void updateMinPopupSize(int width, int height);
    static int getMinWidthPopup();
    static int getMinHeightPopup();
    static int readIntegerInFloatingPopup(const std::string& message, int limitMin, int limitMax, Color themeColor = Color::WHITE);
    static void cleanPopupPrevious();
    static void setLastPopup(int x, int y, int w, int h, const std::string& bgAnsi = "\033[48;2;25;25;25m");
    static std::vector<std::string> reduceScaleAscii(const std::vector<std::string>& artOriginal, float factorX = 2.0f, float factorY = 2.0f);
    static std::string superimposeSoonAnsi(const std::string& backgroundLine, const std::vector<std::string>& soonChars, int startX, const std::string& fgColor, int terminalWidth);
    static std::string fatarLineAnsi(const std::string& line, float ratio);
    
    static void cheerTransitionScene3D(
        const std::vector<std::string>& soonBase,
        const std::string& colorEndSoon,
        const std::vector<std::string>& backgroundFree,
        std::function<std::vector<std::string>(float)> getBackgroundEnd,
        int targetY = 2
    );

    static int FACTOR_COMPRESSION_GLOBAL;

    static void registerBattleLog(const std::string& text);
    static void cleanLogBattle();
    static void displayLatestLogs(int quantity);
    static void displayHistoryComplete();

    static void sortAlphabetically(std::vector<std::string>& list);

    template <typename T, typename Extractor>
    static void sortAlphabetically(std::vector<T>& list, Extractor extractorDeText) {
        std::sort(list.begin(), list.end(), [&extractorDeText](const T& a, const T& b) {
            return extractorDeText(a) < extractorDeText(b);
        });
    }
};
