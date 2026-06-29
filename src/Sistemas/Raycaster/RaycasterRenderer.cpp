#include "RaycasterRenderer.h"
#include "RaycasterMundo.h"
#include <cmath>
#include <algorithm>
#include <thread>
#include <vector>

using namespace std;

struct EntidadeAtingida {
    float dist;
    char c;
    float texX;
};

void RaycasterRenderer::renderizar3D(vector<Pixel3D>& tela, int LARGURA_TELA, int ALTURA_TELA, float jogadorX, float jogadorY, float anguloVisao, float horizonte, int bobbingOffset, float profundidadeMaxima, float tempoAbsoluto, const vector<string>& matrizDoMapa, const string& tituloMapa, bool temaFloresta, int temaCeu, const map<char, SpriteCache>& cacheSprites) {
    float campoVisao = 3.14159f / 4.0f; // FOV 45 graus
    int larguraMapa = matrizDoMapa.empty() ? 0 : matrizDoMapa[0].size();
    int alturaMapa = matrizDoMapa.size();

    RaycasterMundo::atualizarMapHash(matrizDoMapa);

    static thread_local size_t lastMapForLuzesHash = 0;
    static thread_local std::vector<std::tuple<int, int, int>> cachedLuzes;

    size_t currentHash = RaycasterMundo::obterMapHash();
    if (currentHash != lastMapForLuzesHash) {
        lastMapForLuzesHash = currentHash;
        cachedLuzes.clear();
        for (int ly = 0; ly < alturaMapa; ly++) {
            for (int lx = 0; lx < larguraMapa; lx++) {
                char c = matrizDoMapa[ly][lx];
                if (RaycasterMundo::isMapLabel(lx, ly, matrizDoMapa)) continue;

                if (c == '^') {
                    cachedLuzes.push_back({lx, ly, 1}); // White
                } else if (c == 'P' || c == 'F' || c == 'B' || c == 'A' || c == 'Q') {
                    cachedLuzes.push_back({lx, ly, 0}); // Orange
                }
            }
        }
    }
    const auto& luzes = cachedLuzes;

    std::string tituloUpper = tituloMapa;
    for (char& ch : tituloUpper) ch = std::toupper(static_cast<unsigned char>(ch));
    bool isReino = (tituloUpper.find("PATIO DO REINO") != std::string::npos || tituloUpper.find("REINO") != std::string::npos);

    // Mover o vetor para fora do loop horizontal para evitar alocações constantes na heap
    vector<EntidadeAtingida> entidadesAtingidas;

    for (int x = 0; x < LARGURA_TELA; x++) {
        float raioAngulo = (anguloVisao - campoVisao / 2.0f) + ((float)x / (float)LARGURA_TELA) * campoVisao;
        float distanciaAteParede = 0.0f;
        bool bateuNaParede = false;

        float olhoX = cosf(raioAngulo);
        float olhoY = sinf(raioAngulo);
        
        entidadesAtingidas.clear();
        int lastEntX = -1, lastEntY = -1;
        char charParede = '#';

        // DDA algorithm setup
        float rayDirX = olhoX;
        float rayDirY = olhoY;

        int mapX = (int)jogadorX;
        int mapY = (int)jogadorY;

        float deltaDistX = (rayDirX == 0.0f) ? 1e30f : std::abs(1.0f / rayDirX);
        float deltaDistY = (rayDirY == 0.0f) ? 1e30f : std::abs(1.0f / rayDirY);

        int stepX, stepY;
        float sideDistX, sideDistY;

        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (jogadorX - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0f - jogadorX) * deltaDistX;
        }
        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (jogadorY - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0f - jogadorY) * deltaDistY;
        }

        int side = 0;

        while (!bateuNaParede && distanciaAteParede < profundidadeMaxima) {
            if (sideDistX < sideDistY) {
                distanciaAteParede = sideDistX;
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                distanciaAteParede = sideDistY;
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }

            if (mapX < 0 || mapX >= larguraMapa || mapY < 0 || mapY >= alturaMapa) {
                bateuNaParede = true;
                distanciaAteParede = profundidadeMaxima;
            } else {
                char c = matrizDoMapa[mapY][mapX];
                if (c != '.' && c != ' ' && c != '~') {
                    bool isLabel = RaycasterMundo::isMapLabel(mapX, mapY, matrizDoMapa);
                    if (!isLabel) {
                        bool isEntity = RaycasterMundo::isEntity(c);
                        if (isEntity) {
                            if (mapX != lastEntX || mapY != lastEntY) {
                                lastEntX = mapX;
                                lastEntY = mapY;
                                char spriteChar = RaycasterMundo::obterSpriteChar(mapX, mapY, c, tituloMapa);

                                float centerX = mapX + 0.5f;
                                float centerY = mapY + 0.5f;

                                if (spriteChar == 'H') { centerX = 54.0f; centerY = 28.5f; }

                                float dx = centerX - jogadorX;
                                float dy = centerY - jogadorY;
                                
                                float anguloProCentro = atan2(dy, dx);
                                float diffAngulo = raioAngulo - anguloProCentro;
                                
                                while (diffAngulo < -3.14159f) diffAngulo += 2.0f * 3.14159f;
                                while (diffAngulo > 3.14159f) diffAngulo -= 2.0f * 3.14159f;
                                
                                float distProCentro = sqrt(dx*dx + dy*dy);
                                float larguraSpriteRadianos = atan2(0.45f, distProCentro); 
                                if (spriteChar == 'H') larguraSpriteRadianos = atan2(2.5f, distProCentro); 
                                if (spriteChar == '*') larguraSpriteRadianos = atan2(0.60f, distProCentro); 
                                
                                if (fabs(diffAngulo) <= larguraSpriteRadianos) {
                                    float texX = (diffAngulo / larguraSpriteRadianos) * 0.5f + 0.5f;
                                    entidadesAtingidas.push_back({distProCentro, spriteChar, texX});
                                }
                            }
                        } else {
                            bateuNaParede = true;
                            charParede = c;
                        }
                    }
                }
            }
        } 

        float hitX = jogadorX + olhoX * distanciaAteParede;
        float hitY = jogadorY + olhoY * distanciaAteParede;

        float texXParede = 0.0f;
        bool isSideWall = false;
        if (side == 0) {
            texXParede = hitY - floorf(hitY); 
            isSideWall = true;
        } else {
            texXParede = hitX - floorf(hitX); 
            isSideWall = false;
        }

        int teto = (int)(horizonte - ALTURA_TELA / ((float)distanciaAteParede));
        int chao = (int)(horizonte + ALTURA_TELA / ((float)distanciaAteParede));

        if (temaFloresta && charParede == '#') teto -= (int)(ALTURA_TELA / ((float)distanciaAteParede) * 1.5f); 
        if (temaFloresta && RaycasterMundo::obterNPCProximo(tituloMapa, (int)hitX, (int)hitY) == 'M') teto -= (int)(ALTURA_TELA / ((float)distanciaAteParede) * 1.2f);
        if (charParede == '*') teto -= (int)(ALTURA_TELA / ((float)distanciaAteParede) * 1.5f); 
        if (isReino && charParede == '|') teto -= (int)(ALTURA_TELA / ((float)distanciaAteParede) * 3.0f); // Portao super alto

        for (int y = 0; y < ALTURA_TELA; y++) {
            bool drawFloor = false;

            if (y < teto) {
                tela[y * LARGURA_TELA + x] = RaycasterMundo::obterPixelTeto(temaCeu, raioAngulo, y - bobbingOffset, ALTURA_TELA, tempoAbsoluto);
            } else if (y >= teto && y <= chao) {
                Pixel3D pixel = RaycasterMundo::obterPixelParede(tituloMapa, temaFloresta, distanciaAteParede, profundidadeMaxima, charParede, y, teto, chao, texXParede, tempoAbsoluto, isSideWall, luzes, hitX, hitY);
                if (pixel.isFundo) {
                    if (y <= horizonte) tela[y * LARGURA_TELA + x] = RaycasterMundo::obterPixelTeto(temaCeu, raioAngulo, y - bobbingOffset, ALTURA_TELA, tempoAbsoluto);
                    else drawFloor = true;
                } else {
                    tela[y * LARGURA_TELA + x] = pixel;
                }
            } else {
                drawFloor = true;
            }

            if (drawFloor) {
                float currentDist = (float)ALTURA_TELA / ((float)y - horizonte);
                float currentX = jogadorX + olhoX * currentDist;
                float currentY = jogadorY + olhoY * currentDist;
                char floorChar = '.';
                if (currentX >= 0 && currentX < larguraMapa && currentY >= 0 && currentY < alturaMapa) {
                    floorChar = matrizDoMapa[(int)currentY][(int)currentX];
                }
                if (floorChar == '~') tela[y * LARGURA_TELA + x] = RaycasterMundo::obterPixelAgua(currentX, currentY, currentDist, profundidadeMaxima, raioAngulo, tempoAbsoluto, temaCeu);
                else tela[y * LARGURA_TELA + x] = RaycasterMundo::obterPixelChao(tituloMapa, currentX, currentY, currentDist, profundidadeMaxima, luzes);
            }
        }

        std::sort(entidadesAtingidas.begin(), entidadesAtingidas.end(), [](const EntidadeAtingida& a, const EntidadeAtingida& b) { return a.dist > b.dist; });

        for (const auto& ent : entidadesAtingidas) {
            int tetoEnt = (int)(horizonte - ALTURA_TELA / ent.dist);
            if (ent.c == '*') tetoEnt -= (int)(ALTURA_TELA / ent.dist * 1.5f); 
            int chaoEnt = (int)(horizonte + ALTURA_TELA / ent.dist);

            if (ent.c != '^' && ent.c != 'P' && ent.c != 'X' && ent.c != '*') {
                float offset = (float)ent.c; 
                tetoEnt -= (int)((sinf(tempoAbsoluto * 3.5f + offset)) * (ALTURA_TELA / ent.dist) * 0.05f); 
            }

            int altEnt = chaoEnt - tetoEnt;
            if (altEnt > 0) {
                if (cacheSprites.count(ent.c)) {
                    auto& sc = cacheSprites.at(ent.c);
                    int spriteX = (int)(ent.texX * sc.width);
                    if (spriteX < 0) spriteX = 0;
                    if (spriteX >= sc.width) spriteX = sc.width - 1;
                    
                    for (int y = tetoEnt; y <= chaoEnt; y++) {
                        if (y >= 0 && y < ALTURA_TELA) {
                            int spriteY = ((y - tetoEnt) * sc.height) / altEnt;
                            if (spriteY >= 0 && spriteY < sc.height && spriteX < (int)sc.pixels[spriteY].size()) {
                                SpritePixel sp = sc.pixels[spriteY][spriteX];
                                if (!sp.isTransparente) {
                                    Pixel3D px;
                                    px.r = sp.r;
                                    px.g = sp.g;
                                    px.b = sp.b;
                                    px.ch = sp.ch;
                                    px.fgR = sp.fgR;
                                    px.fgG = sp.fgG;
                                    px.fgB = sp.fgB;
                                    px.hasFg = sp.hasFg;
                                    px.isFundo = false;
                                    
                                    if (sp.ch == ' ' && (ent.c == '^' || (ent.c >= '1' && ent.c <= '5'))) {
                                        float wave = sinf(tempoAbsoluto * 6.0f + y * 0.2f + spriteX * 0.2f);
                                        px.r = static_cast<uint8_t>(210 + (int)(wave * 45));
                                        px.g = static_cast<uint8_t>(190 + (int)(wave * 65));
                                        px.b = 255;
                                    }
                                    tela[y * LARGURA_TELA + x] = px; 
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
