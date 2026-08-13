#include "Core/Terminal/Appearance/Appearance.h"
#include "UI/PerspectiveManager.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <thread>
#include <chrono>

void Appearance::printCentralized(const std::string& text, const std::string& colorAnsi) {
    size_t end = text.find_last_not_of(' ');
    std::string trimmed = (end != std::string::npos) ? text.substr(0, end + 1) : "";
    std::cout << "\033[0m" << spacesToCenter(getVisualLength(trimmed)) << colorAnsi << text << "\033[0m\n";
}

void Appearance::printCentralizedMultiline(const std::vector<std::string>& lines, int widthVisual, const std::string& colorAnsi, int delayLineMs) {
    for (const std::string& line : lines) {
        if (widthVisual > 0) {
            std::cout << spacesToCenter(widthVisual) << colorAnsi << line << (colorAnsi.empty() ? "" : color(Color::RESET)) << "\n";
        } else {
            printCentralized(line, colorAnsi);
        }
        if (delayLineMs > 0) {
            std::cout << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(delayLineMs));
        }
    }
}

void Appearance::printBlockCentralized(const std::vector<std::string>& lines, const std::string& colorAnsi, int delayLineMs) {
    int longestLineSize = 0;
    for (const std::string& line : lines) {
        size_t end = line.find_last_not_of(' ');
        std::string trimmed = (end != std::string::npos) ? line.substr(0, end + 1) : "";
        longestLineSize = std::max(longestLineSize, getVisualLength(trimmed));
    }
    printCentralizedMultiline(lines, longestLineSize, colorAnsi, delayLineMs);
}

void Appearance::displayPanel(
    const std::string& title, 
    Color colorMain, 
    const std::vector<std::string>& artMain, 
    int artWidth, 
    const std::vector<std::string>& artSecondary, 
    Color colorSecondary, 
    bool animateFadeIn
) {
    std::string upperTitle = title;
    std::transform(upperTitle.begin(), upperTitle.end(), upperTitle.begin(), [](unsigned char c){ return std::toupper(c); });
    
    int terminalWidth = getTerminalWidth();
    std::string lineDivider = "";
    for(int i = 0; i < terminalWidth; ++i) lineDivider += "═";

    bool isEngineIDE = !PerspectiveManager::getInstance().is3DViewActive();

    if (isEngineIDE) {
        std::cout << "\n\n";
        std::string classDef = "\033[38;2;86;156;214mclass\033[0m \033[38;2;78;201;176m" + upperTitle + "\033[0m {";
        std::string publ = "\033[38;2;86;156;214mpublic:\033[0m";
        
        if (!artMain.empty()) {
            int recoil = std::max(0, (terminalWidth - artWidth) / 2);
            std::string margin(recoil, ' ');
            std::cout << margin << "\033[38;2;96;139;78m/* ==========================================================\033[0m\n";
            for (size_t i = 0; i < artMain.size(); ++i) {
                std::cout << margin << "\033[38;2;96;139;78m * \033[0m" << artMain[i];
                if (!artSecondary.empty() && i < artSecondary.size()) {
                    std::cout << artSecondary[i];
                }
                std::cout << "\n";
            }
            std::cout << margin << "\033[38;2;96;139;78m ========================================================== */\033[0m\n\n";
        }
        
        printCentralized(classDef);
        std::cout << "\n";
        printCentralized(publ);
        std::cout << "\n";
        return;
    }

    bool hasArt = !artMain.empty();
    bool hasArtSecondary = !artSecondary.empty();
    bool hasTitle = !upperTitle.empty();

    if (animateFadeIn) {
        int targetY = 2;

        std::vector<std::string> linesFinals;
        if (hasArt) {
            int recoil = std::max(0, (terminalWidth - artWidth) / 2);
            std::string margin(recoil, ' ');
            for (size_t i = 0; i < artMain.size(); ++i) {
                std::string line = margin + color(colorMain) + artMain[i];
                if (hasArtSecondary && i < artSecondary.size()) {
                    line += color(colorSecondary) + artSecondary[i];
                }
                linesFinals.push_back(line);
            }
            linesFinals.push_back(color(colorMain) + lineDivider);
        }
        if (hasTitle) {
            linesFinals.push_back(spacesToCenter(getVisualLength(upperTitle)) + color(colorMain) + upperTitle);
            linesFinals.push_back(color(colorMain) + lineDivider);
        }

        Appearance::animateFadeIn(30, 40, [&](int /*frame*/, int intensity) {
            float pct = intensity / 255.0f;
            std::ostringstream buffer;
            for (size_t i = 0; i < linesFinals.size(); ++i) {
                int y = targetY + static_cast<int>(i);
                buffer << "\033[" << (y + 1) << ";1H" << fatarLineAnsi(linesFinals[i], pct) << "\033[K";
                if (i < linesFinals.size() - 1) buffer << "\n";
            }
            buffer << "\033[0m";
            std::cout << buffer.str() << std::flush;
        });
    } else {
        std::cout << "\n\n";
        
        if (hasArt) {
            int recoil = std::max(0, (terminalWidth - artWidth) / 2);
            std::string margin(recoil, ' ');
            for (size_t i = 0; i < artMain.size(); ++i) {
                std::cout << margin << color(colorMain) << artMain[i];
                if (hasArtSecondary && i < artSecondary.size()) {
                    std::cout << color(colorSecondary) << artSecondary[i];
                }
                std::cout << color(Color::RESET) << "\n";
            }
            std::cout << "\n" << color(colorMain) << lineDivider << color(Color::RESET) << "\n";
        }

        if (hasTitle) {
            printCentralized(upperTitle, color(colorMain));
            std::cout << color(colorMain) << lineDivider << color(Color::RESET) << "\n";
        }
    }
}

