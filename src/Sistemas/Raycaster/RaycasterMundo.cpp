#include "RaycasterMundo.h"
#include <algorithm>
#include <cctype>
#include <cmath>

struct MapFlags {
    std::string tituloUpper;
    bool isReino = false;
    bool isCaverna = false;
    bool isLabirinto = false;
    bool isSalaChefe = false;
    bool isSpawn = false;
    bool isTerra = false;
    int temaCeu = 0;
};

static const MapFlags& obterFlagsMapa(const std::string& tituloMapa) {
    static thread_local std::string lastTitulo;
    static thread_local MapFlags flags;
    if (tituloMapa != lastTitulo) {
        lastTitulo = tituloMapa;
        std::string upper = tituloMapa;
        for (char& ch : upper) ch = std::toupper(static_cast<unsigned char>(ch));
        flags.tituloUpper = upper;
        flags.isReino = (upper.find("CASTELO") != std::string::npos || upper.find("REINO") != std::string::npos);
        flags.isCaverna = (upper.find("CAVERNA") != std::string::npos || upper.find("CORACAO") != std::string::npos);
        flags.isLabirinto = (upper.find("LABIRINTO") != std::string::npos);
        flags.isSalaChefe = (upper.find("CHEFE") != std::string::npos);
        flags.isSpawn = (upper.find("INICIO") != std::string::npos);
        flags.isTerra = (upper.find("FLORESTA") != std::string::npos || 
                         upper.find("BOSQUE") != std::string::npos ||
                         upper.find("VILA") != std::string::npos ||
                         upper.find("INICIO") != std::string::npos);
        
        // temaCeu logic
        if (flags.isCaverna || flags.isLabirinto || flags.isSalaChefe) {
            flags.temaCeu = 0;
        } else if (upper.find("FLORESTA") != std::string::npos || upper.find("BOSQUE") != std::string::npos) {
            flags.temaCeu = 1;
        } else {
            flags.temaCeu = 2;
        }
    }
    return flags;
}

static std::string aplicarNevoa(int r, int g, int b, float distancia, float profundidadeMaxima, int temaCeu, const std::vector<std::tuple<int, int, int>>& luzes, float hitX, float hitY, bool escurecer = false) {
    float nevoaPercent = distancia / (profundidadeMaxima * 0.8f);
    if (nevoaPercent < 0.0f) nevoaPercent = 0.0f;
    if (nevoaPercent > 1.0f) nevoaPercent = 1.0f;
    nevoaPercent = nevoaPercent * nevoaPercent; 

    int fogR = 0, fogG = 0, fogB = 0;
    if (temaCeu == 1) { 
        fogR = 5; fogG = 5; fogB = 15;
    } else if (temaCeu == 2) { 
        fogR = 10; fogG = 60; fogB = 150;
    }
    
    float luzR = 0, luzG = 0, luzB = 0;
    for (const auto& luz : luzes) {
        float dx = hitX - (std::get<0>(luz) + 0.5f);
        float dy = hitY - (std::get<1>(luz) + 0.5f);
        float distLuzSq = dx*dx + dy*dy;
        int tipoLuz = std::get<2>(luz);
        float maxDist = (tipoLuz == 1) ? 6.0f : 5.0f; // White aura slightly larger
        float maxDistSq = maxDist * maxDist;
        if (distLuzSq < maxDistSq) {
            float distLuz = std::sqrt(distLuzSq);
            float intensity = 1.0f - (distLuz / maxDist);
            intensity = intensity * intensity; 
            if (tipoLuz == 0) { // Orange
                luzR += 220 * intensity;
                luzG += 120 * intensity;
                luzB += 30 * intensity;
            } else if (tipoLuz == 1) { // White Aura
                luzR += 180 * intensity;
                luzG += 220 * intensity;
                luzB += 255 * intensity;
            }
        }
    }
    
    int finalR = r + (int)luzR;
    int finalG = g + (int)luzG;
    int finalB = b + (int)luzB;
    
    finalR = finalR + (fogR - finalR) * nevoaPercent;
    finalG = finalG + (fogG - finalG) * nevoaPercent;
    finalB = finalB + (fogB - finalB) * nevoaPercent;
    
    if (escurecer) {
        finalR = (int)(finalR * 0.65f);
        finalG = (int)(finalG * 0.65f);
        finalB = (int)(finalB * 0.65f);
    }
    
    if (finalR > 255) finalR = 255;
    if (finalG > 255) finalG = 255;
    if (finalB > 255) finalB = 255;
    if (finalR < 0) finalR = 0;
    if (finalG < 0) finalG = 0;
    if (finalB < 0) finalB = 0;
    
    return "\033[48;2;" + std::to_string(finalR) + ";" + std::to_string(finalG) + ";" + std::to_string(finalB) + "m \033[0m";
}

