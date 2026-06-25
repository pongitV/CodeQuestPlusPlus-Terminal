#include "RaycasterSprites.h"
#include <vector>
#include <string>

SpriteCache RaycasterSprites::parseArte(const std::vector<std::string>& raw) {
    SpriteCache sc;
    sc.height = raw.size();
    sc.width = 0;
    sc.pixels.resize(sc.height);
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
            if (pixelChar == " " || pixelChar == "") sc.pixels[i].push_back(""); // Transparencia
            else {
                std::string bgAnsi = currentAnsi;
                size_t pos = 0;
                while ((pos = bgAnsi.find("\033[3", pos)) != std::string::npos) {
                    bgAnsi.replace(pos, 3, "\033[4"); // Converte cor de Frente (Foreground) para cor de Fundo (Background)
                    pos += 3;
                }
                pos = 0;
                while ((pos = bgAnsi.find("\033[1;3", pos)) != std::string::npos) {
                    bgAnsi.replace(pos, 5, "\033[1;4"); // Converte cor de Frente (Negrito) para cor de Fundo (Background)
                    pos += 5;
                }
                sc.pixels[i].push_back(bgAnsi + " \033[0m"); // Substitui o simbolo ASCII por um espaco em branco solido ("blank")
            }
            j += len;
        }
        if((int)sc.pixels[i].size() > sc.width) sc.width = sc.pixels[i].size();
    }
    return sc;
}

SpriteCache RaycasterSprites::parseSprite(const std::vector<std::string>& raw, int r, int g, int b, bool isMahoraga) {
    SpriteCache sc;
    sc.height = raw.size();
    sc.width = 0;
    
    int minX = 999999, maxX = -1;
    
    // Auto-Crop
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
    sc.pixels.resize(sc.height);
    
    // Texturizacao com base no Caractere
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
                if (pixelChar == " " || pixelChar == "") {
                    sc.pixels[i].push_back("");
                } else {
                    char c = pixelChar[0];
                    int currentBaseR = r;
                    int currentBaseG = g;
                    int currentBaseB = b;
                    if (isMahoraga && i < 24) {
                        currentBaseR = 255;
                        currentBaseG = 215;
                        currentBaseB = 0; // Amarelo/Dourado para a roda
                    }
                    int rMod = currentBaseR, gMod = currentBaseG, bMod = currentBaseB;
                    if (c == '@' || c == 'M' || c == 'W' || c == '#' || c == '&' || c == '8') { rMod = currentBaseR * 0.4; gMod = currentBaseG * 0.4; bMod = currentBaseB * 0.4; } // Sombra Profunda
                    else if (c == '%' || c == 'O' || c == 'X' || c == 'S' || c == 'Q') { rMod = currentBaseR * 0.6; gMod = currentBaseG * 0.6; bMod = currentBaseB * 0.6; } // Sombra
                    else if (c == '*' || c == '+' || c == 'x' || c == 'o' || c == '=' || c == 'H') { rMod = currentBaseR * 0.8; gMod = currentBaseG * 0.8; bMod = currentBaseB * 0.8; } // Cor Base
                    else if (c == '-' || c == '~' || c == ':' || c == ';') { rMod = std::min(255, (int)(currentBaseR * 1.2)); gMod = std::min(255, (int)(currentBaseG * 1.2)); bMod = std::min(255, (int)(currentBaseB * 1.2)); } // Iluminado
                    else if (c == '.' || c == ',' || c == '\'') { rMod = std::min(255, (int)(currentBaseR * 1.5)); gMod = std::min(255, (int)(currentBaseG * 1.5)); bMod = std::min(255, (int)(currentBaseB * 1.5)); } // Brilho Forte
                    else if (c == '_' || c == '|' || c == '\\' || c == '/' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || c == '<' || c == '>') { rMod = currentBaseR * 0.5; gMod = currentBaseG * 0.5; bMod = currentBaseB * 0.5; } // Contornos internos
                    
                    sc.pixels[i].push_back("\033[48;2;" + std::to_string(rMod) + ";" + std::to_string(gMod) + ";" + std::to_string(bMod) + "m \033[0m");
                }
            }
            j_char++;
            j += len;
        }
        
        while (j_char <= maxX) {
            if (j_char >= minX) sc.pixels[i].push_back("");
            j_char++;
        }
    }
    
    SpriteCache finalSc;
    finalSc.width = sc.width;
    finalSc.height = sc.height;
    finalSc.pixels.resize(sc.height);
    
    for (int y = 0; y < sc.height; ++y) {
        for (int x = 0; x < sc.width; ++x) {
            if (sc.pixels[y][x] == "") {
                finalSc.pixels[y].push_back("");
            } else {
                bool isEdge = false;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) continue;
                        int ny = y + dy;
                        int nx = x + dx;
                        if (ny < 0 || ny >= sc.height || nx < 0 || nx >= sc.width) isEdge = true;
                        else if (sc.pixels[ny][nx] == "") isEdge = true;
                    }
                }
                if (isEdge) finalSc.pixels[y].push_back("\033[48;2;0;0;0m \033[0m");
                else finalSc.pixels[y].push_back(sc.pixels[y][x]);
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