void Appearance::displayTextPanel(const std::string& title, Color colorDoHeader, bool animateFadeIn) {
    displayPanel(title, colorDoHeader, {}, 0, {}, Color::RESET, animateFadeIn);
}

void Appearance::displayTitlePattern(const std::string& title, Color themeColor) {
    std::string upperTitle = title;
    std::transform(upperTitle.begin(), upperTitle.end(), upperTitle.begin(), [](unsigned char c){ return std::toupper(c); });
    
    int terminalWidth = getTerminalWidth();
    std::string lineDividerStr = "";
    for(int i = 0; i < terminalWidth; ++i) lineDividerStr += "═";

    std::string lineTitle = spacesToCenter(getVisualLength(upperTitle)) + color(themeColor) + upperTitle + color(Color::RESET);
    std::string lineDiv = color(themeColor) + lineDividerStr + color(Color::RESET);

    std::vector<std::string> linesFinals = { lineTitle, lineDiv };
    int targetY = 2;

    Appearance::animateFadeIn(30, 40, [&](int /*frame*/, int intensity) {
        float pct = intensity / 255.0f;
        std::ostringstream buffer;
        for (size_t i = 0; i < linesFinals.size(); ++i) {
            int y = targetY + static_cast<int>(i);
            buffer << "\033[" << (y + 1) << ";1H" << fatarLineAnsi(linesFinals[i], pct) << "\033[K";
            if (i < linesFinals.size() - 1) buffer << "\n";
        }
        buffer << "\033[0m";
        std::cout << buffer.str() << std::flush;
    });

    for (size_t i = 0; i < linesFinals.size(); ++i) {
        int y = targetY + static_cast<int>(i);
        std::cout << "\033[" << (y + 1) << ";1H" << linesFinals[i] << "\033[K";
        if (i < linesFinals.size() - 1) std::cout << "\n";
    }
    std::cout << "\033[J" << std::flush;
}

int Appearance::printSideASide(const std::vector<std::string>& columnLeft, const std::vector<std::string>& columnRight, int minWidthLeft, int spacing, Color colorLeft, Color colorRight, int delayLineMs) {
    int widthLeft = minWidthLeft;
    for (const auto& s : columnLeft) {
        if (getVisualLength(s) > widthLeft) {
            widthLeft = getVisualLength(s);
        }
    }
    
    int widthSay = 0;
    for (const auto& s : columnRight) {
        if (getVisualLength(s) > widthSay) {
            widthSay = getVisualLength(s);
        }
    }

    int recoil = (getTerminalWidth() - (widthLeft + spacing + widthSay)) / 2;
    if (recoil < 0) recoil = 0;

    size_t maxLines = std::max(columnLeft.size(), columnRight.size());
    std::cout << "\n";
    for (size_t i = 0; i < maxLines; ++i) {
        std::cout << std::string(recoil, ' ');
        
        if (i < columnLeft.size()) {
            std::string textLeft = columnLeft[i];
            int padding = widthLeft - getVisualLength(textLeft);
            std::cout << color(colorLeft) << textLeft << std::string(padding > 0 ? padding : 0, ' ') << color(Color::RESET);
        } else {
            std::cout << std::string(widthLeft, ' ');
        }
        
        std::cout << std::string(spacing, ' ');
        
        if (i < columnRight.size()) {
            std::cout << color(colorRight) << columnRight[i] << color(Color::RESET);
        }
        std::cout << "\n";
        
        if (delayLineMs > 0) {
            std::cout << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(delayLineMs));
        }
    }
    
    return recoil;
}

void Appearance::displayPrompt(const std::string& message) {
    std::cout << "\n" << spacesToCenter(getVisualLength(message)) << message;
}

void Appearance::displayArtPanel(const std::vector<std::string>& asciiArt, int widthVisual, Color artColor, const std::string& titleSecondary, bool animateFadeIn) {
    displayPanel(titleSecondary, artColor, asciiArt, widthVisual, {}, Color::RESET, animateFadeIn);
}
