#include "RaycasterRenderizador.h"
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

void RaycasterRenderizador::renderizar3D(vector<Pixel3D>& tela, int LARGURA_TELA, int ALTURA_TELA, float jogadorX, float jogadorY, float anguloVisao, float horizonte, int bobbingOffset, float profundidadeMaxima, float tempoAbsoluto, const vector<string>& matrizDoMapa, const string& tituloMapa, bool temaFloresta, int temaCeu, const map<char, SpriteCache>& cacheSprites) {
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

    std::vector<float> ZBuffer(LARGURA_TELA, 0.0f);

    for (int x = 0; x < LARGURA_TELA; x++) {
        float raioAngulo = (anguloVisao - campoVisao / 2.0f) + ((float)x / (float)LARGURA_TELA) * campoVisao;
        float distanciaAteParede = 0.0f;
        bool bateuNaParede = false;

        float olhoX = cosf(raioAngulo);
        float olhoY = sinf(raioAngulo);
        
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
                    if (!RaycasterMundo::isMapLabel(mapX, mapY, matrizDoMapa) && !RaycasterMundo::isEntity(c)) {
                        bateuNaParede = true;
                        charParede = c;
                    }
                }
            }
        } 

        float hitX = jogadorX + olhoX * distanciaAteParede;
        float hitY = jogadorY + olhoY * distanciaAteParede;
        
        // Corrigindo Fisheye
        float perpWallDist = distanciaAteParede * cosf(raioAngulo - anguloVisao);
        if (perpWallDist < 0.1f) perpWallDist = 0.1f;

        ZBuffer[x] = perpWallDist;

        float texXParede = 0.0f;
        bool isSideWall = false;
        if (side == 0) {
            texXParede = hitY - floorf(hitY); 
            isSideWall = true;
        } else {
            texXParede = hitX - floorf(hitX); 
            isSideWall = false;
        }

        int teto = (int)(horizonte - ALTURA_TELA / perpWallDist);
        int chao = (int)(horizonte + ALTURA_TELA / perpWallDist);

        char npcEncontradoNaColuna = RaycasterMundo::obterNPCProximo(tituloMapa, (int)hitX, (int)hitY);

        if (temaFloresta && charParede == '#') teto -= (int)(ALTURA_TELA / perpWallDist * 1.5f); 
        if (temaFloresta && npcEncontradoNaColuna == 'M') teto -= (int)(ALTURA_TELA / perpWallDist * 1.2f);
        if (charParede == '*') teto -= (int)(ALTURA_TELA / perpWallDist * 1.5f); 
        if (isReino && charParede == '|') teto -= (int)(ALTURA_TELA / perpWallDist * 3.0f); // Portao super alto

        for (int y = 0; y < ALTURA_TELA; y++) {
            bool drawFloor = false;

            if (y < teto) {
                tela[y * LARGURA_TELA + x] = RaycasterMundo::obterPixelTeto(temaCeu, raioAngulo, y - bobbingOffset, ALTURA_TELA, tempoAbsoluto);
            } else if (y >= teto && y <= chao) {
                Pixel3D pixel = RaycasterMundo::obterPixelParede(tituloMapa, temaFloresta, perpWallDist, profundidadeMaxima, charParede, y, teto, chao, texXParede, tempoAbsoluto, isSideWall, luzes, hitX, hitY, npcEncontradoNaColuna);
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
                float currentDist = (float)ALTURA_TELA / ((float)y - horizonte) / cosf(raioAngulo - anguloVisao);
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
    }

    // SPRITE CASTING (Multi-Pass)
    struct SpriteProj { float x, y, dist; char c, sprCh; };
    std::vector<SpriteProj> spritesGlobais;
    for (int y = 0; y < alturaMapa; y++) {
        for (int x = 0; x < larguraMapa; x++) {
            char c = matrizDoMapa[y][x];
            if (RaycasterMundo::isEntity(c) && !RaycasterMundo::isMapLabel(x, y, matrizDoMapa)) {
                char sprCh = RaycasterMundo::obterSpriteChar(x, y, c, tituloMapa);
                float dx = (x + 0.5f) - jogadorX;
                float dy = (y + 0.5f) - jogadorY;
                float distSq = dx*dx + dy*dy;
                spritesGlobais.push_back({x + 0.5f, y + 0.5f, distSq, c, sprCh});
            }
        }
    }

    // Boss correction
    for (auto& sp : spritesGlobais) {
        if (sp.sprCh == 'H') {
            sp.x = 54.0f; sp.y = 28.5f;
            float dx = sp.x - jogadorX;
            float dy = sp.y - jogadorY;
            sp.dist = dx*dx + dy*dy;
        }
    }

    std::sort(spritesGlobais.begin(), spritesGlobais.end(), [](const SpriteProj& a, const SpriteProj& b) { return a.dist > b.dist; });

    float dirX = cosf(anguloVisao);
    float dirY = sinf(anguloVisao);
    float planeX = -sinf(anguloVisao) * tanf(campoVisao / 2.0f);
    float planeY = cosf(anguloVisao) * tanf(campoVisao / 2.0f);

    for (const auto& sp : spritesGlobais) {
        float spriteX = sp.x - jogadorX;
        float spriteY = sp.y - jogadorY;

        float invDet = 1.0f / (planeX * dirY - dirX * planeY);
        float transformX = invDet * (dirY * spriteX - dirX * spriteY);
        float transformY = invDet * (-planeY * spriteX + planeX * spriteY);

        if (transformY <= 0.1f) continue;

        int spriteScreenX = (int)((LARGURA_TELA / 2) * (1 + transformX / transformY));
        int spriteHeight = std::abs((int)(ALTURA_TELA / transformY));
        
        int tetoEnt = (int)(horizonte - spriteHeight);
        int chaoEnt = (int)(horizonte + spriteHeight);

        if (sp.sprCh == '*') tetoEnt -= (int)(spriteHeight * 1.5f);
        if (sp.sprCh != '^' && sp.sprCh != 'P' && sp.sprCh != 'X' && sp.sprCh != '*') {
            float offset = (float)sp.sprCh;
            tetoEnt -= (int)((sinf(tempoAbsoluto * 3.5f + offset)) * spriteHeight * 0.05f);
        }

        int altEnt = chaoEnt - tetoEnt;
        if (altEnt <= 0) continue;

        int spriteWidth = spriteHeight;
        if (sp.sprCh == 'H') spriteWidth = (int)(spriteHeight * 4.0f); // Boss is very wide
        if (sp.sprCh == '*') spriteWidth = (int)(spriteHeight * 1.5f); 

        int drawStartX = -spriteWidth / 2 + spriteScreenX;
        if (drawStartX < 0) drawStartX = 0;
        int drawEndX = spriteWidth / 2 + spriteScreenX;
        if (drawEndX > LARGURA_TELA) drawEndX = LARGURA_TELA;

        if (cacheSprites.count(sp.sprCh) == 0) continue;
        const auto& sc = cacheSprites.at(sp.sprCh);
        bool isAnimated = (sp.sprCh == '^' || (sp.sprCh >= '1' && sp.sprCh <= '5'));

        for (int stripe = drawStartX; stripe < drawEndX; stripe++) {
            if (transformY < ZBuffer[stripe]) {
                int texX = (int)(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * sc.width / spriteWidth) / 256;
                if (texX < 0) texX = 0;
                if (texX >= sc.width) texX = sc.width - 1;

                int startY = std::max(0, tetoEnt);
                int endY = std::min(ALTURA_TELA - 1, chaoEnt);
                float stepY = (float)sc.height / (float)altEnt;
                float texY = (startY - tetoEnt) * stepY;

                for (int y = startY; y <= endY; y++) {
                    int spriteY = (int)texY;
                    texY += stepY;

                    if (spriteY >= 0 && spriteY < sc.height) {
                        const SpritePixel& spix = sc.pixels[spriteY * sc.width + texX];
                        if (!spix.isTransparente) {
                            Pixel3D& px = tela[y * LARGURA_TELA + stripe];
                            px.r = spix.r;
                            px.g = spix.g;
                            px.b = spix.b;
                            px.ch = spix.ch;
                            px.fgR = spix.fgR;
                            px.fgG = spix.fgG;
                            px.fgB = spix.fgB;
                            px.hasFg = spix.hasFg;
                            px.isFundo = false;

                            if (spix.ch == ' ' && isAnimated) {
                                float wave = sinf(tempoAbsoluto * 6.0f + y * 0.2f + texX * 0.2f);
                                px.r = static_cast<uint8_t>(210 + (int)(wave * 45));
                                px.g = static_cast<uint8_t>(190 + (int)(wave * 65));
                                px.b = 255;
                            }
                        }
                    }
                }
            }
        }
    }
}