bool RaycasterMundo::isTemaFloresta(const std::string& tituloMapa) {
    const auto& flags = obterFlagsMapa(tituloMapa);
    return (flags.tituloUpper.find("FLORESTA") != std::string::npos || flags.tituloUpper.find("BOSQUE") != std::string::npos);
}

bool RaycasterMundo::isEntity(char c) {
    return (c == 'G' || c == 'O' || c == 'B' || c == 'F' || c == 'S' || c == 'A' || c == 'M' || c == 'T' || c == 'H' || c == 'R' || c == 'P' || c == '^' || c == '*' || c == 'C');
}

bool RaycasterMundo::isTeleport(char c) { return c == '^'; }

bool RaycasterMundo::isWalkable(int mapX, int mapY, const std::vector<std::string>& matrizDoMapa) {
    char c = matrizDoMapa[mapY][mapX];
    if (c == '*') return false; // Impede atravessar arvores
    
    if (c == '=' || c == '|' || c == '\'' || c == '+') return false; // Paredes do labirinto

    if (c == '.' || c == ' ' || c == '^' || c == '~' || isEntity(c)) return true;
    if (isMapLabel(mapX, mapY, matrizDoMapa)) return true;
    return false;
}

bool RaycasterMundo::isMapLabel(int mapX, int mapY, const std::vector<std::string>& matrizDoMapa) {
    static thread_local std::vector<std::string> lastMap;
    static thread_local std::vector<std::vector<char>> cachedLabels;

    int height = matrizDoMapa.size();
    if (height == 0) return false;
    int width = matrizDoMapa[0].size();
    if (mapY < 0 || mapY >= height || mapX < 0 || mapX >= width) return false;

    if (matrizDoMapa != lastMap) {
        lastMap = matrizDoMapa;
        cachedLabels.assign(height, std::vector<char>(width, 2));
    }

    if (cachedLabels[mapY][mapX] != 2) {
        return cachedLabels[mapY][mapX] == 1;
    }

    char c = matrizDoMapa[mapY][mapX];
    bool result = false;
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
        if (isEntity(c)) {
            bool hasAdjacentText = false;
            for (int dx = -1; dx <= 1; dx += 2) {
                int nx = mapX + dx;
                if (mapY >= 0 && mapY < height && nx >= 0 && nx < (int)matrizDoMapa[mapY].size()) {
                    char adj = matrizDoMapa[mapY][nx];
                    if (adj == '^' || (adj >= 'a' && adj <= 'z')) {
                        hasAdjacentText = true;
                        break;
                    }
                }
            }
            if (!hasAdjacentText) {
                for (int dy = -1; dy <= 1; dy += 2) {
                    int ny = mapY + dy;
                    if (ny >= 0 && ny < height && mapX >= 0 && mapX < (int)matrizDoMapa[ny].size()) {
                        char adj = matrizDoMapa[ny][mapX];
                        if (adj == '^') {
                            hasAdjacentText = true;
                            break;
                        }
                    }
                }
            }
            result = hasAdjacentText;
        } else {
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = mapX + dx;
                    int ny = mapY + dy;
                    if (ny >= 0 && ny < height && nx >= 0 && nx < (int)matrizDoMapa[ny].size()) {
                        char adj = matrizDoMapa[ny][nx];
                        if (adj == '^' || (adj >= 'a' && adj <= 'z')) {
                            result = true;
                            break;
                        }
                    }
                }
                if (result) break;
            }
        }
    }
    cachedLabels[mapY][mapX] = result ? 1 : 0;
    return result;
}

