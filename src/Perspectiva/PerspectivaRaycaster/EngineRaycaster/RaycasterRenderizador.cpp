#include "RaycasterRenderizador.h"
#include "GerenciadorTexturas.h"
#include "RaycasterMundo.h"
#include <cmath>
#include <algorithm>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>

using namespace std;

struct ThreadPool {
    std::vector<std::thread> threads;
    std::atomic<bool> stop{false};
    std::atomic<int> activeTasks{0};
    
    std::mutex mtx;
    std::condition_variable cv;
    std::vector<std::function<void()>> tasks;
    
    ThreadPool() {
        int numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 4;
        for (int i = 0; i < numThreads; ++i) {
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

static ThreadPool& obterThreadPool() {
    static ThreadPool pool;
    return pool;
}

struct EntidadeAtingida {
    float dist;
    char c;
    float texX;
};

void RaycasterRenderizador::renderizar3D(vector<Pixel3D>& tela, int LARGURA_TELA, int ALTURA_TELA, float jogadorX, float jogadorY, float anguloVisao, float horizonte, int bobbingOffset, float profundidadeMaxima, float tempoAbsoluto, const vector<string>& matrizDoMapa, const string& tituloMapa, bool temaFloresta, int temaCeu, const map<char, SpriteCache>& cacheSprites) {
    GerenciadorTexturas::inicializar();
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

                char mappedC = RaycasterMundo::obterSpriteChar(lx, ly, c, tituloMapa);
                if (RaycasterMundo::isMapLabel(lx, ly, matrizDoMapa)) continue;

                if (mappedC == '^' || mappedC == '1' || mappedC == '2' || mappedC == '3' || mappedC == '4' || mappedC == '5') {
                    cachedLuzes.push_back({lx, ly, 1}); // Teleport/Door (Soft Brown)
                } else if (mappedC == 'F') {
                    cachedLuzes.push_back({lx, ly, 0}); // Fire (Orange)
                } else if (mappedC == 'G' || mappedC == 'O' || mappedC == 'S' || mappedC == 'A' || mappedC == 'M' || mappedC == 'T' || mappedC == 'Y') {
                    cachedLuzes.push_back({lx, ly, 2}); // Enemy (Red)
                } else if (mappedC == 'H' || mappedC == 'R' || mappedC == 'P' || mappedC == 'Q' || mappedC == 'B' || mappedC == 'W' || mappedC == 'V' || mappedC == 'C' || mappedC == 'J' || mappedC == 'K' || mappedC == 'Z') {
                    cachedLuzes.push_back({lx, ly, 3}); // NPC (Yellow)
                }
            }
        }
    }

    std::vector<std::tuple<int, int, int>> luzesVisiveis;
    float maxDistLuz = profundidadeMaxima + 8.0f;
    float maxDistLuzSq = maxDistLuz * maxDistLuz;
    for (const auto& l : cachedLuzes) {
        float dx = std::get<0>(l) - jogadorX;
        float dy = std::get<1>(l) - jogadorY;
        if (dx * dx + dy * dy <= maxDistLuzSq) {
            luzesVisiveis.push_back(l);
        }
    }
    const auto& luzes = luzesVisiveis;

    std::string tituloUpper = tituloMapa;
    for (char& ch : tituloUpper) ch = std::toupper(static_cast<unsigned char>(ch));
    bool isReino = (tituloUpper.find("PATIO DO REINO") != std::string::npos || tituloUpper.find("REINO") != std::string::npos);

    std::vector<float> ZBuffer(LARGURA_TELA, 0.0f);

    int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 4;
    int chunkSize = 16;
    int numChunks = (LARGURA_TELA + chunkSize - 1) / chunkSize;
    std::vector<std::function<void()>> tasks;

