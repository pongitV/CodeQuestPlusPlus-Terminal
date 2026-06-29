#include "RaycasterSprites.h"
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

static void parseAnsiColors(const std::string& ansi, SpritePixel& pixel) {
    size_t i = 0;
    while (i < ansi.size()) {
        if (ansi[i] == '\033' && i + 1 < ansi.size() && ansi[i+1] == '[') {
            i += 2;
            size_t start = i;
            while (i < ansi.size() && ansi[i] != 'm') {
                i++;
            }
            if (i < ansi.size()) { // Found 'm'
                std::string content = ansi.substr(start, i - start);
                size_t p = 0;
                while (p < content.size()) {
                    if (p + 4 < content.size() && content.substr(p, 5) == "48;2;") {
                        pixel.hasBg = true;
                        p += 5;
                        size_t next = content.find(';', p);
                        if (next != std::string::npos) {
                            try {
                                pixel.r = std::stoi(content.substr(p, next - p));
                            } catch(...) { pixel.r = 0; }
                            p = next + 1;
                            next = content.find(';', p);
                            if (next != std::string::npos) {
                                try {
                                    pixel.g = std::stoi(content.substr(p, next - p));
                                } catch(...) { pixel.g = 0; }
                                p = next + 1;
                                size_t nextB = content.find(';', p);
                                if (nextB == std::string::npos) nextB = content.size();
                                try {
                                    pixel.b = std::stoi(content.substr(p, nextB - p));
                                } catch(...) { pixel.b = 0; }
                                p = nextB;
                            }
                        }
                    } else if (p + 4 < content.size() && content.substr(p, 5) == "38;2;") {
                        pixel.hasFg = true;
                        p += 5;
                        size_t next = content.find(';', p);
                        if (next != std::string::npos) {
                            try {
                                pixel.fgR = std::stoi(content.substr(p, next - p));
                            } catch(...) { pixel.fgR = 0; }
                            p = next + 1;
                            next = content.find(';', p);
                            if (next != std::string::npos) {
                                try {
                                    pixel.fgG = std::stoi(content.substr(p, next - p));
                                } catch(...) { pixel.fgG = 0; }
                                p = next + 1;
                                size_t nextB = content.find(';', p);
                                if (nextB == std::string::npos) nextB = content.size();
                                try {
                                    pixel.fgB = std::stoi(content.substr(p, nextB - p));
                                } catch(...) { pixel.fgB = 0; }
                                p = nextB;
                            }
                        }
                    } else {
                        // Parse standard code
                        int val = 0;
                        size_t endCode = content.find(';', p);
                        if (endCode == std::string::npos) endCode = content.size();
                        try {
                            val = std::stoi(content.substr(p, endCode - p));
                        } catch(...) { val = -1; }
                        p = endCode + 1;

                        if (val == 0) {
                            pixel.hasBg = false;
                            pixel.hasFg = false;
                        } else if (val >= 30 && val <= 37) {
                            pixel.hasFg = true;
                            uint8_t base = 180;
                            pixel.fgR = (val == 31 || val == 33 || val == 35 || val == 37) ? base : 0;
                            pixel.fgG = (val == 32 || val == 33 || val == 36 || val == 37) ? base : 0;
                            pixel.fgB = (val == 34 || val == 35 || val == 36 || val == 37) ? base : 0;
                        } else if (val >= 40 && val <= 47) {
                            pixel.hasBg = true;
                            uint8_t base = 180;
                            pixel.r = (val == 41 || val == 43 || val == 45 || val == 47) ? base : 0;
                            pixel.g = (val == 42 || val == 43 || val == 46 || val == 47) ? base : 0;
                            pixel.b = (val == 44 || val == 45 || val == 46 || val == 47) ? base : 0;
                        } else if (val >= 90 && val <= 97) {
                            pixel.hasFg = true;
                            uint8_t base = 255;
                            pixel.fgR = (val == 91 || val == 93 || val == 95 || val == 97) ? base : 0;
                            pixel.fgG = (val == 92 || val == 93 || val == 96 || val == 97) ? base : 0;
                            pixel.fgB = (val == 94 || val == 95 || val == 96 || val == 97) ? base : 0;
                        } else if (val >= 100 && val <= 107) {
                            pixel.hasBg = true;
                            uint8_t base = 255;
                            pixel.r = (val == 101 || val == 103 || val == 105 || val == 107) ? base : 0;
                            pixel.g = (val == 102 || val == 103 || val == 106 || val == 107) ? base : 0;
                            pixel.b = (val == 104 || val == 105 || val == 106 || val == 107) ? base : 0;
                        }
                    }
                }
                i++; // Skip 'm'
            }
        } else {
            i++;
        }
    }
}

