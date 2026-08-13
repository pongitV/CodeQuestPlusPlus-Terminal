#include "UI/Renderers/IDE/IDEScreens/Map/IDEMapScreen.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <thread>
#include <chrono>
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Systems/Progression/Progression.h"
#include "Systems/Progression/ProgressionFlags.h"
#include "UI/Screens/WorldMap/WorldMapScreen.h"
#include "UI/Renderers/IDE/IDETheme.h"

int IDEMapScreen::display(MapLocation locationCurrent, int villageProgress, int forestProgress, int kingdomBridgeProgress, int kingdomProgress, const std::string& msgExtra) {
    int idCurrent = (locationCurrent == MapLocation::InitialVillage) ? 0 :
                  (locationCurrent == MapLocation::Forest) ? 1 :
                  (locationCurrent == MapLocation::KingdomBridge) ? 2 : 3;

    int idSelected = idCurrent;
    bool mapsDiscovered = Progression::instance().getFlag(Flags::Discovered_Maps);

    bool blinksYellow = true;
    auto lastBlink = std::chrono::steady_clock::now();
    bool needsRender = true;
    std::string msg = msgExtra;

    while (true) {
        if (needsRender) {
            Appearance::clearScreen();

            std::cout << "\n";
            Appearance::printCentralized("MAPA MUNDIAL - VIAGEM RAPIDA", Appearance::color(Color::BOLD, Color::WHITE));
            std::cout << "\n\n";

            auto getProgram = [](int val, bool disc) {
                if (!disc) return Appearance::color(Color::GRAY) + "??? " + Appearance::color(Color::RESET);
                std::string p = std::to_string(val);
                if (p.length() < 3) p = std::string(3 - p.length(), ' ') + p;
                std::string c = (val >= 100) ? Appearance::color(Color::CYAN) : (val > 0) ? Appearance::color(Color::YELLOW) : Appearance::color(Color::WHITE);
                return c + p + "%" + Appearance::color(Color::RESET);
            };

            auto getColor = [&](int id, bool disc) {
                if (id == idSelected) return Appearance::color(Color::BOLD, Color::YELLOW);
                if (!disc) return Appearance::color(Color::GRAY);
                return Appearance::color(Color::WHITE);
            };

            auto getText = [&](std::string txt, int id, bool disc) {
                if (!disc) {
                    if (txt.length() == 6) txt = std::string(" ???  ");
                    else if (txt.length() == 10) txt = std::string("   ???    ");
                    else if (txt.length() == 7) txt = std::string("  ???  ");
                    else if (txt.length() == 9) txt = std::string("   ???   ");
                }

                std::string res = txt;
                if (id == idSelected && blinksYellow) {
                    res.front() = '>';
                    res.back() = '<';
                }

                if (id == idSelected) {
                    if (blinksYellow) {
                        res = Appearance::color(Color::BOLD, Color::YELLOW) + res;
                    } else {
                        if (id == idCurrent) res = Appearance::color(Color::BOLD, Color::GREEN) + res;
                        else res = Appearance::color(Color::BOLD, Color::WHITE) + res;
                    }
                } else if (id == idCurrent) {
                    res = Appearance::color(Color::BOLD, Color::GREEN) + res;
                } else {
                    res = Appearance::color(Color::BOLD, Color::WHITE) + res;
                }

                res += getColor(id, disc);
                return res;
            };

            auto getMarginLeft = [&](int) {
                return std::string("   ") + Appearance::color(Color::WHITE) + "\u2551" + Appearance::color(Color::RESET);
            };

            std::string marginRight = Appearance::color(Color::WHITE) + "\u2551" + Appearance::color(Color::RESET) + "   ";

            std::vector<std::string> lines;
            lines.push_back(Appearance::color(Color::WHITE) + "   \u2554\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2557   " + Appearance::color(Color::RESET));
            lines.push_back(getMarginLeft(-1) + std::string(47, ' ') + marginRight);

            lines.push_back(getMarginLeft(-1) + std::string(28, ' ') + getColor(0, true) + "\u2554\u2550\u2550\u2550\u2550\u2550\u2557" + Appearance::color(Color::RESET) + std::string(11, ' ') + marginRight);
            lines.push_back(getMarginLeft(0) + std::string(28, ' ') + getColor(0, true) + "\u2551" + getText(" VILA ", 0, true) + "\u2551" + Appearance::color(Color::RESET) + " " + getProgram(villageProgress, true) + std::string(6, ' ') + marginRight);
            lines.push_back(getMarginLeft(-1) + std::string(28, ' ') + getColor(0, true) + "\u255A\u2550\u2550\u2550\u2550\u2550\u255D" + Appearance::color(Color::RESET) + std::string(11, ' ') + marginRight);

            lines.push_back(getMarginLeft(-1) + std::string(31, ' ') + getColor(0, true) + "\u2551\u2551" + Appearance::color(Color::RESET) + std::string(14, ' ') + marginRight);

            lines.push_back(getMarginLeft(-1) + std::string(26, ' ') + getColor(1, mapsDiscovered) + (mapsDiscovered ? "\u2554\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2557" : "  \u2554\u2550\u2550\u2550\u2550\u2557   ") + Appearance::color(Color::RESET) + std::string(9, ' ') + marginRight);
            lines.push_back(getMarginLeft(1) + std::string(26, ' ') + getColor(1, mapsDiscovered) + (mapsDiscovered ? "\u2551" : "  \u2551") + getText(mapsDiscovered ? " FLORESTA " : " ??? ", 1, mapsDiscovered) + (mapsDiscovered ? "\u2551" : "\u2551   ") + Appearance::color(Color::RESET) + " " + getProgram(forestProgress, mapsDiscovered) + std::string(4, ' ') + marginRight);
            lines.push_back(getMarginLeft(-1) + std::string(26, ' ') + getColor(1, mapsDiscovered) + (mapsDiscovered ? "\u255A\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u255D" : "  \u255A\u2550\u2550\u2550\u2550\u255D   ") + Appearance::color(Color::RESET) + std::string(9, ' ') + marginRight);

            lines.push_back(getMarginLeft(-1) + std::string(31, ' ') + getColor(1, mapsDiscovered) + "\u2551\u2551" + Appearance::color(Color::RESET) + std::string(14, ' ') + marginRight);

            lines.push_back(getMarginLeft(-1) + std::string(15, ' ') + getColor(2, mapsDiscovered) + (mapsDiscovered ? "\u2554\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2557" : "  \u2554\u2550\u2550\u2550\u2550\u2557") + Appearance::color(Color::RESET) + std::string(7, ' ') + getColor(1, mapsDiscovered) + "\u2551\u2551" + Appearance::color(Color::RESET) + std::string(14, ' ') + marginRight);
            lines.push_back(getMarginLeft(2) + std::string(15, ' ') + getColor(2, mapsDiscovered) + (mapsDiscovered ? "\u2551" : "  \u2551") + getText(mapsDiscovered ? " Ponte " : " ??? ", 2, mapsDiscovered) + (mapsDiscovered ? "\u2560" : "\u2560") + Appearance::color(Color::RESET) + getColor(1, mapsDiscovered) + "\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2563\u2551 " + Appearance::color(Color::RESET) + getProgram(kingdomBridgeProgress, mapsDiscovered) + std::string(9, ' ') + marginRight);
            lines.push_back(getMarginLeft(-1) + std::string(15, ' ') + getColor(2, mapsDiscovered) + (mapsDiscovered ? "\u255A\u2550\u2550\u2550\u2566\u2550\u2550\u2550\u255D" : "  \u255A\u2550\u2550\u2550\u2550\u255D") + Appearance::color(Color::RESET) + std::string(23, ' ') + marginRight);

            lines.push_back(getMarginLeft(-1) + std::string(19, ' ') + getColor(2, mapsDiscovered) + "\u2551\u2551" + Appearance::color(Color::RESET) + std::string(26, ' ') + marginRight);

            lines.push_back(getMarginLeft(-1) + std::string(15, ' ') + getColor(3, mapsDiscovered) + (mapsDiscovered ? "\u2554\u2550\u2550\u2550\u2569\u2550\u2550\u2550\u2550\u2550\u2557" : "  \u2554\u2550\u2550\u2550\u2550\u2557  ") + Appearance::color(Color::RESET) + std::string(21, ' ') + marginRight);
            lines.push_back(getMarginLeft(3) + std::string(15, ' ') + getColor(3, mapsDiscovered) + (mapsDiscovered ? "\u2551" : "  \u2551") + getText(mapsDiscovered ? " Reino   " : " ???   ", 3, mapsDiscovered) + (mapsDiscovered ? "\u2551" : "\u2551") + Appearance::color(Color::RESET) + " " + getProgram(kingdomProgress, mapsDiscovered) + std::string(16, ' ') + marginRight);
            lines.push_back(getMarginLeft(-1) + std::string(15, ' ') + getColor(3, mapsDiscovered) + (mapsDiscovered ? "\u255A\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u255D" : "  \u255A\u2550\u2550\u2550\u2550\u255D  ") + Appearance::color(Color::RESET) + std::string(21, ' ') + marginRight);

            lines.push_back(getMarginLeft(-1) + std::string(47, ' ') + marginRight);
            lines.push_back(Appearance::color(Color::WHITE) + "   \u255A\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u255D   " + Appearance::color(Color::RESET));

            int terminalWidth = Appearance::getTerminalWidth();
            int spacesMargin = std::max(0, (terminalWidth - 55) / 2);
            std::string marginCentral(spacesMargin, ' ');

            for (const auto& l : lines) {
                std::cout << marginCentral << l << "\n";
            }

            std::cout << "\n";
            if (!msg.empty()) {
                Appearance::printCentralized(msg, Appearance::color(Color::YELLOW));
                msg = "";
            } else {
                Appearance::printCentralized("Utilize [W][S] ou [Setas] para selecionar um destino.", Appearance::color(Color::GRAY));
                Appearance::printCentralized("[ENTER] Viajar | [Q]/[M] Cancelar", Appearance::color(Color::GRAY));
            }
            std::cout << "\n";

            needsRender = false;
        }

        if (InputControl::pressedKey()) {
            char key = InputControl::readKey();
            if (key == '\033') {
                char nextKey = InputControl::readKey();
                if (nextKey == '[') {
                    nextKey = InputControl::readKey();
                    if (nextKey == 'A') key = 'W';
                    else if (nextKey == 'B') key = 'S';
                }
            } else if (key == -32 || key == 0) {
                char nextKey = InputControl::readKey();
                if (nextKey == 72) key = 'W';
                else if (nextKey == 80) key = 'S';
            } else {
                key = toupper(key);
            }

            if (key == 'W') {
                if (idSelected > 0) idSelected--;
                needsRender = true;
                blinksYellow = true;
                lastBlink = std::chrono::steady_clock::now();
            } else if (key == 'S') {
                if (idSelected < 3) idSelected++;
                needsRender = true;
                blinksYellow = true;
                lastBlink = std::chrono::steady_clock::now();
            } else if (key == 'Q' || key == 'M') {
                return -1;
            } else if (key == '\n' || key == '\r') {
                return idSelected;
            }
        } else {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBlink).count() >= 500) {
                blinksYellow = !blinksYellow;
                lastBlink = now;
                needsRender = true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }
}
