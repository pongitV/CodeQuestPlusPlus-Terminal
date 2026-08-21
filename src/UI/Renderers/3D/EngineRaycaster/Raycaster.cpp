#include "UI/Renderers/3D/EngineRaycaster/Raycaster.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "UI/Screens/Combat/CombatScreen.h"
#include "Systems/Inventory/CombatInventory.h"
#include "UI/Screens/Attributes/AttributesScreen.h"
#include "UI/Screens/Diary/DiaryScreen.h"
#include "UI/Screens/Pause/PauseScreen.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterSprites.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterEnemies.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterNPCs.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterWorld.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterHUD.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterRendererBase.h"
#include "Core/Engine/Debug.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterControls.h"
#include <map>
#include <iostream>
#include <fstream>
#include <cmath>
#include <chrono>
#include <thread>
#include <string_view>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#endif

#include "World/Village/Map1VillageLayout.h"
#include "World/Forest/Map2ForestLayout.h"
#include "World/Kingdom/Map3KingdomBridgeLayout.h"
#include "World/Kingdom/Map4KingdomLayout.h"

using namespace std;

float Raycaster::sensitivityX = 0.0008f;
float Raycaster::sensitivityY = 0.048f;

std::string RaycasterFrame::s_lastFrameRendered;

void RaycasterFrame::restoreLastFrame() {
    if (s_lastFrameRendered.empty()) return;
    std::cout << "\033[0m" << s_lastFrameRendered << std::flush;
}

static inline char* writeByteFast(char* p, uint8_t val) {
    if (val >= 100) {
        *p++ = '0' + (val / 100);
        *p++ = '0' + ((val / 10) % 10);
        *p++ = '0' + (val % 10);
    } else if (val >= 10) {
        *p++ = '0' + (val / 10);
        *p++ = '0' + (val % 10);
    } else {
        *p++ = '0' + val;
    }
    return p;
}

static inline char* writeAnsiColorFast(char* p, int type, uint8_t r, uint8_t g, uint8_t b) {
    *p++ = '\033';
    *p++ = '[';
    if (type == 38) {
        *p++ = '3';
        *p++ = '8';
    } else {
        *p++ = '4';
        *p++ = '8';
    }
    *p++ = ';';
    *p++ = '2';
    *p++ = ';';
    p = writeByteFast(p, r);
    *p++ = ';';
    p = writeByteFast(p, g);
    *p++ = ';';
    p = writeByteFast(p, b);
    *p++ = 'm';
    return p;
}

static inline void writeAnsiPixel(std::string& s, const Pixel3D& px) {
    char buff[64];
    char* p = buff;
    p = writeAnsiColorFast(p, 48, px.r, px.g, px.b);
    if (px.hasFg) {
        p = writeAnsiColorFast(p, 38, px.fgR, px.fgG, px.fgB);
    }
    *p++ = px.ch;
    s.assign(buff, p - buff);
}

static void downsampleScreenBuffer(const vector<Pixel3D>& screen3D, vector<string>& screen, int SCREEN_WIDTH, int SCREEN_HEIGHT) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            const Pixel3D& top = screen3D[(y * 2) * SCREEN_WIDTH + x];
            const Pixel3D& bot = screen3D[(y * 2 + 1) * SCREEN_WIDTH + x];
            
            std::string& combined = screen[y * SCREEN_WIDTH + x];
            
            if (top.ch == ' ' && bot.ch == ' ') {
                char buff[64];
                char* p = buff;
                p = writeAnsiColorFast(p, 48, bot.r, bot.g, bot.b);
                p = writeAnsiColorFast(p, 38, top.r, top.g, top.b);
                *p++ = '\xE2';
                *p++ = '\x96';
                *p++ = '\x80';
                combined.assign(buff, p - buff);
            } else if (top.ch != ' ') {
                writeAnsiPixel(combined, top);
            } else {
                writeAnsiPixel(combined, bot);
            }
        }
    }
}

