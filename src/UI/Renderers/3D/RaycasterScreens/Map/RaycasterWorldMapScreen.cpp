#include "UI/Renderers/3D/RaycasterScreens/Map/RaycasterWorldMapScreen.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include "UI/Screens/BaseScreen.h"
#include "UI/Screens/WorldMap/WorldMapScreenLayout.h"
#include "UI/Renderers/3D/EngineRaycaster/Raycaster.h"
#include "Core/Utils/Appearance.h"

namespace {

struct RGB { int r, g, b; };

RGB charColor(char c) {
    if (c == ' ') return {0, 0, 0};
    if (c == '_' || c == '-' || c == '|' || c == '/' || c == '\\'
        || c == '>' || c == '<' || c == '?' || c == '`' || c == '!')
        return {70, 170, 230};
    if (c == '#')
        return {25, 100, 200};
    return {60, 180, 55};
}

}

void RaycasterWorldMapScreen::renderPopup(const std::vector<std::string>& art, const std::vector<std::string>& places, int selection, bool redesignComplete) {
    int widthTerm = Appearance::getTerminalWidth();
    int heightTerm = Appearance::getTerminalHeight();

    int artH = (int)art.size();
    int artW = 0;
    for (const auto& l : art) {
        int w = (int)l.length();
        if (w > artW) artW = w;
    }

    int listH = 1 + (int)places.size();
    int listX = artW + 4;
    int maxListTextW = 24;
    int contW = listX + maxListTextW;
    int contH = std::max(artH, listH) + 2;

    std::string bgDark = "\033[48;2;25;25;25m";
    std::string resetBg = "\033[0m";

    std::vector<std::string> content(contH, std::string(contW, ' '));
    auto box = BaseScreen::createBox(content, " MAPA MUNDI - VIAGEM RAPIDA ", 0, Color::CYAN, bgDark);

    int outW = Appearance::getVisualLength(box[0]);
    int outH = (int)box.size();

    int startX = std::max(0, (widthTerm - outW) / 2);
    int startY = std::max(0, (heightTerm - outH) / 2);
    if (startY + outH > heightTerm) startY = std::max(0, heightTerm - outH);
    if (startX + outW > widthTerm) startX = std::max(0, widthTerm - outW);

    std::cout << "\033[?25l";

    if (redesignComplete) {
        Raycaster::restoreLastFrame();

        for (int i = 0; i < outH; ++i) {
            if (startY + i < heightTerm) {
                Appearance::moveCursor(startX, startY + i);
                std::cout << box[i];
            }
        }

        int innerX = startX + 2;
        int innerY = startY + 2;

        for (int y = 0; y < artH; ++y) {
            if (innerY + y >= heightTerm) continue;
            Appearance::moveCursor(innerX, innerY + y);
            std::string lineBuffer = "";
            for (int x = 0; x < (int)art[y].length(); ++x) {
                if (innerX + x >= widthTerm) break;
                char c = art[y][x];
                if (c == ' ') {
                    lineBuffer += bgDark + " ";
                } else {
                    RGB color = charColor(c);
                    lineBuffer += "\033[48;2;" + std::to_string(color.r) + ";" + std::to_string(color.g) + ";" + std::to_string(color.b) + "m ";
                }
            }
            lineBuffer += resetBg;
            std::cout << lineBuffer;
        }
    }

    int innerX = startX + 2;
    int innerY = startY + 2;
    int listOx = innerX + listX;
    int listOy = innerY;

    auto writeOption = [&](int idx, const std::string& text) {
        if (listOy + idx >= heightTerm) return;
        Appearance::moveCursor(listOx, listOy + idx);
        std::string fullText;
        if (idx == selection) {
            fullText = Appearance::color(Color::GREEN) + " > " + Appearance::color(Color::WHITE) + text;
        } else {
            fullText = "   " + text;
        }
        int curW = Appearance::getVisualLength(fullText);
        int pad = maxListTextW - curW;
        std::cout << bgDark << fullText << bgDark << std::string(pad > 0 ? pad : 0, ' ') << resetBg;
    };

    writeOption(0, "[ VOLTAR ]");
    for (size_t i = 0; i < places.size(); ++i)
        writeOption(1 + (int)i, places[i]);

    std::cout << std::flush;
}