SpriteCache RaycasterSprites::parseArte(const std::vector<std::string>& raw) {
    SpriteCache sc;
    sc.height = raw.size();
    sc.width = 0;
    std::vector<std::vector<SpritePixel>> tempPixels;
    tempPixels.resize(sc.height);
    for(int i=0; i<sc.height; ++i) {
        std::string currentAnsi = "";
        std::string line = raw[i];
        for(size_t j=0; j<line.length(); ) {
            if(line[j] == '\033') {
                size_t end = line.find('m', j);
                if(end != std::string::npos) {
                    std::string code = line.substr(j, end - j + 1);
                    if(code == "\033[0m") currentAnsi = ""; 
                    else currentAnsi += code; 
                    j = end + 1;
                    continue;
                }
            }
            int len = 1;
            unsigned char c = line[j];
            if ((c & 0x80) == 0) len = 1;
            else if ((c & 0xE0) == 0xC0) len = 2;
            else if ((c & 0xF0) == 0xE0) len = 3;
            else if ((c & 0xF8) == 0xF0) len = 4;
            
            std::string pixelChar = line.substr(j, len);
            SpritePixel sp;
            if (pixelChar == " " || pixelChar == "") {
                sp.isTransparente = true;
            } else {
                sp.isTransparente = false;
                parseAnsiColors(currentAnsi, sp);
                if (sp.hasFg && !sp.hasBg) {
                    sp.r = sp.fgR;
                    sp.g = sp.fgG;
                    sp.b = sp.fgB;
                    sp.hasBg = true;
                }
                sp.ch = ' ';
            }
            tempPixels[i].push_back(sp);
            j += len;
        }
        if((int)tempPixels[i].size() > sc.width) sc.width = tempPixels[i].size();
    }
    
    sc.pixels.resize(sc.width * sc.height);
    for (int y = 0; y < sc.height; ++y) {
        for (int x = 0; x < sc.width; ++x) {
            if (x < (int)tempPixels[y].size()) {
                sc.pixels[y * sc.width + x] = tempPixels[y][x];
            } else {
                SpritePixel blank;
                blank.isTransparente = true;
                sc.pixels[y * sc.width + x] = blank;
            }
        }
    }
    
    return sc;
}