std::string RaycasterMundo::obterPixelParedeInternal(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao, const std::vector<std::tuple<int, int, int>>& luzes, float hitX, float hitY, bool isSideWall) {
    const auto& flags = obterFlagsMapa(tituloMapa);
    int temaCeu = flags.temaCeu;
    int baseR=0, baseG=0, baseB=0;
    distanciaAteParede *= 0.55f;

    int alturaParede = chao - teto;
    
    float texY = 0.0f;
    if (alturaParede > 0) texY = (float)(y - teto) / (float)alturaParede;
    if (texY > 0.999f) texY = 0.999f;
    int tx = (int)(texX * 64.0f) % 64;
    int ty = (int)(texY * 64.0f) % 64;

    bool isReino = flags.isReino;

    bool isEstrutura = false;
    std::string charsEstrutura = "|_[]{}/\\<>;=-:+";
    if (charsEstrutura.find(charParede) != std::string::npos) isEstrutura = true;

    if (flags.isLabirinto) {
        bool isWoodBase = (ty > 54);
        bool isWoodenPillar = (tx % 32 < 4);
        bool isWoodenFrameX = (tx % 16 < 2);
        bool isWoodenFrameY = (ty % 16 < 2);

        if (isWoodBase || isWoodenPillar || isWoodenFrameX || isWoodenFrameY) {
            baseR = 70; baseG = 40; baseB = 20;
        } else {
            baseR = 235; baseG = 220; baseB = 190;
        }
    }

    if (isReino && (isEstrutura || charParede == '#')) {
        if (charParede == '|') {
            bool isTabua = (tx % 8 == 0); 
            if (isTabua) {
                baseR = 45; baseG = 25; baseB = 10;
            } else {
                bool hasGrain = ((tx * 3 + ty * 7) % 5) == 0;
                if (hasGrain) {
                    baseR = 80; baseG = 50; baseB = 20;
                } else {
                    baseR = 101; baseG = 67; baseB = 33;
                }
            }
        } else {
            bool isBattlementGap = (ty < 12 && (tx % 32) >= 16);
            if (isBattlementGap) {
                return "FUNDO";
            }
            bool isJunta = (ty % 4 == 0) || (((ty / 4) % 2 == 0) && tx % 8 == 0) || (((ty / 4) % 2 == 1) && (tx + 4) % 8 == 0); 
            if (isJunta) {
                baseR = 60; baseG = 60; baseB = 60;
            } else {
                bool hasGrain = ((tx * 7 + ty * 13) % 10) < 3;
                if (hasGrain) {
                    baseR = 100; baseG = 100; baseB = 100;
                } else {
                    baseR = 120; baseG = 120; baseB = 120;
                }
            }
        }
    } else if (isEstrutura) {
        if (temaFloresta) {
            bool isTabua = (tx % 8 == 0); 
            if (isTabua) {
                baseR = 45; baseG = 25; baseB = 10;
            } else {
                bool hasGrain = ((tx * 3 + ty * 7) % 5) == 0;
                if (hasGrain) {
                    baseR = 80; baseG = 50; baseB = 20;
                } else {
                    baseR = 101; baseG = 67; baseB = 33;
                }
            }
        } else {
            bool isJunta = (ty % 4 == 0) || (((ty / 4) % 2 == 0) && tx % 8 == 0) || (((ty / 4) % 2 == 1) && (tx + 4) % 8 == 0); 
            if (isJunta) {
                baseR = 120; baseG = 120; baseB = 120;
            } else {
                bool hasGrain = ((tx * 7 + ty * 13) % 10) < 3;
                if (hasGrain) {
                    baseR = 140; baseG = 50; baseB = 30;
                } else {
                    baseR = 160; baseG = 60; baseB = 40;
                }
            }
        }
    }

    if (!isReino && temaFloresta && charParede == '#') {
        int folhaTx = tx;
        int limiteFolhas = 28 + ((tx * 7) % 10);

        if (ty < limiteFolhas) {
            int animOffset = (int)(std::sin(tempoAnimacao * 1.5f + texX * 10.0f) * 4.0f);
            folhaTx = (tx + animOffset) % 64;
            if (folhaTx < 0) folhaTx += 64;
            
            bool sombraFolha = ((folhaTx * 7 + ty * 13) % 11) < 4; 
            if (sombraFolha) {
                baseR = 22; baseG = 89; baseB = 22;
            } else {
                baseR = 34; baseG = 139; baseB = 34;
            }
        } else {
            bool isBordaEscura = (tx < 6 || tx > 57);
            bool isSombra = (tx >= 6 && tx <= 12) || (tx >= 51 && tx <= 57);
            bool hasWoodGrain = ((tx * 3 + ty * 7) % 5) == 0;

            if (isBordaEscura) {
                baseR = 15; baseG = 10; baseB = 5;
            } else if (isSombra) {
                baseR = 40; baseG = 25; baseB = 10;
            } else {
                if (hasWoodGrain) {
                    baseR = 60; baseG = 40; baseB = 20;
                } else {
                    baseR = 80; baseG = 55; baseB = 25;
                }
            }
        }
    } else if (!isReino) {
        bool isSpawn = flags.isSpawn;
        bool isSalaChefe = flags.isSalaChefe;
        bool isCaverna = flags.isCaverna;
        
        if (isSpawn) {
            bool isJuntaBranca = (ty % 4 == 0) || (((ty / 4) % 2 == 0) && tx % 8 == 0) || (((ty / 4) % 2 == 1) && (tx + 4) % 8 == 0); 
            if (isJuntaBranca) {
                baseR = 140; baseG = 140; baseB = 140;
            } else {
                bool hasGrain = ((tx * 11 + ty * 17) % 10) < 3;
                if (hasGrain) {
                    baseR = 210; baseG = 210; baseB = 210;
                } else {
                    baseR = 240; baseG = 240; baseB = 240;
                }
            }
        } else if (isSalaChefe) {
            float cx = (tx - 32.0f);
            float cy = (ty - 32.0f);
            float dist = std::sqrt(cx*cx + cy*cy);
            float angle = std::atan2(cy, cx);
            float spiral = std::sin(dist * 0.5f - angle * 3.0f);
            
            if (spiral > 0.0f) {
                baseR = 50; baseG = 50; baseB = 50;
            } else {
                baseR = 15; baseG = 15; baseB = 15;
            }
        } else if (isCaverna) {
            bool isJuntaPedra = ((tx * 3 + ty * 7) % 9) < 2 || ((tx * 11 + ty * 5) % 13) < 2;
            if (isJuntaPedra) {
                baseR = 30; baseG = 30; baseB = 30;
            } else {
                bool hasGrain = ((tx * 17 + ty * 23) % 7) < 3;
                if (hasGrain) {
                    baseR = 55; baseG = 50; baseB = 45;
                } else {
                    baseR = 75; baseG = 70; baseB = 65;
                }
            }
        } else {
            bool isJuntaPedra = ((tx * 3 + ty * 7) % 9) < 2 || ((tx * 11 + ty * 5) % 13) < 2;
            if (isJuntaPedra) {
                baseR = 50; baseG = 50; baseB = 50;
            } else {
                bool hasGrain = ((tx * 17 + ty * 23) % 7) < 3;
                if (hasGrain) {
                    baseR = 90; baseG = 90; baseB = 90;
                } else {
                    baseR = 110; baseG = 110; baseB = 110;
                }
            }
        }
    }
    return aplicarNevoa(baseR, baseG, baseB, distanciaAteParede, profundidadeMaxima, temaCeu, luzes, hitX, hitY, isSideWall);
}

