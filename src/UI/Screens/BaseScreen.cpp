#include "UI/Screens/BaseScreen.h"
#include "UI/PerspectiveManager.h"
#include "Core/Utils/InputControl.h"
#include "Core/Utils/Appearance.h"
#include "Domain/Characters/Character.h"
#include "Domain/Characters/Races/BaseRace.h"
#include "Domain/Characters/Classes/BaseClass.h"
#include "Systems/Inventory/Inventory.h"
#include <iostream>
#include <algorithm>

std::string BaseScreen::generateBarGradient(double pct, int size, Color colorEnd) {
    if (pct < 0.0) pct = 0.0;
    if (pct > 1.0) pct = 1.0;
    int qtyReal = static_cast<int>(pct * size * 8);
    std::string bar = "";
    for (int i = 0; i < size; ++i) {
        int intensity = 130 + (125 * i) / std::max(1, size - 1);
        std::string colorCurrent = Appearance::getColorRGBFade(colorEnd, intensity);
        int charIdx = i * 8;
        if (qtyReal >= charIdx + 4) bar += colorCurrent + "█";
        else bar += Appearance::color(Color::GRAY) + "░";
    }
    return bar;
}

void BaseScreen::printLineDivider(char character) {
    std::string line = "";
    int width = Appearance::getTerminalWidth();
    
    bool isEngineIDE = !PerspectiveManager::getInstance().is3DViewActive();
    if (isEngineIDE) {
        line = "\033[38;2;96;139;78m// ";
        for (int i = 0; i < width - 3; ++i) line += "=";
        line += "\033[0m";
        std::cout << line << "\n";
        return;
    }

    if (character == '=') {
        for (int i = 0; i < width; ++i) line += "═";
    } else if (character == '-') {
        for (int i = 0; i < width; ++i) line += "─";
    } else {
        line = std::string(width, character);
    }
    std::cout << line << "\n";
}

void BaseScreen::executeLoop(
    const std::function<void(bool)>& renderHeader,
    const std::function<void()>& renderContent,
    const std::function<std::vector<std::string>()>& builderOptionsMenu,
    const std::function<bool(int)>& processChoose,
    bool centralizeMenu,
    const std::string& marginMenu)
{
    bool firstTime = true;
    while (true) {
        Appearance::clearScreen();
        
        if (renderHeader) {
            renderHeader(firstTime);
            firstTime = false;
        }

        if (renderContent) {
            renderContent();
        }

        std::vector<std::string> options = builderOptionsMenu();
        std::cout << "\n";
        int choice = InputControl::readSelectionMenuWithArrows(options, centralizeMenu, marginMenu);
        
        if (!processChoose(choice)) {
            break;
        }
    }
}

void BaseScreen::executeDefaultLoop(
    const std::string& title,
    Color themeColor,
    const std::function<void()>& renderContent,
    const std::function<std::vector<std::string>()>& builderOptionsMenu,
    const std::function<bool(int)>& processChoose)
{
    executeLoop(
        [title, themeColor](bool animate) { Appearance::displayTextPanel(title, themeColor, animate); },
        renderContent,
        builderOptionsMenu,
        processChoose
    );
}

static std::string replaceAllResetsWithBg(const std::string& input, const std::string& bgAnsi) {
    if (bgAnsi.empty()) return input;
    std::string result = "";
    result.reserve(input.size() * 2);
    for (size_t i = 0; i < input.size(); ) {
        if (input[i] == '\033' && i + 3 < input.size() && input[i+1] == '[' && input[i+2] == '0' && input[i+3] == 'm') {
            result += "\033[0m" + bgAnsi;
            i += 4;
        } else if (input[i] == '\033' && i + 2 < input.size() && input[i+1] == '[' && input[i+2] == 'm') {
            result += "\033[0m" + bgAnsi;
            i += 3;
        } else {
            result += input[i];
            i++;
        }
    }
    return result;
}

