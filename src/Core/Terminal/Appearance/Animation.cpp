#include "Core/Terminal/Appearance/Appearance.h"
#include "Core/Utils/InputControl.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <thread>
#include <chrono>

void Appearance::animateFadeIn(int framesTotals, int timeByFrameMs, const std::function<void(int frame, int intensity)>& renderFrame) {
    for (int frame = 1; frame <= framesTotals; ++frame) {
        auto homeFrame = std::chrono::steady_clock::now();

        int intensity = (255 * frame) / framesTotals;
        renderFrame(frame, intensity);

        auto endFrame = std::chrono::steady_clock::now();
        auto durationFrame = std::chrono::duration_cast<std::chrono::milliseconds>(endFrame - homeFrame).count();
        int timeWait = std::max(0, timeByFrameMs - static_cast<int>(durationFrame));
        for (int i = 0; i < timeWait; i += 10) {
            if (InputControl::pressedKey()) {
                InputControl::clearBuffer();
                int endIntensity = 255;
                renderFrame(framesTotals, endIntensity);
                return;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(std::min(10, timeWait - i)));
        }
    }
}

void Appearance::displayScreenIntro(const std::vector<std::string>& logoArt, const std::vector<std::string>& textNarration, Color themeColor) {
    int widthConsole = getTerminalWidth();

    int artWidth = 0;
    for (const auto& line : logoArt) {
        int comp = getVisualLength(line);
        if (comp > artWidth) artWidth = comp;
    }
    int spacesArt = std::max(0, (widthConsole - artWidth) / 2);
    std::string marginArt(spacesArt, ' ');

    int blankLinesBeforeText = 3;

    hideCursor();
    clearScreen();

    animateFadeIn(30, 30, [&](int /*frame*/, int intensity) {
        std::ostringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
        
        std::string colorRGB = getColorRGBFade(themeColor, intensity);
        
        buffer << "\n\n";
        for (const auto& line : logoArt) {
            buffer << marginArt << colorRGB << line << "\033[0m\n";
        }
        
        buffer << "\033[J";
        
        std::cout.rdbuf(oldCout);
        std::cout << "\033[H" << buffer.str() << std::flush;
    });

    for(int i = 0; i < blankLinesBeforeText; ++i) std::cout << "\n";

    printBlockCentralizedTyping(textNarration);
    
    std::cout << "\n";
    std::string pressEnter = "[ PRESSIONE ENTER PARA INICIAR ]";
    printCentralized(pressEnter, color(Color::GRAY));
    
    InputControl::waitForEnter();

    clearScreen();
}

void Appearance::printBlockCentralizedTyping(const std::vector<std::string>& lines, int delayMs) {
    int longestLineSize = 0;
    for (const std::string& line : lines) {
        size_t end = line.find_last_not_of(' ');
        std::string trimmed = (end != std::string::npos) ? line.substr(0, end + 1) : "";
        longestLineSize = std::max(longestLineSize, getVisualLength(trimmed));
    }
    std::string margin = spacesToCenter(longestLineSize);
    for (const std::string& line : lines) {
        printTyping(margin + line, delayMs, true);
    }
}

void Appearance::printCentralizedTyping(const std::string& text, int delayMs) {
    std::string margin = spacesToCenter(getVisualLength(text));
    printTyping(margin + text, delayMs, true);
}

void Appearance::printTyping(const std::string& text, int delayMs, bool addNewline) {
    int lineMsg = Appearance::getTerminalHeight() - 1;
    if (lineMsg < 1) lineMsg = 1;
    std::cout << "\033[s\033[" << lineMsg << ";1H" << color(Color::BOLD, Color::GRAY) << "[Pressione ENTER para pular]" << color(Color::RESET) << "\033[u";

    size_t i = 0;
    while (i < text.length() && text[i] == ' ') {
        std::cout << text[i];
        i++;
    }
    std::cout << std::flush;

    for (; i < text.length(); ++i) {
        if (text[i] == '\033') {
            size_t startAnsi = i;
            if (i + 1 < text.length() && text[i+1] == '[') {
                i += 2;
                while (i < text.length() && !(text[i] >= 0x40 && text[i] <= 0x7E)) {
                    i++;
                }
                std::cout << text.substr(startAnsi, i - startAnsi + 1) << std::flush;
                continue;
            }
        }

        if (InputControl::pressedKey()) { 
            char key = InputControl::readKey(); 
            if (key == '\r' || key == '\n') { 
                std::cout << "\033[s\033[" << lineMsg << ";1H\033[K\033[u" << text.substr(i) << std::flush; 
                if (addNewline) {
                    std::cout << std::endl;
                }
                return;
            } 
        }
        std::cout << text[i] << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }

    std::cout << "\033[s\033[" << lineMsg << ";1H\033[K\033[u" << std::flush;

    if (addNewline) {
        std::cout << std::endl;
    }
}

void Appearance::printVectorExcited(const std::vector<std::string>& lines, int delayMs) {
    for (const std::string& line : lines) {
        std::cout << line << "\n";
        std::cout << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }
}

void Appearance::cheerTransitionScene3D(
    const std::vector<std::string>& soonBase,
    const std::string& colorEndSoon,
    const std::vector<std::string>& backgroundFree,
    std::function<std::vector<std::string>(float)> getBackgroundEnd,
    int targetY
) {
    int terminalWidth = getTerminalWidth();
    int height3D = static_cast<int>(backgroundFree.size());
    int soonHeight = static_cast<int>(soonBase.size());
    
    int soonWidth = 0;
    for(const auto& l : soonBase) {
        soonWidth = std::max(soonWidth, getVisualLength(l));
    }
    int soonX = (terminalWidth - soonWidth) / 2;
    if (soonX < 0) soonX = 0;

    std::vector<std::vector<std::string>> decomposedSoon(soonHeight);
    for (int i = 0; i < soonHeight; i++) {
        decomposedSoon[i] = Appearance::splitUtf8(soonBase[i]);
    }

    std::vector<std::string> sceneBase = getBackgroundEnd(1.0f);
    {
        std::ostringstream buffer;
        buffer << "\033[H";
        for (int y = 0; y < height3D; y++) {
            buffer << sceneBase[y];
            if (y < height3D - 1) buffer << "\n";
        }
        std::cout << buffer.str() << std::flush;
    }

    Appearance::animateFadeIn(30, 40, [&](int frame, int /*intensidade*/) {
        float opacity = frame / 30.0f;
        
        std::string fadedColorSoon = colorEndSoon;
        if (colorEndSoon == "\033[1;31m") {
            fadedColorSoon = "\033[38;2;" + std::to_string((int)(255 * opacity)) + ";0;0m";
        } else if (colorEndSoon == "\033[1;32m") {
            fadedColorSoon = "\033[38;2;0;" + std::to_string((int)(255 * opacity)) + ";0m";
        } else if (colorEndSoon == "\033[1;37m" || colorEndSoon == "\033[37m") {
            int c = (int)(255 * opacity);
            fadedColorSoon = "\033[38;2;" + std::to_string(c) + ";" + std::to_string(c) + ";" + std::to_string(c) + "m";
        }
        
        std::ostringstream buffer;
        for (int y = targetY; y < targetY + soonHeight && y < height3D; y++) {
            int soonRowIdx = y - targetY;
            std::string lineSoon = superimposeSoonAnsi(sceneBase[y], decomposedSoon[soonRowIdx], soonX, fadedColorSoon, terminalWidth);
            buffer << "\033[" << (y + 1) << ";1H" << lineSoon << "\033[K";
        }
        std::cout << buffer.str() << std::flush;
    });

}
