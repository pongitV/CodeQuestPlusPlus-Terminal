#include "UI/Renderers/3D/RaycasterScreens/Diary/RaycasterDiaryScreen.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "Core/Utils/Appearance.h"
#include "UI/Screens/BaseScreen.h"
#include "UI/Screens/Diary/DiaryScreenLayout.h"
#include "UI/Renderers/3D/EngineRaycaster/Raycaster.h"

void ScreenDiaryRaycaster::renderBackground() {
    Raycaster::restoreLastFrame();
}

void ScreenDiaryRaycaster::displayHeader(int startY) {
    int widthConsole = Appearance::getTerminalWidth();
    int soonHeight = ArtsDiary::soonDiary.size();
    int soonY = startY > 0 ? (startY - 1 - soonHeight) : 2;
    if (soonY < 0) soonY = 0;

    int compVisualSoon = 0;
    for (const auto& line : ArtsDiary::soonDiary) {
        int comp = Appearance::getVisualLength(line);
        if (comp > compVisualSoon) compVisualSoon = comp;
    }
    int soonX = (widthConsole - compVisualSoon) / 2;
    if (soonX < 0) soonX = 0;

    std::string colorTitle = Appearance::color(Color::YELLOW);
    for (int i = 0; i < (int)ArtsDiary::soonDiary.size(); ++i) {
        Appearance::moveCursor(soonX, soonY + i);
        const std::string& line = ArtsDiary::soonDiary[i];

        std::string buffer = colorTitle;
        int spaceCount = 0;

        for (size_t j = 0; j < line.length(); ) {
            if (line[j] == ' ') {
                spaceCount++;
                j++;
            } else {
                if (spaceCount > 0) {
                    buffer += "\033[" + std::to_string(spaceCount) + "C";
                    spaceCount = 0;
                }
                unsigned char uc = line[j];
                int charLen = 1;
                if ((uc & 0x80) == 0) charLen = 1;
                else if ((uc & 0xE0) == 0xC0) charLen = 2;
                else if ((uc & 0xF0) == 0xE0) charLen = 3;
                else if ((uc & 0xF8) == 0xF0) charLen = 4;
                buffer += line.substr(j, charLen);
                j += charLen;
            }
        }
        buffer += "\033[0m";
        std::cout << buffer;
    }
    std::cout << std::flush;
}

void ScreenDiaryRaycaster::renderBox(const std::vector<std::string>& lines, const std::string& title, Color colorBox, int minY, int startYOverride) {
    std::vector<std::string> boxEnd = BaseScreen::createBox(lines, title, 0, colorBox, "\033[48;2;25;25;25m");
    int outW = Appearance::getVisualLength(boxEnd[0]);
    int outH = boxEnd.size();

    int startX = (Appearance::getTerminalWidth() - outW) / 2;
    int startY = startYOverride >= 0 ? startYOverride : (Appearance::getTerminalHeight() - outH) / 2;
    if (startX < 0) startX = 0;
    if (startY < minY) startY = minY;
    if (startY < 8) startY = 8;

    std::cout << "\033[?25l";
    for (size_t i = 0; i < boxEnd.size(); ++i) {
        Appearance::moveCursor(startX, startY + i);
        std::cout << boxEnd[i];
    }
    std::cout << std::flush;
}

void ScreenDiaryRaycaster::renderPopupMessage(const std::string& title, const std::vector<std::string>& text) {
    std::vector<std::string> lines;
    for (const auto& t : text) {
        lines.push_back(" " + t + " ");
    }
    lines.push_back("");
    lines.push_back("   " + Appearance::color(Color::GREEN) + "> Voltar" + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m");

    std::vector<std::string> boxEnd = BaseScreen::createBox(lines, title, 0, Color::YELLOW, "\033[48;2;25;25;25m");
    int outW = Appearance::getVisualLength(boxEnd[0]);
    int outH = boxEnd.size();

    int startX = (Appearance::getTerminalWidth() - outW) / 2;
    int heightTerm = Appearance::getTerminalHeight();
    int startY = (heightTerm - outH) / 2;
    if (startX < 0) startX = 0;
    if (startY < 8) startY = 8;
    if (startY + outH > heightTerm) startY = std::max(0, heightTerm - outH);

    for (size_t i = 0; i < boxEnd.size(); ++i) {
        Appearance::moveCursor(startX, startY + i);
        std::cout << boxEnd[i];
    }
    std::cout << std::flush;
}

void ScreenDiaryRaycaster::renderPopupInspectionWithArt(const std::string& title, const std::vector<std::string>& art, const std::vector<std::string>& info, const std::string& subtitle) {
    std::vector<std::string> textPopup;

    if (!subtitle.empty()) {
        textPopup.push_back(Appearance::color(Color::CYAN) + " === " + subtitle + " ===" + Appearance::color(Color::RESET));
        textPopup.push_back("");
    }
    for (const auto& l : info) textPopup.push_back(l);

    int maxArt = std::max(art.size(), textPopup.size());
    int wideArtBase = 0;
    for (const auto& l : art) {
        int w = Appearance::getVisualLength(l);
        if (w > wideArtBase) wideArtBase = w;
    }
    if (wideArtBase > 0) wideArtBase += 4;
    if (art.empty()) wideArtBase = 0;

    std::vector<std::string> linesEnd;
    for (int i = 0; i < maxArt; ++i) {
        std::string rL = (i < (int)art.size()) ? art[i] : "";
        std::string cL = (i < (int)textPopup.size()) ? textPopup[i] : "";
        int lenR = Appearance::getVisualLength(rL);
        if (lenR < wideArtBase) rL += std::string(wideArtBase - lenR, ' ');
        linesEnd.push_back((wideArtBase > 0 ? " " : "") + rL + " " + cL);
    }
    linesEnd.push_back("");
    linesEnd.push_back("   " + Appearance::color(Color::GREEN) + "> Voltar" + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m");

    std::vector<std::string> boxEnd = BaseScreen::createBox(linesEnd, title, 0, Color::YELLOW, "\033[48;2;25;25;25m");
    int outW = Appearance::getVisualLength(boxEnd[0]);
    int outH = boxEnd.size();

    int startX = (Appearance::getTerminalWidth() - outW) / 2;
    int heightTerm = Appearance::getTerminalHeight();
    int startY = (heightTerm - outH) / 2;
    if (startX < 0) startX = 0;
    if (startY < 8) startY = 8;
    if (startY + outH > heightTerm) startY = std::max(0, heightTerm - outH);

    for (size_t i = 0; i < boxEnd.size(); ++i) {
        Appearance::moveCursor(startX, startY + i);
        std::cout << boxEnd[i];
    }
    std::cout << std::flush;
}
