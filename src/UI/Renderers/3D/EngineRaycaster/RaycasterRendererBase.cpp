#include "UI/Renderers/3D/EngineRaycaster/RaycasterRendererBase.h"
#include "UI/Renderers/3D/EngineRaycaster/TextureManager.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterWorld.h"
#include <cmath>
#include <algorithm>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>

using namespace std;

/*
 * Estrutura de ThreadPool otimizada para o motor de Raycasting.
 * Utiliza o maximo de nucleos disponiveis (hardware_concurrency) para processar 
 * o laco principal de colunas da tela em paralelo, garantindo 60 FPS.
 */
struct ThreadPool {
    std::vector<std::thread> threads;
    std::atomic<bool> stop{false};
    std::atomic<int> activeTasks{0};
    
    std::mutex mtx;
    std::condition_variable cv;
    std::vector<std::function<void()>> tasks;
    
    ThreadPool() {
        int indexInThreads = std::thread::hardware_concurrency();
        if (indexInThreads == 0) indexInThreads = 4;
        for (int i = 0; i < indexInThreads; ++i) {
            threads.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        cv.wait(lock, [this]() { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.back());
                        tasks.pop_back();
                    }
                    task();
                    activeTasks--;
                }
            });
        }
    }
    
    ~ThreadPool() {
        stop = true;
        cv.notify_all();
        for (auto& t : threads) t.join();
    }
    
    void execute(std::vector<std::function<void()>> newTasks) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            tasks = std::move(newTasks);
            activeTasks = tasks.size();
        }
        cv.notify_all();
        while (activeTasks > 0) {
            std::this_thread::yield();
        }
    }
};

static ThreadPool& getThreadPool() {
    static ThreadPool pool;
    return pool;
}

struct EntityReached {
    float dist;
    char c;
    float texX;
};

