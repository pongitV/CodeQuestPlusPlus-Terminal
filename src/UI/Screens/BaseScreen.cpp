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
        std::string padBg = "";
        if (!bgAnsi.empty()) {
            padBg = bgAnsi;
        } else {
            padBg = "\033[48;2;25;25;25m";
        }
        std::string titleGO = title.empty() ? "Info" : title;
        std::replace(titleGO.begin(), titleGO.end(), ' ', '_');
        
        box.push_back(padBg + "\033[38;2;86;156;214mstruct\033[0m \033[38;2;78;201;176m" + titleGO + "\033[0m {");
        for (const auto& line : lines) {
            std::string cleanLine = line;
            if (cleanLine.find(":") != std::string::npos) {
                size_t post = cleanLine.find(":");
                std::string key = cleanLine.substr(0, post);
                std::string value = cleanLine.substr(post + 1);
                
                std::string keyVar = Appearance::removeANSIColors(key);
                keyVar.erase(std::remove(keyVar.begin(), keyVar.end(), ' '), keyVar.end());
                
                cleanLine = padBg + "    \033[38;2;86;156;214mauto\033[0m " + keyVar + " = " + value + ";";
            } else {
                cleanLine = padBg + "    " + line + ";";
            }
            
            int comp = Appearance::getVisualLength(cleanLine);
            int padding = maxWidth - comp;
            if (padding > 0) cleanLine += std::string(padding, ' ');
            
            box.push_back(cleanLine);
        }
        box.push_back(padBg + "};");

        for (auto& c : box) {
            c += "\033[0m";
            std::string toReplace = "\033[0m";
            std::string replaceWith = "\033[0m" + padBg;
            size_t post = c.find(toReplace);
            while (post != std::string::npos) {
                size_t nextPost = c.find(toReplace, post + toReplace.length());
                if (nextPost != std::string::npos) {
                    c.replace(post, toReplace.length(), replaceWith);
                    post = c.find(toReplace, post + replaceWith.length());
                } else {
                    post = std::string::npos;
                }
            }
        }

        return box;
    }

    std::string padBg = "";
    if (!bgAnsi.empty()) {
        padBg = bgAnsi;
    } else {
        padBg = "\033[48;2;0;0;0m";
    }

    std::string top = "╔";
    int titleLen = Appearance::getVisualLength(title);
    
    // Em modo 3D (raycaster), nao colocamos o texto na borda, pois um titulo em ASCII flutua acima!
    if (!isEngineIDE) {
        titleLen = 0;
    }
    
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
    box.push_back(padBg + colorStr + top + resetStr);

    for (const auto& line : lines) {
        int comp = Appearance::getVisualLength(line);
        int padding = maxWidth - comp;
        box.push_back(padBg + colorStr + "║ " + resetStr + padBg + line + padBg + std::string(padding > 0 ? padding : 0, ' ') + colorStr + padBg + " ║" + resetStr);
    }

    std::string bottom = "╚";
    for (int i = 0; i < maxWidth + 2; ++i) {
        bottom += "═";
    }
    bottom += "╝";
    box.push_back(padBg + colorStr + bottom + resetStr);

    if (!isEngineIDE) {
        for (auto& c : box) {
            std::string toReplace = "\033[0m";
            std::string replaceWith = "\033[0m" + padBg;
            size_t post = c.find(toReplace);
            while (post != std::string::npos) {
                size_t nextPost = c.find(toReplace, post + toReplace.length());
                if (nextPost != std::string::npos) {
                    c.replace(post, toReplace.length(), replaceWith);
                    post = c.find(toReplace, post + replaceWith.length());
                } else {
                    post = std::string::npos;
                }
            }
        }
    }

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
    std::string resetStr = Appearance::color(Color::RESET);
    std::string padBg = bgAnsi.empty() ? "" : bgAnsi;

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
    top += "╗" + resetStr;
    box.push_back(top);

    for (int i = 0; i < boxHeight; ++i) {
        std::string lineArt = (i < static_cast<int>(art.size())) ? art[i] : "";
        int compArt = Appearance::getVisualLength(lineArt);
        int padArt = artWidth - compArt;

        std::string lineText = (i < static_cast<int>(linesText.size())) ? linesText[i] : "";
        int compText = Appearance::getVisualLength(lineText);
        int padText = widthText - compText;

        std::string row;
        if (hasArt) {
            row = padBg + colorStr + "║ " + resetStr + padBg + lineArt + std::string(padArt > 0 ? padArt : 0, ' ') + colorStr + padBg + " ║ " + resetStr + padBg + lineText + std::string(padText > 0 ? padText : 0, ' ') + colorStr + padBg + " ║" + resetStr;
        } else {
            row = padBg + colorStr + "║ " + resetStr + padBg + lineText + std::string(padText > 0 ? padText : 0, ' ') + colorStr + padBg + " ║" + resetStr;
        }
        box.push_back(row);
    }

    std::string bottom = padBg + colorStr + "╚";
    for (int i = 0; i < totalWidth + 2; ++i) bottom += "═";
    bottom += "╝" + resetStr;
    box.push_back(bottom);

    if (!bgAnsi.empty()) {
        for (auto& c : box) {
            std::string toReplace = "\033[0m";
            std::string replaceWith = "\033[0m" + bgAnsi;
            size_t post = c.find(toReplace);
            while (post != std::string::npos) {
                size_t nextPost = c.find(toReplace, post + toReplace.length());
                if (nextPost != std::string::npos) {
                    c.replace(post, toReplace.length(), replaceWith);
                    post = c.find(toReplace, post + replaceWith.length());
                } else {
                    post = std::string::npos;
                }
            }
        }
    }

    return box;
}

bool BaseScreen::mustAnimateScreenEntry(std::chrono::steady_clock::time_point& lastAccess, int delayMilliseconds) {
    auto now = std::chrono::steady_clock::now();
    bool animate = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastAccess).count() > delayMilliseconds;
    lastAccess = now;
    return animate;
}