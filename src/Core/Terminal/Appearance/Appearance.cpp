#include "Core/Terminal/Appearance/Appearance.h"
#include "UI/PerspectiveManager.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>

int Appearance::FACTOR_COMPRESSION_GLOBAL = 2;

int Appearance::typingDelayMS = 25;
Color Appearance::colorBackgroundActive = Color::RESET;
char Appearance::customPlayerIcon = '@';
Color Appearance::customPlayerColor = Color::GREEN;

std::vector<std::string> Appearance::loadArt(const std::string& pathFile) {
    std::vector<std::string> art;
    
    std::vector<std::string> pathsBase = {
        "",
        "../",
        "../../",
        "../../../"
    };

    std::ifstream file;
    std::string pathSuccess;
    for (const auto& base : pathsBase) {
        file.open(base + pathFile);
        if (file.is_open()) {
            pathSuccess = base + pathFile;
            break;
        }
    }

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            art.push_back(line);
        }
    } else {
        art.push_back("ERRO: Nao foi possivel carregar a arte.");
        art.push_back("Caminho procurado: " + pathFile);
    }
    return art;
}

std::vector<std::string> Appearance::reduceScaleAscii(const std::vector<std::string>& artOriginal, float factorX, float factorY) {
    std::vector<std::string> artReduced;
    if (factorX <= 0.0f) factorX = 1.0f;
    if (factorY <= 0.0f) factorY = 1.0f;

    for (float i = 0; i < artOriginal.size(); i += factorY) {
        std::string newLine = "";
        int intI = static_cast<int>(i);
        if (intI >= (int)artOriginal.size()) break;
        for (float j = 0; j < artOriginal[intI].length(); j += factorX) {
            int intJ = static_cast<int>(j);
            if (intJ < (int)artOriginal[intI].length()) {
                newLine += artOriginal[intI][intJ];
            }
        }
        artReduced.push_back(newLine);
    }
    return artReduced;
}

void Appearance::sortAlphabetically(std::vector<std::string>& list) {
    std::sort(list.begin(), list.end());
}