void RaycasterRenderer::render3D(vector<Pixel3D>& screen, int SCREEN_WIDTH, int SCREEN_HEIGHT, float playerX, float playerY, float angleVisa, float horizon, int bobbingOffset, float depthMaximum, float timeAbsolute, const vector<string>& mapMatrix, const string& titleMap, bool themeForest, int themeSky, const map<char, SpriteCache>& cacheSprites) {
    ManagerTextures::boot();
    float fieldVisa = 3.14159f / 4.0f; // FOV 45 graus
    int widthMap = mapMatrix.empty() ? 0 : mapMatrix[0].size();
    int heightMap = mapMatrix.size();

    RaycasterWorld::updateMapHash(mapMatrix);

    /*
     * Sistema de Cache de Iluminacao:
     * O mapa eh varrido apenas uma vez sempre que o player muda de sala ou layout, 
     * armazenando a localizacao e a intensidade das fontes de luz (Fogo, NPCs, Portas).
     */
    static thread_local size_t lastMapForLightsHash = 0;
    static thread_local std::vector<std::tuple<int, int, int>> cachedLights;

    size_t currentHash = RaycasterWorld::getMapHash();
    if (currentHash != lastMapForLightsHash) {
        lastMapForLightsHash = currentHash;
        cachedLights.clear();
        for (int ly = 0; ly < heightMap; ly++) {
            for (int lx = 0; lx < widthMap; lx++) {
                char c = mapMatrix[ly][lx];
                if (RaycasterWorld::isMapLabel(lx, ly, mapMatrix)) continue;

                char mappedC = RaycasterWorld::getSpriteChar(lx, ly, c, titleMap);
                if (RaycasterWorld::isMapLabel(lx, ly, mapMatrix)) continue;

                if (mappedC == '^' || mappedC == '1' || mappedC == '2' || mappedC == '3' || mappedC == '4' || mappedC == '5') {
                    cachedLights.push_back({lx, ly, 1}); // Teleport/Door (Soft Brown)
                } else if (mappedC == 'F') {
                    cachedLights.push_back({lx, ly, 0}); // Fire (Orange)
                } else if (mappedC == 'G' || mappedC == 'O' || mappedC == 'S' || mappedC == 'A' || mappedC == 'M' || mappedC == 'T' || mappedC == 'Y') {
                    cachedLights.push_back({lx, ly, 2}); // Enemy (Red)
                } else if (mappedC == 'H' || mappedC == 'R' || mappedC == 'P' || mappedC == 'Q' || mappedC == 'B' || mappedC == 'W' || mappedC == 'V' || mappedC == 'C' || mappedC == 'J' || mappedC == 'K' || mappedC == 'Z') {
                    cachedLights.push_back({lx, ly, 3}); // NPC (Yellow)
                }
            }
        }
    }

    std::vector<std::tuple<int, int, int>> lightsVisible;
    float maxDistLight = depthMaximum + 8.0f;
    float maxDistLightSq = maxDistLight * maxDistLight;
    for (const auto& l : cachedLights) {
        float dx = std::get<0>(l) - playerX;
        float dy = std::get<1>(l) - playerY;
        if (dx * dx + dy * dy <= maxDistLightSq) {
            lightsVisible.push_back(l);
        }
    }
    const auto& lights = lightsVisible;

    std::string upperTitle = titleMap;
    for (char& ch : upperTitle) ch = std::toupper(static_cast<unsigned char>(ch));
    bool isKingdom = (upperTitle.find("PATIO DO REINO") != std::string::npos || upperTitle.find("REINO") != std::string::npos);

    std::vector<float> ZBuffer(SCREEN_WIDTH, 0.0f);

    long long globalMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    float globalAngleSlow = ((globalMs % 300000) / 300000.0f) * 6.2831853f;

    std::vector<float> rayDirX(SCREEN_WIDTH);
    std::vector<float> rayDirY(SCREEN_WIDTH);
    std::vector<float> fisheyeCorrection(SCREEN_WIDTH);
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        float radiusAngle = (angleVisa - fieldVisa / 2.0f) + ((float)x / (float)SCREEN_WIDTH) * fieldVisa;
        rayDirX[x] = cosf(radiusAngle);
        rayDirY[x] = sinf(radiusAngle);
        fisheyeCorrection[x] = 1.0f / cosf(radiusAngle - angleVisa);
    }

    int indexInThreads = std::thread::hardware_concurrency();
    if (indexInThreads == 0) indexInThreads = 4;
    int chunkSize = 16;
    int indexInChunks = (SCREEN_WIDTH + chunkSize - 1) / chunkSize;
    std::vector<std::function<void()>> tasks;

    for (int i = 0; i < indexInChunks; i++) {
        int startX = i * chunkSize;
        int endX = std::min(startX + chunkSize, SCREEN_WIDTH);
        tasks.push_back([&, startX, endX, globalAngleSlow]() {
            static thread_local std::vector<std::tuple<int, int, int>> wallLights;
            
            for (int x = startX; x < endX; x++) {
        float radiusAngle = (angleVisa - fieldVisa / 2.0f) + ((float)x / (float)SCREEN_WIDTH) * fieldVisa;
        float distanceUntilWall = 0.0f;
        bool hitNaWall = false;

        float eyeX = rayDirX[x];
        float eyeY = rayDirY[x];
        
        char charWall = '#';

        /*
         * Algoritmo DDA (Digital Differential Analyzer):
         * Traca a trajetoria do raio pulando perfeitamente pelas grades do mapa de forma rapida,
         * sem a necessidade de pequenos incrementos variaveis, calculando a colisao exata.
         */
        float raySayX = eyeX;
        float raySayY = eyeY;

        int mapX = (int)playerX;
        int mapY = (int)playerY;

        float deltaDistX = (raySayX == 0.0f) ? 1e30f : std::abs(1.0f / raySayX);
        float deltaDistY = (raySayY == 0.0f) ? 1e30f : std::abs(1.0f / raySayY);

        int stepX, stepY;
        float sideDistX, sideDistY;

        if (raySayX < 0) {
            stepX = -1;
            sideDistX = (playerX - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0f - playerX) * deltaDistX;
        }
        if (raySayY < 0) {
            stepY = -1;
            sideDistY = (playerY - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0f - playerY) * deltaDistY;
        }

        int side = 0;

        while (!hitNaWall && distanceUntilWall < depthMaximum) {
            if (sideDistX < sideDistY) {
                distanceUntilWall = sideDistX;
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                distanceUntilWall = sideDistY;
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }

            if (mapX < 0 || mapX >= widthMap || mapY < 0 || mapY >= heightMap) {
                hitNaWall = true;
                distanceUntilWall = depthMaximum;
            } else {
                char c = mapMatrix[mapY][mapX];
                if (c != '.' && c != ' ' && c != '~' && c != ',') {
                    if (!RaycasterWorld::isMapLabel(mapX, mapY, mapMatrix) && !RaycasterWorld::isEntity(c)) {
                        hitNaWall = true;
                        charWall = c;
                    }
                }
            }
        } 

        float hitX = playerX + eyeX * distanceUntilWall;
        float hitY = playerY + eyeY * distanceUntilWall;
        
        /* 
         * Correcao do Efeito "Olho de Peixe" (Fisheye):
         * A distancia perpendicular ate a parede eh calculada ao inves da distancia Euclidiana 
         * reta, evitando que as paredes parecam arredondadas nas bordas da tela.
         */
        float perpWallDist = distanceUntilWall / fisheyeCorrection[x];
        if (perpWallDist < 0.1f) perpWallDist = 0.1f;

        ZBuffer[x] = perpWallDist;

        float factorDist = (float)SCREEN_HEIGHT * fisheyeCorrection[x];

        float texXWall = 0.0f;
        bool isSideWall = false;
        if (side == 0) {
            texXWall = hitY - floorf(hitY); 
            isSideWall = true;
        } else {
            texXWall = hitX - floorf(hitX); 
            isSideWall = false;
        }

        int ceiling = (int)(horizon - SCREEN_HEIGHT / perpWallDist);
        int floor = (int)(horizon + SCREEN_HEIGHT / perpWallDist);

        char npcFoundInColumn = RaycasterWorld::getNPCNext(titleMap, (int)hitX, (int)hitY, &mapMatrix);

        if (themeForest && charWall == '#') ceiling -= (int)(SCREEN_HEIGHT / perpWallDist * 1.5f); 
        if (themeForest && npcFoundInColumn == 'M') ceiling -= (int)(SCREEN_HEIGHT / perpWallDist * 1.2f);
        if (charWall == '*') ceiling -= (int)(SCREEN_HEIGHT / perpWallDist * 1.5f); 
        if (isKingdom && charWall == '|') ceiling -= (int)(SCREEN_HEIGHT / perpWallDist * 3.0f); // Portao super alto

        wallLights.clear();
        int nx = (side == 0) ? -stepX : 0;
        int ny = (side == 1) ? -stepY : 0;
        for (const auto& l : lights) {
            float sayLightX = std::get<0>(l) + 0.5f - hitX;
            float sayLightY = std::get<1>(l) + 0.5f - hitY;
            if (sayLightX * nx + sayLightY * ny >= -0.5f) {
                wallLights.push_back(l);
            }
        }

        float pushX = hitX + nx * 0.01f;
        float pushY = hitY + ny * 0.01f;
        Highlighter::InfoLight infoLightWall = Highlighter::calculateInfoLight(perpWallDist * 0.55f, depthMaximum, themeSky, wallLights, pushX, pushY, &mapMatrix, timeAbsolute);
        float angleSky = radiusAngle;
        if (themeSky == 0) { // Dynamic Outdoors
            angleSky -= globalAngleSlow;
        }

        int lastCeilingY = -1;
        Highlighter::InfoLight ceilingCurrentInfo, ceilingNextInfo;
        int lastFloorY = -1;
        Highlighter::InfoLight floorCurrentInfo, floorNextInfo;

        auto getCeilingInfoLight = [&](int y) -> Highlighter::InfoLight {
            if (y >= lastCeilingY + 2 || lastCeilingY == -1) {
                lastCeilingY = y - (y % 2);
                float dist0 = factorDist / ((float)horizon - lastCeilingY);
                float cx0 = playerX + eyeX * dist0;
                float cy0 = playerY + eyeY * dist0;
                ceilingCurrentInfo = Highlighter::calculateInfoLight(dist0, depthMaximum, themeSky, lights, cx0, cy0, &mapMatrix, timeAbsolute);
                
                int nextY = lastCeilingY + 2;
                float dist1 = factorDist / ((float)horizon - nextY);
                float cx1 = playerX + eyeX * dist1;
                float cy1 = playerY + eyeY * dist1;
                ceilingNextInfo = Highlighter::calculateInfoLight(dist1, depthMaximum, themeSky, lights, cx1, cy1, &mapMatrix, timeAbsolute);
            }
            Highlighter::InfoLight interp = ceilingCurrentInfo;
            float t = (float)(y % 2) / 2.0f;
            interp.lightR += (ceilingNextInfo.lightR - ceilingCurrentInfo.lightR) * t;
            interp.lightG += (ceilingNextInfo.lightG - ceilingCurrentInfo.lightG) * t;
            interp.lightB += (ceilingNextInfo.lightB - ceilingCurrentInfo.lightB) * t;
            interp.sunIntensity += (ceilingNextInfo.sunIntensity - ceilingCurrentInfo.sunIntensity) * t;
            interp.fogPercentage += (ceilingNextInfo.fogPercentage - ceilingCurrentInfo.fogPercentage) * t;
            return interp;
        };

        auto getFloorInfoLight = [&](int y) -> Highlighter::InfoLight {
            if (y >= lastFloorY + 2 || lastFloorY == -1) {
                lastFloorY = y - (y % 2);
                float dist0 = factorDist / ((float)lastFloorY - horizon);
                if (dist0 > 10000.0f || std::isnan(dist0)) dist0 = 10000.0f;
                float cx0 = playerX + eyeX * dist0;
                float cy0 = playerY + eyeY * dist0;
                floorCurrentInfo = Highlighter::calculateInfoLight(dist0, depthMaximum, themeSky, lights, cx0, cy0, &mapMatrix, timeAbsolute);
                
                int nextY = lastFloorY + 2;
                float dist1 = factorDist / ((float)nextY - horizon);
                if (dist1 > 10000.0f || std::isnan(dist1)) dist1 = 10000.0f;
                float cx1 = playerX + eyeX * dist1;
                float cy1 = playerY + eyeY * dist1;
                floorNextInfo = Highlighter::calculateInfoLight(dist1, depthMaximum, themeSky, lights, cx1, cy1, &mapMatrix, timeAbsolute);
            }
            Highlighter::InfoLight interp = floorCurrentInfo;
            float t = (float)(y % 2) / 2.0f;
            interp.lightR += (floorNextInfo.lightR - floorCurrentInfo.lightR) * t;
            interp.lightG += (floorNextInfo.lightG - floorCurrentInfo.lightG) * t;
            interp.lightB += (floorNextInfo.lightB - floorCurrentInfo.lightB) * t;
            interp.sunIntensity += (floorNextInfo.sunIntensity - floorCurrentInfo.sunIntensity) * t;
            interp.fogPercentage += (floorNextInfo.fogPercentage - floorCurrentInfo.fogPercentage) * t;
            return interp;
        };

        int startY = 0;
        int endCeiling = std::min(ceiling, SCREEN_HEIGHT);
        for (int y = startY; y < endCeiling; y++) {
            if (themeSky == 3) {
                float currentDist = factorDist / ((float)horizon - y);
                float currentX = playerX + eyeX * currentDist;
                float currentY = playerY + eyeY * currentDist;
                float fractionX = currentX - std::floor(currentX);
                float fractionY = currentY - std::floor(currentY);
                screen[y * SCREEN_WIDTH + x] = RaycasterWorld::getPixelWall(titleMap, themeForest, currentDist, depthMaximum, 'T', (int)(fractionY * 1000.0f), 0, 1000, fractionX, timeAbsolute, false, getCeilingInfoLight(y), currentX, currentY, ' ', 0.0f, 0.0f);
            } else {
                screen[y * SCREEN_WIDTH + x] = RaycasterWorld::getPixelCeiling(themeSky, radiusAngle, angleSky, y - bobbingOffset, SCREEN_HEIGHT, timeAbsolute);
            }
        }
        
        int startWall = std::max(0, ceiling);
        int endWall = std::min(floor, SCREEN_HEIGHT - 1);
        for (int y = startWall; y <= endWall; y++) {
            Pixel3D pixel = RaycasterWorld::getPixelWall(titleMap, themeForest, perpWallDist, depthMaximum, charWall, y, ceiling, floor, texXWall, timeAbsolute, isSideWall, infoLightWall, hitX, hitY, npcFoundInColumn, (float)nx, (float)ny);
            if (pixel.isBackground) {
                if (y <= horizon) {
                    if (themeSky == 3) {
                        float currentDist = factorDist / ((float)horizon - y);
                        float currentX = playerX + eyeX * currentDist;
                        float currentY = playerY + eyeY * currentDist;
                        float fractionX = currentX - std::floor(currentX);
                        float fractionY = currentY - std::floor(currentY);
                        screen[y * SCREEN_WIDTH + x] = RaycasterWorld::getPixelWall(titleMap, themeForest, currentDist, depthMaximum, 'T', (int)(fractionY * 1000.0f), 0, 1000, fractionX, timeAbsolute, false, getCeilingInfoLight(y), currentX, currentY, ' ', 0.0f, 0.0f);
                    } else {
                        screen[y * SCREEN_WIDTH + x] = RaycasterWorld::getPixelCeiling(themeSky, radiusAngle, angleSky, y - bobbingOffset, SCREEN_HEIGHT, timeAbsolute);
                    }
                } else {
                    float currentDist = factorDist / ((float)y - horizon);
                    if (currentDist > 10000.0f || std::isnan(currentDist)) currentDist = 10000.0f;
                    float currentX = playerX + eyeX * currentDist;
                    float currentY = playerY + eyeY * currentDist;
                    char floorChar = '.';
                    if (currentX >= 0 && currentX < widthMap && currentY >= 0 && currentY < heightMap) {
                        floorChar = mapMatrix[(int)currentY][(int)currentX];
                    }
                    if (floorChar == '~') screen[y * SCREEN_WIDTH + x] = RaycasterWorld::getPixelWater(currentX, currentY, currentDist, depthMaximum, radiusAngle, timeAbsolute, themeSky);
                    else screen[y * SCREEN_WIDTH + x] = RaycasterWorld::getFloorPixel(titleMap, currentX, currentY, currentDist, depthMaximum, getFloorInfoLight(y));
                }
            } else {
                screen[y * SCREEN_WIDTH + x] = pixel;
            }
        }
        
        int floorStart = std::max((int)horizon + 1, endWall + 1);
        for (int y = floorStart; y < SCREEN_HEIGHT; y++) {
            float currentDist = factorDist / ((float)y - horizon);
            if (currentDist > 10000.0f || std::isnan(currentDist)) currentDist = 10000.0f;
            float currentX = playerX + eyeX * currentDist;
            float currentY = playerY + eyeY * currentDist;
            char floorChar = '.';
            if (currentX >= 0 && currentX < widthMap && currentY >= 0 && currentY < heightMap) {
                floorChar = mapMatrix[(int)currentY][(int)currentX];
            }
            if (floorChar == '~') screen[y * SCREEN_WIDTH + x] = RaycasterWorld::getPixelWater(currentX, currentY, currentDist, depthMaximum, radiusAngle, timeAbsolute, themeSky);
            else screen[y * SCREEN_WIDTH + x] = RaycasterWorld::getFloorPixel(titleMap, currentX, currentY, currentDist, depthMaximum, getFloorInfoLight(y));
        }
        } // para x
        }); // lambda
    } // para i (tarefas)
    
    getThreadPool().execute(tasks);

    /*
     * Renderizacao de Sprites (Billboarding):
     * Apos as paredes (Z-Buffer) terem sido desenhadas, entidades (Inimigos, NPCs, Arvores) 
     * sao capturadas da matriz. O renderizador calcula a projecao 2D dessas entidades no 
     * plano da camera, aplicando escalonamentos baseados no tipo do inimigo e distancia.
     */
    struct SpriteProject { float x, y, dist; char c, sprCh; };
    std::vector<SpriteProject> spritesGlobal;
    for (int y = 0; y < heightMap; y++) {
        for (int x = 0; x < widthMap; x++) {
            char c = mapMatrix[y][x];
            if (RaycasterWorld::isEntity(c) && !RaycasterWorld::isMapLabel(x, y, mapMatrix)) {
                char sprCh = RaycasterWorld::getSpriteChar(x, y, c, titleMap);
                float dx = (x + 0.5f) - playerX;
                float dy = (y + 0.5f) - playerY;
                float distSq = dx*dx + dy*dy;
                spritesGlobal.push_back({x + 0.5f, y + 0.5f, distSq, c, sprCh});
            }
        }
    }

    // Boss correction
    for (auto& sp : spritesGlobal) {
        if (sp.sprCh == 'H') {
            sp.x = 54.0f; sp.y = 28.5f;
            float dx = sp.x - playerX;
            float dy = sp.y - playerY;
            sp.dist = dx*dx + dy*dy;
        }
    }

    std::sort(spritesGlobal.begin(), spritesGlobal.end(), [](const SpriteProject& a, const SpriteProject& b) { return a.dist > b.dist; });

    float sayX = cosf(angleVisa);
    float sayY = sinf(angleVisa);
    float planeX = -sinf(angleVisa) * tanf(fieldVisa / 2.0f);
    float planeY = cosf(angleVisa) * tanf(fieldVisa / 2.0f);

    for (const auto& sp : spritesGlobal) {
        float spriteX = sp.x - playerX;
        float spriteY = sp.y - playerY;

        float invDet = 1.0f / (planeX * sayY - sayX * planeY);
        float transformX = invDet * (sayY * spriteX - sayX * spriteY);
        float transformY = invDet * (-planeY * spriteX + planeX * spriteY);

        if (transformY <= 0.1f) continue;
        char renderCh = sp.sprCh;
        if (renderCh == '*' && themeForest) renderCh = 127;
        if (cacheSprites.count(renderCh) == 0) continue;
        const auto& sc = cacheSprites.at(renderCh);
        int spriteScreenX = (int)((SCREEN_WIDTH / 2) * (1 + transformX / transformY));
        int spriteHeightBase = std::abs((int)(SCREEN_HEIGHT / transformY));
        int spriteHeight = spriteHeightBase;
        
        float entityScale = 1.0f;
        bool isEnemy = false;
        
        if (sp.sprCh == 'G' || sp.sprCh == 'O' || sp.sprCh == 'S' || sp.sprCh == 'A' || sp.sprCh == 'T' || sp.sprCh == 'M' || sp.sprCh == 'F' || sp.sprCh == 'H') {
            float cbFactor = 2.5f; 
            switch(sp.sprCh) {
                case 'O': cbFactor = 2.7f; break; 
                case 'G': cbFactor = 2.5f; break; 
                case 'S': cbFactor = 2.5f; break; 
                case 'H': cbFactor = 3.0f; break; 
                case 'A': cbFactor = 1.5f; break; 
                case 'T': cbFactor = 1.9f; break; 
                case 'M': cbFactor = 2.5f; break; 
                case 'F': cbFactor = 3.2f; break; 
            }
            float combat_height = sc.height * (2.5f / cbFactor);
            entityScale = combat_height / 100.0f; 
            isEnemy = true;
        }
        else if (sp.sprCh == 'V' || sp.sprCh == 'Q' || sp.sprCh == 'Z' || sp.sprCh == 'J') {
            entityScale = 0.5f; 
        }
        else if (sp.sprCh == 'C') {
            entityScale = 0.6f;
        }
        else if (sp.sprCh == 'W' || sp.sprCh == 'B') {
            entityScale = 0.4f;
        }
        else if (sp.sprCh == '*') {
            // Arvores: manter o tamanho gigante que estava com o bug anterior (1.0 * 1.725), reduzido em 20% a pedido
            entityScale = 1.725f * 0.80f;
        }
        else if (sp.sprCh == '^' || (sp.sprCh >= '1' && sp.sprCh <= '5')) {
            // Portas: levemente maiores que o player (0.5f da camera) + aumentos acumulados
            entityScale = 0.6f * 1.25f * 1.50f;
        }

        if (isEnemy) {
            int mapSeeds = (int)(sp.x * 100) ^ (int)(sp.y * 100);
            float pct = ((mapSeeds % 101) - 50.0f) / 1000.0f; 
            entityScale *= (1.0f + pct);
            
            // Inimigos: escala base + 50%, e depois reduzido em 10%
            entityScale *= (1.5f * 0.90f);
            
            // Ajustes finos adicionais pedidos pelo usuario
            if (sp.sprCh == 'O') {
                entityScale *= (1.25f * 1.10f); // +10% em cima dos 25% anteriores
            } else if (sp.sprCh == 'S') {
                entityScale *= (1.25f * 1.10f); // +10% em cima dos 25% anteriores
            } else if (sp.sprCh == 'T') {
                entityScale *= (1.25f * 1.20f * 1.20f * 1.15f); // +15% extra
            }
        } 
        else if (sp.sprCh == 'V' || sp.sprCh == 'Q' || sp.sprCh == 'Z' || sp.sprCh == 'J' || sp.sprCh == 'C' || sp.sprCh == 'B' || sp.sprCh == 'W') {
            // NPCs: escala base + 50%, e depois aumentado em 15%
            entityScale *= (1.5f * 1.15f);
            
            // Ajuste fino para o Cavaleiro a pedido do usuario
            if (sp.sprCh == 'C') {
                entityScale *= 0.85f; // -15%
            }
        }
        
        spriteHeight = (int)(spriteHeightBase * entityScale);
        
        int floorY = (int)(horizon + spriteHeightBase); // Fixa os pes no chao
        int ceilingThen = floorY - (spriteHeight * 2);

        if (sp.sprCh == '*') ceilingThen -= (int)(spriteHeight * 1.5f);
        if (sp.sprCh != '^' && sp.sprCh != 'P' && sp.sprCh != 'X' && sp.sprCh != '*') {
            float offset = (float)sp.sprCh;
            ceilingThen -= (int)((sinf(timeAbsolute * 3.5f + offset)) * spriteHeight * 0.05f);
        }

        int altThen = floorY - ceilingThen;
        if (altThen <= 0) continue;

        int spriteWidth = spriteHeight;
        if (sp.sprCh == 'H') spriteWidth = (int)(spriteHeight * 4.0f); // Chefe e muito largo
        if (sp.sprCh == '*') spriteWidth = (int)(spriteHeight * 1.5f); 

        int drawStartX = -spriteWidth / 2 + spriteScreenX;
        if (drawStartX < 0) drawStartX = 0;
        int drawEndX = spriteWidth / 2 + spriteScreenX;
        if (drawEndX > SCREEN_WIDTH) drawEndX = SCREEN_WIDTH;

        bool isAnimated = (sp.sprCh == '^' || (sp.sprCh >= '1' && sp.sprCh <= '5'));

        for (int stripe = drawStartX; stripe < drawEndX; stripe++) {
            if (transformY < ZBuffer[stripe]) {
                int texX = (int)(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * sc.width / spriteWidth) / 256;
                if (texX < 0) texX = 0;
                if (texX >= sc.width) texX = sc.width - 1;

                int startY = std::max(0, ceilingThen);
                int endY = std::min(SCREEN_HEIGHT - 1, floorY);
                float stepY = (float)sc.height / (float)altThen;
                float texY = (startY - ceilingThen) * stepY;

                for (int y = startY; y <= endY; y++) {
                    int spriteY = (int)texY;
                    texY += stepY;

                    if (spriteY >= 0 && spriteY < sc.height) {
                        const SpritePixel& spix = sc.pixels[spriteY * sc.width + texX];
                        if (!spix.isTransparent) {
                            Pixel3D& px = screen[y * SCREEN_WIDTH + stripe];
                            px.r = spix.r;
                            px.g = spix.g;
                            px.b = spix.b;
                            px.ch = spix.ch;
                            px.fgR = spix.fgR;
                            px.fgG = spix.fgG;
                            px.fgB = spix.fgB;
                            px.hasFg = spix.hasFg;
                            px.isBackground = false;

                            if (spix.ch == ' ' && isAnimated) {
                                float wave = ManagerTextures::fastYes(timeAbsolute * 6.0f + y * 0.2f + texX * 0.2f);
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