char Raycaster::start3DExploration(const vector<string>& mapMatrix, float& playerX, float& playerY, float& angleVisa, const string& titleMap, Character* player, int& outHitX, int& outHitY, int typeAnimationEntry) {
    outHitX = -1;
    outHitY = -1;
    if (mapMatrix.empty() || !player) return 0;

    bool themeForest = RaycasterWorld::isThemeForest(titleMap);
    int themeSky = RaycasterWorld::getThemeSky(titleMap);

    int SCREEN_WIDTH = Appearance::getTerminalWidth();
    int SCREEN_HEIGHT = Appearance::getTerminalHeight();
    if (SCREEN_WIDTH <= 0) SCREEN_WIDTH = 120;
    if (SCREEN_HEIGHT <= 0) SCREEN_HEIGHT = 40;
    
    map<char, SpriteCache> cacheSprites;
    RaycasterEnemies::bootSprites(cacheSprites);
    RaycasterNPCs::bootSprites(cacheSprites);



    float depthMaximum = 150.0f;  // Profundidade infinita/maxima do mapa
    float speedMovement = 5.0f;


    Appearance::clearScreen();
    cout << "\033[?25l";

    auto tp1 = chrono::steady_clock::now();
    auto tp2 = chrono::steady_clock::now();
    static auto timeHome = chrono::steady_clock::now();
    float bobbingTeam = 0.0f;
    float bobbingAmplitude = 0.0f;
    float pitchOffset = 0.0f;
    int bobbingOffset = 0;

    int HEIGHT_INTERNAL = SCREEN_HEIGHT * 2;
    vector<Pixel3D> screen3D(SCREEN_WIDTH * HEIGHT_INTERNAL);
    vector<string> screen(SCREEN_WIDTH * SCREEN_HEIGHT);

    auto downsampleScreen = [&]() {
        downsampleScreenBuffer(screen3D, screen, SCREEN_WIDTH, SCREEN_HEIGHT);
    };

    auto cheerEye = [&](bool opening, const vector<string>& frameBase) {
        int maxSteps = 8; 
        for (int step = 0; step <= maxSteps; step++) {
            int p = opening ? step : (maxSteps - step);
            float openingPercentage = (float)p / maxSteps;
            
            string bufferFrame = "\033[?2026h\033[?25l\033[H";
            bufferFrame.reserve(SCREEN_WIDTH * SCREEN_HEIGHT * 15);
            int centerY = SCREEN_HEIGHT / 2;
            int centerX = SCREEN_WIDTH / 2;
            
            for (int y = 0; y < SCREEN_HEIGHT; y++) {
                for (int x = 0; x < SCREEN_WIDTH; x++) {
                    if (y == SCREEN_HEIGHT - 1 && x == SCREEN_WIDTH - 1) break;
                    
                    float dx = std::abs(x - centerX);
                    float rx = dx / (float)centerX; 
                    
                    float heightAtX = -1.0f;
                    if (openingPercentage > 0.0f) {
                        heightAtX = (SCREEN_HEIGHT * 0.6f * (1.0f - rx*rx) + SCREEN_HEIGHT * 0.5f) * openingPercentage; 
                    }
                    
                    if (std::abs(y - centerY) > heightAtX) {
                        bufferFrame += "\033[40m \033[0m";
                    } else {
                        const string& hs = frameBase[y * SCREEN_WIDTH + x];
                        if (hs.empty()) {
                            bufferFrame += " ";
                        } else if (hs[0] != '\1') {
                            bufferFrame += hs;
                        }
                    }
                }
                if (y < SCREEN_HEIGHT - 1) bufferFrame += "\n";
            }
            cout << bufferFrame << "\033[?2026l" << flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    };

    auto cheerDoorOpening = [&]() {
        int maxSteps = 11;
        for (int step = 0; step <= maxSteps; step++) {
            float percentage = (float)step / maxSteps;
            string bufferFrame = "\033[?2026h\033[?25l\033[H";
            bufferFrame.reserve(SCREEN_WIDTH * SCREEN_HEIGHT * 15);
            
            int doorLeftOffset = (int)(SCREEN_WIDTH * percentage); 
            
            for (int y = 0; y < SCREEN_HEIGHT; y++) {
                for (int x = 0; x < SCREEN_WIDTH; x++) {
                    if (y == SCREEN_HEIGHT - 1 && x == SCREEN_WIDTH - 1) break;
                    
                    if (x < SCREEN_WIDTH - doorLeftOffset) {
                        bufferFrame += "\033[48;2;10;10;10m \033[0m";
                    } else {
                        const string& hs = screen[y * SCREEN_WIDTH + x];
                        if (hs.empty()) {
                            bufferFrame += " ";
                        } else if (hs[0] != '\1') {
                            bufferFrame += hs;
                        }
                    }
                }
                if (y < SCREEN_HEIGHT - 1) bufferFrame += "\n";
            }
            cout << bufferFrame << "\033[?2026l" << flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(22));
        }
    };

    auto cheerBanner3D = [&](const string& title) {
        vector<string> banner;
        string upper = title;
        for(char& c : upper) c = toupper((unsigned char)c);
        
        if (upper.find("VILA") != string::npos) banner = Map1VillageLayouts::getVillageLogo();
        else if (upper.find("INICIO") != string::npos) banner = Map1VillageLayouts::getSpawnLogo();
        else if (upper.find("FLORESTA") != string::npos) banner = Map2ForestLayouts::getForestLogo();
        else if (upper.find("PONTE DO REINO") != string::npos || upper.find("PONTE") != string::npos) banner = Map3KingdomBridgeLayouts::getKingdomBridgeLogo();
        else if (upper.find("REINO") != string::npos) banner = Map4KingdomLayouts::getKingdomLogo();
        else {
            banner = {
                "==================================",
                "   " + title,
                "=================================="
            };
        }
        
        for (auto& l : banner) {
            l = Appearance::removeANSIColors(l);
        }
        
        int bannerWidth = 0;
        for (const auto& l : banner) {
            int w = Appearance::getVisualLength(l);
            if (w > bannerWidth) bannerWidth = w;
        }
        
        int startX = (SCREEN_WIDTH - bannerWidth) / 2;
        if (startX < 0) startX = 0;
        
        int bannerY = 2;
        
        // Decompoe cada linha do banner em caracteres visuais individuais
        vector<vector<string>> bannerChars(banner.size());
        for (size_t i = 0; i < banner.size(); i++) {
            bannerChars[i] = Appearance::splitUtf8(banner[i]);
        }
        
        // Renderiza a cena base uma unica vez (sem banner)
        {
            string sceneInitial;
            sceneInitial.reserve(SCREEN_WIDTH * SCREEN_HEIGHT * 15);
            sceneInitial += "\033[?2026h\033[?25l\033[H";
            for (int y = 0; y < SCREEN_HEIGHT; y++) {
                for (int x = 0; x < SCREEN_WIDTH; x++) {
                    if (y == SCREEN_HEIGHT - 1 && x == SCREEN_WIDTH - 1) break;
                    const string& hs = screen[y * SCREEN_WIDTH + x];
                    if (hs.empty()) {
                        sceneInitial += " ";
                    } else if (hs[0] != '\1') {
                        sceneInitial += hs;
                    }
                }
                if (y < SCREEN_HEIGHT - 1) sceneInitial += "\n";
            }
            std::cout << sceneInitial << "\033[?2026l" << std::flush;
        }
        
        // Fade-in: atualiza apenas as linhas do banner via posicionamento de cursor
        int maxSteps = 15;
        for (int step = 0; step <= maxSteps; step++) {
            float opacity = (float)step / maxSteps;
            int c = (int)(255 * opacity);
            string colorBanner = "\033[38;2;" + to_string(c) + ";" + to_string(c) + ";" + to_string(c) + "m";
            
            std::ostringstream buffer;
            for (size_t i = 0; i < banner.size(); i++) {
                int drawY = bannerY + i;
                if (drawY >= SCREEN_HEIGHT) continue;
                
                buffer << "\033[" << (drawY + 1) << ";1H";
                for (int x = 0; x < SCREEN_WIDTH; x++) {
                    int bannerCol = x - startX;
                    if (bannerCol >= 0 && bannerCol < (int)bannerChars[i].size() && bannerChars[i][bannerCol] != " ") {
                        buffer << colorBanner << bannerChars[i][bannerCol] << "\033[0m";
                    } else {
                        const string& hs = screen[drawY * SCREEN_WIDTH + x];
                        if (hs.empty()) {
                            buffer << " ";
                        } else if (hs[0] != '\1') {
                            buffer << hs;
                        }
                    }
                }
                buffer << "\033[K";
            }
            std::cout << buffer.str() << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    };

    int themeActiveInitial = themeSky;
    if (themeSky == 1 || themeSky == 2) {
        long long globalMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        float angleGlobal = ((globalMs % 60000) / 60000.0f) * 6.2831853f;
        if (angleGlobal > 1.5707f && angleGlobal < 4.7123f) {
            themeActiveInitial = 1; 
        } else {
            themeActiveInitial = 2; 
        }
    }
    
    auto tpNow = std::chrono::steady_clock::now();
    std::chrono::duration<float> diffHomeInitial = tpNow - timeHome;
    float timeAbsoluteInitial = diffHomeInitial.count();

    RaycasterRenderer::render3D(screen3D, SCREEN_WIDTH, HEIGHT_INTERNAL, playerX, playerY, angleVisa, (HEIGHT_INTERNAL / 2.0f), 0, depthMaximum, timeAbsoluteInitial, mapMatrix, titleMap, themeForest, themeActiveInitial, cacheSprites);
    downsampleScreen();
    if (typeAnimationEntry == 1) {
        cheerEye(true, screen);
        RaycasterHUD::draw(screen, SCREEN_WIDTH, SCREEN_HEIGHT, playerX, playerY, angleVisa, mapMatrix, titleMap, themeForest, player);
    } else if (typeAnimationEntry == 2) {
        cheerDoorOpening();
        RaycasterHUD::draw(screen, SCREEN_WIDTH, SCREEN_HEIGHT, playerX, playerY, angleVisa, mapMatrix, titleMap, themeForest, player);
        cheerBanner3D(titleMap);
    } else {
        RaycasterHUD::draw(screen, SCREEN_WIDTH, SCREEN_HEIGHT, playerX, playerY, angleVisa, mapMatrix, titleMap, themeForest, player);
    }

#ifdef _WIN32
    while (GetAsyncKeyState('V') & 0x8000) std::this_thread::sleep_for(std::chrono::milliseconds(10));
#endif


    bool firstIterationMouse = true;
    bool running = true;
#ifdef _WIN32
    MouseHider mouseHider;
#endif
    while (running) {
        tp2 = chrono::steady_clock::now();
        chrono::duration<float> elapsedTeam = tp2 - tp1;
        tp1 = tp2;
        float timeDelta = elapsedTeam.count();
        
        chrono::duration<float> diffHome = tp2 - timeHome;
        float timeAbsolute = diffHome.count();

        if (timeDelta > 0.1f) timeDelta = 0.1f;

        if (player->getHealth() <= 0 || player->getReturnToMenu()) {
            running = false;
            break;
        }

        bool isMoving = false;
        char actionReturn = RaycasterControls::processInputEControls(
            player,
            playerX,
            playerY,
            angleVisa,
            pitchOffset,
            timeDelta,
            speedMovement,
            mapMatrix,
            SCREEN_HEIGHT,
            sensitivityX,
            sensitivityY,
            firstIterationMouse,
            outHitX,
            outHitY,
            running,
            tp1,
#ifdef _WIN32
            mouseHider,
#endif
            isMoving,
            bobbingTeam,
            bobbingAmplitude,
            bobbingOffset
        );
        if (actionReturn != '\0') {
            return actionReturn;
        }

        // --- RENDERIZACAO RAYCASTING (3D) ---
        float horizonInternal = (HEIGHT_INTERNAL / 2.0f) + (bobbingOffset * 2) + (pitchOffset * 2.0f);
        int offsetGeneral = (bobbingOffset * 2) + (int)(pitchOffset * 2.0f);
        

        // --- CICLO DIA/NOITE GLOBAL ---
        int themeActive = themeSky;
        if (themeSky == 1 || themeSky == 2) {
            long long globalMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            float angleGlobal = ((globalMs % 60000) / 60000.0f) * 6.2831853f;
            if (angleGlobal > 1.5707f && angleGlobal < 4.7123f) {
                themeActive = 1; 
            } else {
                themeActive = 2; 
            }
        }

        RaycasterRenderer::render3D(screen3D, SCREEN_WIDTH, HEIGHT_INTERNAL, playerX, playerY, angleVisa, horizonInternal, offsetGeneral, depthMaximum, timeAbsolute, mapMatrix, titleMap, themeForest, themeActive, cacheSprites);
        // --- LIMPA A TELA HUD ---
        for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
            screen[i].clear();
        }

        // --- RENDERIZACAO HUD E OVERLAYS (2D) ---
        RaycasterHUD::draw(screen, SCREEN_WIDTH, SCREEN_HEIGHT, playerX, playerY, angleVisa, mapMatrix, titleMap, themeForest, player);

        string bufferFrame = "\033[?2026h\033[?25l\033[H"; 
        bufferFrame.reserve(SCREEN_WIDTH * SCREEN_HEIGHT * 15); 

        int curBgR = -1, curBgG = -1, curBgB = -1;
        int curFgR = -1, curFgG = -1, curFgB = -1;

        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                if (y == SCREEN_HEIGHT - 1 && x == SCREEN_WIDTH - 1) break;
                
                const string& hudStr = screen[y * SCREEN_WIDTH + x];
                if (!hudStr.empty()) {
                    if (hudStr[0] != '\1') {
                        bufferFrame += hudStr;
                    }
                    curBgR = -1; curBgG = -1; curBgB = -1;
                    curFgR = -1; curFgG = -1; curFgB = -1;
                } else {
                    const Pixel3D& top = screen3D[(y * 2) * SCREEN_WIDTH + x];
                    const Pixel3D& bot = screen3D[(y * 2 + 1) * SCREEN_WIDTH + x];
                    
                    int bgR, bgG, bgB;
                    int fgR=0, fgG=0, fgB=0;
                    bool hasFg = false;
                    char ch = ' ';
                    bool isHalfBlock = false;
                    
                    if (top.ch == ' ' && bot.ch == ' ') {
                        bgR = bot.r; bgG = bot.g; bgB = bot.b;
                        fgR = top.r; fgG = top.g; fgB = top.b;
                        hasFg = true;
                        isHalfBlock = true;
                    } else if (top.ch != ' ') {
                        bgR = top.r; bgG = top.g; bgB = top.b;
                        if (top.hasFg) { fgR = top.fgR; fgG = top.fgG; fgB = top.fgB; hasFg = true; }
                        ch = top.ch;
                    } else {
                        bgR = bot.r; bgG = bot.g; bgB = bot.b;
                        if (bot.hasFg) { fgR = bot.fgR; fgG = bot.fgG; fgB = bot.fgB; hasFg = true; }
                        ch = bot.ch;
                    }
                    
                    char buff[64];
                    char* p = buff;
                    
                    if (bgR != curBgR || bgG != curBgG || bgB != curBgB) {
                        p = writeAnsiColorFast(p, 48, bgR, bgG, bgB);
                        curBgR = bgR; curBgG = bgG; curBgB = bgB;
                    }
                    if (hasFg) {
                        if (fgR != curFgR || fgG != curFgG || fgB != curFgB) {
                            p = writeAnsiColorFast(p, 38, fgR, fgG, fgB);
                            curFgR = fgR; curFgG = fgG; curFgB = fgB;
                        }
                    }
                    
                    if (isHalfBlock) {
                        *p++ = '\xE2';
                        *p++ = '\x96';
                        *p++ = '\x80';
                    } else {
                        *p++ = ch;
                    }
                    
                    bufferFrame.append(buff, p - buff);
                }
            }
            if (y < SCREEN_HEIGHT - 1) bufferFrame += "\n";
        }
        bufferFrame += "\033[0m\033[?2026l";
        s_lastFrameRendered = bufferFrame;
        cout << bufferFrame << flush;

        auto frameEnd = chrono::steady_clock::now();
        auto frameDuration = chrono::duration_cast<chrono::milliseconds>(frameEnd - tp2).count();
        int sleepTeam = 16 - static_cast<int>(frameDuration);
        if (sleepTeam > 0) {
            this_thread::sleep_for(chrono::milliseconds(sleepTeam));
        }
    }

    InputControl::clearBuffer();
    if (outHitX == -1 && outHitY == -1) {
        cheerEye(false, screen);
        Appearance::clearScreen();
    }
    return 0;
}