std::string RaycasterMundo::obterPixelParede(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao, bool isSideWall, const std::vector<std::tuple<int, int, int>>& luzes, float hitX, float hitY) {
    return obterPixelParedeInternal(tituloMapa, temaFloresta, distanciaAteParede, profundidadeMaxima, charParede, y, teto, chao, texX, tempoAnimacao, luzes, hitX, hitY, isSideWall);
}

std::string RaycasterMundo::obterPixelChao(const std::string& tituloMapa, float currentX, float currentY, float currentDist, float profundidadeMaxima, const std::vector<std::tuple<int, int, int>>& luzes) {
    const auto& flags = obterFlagsMapa(tituloMapa);
    int temaCeu = flags.temaCeu;
    currentDist *= 0.55f;

    bool isTerra = flags.isTerra;
    bool isLabirinto = flags.isLabirinto;
    bool isSalaChefe = flags.isSalaChefe;

    unsigned int globX = static_cast<unsigned int>(std::abs(currentX * 32.0f));
    unsigned int globY = static_cast<unsigned int>(std::abs(currentY * 32.0f));

    std::string fg;
    char c = ' ';
    int r = 0, g = 0, b = 0;

    if (isLabirinto) {
        fg = "\033[38;2;150;130;90m";
        bool bordaX = (globX % 64 < 2) || (globX % 64 > 61);
        bool bordaY = (globY % 32 < 2) || (globY % 32 > 29);
        if (bordaX || bordaY) {
            r = 40; g = 40; b = 30;
            c = ' ';
        } else {
            if ((globX + globY) % 2 == 0) { r = 180; g = 160; b = 110; }
            else                          { r = 160; g = 140; b = 95; }
            c = ((globX * 3 + globY * 7) % 5 < 2) ? '-' : '=';
        }
    } else if (isSalaChefe) {
        float cx = (globX % 64) - 32.0f;
        float cy = (globY % 64) - 32.0f;
        float dist = std::sqrt(cx*cx + cy*cy);
        float angle = std::atan2(cy, cx);
        float spiral = std::sin(dist * 0.4f - angle * 3.0f);

        r = 5; g = 5; b = 5;
        fg = "\033[38;2;50;50;50m"; 
        if (spiral > 0.3f) c = '@';
        else if (spiral > 0.0f) c = '%';
        else if (spiral > -0.3f) c = '.';
        else c = ' ';
    } else if (isTerra) {
        fg = "\033[38;2;45;25;10m";
        if ((globX + globY) % 2 == 0) { r = 28; g = 18; b = 8; }
        else if ((globX * 3 + globY * 5) % 7 < 2) { r = 22; g = 12; b = 4; }
        else { r = 25; g = 15; b = 5; }
        
        if ((globX * 17 + globY * 23) % 47 < 4) c = '.';
        else if ((globX * globX + globY * 13) % 53 < 3) c = '-';
        else if ((globX * 3 + globY * 7) % 31 < 2) c = '`';
    } else {
        fg = "\033[38;2;60;60;60m";
        if ((globX + globY) % 2 == 0) { r = 24; g = 24; b = 24; }
        else if ((globX * 3 + globY * 5) % 7 < 2) { r = 16; g = 16; b = 16; }
        else { r = 20; g = 20; b = 20; }
        
        if ((globX * 17 + globY * 23) % 47 < 4) c = '.';
        else if ((globX * globX + globY * 13) % 53 < 3) c = '-';
        else if ((globX * 3 + globY * 7) % 31 < 2) c = '`';
    }
    
    std::string newBg = aplicarNevoa(r, g, b, currentDist, profundidadeMaxima, temaCeu, luzes, currentX, currentY);
    if (c == ' ') return newBg;
    
    if (currentDist > profundidadeMaxima * 0.5f) return newBg;
    
    newBg = newBg.substr(0, newBg.find(" \033[0m")); // remove space and reset
    return newBg + fg + std::string(1, c) + "\033[0m";
}