std::vector<std::string> BaseScreen::createBox(const std::vector<std::string>& lines, const std::string& title, int widthMinimal, Color colorBox, const std::string& bgAnsi) {
    int maxWidth = widthMinimal;
    for (const auto& line : lines) {
        int comp = Appearance::getVisualLength(line);
        if (comp > maxWidth) maxWidth = comp;
    }
    
    std::vector<std::string> box;
    std::string colorStr = Appearance::color(colorBox);
    std::string resetStr = Appearance::color(Color::RESET);

    bool isEngineIDE = !PerspectiveManager::getInstance().is3DViewActive();
    if (isEngineIDE) {
        std::string padBg = bgAnsi.empty() ? "\033[48;2;25;25;25m" : bgAnsi;
        std::string titleIDE = title.empty() ? "Info" : title;
        std::replace(titleIDE.begin(), titleIDE.end(), ' ', '_');
        
        box.push_back(padBg + "\033[38;2;86;156;214mstruct\033[0m \033[38;2;78;201;176m" + titleIDE + "\033[0m {");
        for (const auto& line : lines) {
            std::string cleanLine = line;
            if (cleanLine.find(":") != std::string::npos) {
                size_t post = cleanLine.find(":");
                std::string key = cleanLine.substr(0, post);
                std::string value = cleanLine.substr(post + 1);
                
                std::string keyVar = Appearance::removeANSIColors(key);
                keyVar.erase(std::remove(keyVar.begin(), keyVar.end(), ' '), keyVar.end());
                
                cleanLine = "    \033[38;2;86;156;214mauto\033[0m " + keyVar + " = " + value + ";";
            } else {
                cleanLine = "    " + line + ";";
            }
            int comp = Appearance::getVisualLength(cleanLine);
            int padding = maxWidth - comp;
            std::string processedLine = replaceAllResetsWithBg(cleanLine, padBg);
            box.push_back(padBg + processedLine + padBg + std::string(padding > 0 ? padding : 0, ' ') + "\033[0m");
        }
        box.push_back(padBg + "};\033[0m");
        return box;
    }

    std::string padBg = bgAnsi.empty() ? "\033[48;2;25;25;25m" : bgAnsi;

    std::string top = "╔";
    int titleLen = Appearance::getVisualLength(title);
    if (titleLen > 0) {
        top += "══ " + title + " ";
        int remaining = maxWidth + 2 - (titleLen + 4);
        if (remaining < 0) remaining = 0;
        for (int i = 0; i < remaining; ++i) {
            top += "═";
        }
    } else {
        for (int i = 0; i < maxWidth + 2; ++i) {
            top += "═";
        }
    }
    top += "╗";
    box.push_back(padBg + colorStr + top + "\033[0m");

    for (const auto& line : lines) {
        int comp = Appearance::getVisualLength(line);
        int padding = maxWidth - comp;
        std::string processedLine = replaceAllResetsWithBg(line, padBg);
        box.push_back(padBg + colorStr + "║ " + padBg + processedLine + padBg + std::string(padding > 0 ? padding : 0, ' ') + colorStr + padBg + " ║\033[0m");
    }

    std::string bottom = "╚";
    for (int i = 0; i < maxWidth + 2; ++i) {
        bottom += "═";
    }
    bottom += "╝";
    box.push_back(padBg + colorStr + bottom + "\033[0m");

    return box;
}

std::vector<std::string> BaseScreen::createBoxWithArt(const std::vector<std::string>& art, const std::vector<std::string>& linesText, const std::string& title, int widthMinimal, Color colorBox, const std::string& bgAnsi) {
    int artWidth = 0;
    for (const auto& l : art) {
        int len = Appearance::getVisualLength(l);
        if (len > artWidth) artWidth = len;
    }

    int widthText = widthMinimal;
    for (const auto& l : linesText) {
        int len = Appearance::getVisualLength(l);
        if (len > widthText) widthText = len;
    }

    bool hasArt = artWidth > 0;
    int totalWidth = widthText;
    if (hasArt) totalWidth += artWidth + 3;

    if (totalWidth < widthMinimal) totalWidth = widthMinimal;

    int boxHeight = std::max(static_cast<int>(art.size()), static_cast<int>(linesText.size()));

    std::vector<std::string> box;
    std::string colorStr = Appearance::color(colorBox);
    std::string padBg = bgAnsi.empty() ? "\033[48;2;25;25;25m" : bgAnsi;

    std::string top = padBg + colorStr + "╔";
    int titleLen = Appearance::getVisualLength(title);
    if (titleLen > 0) {
        top += "══ " + title + " ";
        int remaining = totalWidth + 2 - (titleLen + 4);
        if (remaining < 0) remaining = 0;
        for (int i = 0; i < remaining; ++i) top += "═";
    } else {
        for (int i = 0; i < totalWidth + 2; ++i) top += "═";
    }
    top += "╗\033[0m";
    box.push_back(top);

    for (int i = 0; i < boxHeight; ++i) {
        std::string lineArt = (i < static_cast<int>(art.size())) ? art[i] : "";
        int compArt = Appearance::getVisualLength(lineArt);
        int padArt = artWidth - compArt;

        std::string lineText = (i < static_cast<int>(linesText.size())) ? linesText[i] : "";
        int compText = Appearance::getVisualLength(lineText);
        int padText = widthText - compText;

        std::string procArt = replaceAllResetsWithBg(lineArt, padBg);
        std::string procText = replaceAllResetsWithBg(lineText, padBg);

        std::string row;
        if (hasArt) {
            row = padBg + colorStr + "║ " + padBg + procArt + padBg + std::string(padArt > 0 ? padArt : 0, ' ') + colorStr + padBg + " ║ " + padBg + procText + padBg + std::string(padText > 0 ? padText : 0, ' ') + colorStr + padBg + " ║\033[0m";
        } else {
            row = padBg + colorStr + "║ " + padBg + procText + padBg + std::string(padText > 0 ? padText : 0, ' ') + colorStr + padBg + " ║\033[0m";
        }
        box.push_back(row);
    }

    std::string bottom = padBg + colorStr + "╚";
    for (int i = 0; i < totalWidth + 2; ++i) bottom += "═";
    bottom += "╝\033[0m";
    box.push_back(bottom);

    return box;
}

bool BaseScreen::mustAnimateScreenEntry(std::chrono::steady_clock::time_point& lastAccess, int delayMilliseconds) {
    auto now = std::chrono::steady_clock::now();
    bool animate = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastAccess).count() > delayMilliseconds;
    lastAccess = now;
    return animate;
}