    for (int i = 0; i < numChunks; i++) {
        int startX = i * chunkSize;
        int endX = std::min(startX + chunkSize, LARGURA_TELA);
        tasks.push_back([&, startX, endX]() {
            for (int x = startX; x < endX; x++) {
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

        float invCosFisheye = 1.0f / cosf(raioAngulo - anguloVisao);
        float factorDist = (float)ALTURA_TELA * invCosFisheye;

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

        std::vector<std::tuple<int, int, int>> luzesDaParede;
        int nx = (side == 0) ? -stepX : 0;
        int ny = (side == 1) ? -stepY : 0;
        for (const auto& l : luzes) {
            float dirLuzX = std::get<0>(l) + 0.5f - hitX;
            float dirLuzY = std::get<1>(l) + 0.5f - hitY;
            if (dirLuzX * nx + dirLuzY * ny >= -0.5f) {
                luzesDaParede.push_back(l);
            }
        }

        float pushX = hitX + nx * 0.01f;
        float pushY = hitY + ny * 0.01f;
        Iluminador::InfoLuz infoLuzParede = Iluminador::calcularInfoLuz(perpWallDist * 0.55f, profundidadeMaxima, temaCeu, luzesDaParede, pushX, pushY, &matrizDoMapa, tempoAbsoluto);
        float anguloCeu = raioAngulo;
        if (temaCeu == 0) { // Dynamic Outdoors
            long long globalMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            float anguloLento = ((globalMs % 300000) / 300000.0f) * 6.2831853f;
            anguloCeu -= anguloLento;
        }

        auto drawIndoorCeiling = [&](int screenY, float currentX, float currentY, float currentDist) {
            (void)screenY;
            float fractX = currentX - std::floor(currentX);
            float fractY = currentY - std::floor(currentY);
            char charTeto = '#';
            Iluminador::InfoLuz infoLuzTeto = Iluminador::calcularInfoLuz(currentDist, profundidadeMaxima, temaCeu, luzes, currentX, currentY, &matrizDoMapa, tempoAbsoluto);
            return RaycasterMundo::obterPixelParede(tituloMapa, temaFloresta, currentDist, profundidadeMaxima, charTeto, (int)(fractY * 1000.0f), 0, 1000, fractX, tempoAbsoluto, false, infoLuzTeto, currentX, currentY, ' ', 0.0f, 0.0f);
        };

        int startY = 0;
        int endTeto = std::min(teto, ALTURA_TELA);
        for (int y = startY; y < endTeto; y++) {
            if (temaCeu == 3) {
                float currentDist = factorDist / ((float)horizonte - y);
                float currentX = jogadorX + olhoX * currentDist;
                float currentY = jogadorY + olhoY * currentDist;
                tela[y * LARGURA_TELA + x] = drawIndoorCeiling(y, currentX, currentY, currentDist);
            } else {
                tela[y * LARGURA_TELA + x] = RaycasterMundo::obterPixelTeto(temaCeu, raioAngulo, anguloCeu, y - bobbingOffset, ALTURA_TELA, tempoAbsoluto);
            }
        }
        
        int startParede = std::max(0, teto);
        int endParede = std::min(chao, ALTURA_TELA - 1);
        for (int y = startParede; y <= endParede; y++) {
            Pixel3D pixel = RaycasterMundo::obterPixelParede(tituloMapa, temaFloresta, perpWallDist, profundidadeMaxima, charParede, y, teto, chao, texXParede, tempoAbsoluto, isSideWall, infoLuzParede, hitX, hitY, npcEncontradoNaColuna, (float)nx, (float)ny);
            if (pixel.isFundo) {
                if (y <= horizonte) {
                    if (temaCeu == 3) {
                        float currentDist = factorDist / ((float)horizonte - y);
                        float currentX = jogadorX + olhoX * currentDist;
                        float currentY = jogadorY + olhoY * currentDist;
                        tela[y * LARGURA_TELA + x] = drawIndoorCeiling(y, currentX, currentY, currentDist);
                    } else {
                        tela[y * LARGURA_TELA + x] = RaycasterMundo::obterPixelTeto(temaCeu, raioAngulo, anguloCeu, y - bobbingOffset, ALTURA_TELA, tempoAbsoluto);
                    }
                } else {
                    float currentDist = factorDist / ((float)y - horizonte);
                    float currentX = jogadorX + olhoX * currentDist;
                    float currentY = jogadorY + olhoY * currentDist;
                    char floorChar = '.';
                    if (currentX >= 0 && currentX < larguraMapa && currentY >= 0 && currentY < alturaMapa) {
                        floorChar = matrizDoMapa[(int)currentY][(int)currentX];
                    }
                    if (floorChar == '~') tela[y * LARGURA_TELA + x] = RaycasterMundo::obterPixelAgua(currentX, currentY, currentDist, profundidadeMaxima, raioAngulo, tempoAbsoluto, temaCeu);
                    else tela[y * LARGURA_TELA + x] = RaycasterMundo::obterPixelChao(tituloMapa, currentX, currentY, currentDist, profundidadeMaxima, luzes, &matrizDoMapa, tempoAbsoluto);
                }
            } else {
                tela[y * LARGURA_TELA + x] = pixel;
            }
        }
        
        int startChao = std::max((int)horizonte + 1, endParede + 1);
        for (int y = startChao; y < ALTURA_TELA; y++) {
            float currentDist = factorDist / ((float)y - horizonte);
            float currentX = jogadorX + olhoX * currentDist;
            float currentY = jogadorY + olhoY * currentDist;
            char floorChar = '.';
            if (currentX >= 0 && currentX < larguraMapa && currentY >= 0 && currentY < alturaMapa) {
                floorChar = matrizDoMapa[(int)currentY][(int)currentX];
            }
            if (floorChar == '~') tela[y * LARGURA_TELA + x] = RaycasterMundo::obterPixelAgua(currentX, currentY, currentDist, profundidadeMaxima, raioAngulo, tempoAbsoluto, temaCeu);
            else tela[y * LARGURA_TELA + x] = RaycasterMundo::obterPixelChao(tituloMapa, currentX, currentY, currentDist, profundidadeMaxima, luzes, &matrizDoMapa, tempoAbsoluto);
        }
        } // for x
        }); // lambda
    } // for i (tasks)
    