std::string RaycasterMundo::obterPixelAgua(float currentX, float currentY, float currentDist, float profundidadeMaxima, float raioAngulo, float tempoAnimacao, int temaCeu) {
    int baseR=0, baseG=0, baseB=0;
    currentDist *= 0.55f; // Empurra o sombreamento da agua mais para o fundo

    int tx = (int)(currentX * 32.0f) % 32;
    int ty = (int)(currentY * 32.0f) % 32;

    bool isOnda = ((tx + ty) % 4 == 0) || (((tx / 2) + ty) % 5 == 0);
    if (isOnda) {
        baseR = 80; baseG = 180; baseB = 255;
    } else {
        baseR = 50; baseG = 150; baseB = 255;
    }
    
    // Reflexo da luz / ceu na agua (Fake reflection)
    float angleOffset = std::sin(tempoAnimacao + currentX * 0.5f) * 0.1f;
    float diffAnguloLua = std::fmod(raioAngulo + angleOffset, 2.0f * 3.14159f);
    if (diffAnguloLua < 0) diffAnguloLua += 2.0f * 3.14159f;
    if (diffAnguloLua < 0.2f || diffAnguloLua > 6.0f) {
        baseR += 40; baseG += 40; baseB += 40; // Brilho de reflexo
    }
    std::vector<std::tuple<int, int, int>> noLuzes;
    return aplicarNevoa(baseR, baseG, baseB, currentDist, profundidadeMaxima, temaCeu, noLuzes, currentX, currentY);
}

