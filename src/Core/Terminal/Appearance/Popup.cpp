#include "Core/Terminal/Appearance/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "UI/Screens/BaseScreen.h"
#include <iostream>
#include <algorithm>
#include <fstream>

namespace {
    int popupMinWidthCurrent = 0;
    int popupMinHeightCurrent = 0;
    int lastPopupX = -1;
    int lastPopupY = -1;
    int lastPopupW = -1;
    int lastPopupH = -1;
    std::string lastPopupBg = "\033[48;2;25;25;25m";
}

void Appearance::startPopupInteraction() {
    popupMinWidthCurrent = 0;
    popupMinHeightCurrent = 0;
}
void Appearance::updateMinPopupSize(int width, int height) {
    if (width > popupMinWidthCurrent) popupMinWidthCurrent = width;
    if (height > popupMinHeightCurrent) popupMinHeightCurrent = height;
}
int Appearance::getMinWidthPopup() { return popupMinWidthCurrent; }
int Appearance::getMinHeightPopup() { return popupMinHeightCurrent; }

void Appearance::cleanPopupPrevious() {
    if (lastPopupH <= 0 || lastPopupW <= 0) return;
    std::string line = lastPopupBg + std::string(lastPopupW, ' ') + "\033[0m";
    for (int i = 0; i < lastPopupH; ++i) {
        moveCursor(lastPopupX, lastPopupY + i);
        std::cout << line;
    }
    std::cout << std::flush;
    lastPopupX = -1; lastPopupY = -1; lastPopupW = -1; lastPopupH = -1;
}

void Appearance::setLastPopup(int x, int y, int w, int h, const std::string& bgAnsi) {
    lastPopupX = x; lastPopupY = y; lastPopupW = w; lastPopupH = h; lastPopupBg = bgAnsi;
}

void Appearance::displayPopup(const std::string& title, const std::vector<std::string>& text, Color themeColor, const std::vector<std::string>& artOriginal) {
    cleanPopupPrevious();

    std::vector<std::string> art = artOriginal;
    if (static_cast<int>(art.size()) > 10) {
        art = reduceScaleAscii(artOriginal, FACTOR_COMPRESSION_GLOBAL, FACTOR_COMPRESSION_GLOBAL);
    }

    std::vector<std::string> linesText = text;

    std::string bgPopup = "\033[48;2;25;25;25m";
    std::vector<std::string> box = BaseScreen::createBoxWithArt(art, linesText, title, 0, themeColor, bgPopup);

    int endBoxW = getVisualLength(box[0]);
    int endBoxH = box.size();

    int widthTerm = getTerminalWidth();
    int heightTerm = getTerminalHeight();
    int startX = (widthTerm - endBoxW) / 2;
    int startY = (heightTerm - endBoxH) / 2;
    if (startY < 0) startY = 0;

    for (int i = 0; i < endBoxH; ++i) {
        moveCursor(startX, startY + i);
        std::cout << box[i];
    }
    std::cout << std::flush;

    setLastPopup(startX, startY, endBoxW, endBoxH, bgPopup);

    InputControl::clearBuffer();
    while (true) {
        char c = InputControl::readKey();
        if (c == '\r' || c == '\n') break;
    }
}

void Appearance::renderBoxPopupLively(const std::vector<std::string>& box, int startX, int startY, bool /*animar*/) {
    if (box.empty()) return;
    for (int i = 0; i < static_cast<int>(box.size()); ++i) {
        moveCursor(startX, startY + i);
        std::cout << box[i];
    }
    std::cout << std::flush;
}

int Appearance::readIntegerInFloatingPopup(const std::string& message, int limitMin, int limitMax, Color themeColor) {
    int termW = getTerminalWidth();
    int termH = getTerminalHeight();
    int msgLen = getVisualLength(message);
    int boxW = msgLen + 8;
    
    int startX = std::max(0, (termW - boxW) / 2);
    int startY = std::max(0, termH / 2);
    
    std::string colorBox = color(themeColor);
    std::string reset = color(Color::RESET);
    std::string bg = "\033[48;2;15;15;15m";

    moveCursor(startX, startY - 1);
    std::string top = "╔"; for(int i = 0; i < boxW - 2; ++i) top += "═"; top += "╗";
    std::cout << bg << colorBox << top << reset;
    
    moveCursor(startX, startY);
    std::cout << bg << colorBox << "║ " << reset << bg << message << "    " << colorBox << "║" << reset;
    
    moveCursor(startX, startY + 1);
    std::string bottom = "╚"; for(int i = 0; i < boxW - 2; ++i) bottom += "═"; bottom += "╝";
    std::cout << bg << colorBox << bottom << reset;

    moveCursor(startX + 2 + msgLen, startY);
    std::cout << bg << reset;
    int result = InputControl::readIntegerWithLimits("", limitMin, limitMax, false);
    std::cout << "\033[0m";
    return result;
}

void Appearance::removeBoxPopupLively(int endBoxWidth, int endBoxHeight, int startX, int startY, bool /*animar*/) {
    if (endBoxWidth <= 0 || endBoxHeight <= 0) return;
    for (int i = 0; i < endBoxHeight; ++i) {
        moveCursor(startX, startY + i);
        std::cout << "\033[0m" << std::string(endBoxWidth, ' ') << "\033[0m";
    }
    std::cout << std::flush;
}