void Raycaster::blinkScreenColor(Color color, int durationMs) {
    int SCREEN_WIDTH = Appearance::getTerminalWidth();
    int SCREEN_HEIGHT = Appearance::getTerminalHeight();
    if (SCREEN_WIDTH <= 0) SCREEN_WIDTH = 120;
    if (SCREEN_HEIGHT <= 0) SCREEN_HEIGHT = 40;

    std::string colorCode = Appearance::bgRGB(255, 50, 50);
    if (color == Color::WHITE) colorCode = Appearance::bgRGB(255, 255, 255);
    else if (color == Color::GREEN) colorCode = Appearance::bgRGB(50, 255, 50);

    std::string resetCode = "\033[0m";
    std::string blankLine = colorCode + std::string(SCREEN_WIDTH, ' ') + resetCode;

    std::string bufferFrame = "\033[?25l\033[H";
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        if (y == SCREEN_HEIGHT - 1) {
            bufferFrame += blankLine.substr(0, blankLine.length() - resetCode.length() - 1) + resetCode; 
        } else {
            bufferFrame += blankLine + "\n";
        }
    }
    std::cout << bufferFrame << std::flush;
    std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
}

std::vector<std::string> Raycaster::drawFrameStatic3D(const std::vector<std::string>& mapMatrix, float playerX, float playerY, float angleVisa, const std::string& titleMap, Character* player, int heightOverride) {
    (void)player;
    int SCREEN_WIDTH = Appearance::getTerminalWidth();
    int SCREEN_HEIGHT = (heightOverride > 0) ? heightOverride : Appearance::getTerminalHeight();
    if (SCREEN_WIDTH <= 0) SCREEN_WIDTH = 120;
    if (SCREEN_HEIGHT <= 0) SCREEN_HEIGHT = 40;

    bool themeForest = RaycasterWorld::isThemeForest(titleMap);
    int themeSky = RaycasterWorld::getThemeSky(titleMap);

    std::map<char, SpriteCache> cacheSprites;
    RaycasterEnemies::bootSprites(cacheSprites);
    RaycasterNPCs::bootSprites(cacheSprites);

    int HEIGHT_INTERNAL = SCREEN_HEIGHT * 2;
    std::vector<Pixel3D> screen3D(SCREEN_WIDTH * HEIGHT_INTERNAL);
    std::vector<std::string> screen(SCREEN_WIDTH * SCREEN_HEIGHT, " ");

    RaycasterRenderer::render3D(screen3D, SCREEN_WIDTH, HEIGHT_INTERNAL, playerX, playerY, angleVisa, (HEIGHT_INTERNAL / 2.0f), 0, 150.0f, 0.0f, mapMatrix, titleMap, themeForest, themeSky, cacheSprites);

    downsampleScreenBuffer(screen3D, screen, SCREEN_WIDTH, SCREEN_HEIGHT);

    return screen;
}