int RaycasterMundo::obterTemaCeu(const std::string& tituloMapa) {
    const auto& flags = obterFlagsMapa(tituloMapa);
    return flags.temaCeu;
}

std::string RaycasterMundo::obterPixelTeto(int temaCeu, float raioAngulo, int y, int alturaTela, float tempoAnimacao) {
    if (temaCeu == 0) return " "; // Interior

    int horizonte = alturaTela / 2;
    float ratio = (horizonte > 0) ? (float)y / (float)horizonte : 1.0f;
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;

    if (temaCeu == 1) { // Floresta (Noite Estrelada)
        int r = 5 + (int)(15 * ratio);  // 5 -> 20 (Ceu bem escuro)
        int g = 5 + (int)(20 * ratio);  // 5 -> 25
        int b = 15 + (int)(30 * ratio); // 15 -> 45
        
        // Lua (Fixada no angulo 0.0 radianos)
        float divHorizonte = (horizonte > 0) ? (float)horizonte : 1.0f;
        float diffAnguloLua = std::fmod(raioAngulo - 0.0f, 2.0f * 3.14159f);
        if (diffAnguloLua < -3.14159f) diffAnguloLua += 2.0f * 3.14159f;
        if (diffAnguloLua > 3.14159f) diffAnguloLua -= 2.0f * 3.14159f;
        
        float distXLua = diffAnguloLua;
        float distYLua = (y - horizonte * 0.25f) / divHorizonte;
        float distLua = std::sqrt(distXLua * distXLua * 6.0f + distYLua * distYLua); // 6.0f ajusta a proporcao dos caracteres
        
        if (distLua < 0.10f) {
            // Sombra da iluminacao parcial (crescente) que pulsa com o tempo
            float shadowOffset = 0.04f + std::sin(tempoAnimacao * 1.5f) * 0.015f; 
            float shadowDist = std::sqrt((distXLua - shadowOffset) * (distXLua - shadowOffset) * 6.0f + distYLua * distYLua);
            
            if (shadowDist < 0.10f) {
                return "\033[48;2;60;60;70m \033[0m"; // Parte escura (sombra da lua)
            }
            int craterNoise = ((int)(distXLua * 100) * 17 + y * 23) % 47;
            if (craterNoise < 8) return "\033[48;2;180;180;190m \033[0m"; // Cratera
            return "\033[48;2;230;230;240m \033[0m"; // Lua brilhante
        } else if (distLua < 0.12f) {
            return "\033[48;2;120;120;130m \033[0m"; // Borda da lua mais escura
        } else if (distLua < 0.25f) {
            // Aura da lua (brilho suave misturado no ceu escuro, pulsando)
            float glowPulse = std::sin(tempoAnimacao * 2.0f) * 0.02f;
            float glow = 1.0f - ((distLua - 0.12f) / (0.13f + glowPulse));
            if (glow > 0.0f) {
                r = std::min(255, r + (int)(40 * glow));
                g = std::min(255, g + (int)(45 * glow));
                b = std::min(255, (int)(b + 60 * glow));
            }
        }

        std::string bg = "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";

        // Normaliza o angulo para nunca ser negativo (evita o bug da "parede" de estrelas)
        float anguloNorm = std::fmod(raioAngulo, 2.0f * 3.14159f);
        if (anguloNorm < 0.0f) anguloNorm += 2.0f * 3.14159f;

        // Ruido espacial nao-linear para criar as estrelas (espalhadas aleatoriamente, sem diagonais)
        unsigned int starX = static_cast<unsigned int>(anguloNorm * 150.0f); // Densidade horizontal de estrelas em 360 graus
        unsigned int starY = static_cast<unsigned int>(y);
        
        unsigned int hash = starX * 374761393U + starY * 668265263U;
        hash = (hash ^ (hash >> 13)) * 1274126177U;
        int noise = hash % 1000;
        
        if (noise == 0 && y < horizonte * 0.8f) return bg + "\033[38;2;255;255;255m*\033[0m"; 
        else if (noise < 4 && y < horizonte * 0.8f) return bg + "\033[38;2;200;200;255m+\033[0m"; 
        else if (noise < 12 && y < horizonte * 0.8f) return bg + "\033[38;2;255;255;255m.\033[0m"; 
        else if (noise < 20 && y < horizonte * 0.8f) return bg + "\033[38;2;200;200;200m.\033[0m"; 
        else if (noise == 25 && y < horizonte * 0.8f) return bg + "\033[38;2;255;255;150m'\033[0m"; 
        
        return bg + " \033[0m";
    }

    // Default (Azul Dia)
    int r = 10 + (int)(60 * ratio);  // 10 -> 70
    int g = 60 + (int)(90 * ratio);  // 60 -> 150
    int b = 150 + (int)(80 * ratio); // 150 -> 230

    // Sol (Fixado no angulo 0.0 radianos / Mesma direcao da lua)
    float divHorizonte = (horizonte > 0) ? (float)horizonte : 1.0f;
    float diffAnguloSol = std::fmod(raioAngulo - 0.0f, 2.0f * 3.14159f);
    if (diffAnguloSol < -3.14159f) diffAnguloSol += 2.0f * 3.14159f;
    if (diffAnguloSol > 3.14159f) diffAnguloSol -= 2.0f * 3.14159f;
    
    float distXSol = diffAnguloSol;
    float distYSol = (y - horizonte * 0.35f) / divHorizonte;
    float distSol = std::sqrt(distXSol * distXSol * 6.0f + distYSol * distYSol);
    
    // Calcula os raios solares rotacionando com o tempo
    float angleSol = std::atan2(distYSol, distXSol * 2.449f); // 2.449f = aprox sqrt(6.0f)
    float rays = std::sin(angleSol * 8.0f + tempoAnimacao * 2.0f); // 8 pontas girando
    float glowRadius = 0.12f + rays * 0.02f;
    
    if (distSol < 0.08f) {
        return "\033[48;2;255;255;255m \033[0m"; // Centro branco e incandescente
    } else if (distSol < glowRadius) {
        return "\033[48;2;255;220;50m \033[0m"; // Coroa amarela forte com raios solares
    } else if (distSol < 0.35f) {
        // Brilho solar clareando e mesclando com o ceu azul (tbm pulsando levemente)
        float glowPulse = std::sin(tempoAnimacao * 1.5f) * 0.03f;
        float glow = 1.0f - ((distSol - glowRadius) / (0.35f - glowRadius + glowPulse)); 
        if (glow > 0.0f) {
            r = std::min(255, r + (int)(150 * glow));
            g = std::min(255, g + (int)(100 * glow));
        }
    }

    return "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m \033[0m"; 
}