    obterThreadPool().execute(tasks);

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
        char renderCh = sp.sprCh;
        if (renderCh == '*' && temaFloresta) renderCh = 127;
        if (cacheSprites.count(renderCh) == 0) continue;
        const auto& sc = cacheSprites.at(renderCh);
        int spriteScreenX = (int)((LARGURA_TELA / 2) * (1 + transformX / transformY));
        int spriteHeightBase = std::abs((int)(ALTURA_TELA / transformY));
        int spriteHeight = spriteHeightBase;
        
        float entityScale = 1.0f;
        bool isEnemy = false;
        
        if (sp.sprCh == 'G' || sp.sprCh == 'O' || sp.sprCh == 'S' || sp.sprCh == 'A' || sp.sprCh == 'T' || sp.sprCh == 'M' || sp.sprCh == 'F' || sp.sprCh == 'H') {
            float cbFator = 2.5f; 
            switch(sp.sprCh) {
                case 'O': cbFator = 2.7f; break; 
                case 'G': cbFator = 2.5f; break; 
                case 'S': cbFator = 2.5f; break; 
                case 'H': cbFator = 3.0f; break; 
                case 'A': cbFator = 1.5f; break; 
                case 'T': cbFator = 1.9f; break; 
                case 'M': cbFator = 2.5f; break; 
                case 'F': cbFator = 3.2f; break; 
            }
            float combat_height = sc.height * (2.5f / cbFator);
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
            int mapSeed = (int)(sp.x * 100) ^ (int)(sp.y * 100);
            float pct = ((mapSeed % 101) - 50.0f) / 1000.0f; 
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
        
        int chaoEnt = (int)(horizonte + spriteHeightBase); // Fixa os pes no chao
        int tetoEnt = chaoEnt - (spriteHeight * 2);

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
                                float wave = GerenciadorTexturas::fastSin(tempoAbsoluto * 6.0f + y * 0.2f + texX * 0.2f);
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