SpriteCache RaycasterSprites::parseSprite(const std::vector<std::string>& raw, int r, int g, int b, bool isMahoraga) {
    SpriteCache sc;
    sc.height = raw.size();
    sc.width = 0;
    
    int minX = 999999, maxX = -1;
    
    for (int i = 0; i < sc.height; ++i) {
        std::string line = raw[i];
        int j_char = 0;
        for (size_t j = 0; j < line.length(); ) {
            if (line[j] == '\033') {
                size_t end = line.find('m', j);
                if (end != std::string::npos) { j = end + 1; continue; }
            }
            int len = 1;
            unsigned char c = line[j];
            if ((c & 0x80) == 0) len = 1;
            else if ((c & 0xE0) == 0xC0) len = 2;
            else if ((c & 0xF0) == 0xE0) len = 3;
            else if ((c & 0xF8) == 0xF0) len = 4;
            
            std::string pixelChar = line.substr(j, len);
            if (pixelChar != " " && pixelChar != "") {
                if (j_char < minX) minX = j_char;
                if (j_char > maxX) maxX = j_char;
            }
            j_char++;
            j += len;
        }
        if (j_char > sc.width) sc.width = j_char;
    }
    
    if (minX > maxX) { minX = 0; maxX = sc.width - 1; }
    
    sc.width = maxX - minX + 1;
    std::vector<std::vector<SpritePixel>> tempPixels;
    tempPixels.resize(sc.height);
    
    for (int i = 0; i < sc.height; ++i) {
        std::string line = raw[i];
        int j_char = 0;
        for (size_t j = 0; j < line.length(); ) {
            if (line[j] == '\033') {
                size_t end = line.find('m', j);
                if (end != std::string::npos) { j = end + 1; continue; }
            }
            int len = 1;
            unsigned char c_utf = line[j];
            if ((c_utf & 0x80) == 0) len = 1;
            else if ((c_utf & 0xE0) == 0xC0) len = 2;
            else if ((c_utf & 0xF0) == 0xE0) len = 3;
            else if ((c_utf & 0xF8) == 0xF0) len = 4;
            
            std::string pixelChar = line.substr(j, len);
            
            if (j_char >= minX && j_char <= maxX) {
                SpritePixel sp;
                if (pixelChar == " " || pixelChar == "") {
                    sp.isTransparente = true;
                } else {
                    char c = pixelChar[0];
                    int currentBaseR = r;
                    int currentBaseG = g;
                    int currentBaseB = b;
                    if (isMahoraga && i < 24) {
                        currentBaseR = 255;
                        currentBaseG = 215;
                        currentBaseB = 0;
                    }
                    int rMod = currentBaseR, gMod = currentBaseG, bMod = currentBaseB;
                    if (c == '@' || c == 'M' || c == 'W' || c == '#' || c == '&' || c == '8') { rMod = currentBaseR * 0.4; gMod = currentBaseG * 0.4; bMod = currentBaseB * 0.4; }
                    else if (c == '%' || c == 'O' || c == 'X' || c == 'S' || c == 'Q') { rMod = currentBaseR * 0.6; gMod = currentBaseG * 0.6; bMod = currentBaseB * 0.6; }
                    else if (c == '*' || c == '+' || c == 'x' || c == 'o' || c == '=' || c == 'H') { rMod = currentBaseR * 0.8; gMod = currentBaseG * 0.8; bMod = currentBaseB * 0.8; }
                    else if (c == '-' || c == '~' || c == ':' || c == ';') { rMod = std::min(255, (int)(currentBaseR * 1.2)); gMod = std::min(255, (int)(currentBaseG * 1.2)); bMod = std::min(255, (int)(currentBaseB * 1.2)); }
                    else if (c == '.' || c == ',' || c == '\'') { rMod = std::min(255, (int)(currentBaseR * 1.5)); gMod = std::min(255, (int)(currentBaseG * 1.5)); bMod = std::min(255, (int)(currentBaseB * 1.5)); }
                    else if (c == '_' || c == '|' || c == '\\' || c == '/' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || c == '<' || c == '>') { rMod = currentBaseR * 0.5; gMod = currentBaseG * 0.5; bMod = currentBaseB * 0.5; }
                    
                    sp.isTransparente = false;
                    sp.r = rMod;
                    sp.g = gMod;
                    sp.b = bMod;
                    sp.ch = ' ';
                    sp.hasBg = true;
                }
                tempPixels[i].push_back(sp);
            }
            j_char++;
            j += len;
        }
        
        while (j_char <= maxX) {
            if (j_char >= minX) {
                SpritePixel sp;
                sp.isTransparente = true;
                tempPixels[i].push_back(sp);
            }
            j_char++;
        }
    }
    
    // Compressão de resolução (downsampling) para melhorar o desempenho
    std::vector<std::vector<SpritePixel>> compressedTempPixels;
    int scale = 4; // Fator de compressão menor para equilibrar performance e qualidade visual
    int compWidth = std::max(1, sc.width / scale);
    int compHeight = std::max(1, sc.height / scale);
    compressedTempPixels.resize(compHeight);
    
    for (int y = 0; y < compHeight; ++y) {
        for (int x = 0; x < compWidth; ++x) {
            SpritePixel bestPixel;
            bestPixel.isTransparente = true;
            for(int dy = 0; dy < scale; ++dy) {
                for(int dx = 0; dx < scale; ++dx) {
                    int oy = y * scale + dy;
                    int ox = x * scale + dx;
                    if (oy < sc.height && ox < sc.width) {
                        if (!tempPixels[oy][ox].isTransparente) {
                            bestPixel = tempPixels[oy][ox];
                            goto foundSprite;
                        }
                    }
                }
            }
            foundSprite:
            compressedTempPixels[y].push_back(bestPixel);
        }
    }
    
    SpriteCache finalSc;
    finalSc.width = compWidth;
    finalSc.height = compHeight;
    finalSc.pixels.resize(compWidth * compHeight);
    
    for (int y = 0; y < compHeight; ++y) {
        for (int x = 0; x < compWidth; ++x) {
            if (compressedTempPixels[y][x].isTransparente) {
                SpritePixel sp;
                sp.isTransparente = true;
                finalSc.pixels[y * compWidth + x] = sp;
            } else {
                bool isEdge = false;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) continue;
                        int ny = y + dy;
                        int nx = x + dx;
                        if (ny < 0 || ny >= compHeight || nx < 0 || nx >= compWidth) isEdge = true;
                        else if (compressedTempPixels[ny][nx].isTransparente) isEdge = true;
                    }
                }
                if (isEdge) {
                    SpritePixel sp;
                    sp.isTransparente = false;
                    sp.r = 0; sp.g = 0; sp.b = 0;
                    sp.ch = ' ';
                    sp.hasBg = true;
                    finalSc.pixels[y * compWidth + x] = sp;
                } else {
                    finalSc.pixels[y * compWidth + x] = compressedTempPixels[y][x];
                }
            }
        }
    }
    
    return finalSc;
}

std::vector<std::string> RaycasterSprites::colorirArte(const std::vector<std::string>& arte, const std::string& corAnsi) {
    std::vector<std::string> arteColorida;
    for (const auto& linha : arte) arteColorida.push_back(corAnsi + linha + "\033[0m");
    return arteColorida;
}