char RaycasterMundo::obterSpriteChar(int mapX, int mapY, char c, const std::string& tituloMapa) {
    if (c == '^') {
        const auto& flags = obterFlagsMapa(tituloMapa);
        if (flags.tituloUpper.find("VILA") != std::string::npos) return '2';
        if (flags.tituloUpper.find("FLORESTA") != std::string::npos) {
            if (mapY > 15) return '1'; 
            return '5'; 
        }
        if (flags.tituloUpper.find("REINO") != std::string::npos) return '1';
        return '^';
    }

    const auto& flags = obterFlagsMapa(tituloMapa);

    if (flags.tituloUpper.find("VILA") != std::string::npos && c == 'F') {
        return 'V'; // Franchesco
    }
    if (flags.tituloUpper.find("SALA DO CHEFE") != std::string::npos && (c == 'M' || c == 'A' || c == 'H' || c == 'O' || c == 'R' || c == 'G')) {
        return 'H'; // Mahoraga
    }
    if ((flags.tituloUpper.find("CABANA") != std::string::npos || flags.tituloUpper.find("FLORESTA") != std::string::npos) && c == 'M') {
        return 'W'; // Morgana
    }
    if (flags.tituloUpper.find("LABIRINTO") != std::string::npos && c == 'B') {
        return 'X'; // Bau
    }

    bool isReino = flags.isReino;
    if (isReino && (c == 'C' || c == 'G')) return 'C'; // Cavaleiro Real
    return c; // Default
}

