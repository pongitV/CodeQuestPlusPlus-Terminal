#include "Aparencia.h"
#include <string>
#include <sstream>

std::string Aparencia::sobreporPainelNaLinhaAnsi(const std::string& backgroundLine, const std::string& panelLine, int startX) {
    std::string result = "";
    result.reserve(backgroundLine.size() + panelLine.size() + 50);

    int panelWidth = Aparencia::obterComprimentoVisual(panelLine);
    int endX = startX + panelWidth;

    int visualX = 0;
    size_t i = 0;

    std::string currentBg = "";
    std::string currentFg = "";

    while (i < backgroundLine.size() && visualX < startX) {
        if (backgroundLine[i] == '\033' && i + 1 < backgroundLine.size() && backgroundLine[i+1] == '[') {
            size_t end = backgroundLine.find('m', i);
            if (end != std::string::npos) {
                std::string esc = backgroundLine.substr(i, end - i + 1);
                if (esc == "\033[0m") {
                    currentBg = "";
                    currentFg = "";
                } else if (esc.find("\033[48;2;") == 0 || esc == "\033[49m") {
                    currentBg = (esc == "\033[49m") ? "" : esc;
                } else if (esc.find("\033[38;2;") == 0 || esc == "\033[39m" || esc == "\033[1;37m" || esc == "\033[1;31m") {
                    currentFg = (esc == "\033[39m") ? "" : esc;
                }
                result += esc;
                i = end + 1;
                continue;
            }
        }

        int len = 1;
        unsigned char c = static_cast<unsigned char>(backgroundLine[i]);
        if ((c & 0x80) == 0) len = 1;
        else if ((c & 0xE0) == 0xC0) len = 2;
        else if ((c & 0xF0) == 0xE0) len = 3;
        else if ((c & 0xF8) == 0xF0) len = 4;

        result += backgroundLine.substr(i, len);
        visualX++;
        i += len;
    }

    while (visualX < startX) {
        result += " ";
        visualX++;
    }

    result += panelLine;

    while (i < backgroundLine.size() && visualX < endX) {
        if (backgroundLine[i] == '\033' && i + 1 < backgroundLine.size() && backgroundLine[i+1] == '[') {
            size_t end = backgroundLine.find('m', i);
            if (end != std::string::npos) {
                std::string esc = backgroundLine.substr(i, end - i + 1);
                if (esc == "\033[0m") {
                    currentBg = "";
                    currentFg = "";
                } else if (esc.find("\033[48;2;") == 0 || esc == "\033[49m") {
                    currentBg = (esc == "\033[49m") ? "" : esc;
                } else if (esc.find("\033[38;2;") == 0 || esc == "\033[39m" || esc == "\033[1;37m" || esc == "\033[1;31m") {
                    currentFg = (esc == "\033[39m") ? "" : esc;
                }
                i = end + 1;
                continue;
            }
        }

        int len = 1;
        unsigned char c = static_cast<unsigned char>(backgroundLine[i]);
        if ((c & 0x80) == 0) len = 1;
        else if ((c & 0xE0) == 0xC0) len = 2;
        else if ((c & 0xF0) == 0xE0) len = 3;
        else if ((c & 0xF8) == 0xF0) len = 4;

        visualX++;
        i += len;
    }

    result += "\033[0m" + currentBg + currentFg;

    if (i < backgroundLine.size()) {
        result += backgroundLine.substr(i);
    }

    return result;
}

std::string Aparencia::sobreporLogoAnsi(const std::string& backgroundLine, const std::vector<std::string>& logoChars, int startX, const std::string& fgColor, int larguraTerminal) {
    std::string result = "";
    result.reserve(backgroundLine.size() + 200);

    std::string currentBg = "";
    std::string currentFg = "";

    int visualX = 0;
    size_t i = 0;

    while (i < backgroundLine.size() && visualX < larguraTerminal) {
        if (backgroundLine[i] == '\033' && i + 1 < backgroundLine.size() && backgroundLine[i+1] == '[') {
            size_t end = backgroundLine.find('m', i);
            if (end != std::string::npos) {
                std::string esc = backgroundLine.substr(i, end - i + 1);
                if (esc == "\033[0m") {
                    currentBg = "";
                    currentFg = "";
                } else if (esc.find("\033[48;2;") == 0 || esc == "\033[49m") {
                    currentBg = (esc == "\033[49m") ? "" : esc;
                } else if (esc.find("\033[38;2;") == 0 || esc == "\033[39m" || esc == "\033[1;37m" || esc == "\033[1;31m") {
                    currentFg = (esc == "\033[39m") ? "" : esc;
                }
                result += esc;
                i = end + 1;
                continue;
            }
        }

        int len = 1;
        unsigned char c = static_cast<unsigned char>(backgroundLine[i]);
        if ((c & 0x80) == 0) len = 1;
        else if ((c & 0xE0) == 0xC0) len = 2;
        else if ((c & 0xF0) == 0xE0) len = 3;
        else if ((c & 0xF8) == 0xF0) len = 4;

        std::string charStr = backgroundLine.substr(i, len);

        int logoCol = visualX - startX;
        if (logoCol >= 0 && logoCol < static_cast<int>(logoChars.size()) && logoChars[logoCol] != " ") {
            result += "\033[0m" + currentBg + fgColor + logoChars[logoCol] + "\033[0m" + currentBg + currentFg;
        } else {
            result += charStr;
        }

        visualX++;
        i += len;
    }
    return result;
}
