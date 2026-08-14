#include "UI/Renderers/3D/RaycasterScreens/Diary/RaycasterDiaryScreen.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "Core/Utils/Appearance.h"
#include "UI/Screens/BaseScreen.h"
#include "UI/Screens/Diary/DiaryScreenLayout.h"
#include "UI/Renderers/3D/EngineRaycaster/Raycaster.h"

void RaycasterDiaryScreen::renderBackground() {
    Raycaster::restoreLastFrame();
}

void RaycasterDiaryScreen::displayHeader(int startY) {
    int widthConsole = Appearance::getTerminalWidth();
    int soonHeight = (int)ArtsDiary::diaryLogo.size();

    int compVisualSoon = 0;
    for (const auto& line : ArtsDiary::diaryLogo) {
        int comp = Appearance::getVisualLength(line);
        if (comp > compVisualSoon) compVisualSoon = comp;
    }

    int soonY = startY > 0 ? (startY - 1 - soonHeight) : 1;
    if (soonY < 0) soonY = 0;
    int soonX = (widthConsole - compVisualSoon) / 2;
    if (soonX < 0) soonX = 0;

    if (widthConsole >= compVisualSoon && (startY >= soonHeight + 1 || soonY == 0)) {
        std::string bgDark = "\033[48;2;20;20;20m";
        std::string colorTitle = Appearance::color(Color::YELLOW);
        std::string reset = "\033[0m";
        
        for (int i = 0; i < soonHeight; ++i) {
            Appearance::moveCursor(soonX, soonY + i);
            const std::string& line = ArtsDiary::diaryLogo[i];

            std::string buffer = bgDark + colorTitle;
            for (size_t j = 0; j < line.length(); ) {
                unsigned char uc = line[j];
                int charLen = 1;
                if ((uc & 0x80) == 0) charLen = 1;
                else if ((uc & 0xE0) == 0xC0) charLen = 2;
                else if ((uc & 0xF0) == 0xE0) charLen = 3;
                else if ((uc & 0xF8) == 0xF0) charLen = 4;
                buffer += line.substr(j, charLen);
                j += charLen;
            }
            buffer += reset;
            std::cout << buffer;
        }
        std::cout << std::flush;
    } else if (startY >= 2) {
        std::string titleCompact = "[ === DIARIO DE AVENTURA === ]";
        int compCompact = Appearance::getVisualLength(titleCompact);
        int cx = std::max(0, (widthConsole - compCompact) / 2);
        int cy = std::max(0, startY - 1);
        Appearance::moveCursor(cx, cy);
        std::cout << Appearance::color(Color::YELLOW) << "\033[48;2;25;25;25m" << titleCompact << "\033[0m" << std::flush;
    }
}

void RaycasterDiaryScreen::renderBox(const std::vector<std::string>& lines, const std::string& title, Color colorBox, int minY, int startYOverride) {
    std::vector<std::string> boxEnd = BaseScreen::createBox(lines, title, 0, colorBox, "\033[48;2;25;25;25m");
    int outW = Appearance::getVisualLength(boxEnd[0]);
    int outH = (int)boxEnd.size();

    int termW = Appearance::getTerminalWidth();
    int termH = Appearance::getTerminalHeight();
    int startX = std::max(0, (termW - outW) / 2);
    int startY = startYOverride >= 0 ? startYOverride : std::max(0, (termH - outH) / 2);
    if (startY < minY) startY = minY;
    if (startY + outH > termH) startY = std::max(0, termH - outH);
    if (startX + outW > termW) startX = std::max(0, termW - outW);

    std::cout << "\033[?25l";
    for (size_t i = 0; i < boxEnd.size(); ++i) {
        if (startY + (int)i < termH) {
            Appearance::moveCursor(startX, startY + i);
            std::cout << boxEnd[i];
        }
    }
    std::cout << std::flush;
}

void RaycasterDiaryScreen::renderPopupMessage(const std::string& title, const std::vector<std::string>& text) {
    std::vector<std::string> lines;
    for (const auto& t : text) {
        lines.push_back(" " + t + " ");
    }
    lines.push_back("");
    lines.push_back("   " + Appearance::color(Color::GREEN) + "> Voltar" + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m");

    std::vector<std::string> boxEnd = BaseScreen::createBox(lines, title, 0, Color::YELLOW, "\033[48;2;25;25;25m");
    int outW = Appearance::getVisualLength(boxEnd[0]);
    int outH = (int)boxEnd.size();

    int termW = Appearance::getTerminalWidth();
    int heightTerm = Appearance::getTerminalHeight();
    int startX = std::max(0, (termW - outW) / 2);
    int startY = std::max(0, (heightTerm - outH) / 2);
    if (startY + outH > heightTerm) startY = std::max(0, heightTerm - outH);
    if (startX + outW > termW) startX = std::max(0, termW - outW);

    for (size_t i = 0; i < boxEnd.size(); ++i) {
        if (startY + (int)i < heightTerm) {
            Appearance::moveCursor(startX, startY + i);
            std::cout << boxEnd[i];
        }
    }
    std::cout << std::flush;
}

void RaycasterDiaryScreen::renderPopupInspectionWithArt(const std::string& title, const std::vector<std::string>& art, const std::vector<std::string>& info, const std::string& subtitle) {
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
    int outH = (int)boxEnd.size();

    int termW = Appearance::getTerminalWidth();
    int heightTerm = Appearance::getTerminalHeight();
    int startX = std::max(0, (termW - outW) / 2);
    int startY = std::max(0, (heightTerm - outH) / 2);
    if (startY + outH > heightTerm) startY = std::max(0, heightTerm - outH);
    if (startX + outW > termW) startX = std::max(0, termW - outW);

    for (size_t i = 0; i < boxEnd.size(); ++i) {
        if (startY + (int)i < heightTerm) {
            Appearance::moveCursor(startX, startY + i);
            std::cout << boxEnd[i];
        }
    }
    std::cout << std::flush;
}