std::string RaycasterMundo::obterCorMinimapaEntidade(char c, const std::string& tituloMapa) {
    const auto& flags = obterFlagsMapa(tituloMapa);

    if (flags.tituloUpper.find("VILA") != std::string::npos && c == 'F') return "\033[1;38;2;255;200;50m"; // Amarelo para Franchesco (Brilhante)
    if ((flags.tituloUpper.find("VILA") != std::string::npos || flags.tituloUpper.find("CAVERNA") != std::string::npos) && c == 'B') return "\033[1;38;2;100;200;255m"; // Ciano para Bjorn
    if (flags.tituloUpper.find("SALA DO CHEFE") != std::string::npos) return "\033[1;38;2;255;255;255m"; // Branco para Boss
    if ((flags.tituloUpper.find("CABANA") != std::string::npos || flags.tituloUpper.find("FLORESTA") != std::string::npos) && c == 'M') return "\033[1;38;2;200;100;255m"; // Morgana Roxa
    if (flags.tituloUpper.find("LABIRINTO") != std::string::npos && c == 'B') return "\033[1;38;2;200;150;50m"; // Bau Dourado
    if (c == 'P') return "\033[1;38;2;139;69;19m"; // Placa (Brilhante)
    if (c == '*') return "\033[38;2;34;139;34m"; // Verde para arvore
    
    bool isReino = flags.isReino;
    if (isReino && c == 'C') return "\033[1;38;2;200;200;220m"; // Prata para Cavaleiro
    if (isReino && c == 'G') return "\033[1;38;2;255;200;50m"; // Amarelo para Guarda do Reino
    return "\033[1;38;2;255;50;50m"; // Vermelho base para Inimigos (Brilhante)
}
