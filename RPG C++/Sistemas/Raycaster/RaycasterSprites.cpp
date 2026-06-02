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
            else sc.pixels[i].push_back(currentAnsi + pixelChar + "\033[0m");
            j += len;
        }
        if((int)sc.pixels[i].size() > sc.width) sc.width = sc.pixels[i].size();
    }
    return sc;
}

std::vector<std::string> RaycasterSprites::colorirArte(const std::vector<std::string>& arte, const std::string& corAnsi) {
    std::vector<std::string> arteColorida;
    for (const auto& linha : arte) arteColorida.push_back(corAnsi + linha + "\033[0m");
    return arteColorida;
}
