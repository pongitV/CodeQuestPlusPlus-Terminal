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
 * o laco principal de colunas da tela em paralelo com sincronizacao nativa (sem busy-spin), garantindo 60 FPS.
 */
struct ThreadPool {
    std::vector<std::thread> threads;
    std::atomic<bool> stop{false};
    
    std::mutex mtx;
    std::condition_variable cvTask;
    std::condition_variable cvDone;
    std::vector<std::function<void()>> tasks;
    int remainingTasks{0};
    
    ThreadPool() {
        int indexInThreads = std::thread::hardware_concurrency();
        if (indexInThreads == 0) indexInThreads = 4;
        for (int i = 0; i < indexInThreads; ++i) {
            threads.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        cvTask.wait(lock, [this]() { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.back());
                        tasks.pop_back();
                    }
                    task();
                    {
                        std::lock_guard<std::mutex> lock(mtx);
                        remainingTasks--;
                        if (remainingTasks == 0) {
                            cvDone.notify_all();
                        }
                    }
                }
            });
        }
    }
    
    ~ThreadPool() {
        stop = true;
        cvTask.notify_all();
        for (auto& t : threads) {
            if (t.joinable()) t.join();
        }
    }
    
    void execute(std::vector<std::function<void()>>& newTasks) {
        if (newTasks.empty()) return;
        {
            std::lock_guard<std::mutex> lock(mtx);
            tasks = std::move(newTasks);
            remainingTasks = static_cast<int>(tasks.size());
        }
        cvTask.notify_all();
        {
            std::unique_lock<std::mutex> lock(mtx);
            cvDone.wait(lock, [this]() { return remainingTasks == 0; });
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

void RaycasterRenderer::render3D(vector<Pixel3D>& screen, int SCREEN_WIDTH, int SCREEN_HEIGHT, float playerX, float playerY, float viewAngle, float horizon, int bobbingOffset, float depthMaximum, float timeAbsolute, const vector<string>& mapMatrix, const string& titleMap, bool themeForest, int themeSky, const map<char, SpriteCache>& cacheSprites) {
    ManagerTextures::boot();
    float fov = 3.14159f / 4.0f; // FOV 45 graus
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

    static thread_local std::vector<std::tuple<int, int, int>> s_lightsVisible;
    s_lightsVisible.clear();
    float maxDistLight = depthMaximum + 8.0f;
    float maxDistLightSq = maxDistLight * maxDistLight;
    for (const auto& l : cachedLights) {
        float dx = std::get<0>(l) - playerX;
        float dy = std::get<1>(l) - playerY;
        if (dx * dx + dy * dy <= maxDistLightSq) {
            s_lightsVisible.push_back(l);
        }
    }
    const auto& lights = s_lightsVisible;

    std::string upperTitle = titleMap;
    for (char& ch : upperTitle) ch = std::toupper(static_cast<unsigned char>(ch));
    bool isKingdom = (upperTitle.find("PATIO DO REINO") != std::string::npos || upperTitle.find("REINO") != std::string::npos);

    static thread_local std::vector<float> s_zBuffer;
    static thread_local std::vector<float> s_rayDirX;
    static thread_local std::vector<float> s_rayDirY;
    static thread_local std::vector<float> s_fisheyeCorrection;
    static thread_local std::vector<std::function<void()>> s_tasks;

    if (static_cast<int>(s_zBuffer.size()) != SCREEN_WIDTH) {
        s_zBuffer.assign(SCREEN_WIDTH, 0.0f);
        s_rayDirX.resize(SCREEN_WIDTH);
        s_rayDirY.resize(SCREEN_WIDTH);
        s_fisheyeCorrection.resize(SCREEN_WIDTH);
    } else {
        std::fill(s_zBuffer.begin(), s_zBuffer.end(), 0.0f);
    }
    auto& ZBuffer = s_zBuffer;
    auto& rayDirX = s_rayDirX;
    auto& rayDirY = s_rayDirY;
    auto& fisheyeCorrection = s_fisheyeCorrection;

    long long globalMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    float globalAngleSlow = ((globalMs % 300000) / 300000.0f) * 6.2831853f;

    for (int x = 0; x < SCREEN_WIDTH; x++) {
        float rayAngle = (viewAngle - fov / 2.0f) + ((float)x / (float)SCREEN_WIDTH) * fov;
        rayDirX[x] = cosf(rayAngle);
        rayDirY[x] = sinf(rayAngle);
        fisheyeCorrection[x] = 1.0f / cosf(rayAngle - viewAngle);
    }

    int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 4;
    int chunkSize = 16;
    int numChunks = (SCREEN_WIDTH + chunkSize - 1) / chunkSize;
    
    s_tasks.clear();
    if (s_tasks.capacity() < static_cast<size_t>(numChunks)) {
        s_tasks.reserve(numChunks);
    }

    for (int i = 0; i < numChunks; i++) {
        int startX = i * chunkSize;
        int endX = std::min(startX + chunkSize, SCREEN_WIDTH);
        s_tasks.push_back([&, startX, endX, globalAngleSlow]() {
            static thread_local std::vector<std::tuple<int, int, int>> wallLights;
            
            for (int x = startX; x < endX; x++) {
        float rayAngle = (viewAngle - fov / 2.0f) + ((float)x / (float)SCREEN_WIDTH) * fov;
        float distToWall = 0.0f;
        bool hitWall = false;

        float eyeX = rayDirX[x];
        float eyeY = rayDirY[x];
        
        char charWall = '#';

        /*
         * Algoritmo DDA (Digital Differential Analyzer):
         * Traca a trajetoria do raio pulando perfeitamente pelas grades do mapa de forma rapida,
         * sem a necessidade de pequenos incrementos variaveis, calculando a colisao exata.
         */
        float rayDirX_DDA = eyeX;
        float rayDirY_DDA = eyeY;

        int mapX = (int)playerX;
        int mapY = (int)playerY;

        float deltaDistX = (rayDirX_DDA == 0.0f) ? 1e30f : std::abs(1.0f / rayDirX_DDA);
        float deltaDistY = (rayDirY_DDA == 0.0f) ? 1e30f : std::abs(1.0f / rayDirY_DDA);

        int stepX, stepY;
        float sideDistX, sideDistY;

        if (rayDirX_DDA < 0) {
            stepX = -1;
            sideDistX = (playerX - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0f - playerX) * deltaDistX;
        }
        if (rayDirY_DDA < 0) {
            stepY = -1;
            sideDistY = (playerY - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0f - playerY) * deltaDistY;
        }

        int side = 0;

        while (!hitWall && distToWall < depthMaximum) {
            if (sideDistX < sideDistY) {
                distToWall = sideDistX;
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                distToWall = sideDistY;
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }

            if (mapX < 0 || mapX >= widthMap || mapY < 0 || mapY >= heightMap) {
                hitWall = true;
                distToWall = depthMaximum;
            } else {
                char c = mapMatrix[mapY][mapX];
                if (c != '.' && c != ' ' && c != '~' && c != ',') {
                    if (!RaycasterWorld::isMapLabel(mapX, mapY, mapMatrix) && !RaycasterWorld::isEntity(c)) {
                        hitWall = true;
                        charWall = c;
                    }
                }
            }
        } 

        float hitX = playerX + eyeX * distToWall;
        float hitY = playerY + eyeY * distToWall;
        
        /* 
         * Correcao do Efeito "Olho de Peixe" (Fisheye):
         * A distancia perpendicular ate a parede eh calculada ao inves da distancia Euclidiana 
         * reta, evitando que as paredes parecam arredondadas nas bordas da tela.
         */
        float perpWallDist = distToWall / fisheyeCorrection[x];
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
        if (isKingdom && charWall == '|' && npcFoundInColumn == ' ') ceiling -= (int)(SCREEN_HEIGHT / perpWallDist * 1.5f); // Portao alto do castelo

        wallLights.clear();
        int nx = (side == 0) ? -stepX : 0;
        int ny = (side == 1) ? -stepY : 0;
        for (const auto& l : lights) {
            float dirLightX = std::get<0>(l) + 0.5f - hitX;
            float dirLightY = std::get<1>(l) + 0.5f - hitY;
            if (dirLightX * nx + dirLightY * ny >= -0.5f) {
                wallLights.push_back(l);
            }
        }

        float pushX = hitX + nx * 0.01f;
        float pushY = hitY + ny * 0.01f;
        Illuminator::InfoLight infoLightWall = Illuminator::calculateInfoLight(perpWallDist * 0.55f, depthMaximum, themeSky, wallLights, pushX, pushY, &mapMatrix, timeAbsolute);
        float angleSky = rayAngle;
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
                screen[y * SCREEN_WIDTH + x] = RaycasterWorld::getPixelCeiling(themeSky, rayAngle, angleSky, y - bobbingOffset, SCREEN_HEIGHT, timeAbsolute);
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
                        screen[y * SCREEN_WIDTH + x] = RaycasterWorld::getPixelCeiling(themeSky, rayAngle, angleSky, y - bobbingOffset, SCREEN_HEIGHT, timeAbsolute);
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
                    if (floorChar == '~') screen[y * SCREEN_WIDTH + x] = RaycasterWorld::getPixelWater(currentX, currentY, currentDist, depthMaximum, rayAngle, timeAbsolute, themeSky);
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
            if (floorChar == '~') screen[y * SCREEN_WIDTH + x] = RaycasterWorld::getPixelWater(currentX, currentY, currentDist, depthMaximum, rayAngle, timeAbsolute, themeSky);
            else screen[y * SCREEN_WIDTH + x] = RaycasterWorld::getFloorPixel(titleMap, currentX, currentY, currentDist, depthMaximum, getFloorInfoLight(y));
        }
        } // para x
        }); // lambda
    } // para i (tarefas)
    
    getThreadPool().execute(s_tasks);

    /*
     * Renderizacao de Sprites (Billboarding):
     * Apos as paredes (Z-Buffer) terem sido desenhadas, entidades (Inimigos, NPCs, Arvores) 
     * sao capturadas da matriz. O renderizador calcula a projecao 2D dessas entidades no 
     * plano da camera, aplicando escalonamentos baseados no tipo do inimigo e distancia.
     */
    struct SpriteProject { float x, y, dist; char c, sprCh; };
    static thread_local std::vector<SpriteProject> s_spritesGlobal;
    s_spritesGlobal.clear();
    auto& spritesGlobal = s_spritesGlobal;

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

    float dirX = cosf(viewAngle);
    float dirY = sinf(viewAngle);
    float planeX = -sinf(viewAngle) * tanf(fov / 2.0f);
    float planeY = cosf(viewAngle) * tanf(fov / 2.0f);

    for (const auto& sp : spritesGlobal) {
        float spriteX = sp.x - playerX;
        float spriteY = sp.y - playerY;

        float invDet = 1.0f / (planeX * dirY - dirX * planeY);
        float transformX = invDet * (dirY * spriteX - dirX * spriteY);
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
            // [PT-BR] Escala calibrada para folhagem de arvores no mundo 3D
            // [EN-US] Calibrated scale for world tree foliage
            constexpr float TREE_SCALE = 1.38f;
            entityScale = TREE_SCALE;
        }
        else if (sp.sprCh == '^' || (sp.sprCh >= '1' && sp.sprCh <= '5')) {
            // [PT-BR] Escala vertical destacada para portais e passagens
            // [EN-US] Heightened vertical scale for doorways and portals
            constexpr float PORTAL_SCALE = 1.125f;
            entityScale = PORTAL_SCALE;
        }

        if (isEnemy) {
            int mapSeeds = (int)(sp.x * 100) ^ (int)(sp.y * 100);
            float pct = ((mapSeeds % 101) - 50.0f) / 1000.0f; 
            entityScale *= (1.0f + pct);
            
            // [PT-BR] Escala base padronizada para entidades hostis
            // [EN-US] Standardized base scale for hostile entities
            constexpr float ENEMY_BASE_SCALE = 1.35f;
            entityScale *= ENEMY_BASE_SCALE;
            
            // [PT-BR] Fatores de escala calibrados por tipo de criatura
            // [EN-US] Calibrated scale factors per creature type
            if (sp.sprCh == 'O' || sp.sprCh == 'S') {
                constexpr float ORC_SLIME_SCALE = 1.375f;
                entityScale *= ORC_SLIME_SCALE;
            } else if (sp.sprCh == 'T') {
                constexpr float TROLL_SCALE = 2.07f;
                entityScale *= TROLL_SCALE;
            }
        } 
        else if (sp.sprCh == 'V' || sp.sprCh == 'Q' || sp.sprCh == 'Z' || sp.sprCh == 'J' || sp.sprCh == 'C' || sp.sprCh == 'B' || sp.sprCh == 'W') {
            // [PT-BR] Escala base padronizada para NPCs aliados e neutros
            // [EN-US] Standardized base scale for friendly and neutral NPCs
            constexpr float NPC_BASE_SCALE = 1.725f;
            entityScale *= NPC_BASE_SCALE;
            
            if (sp.sprCh == 'C') {
                // [PT-BR] Ajuste proporcional para a armadura do Cavaleiro
                // [EN-US] Proportional adjustment for Knight sprite dimensions
                constexpr float KNIGHT_SCALE_ADJUST = 0.85f;
                entityScale *= KNIGHT_SCALE_ADJUST;
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
