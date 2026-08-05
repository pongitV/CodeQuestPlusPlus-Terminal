#include "UI/Renderers/3D/EngineRaycaster/RaycasterHUD.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterWorld.h"
#include "UI/Screens/Combat/CombatScreen.h"
#include "Core/Utils/Appearance.h"
#include "World/MapControl.h"
#include <cmath>
#include <chrono>

using namespace std;

void RaycasterHUD::draw(vector<string>& screen, int widthScreen, int heightScreen, float playerX, float playerY, float angleVisa, const vector<string>& mapMatrix, const string& titleMap, bool themeForest, Character* player) {
    drawBarStatus(screen, widthScreen, heightScreen, player, angleVisa);
    drawControls(screen, widthScreen, heightScreen);
}

void RaycasterHUD::drawMinimap(vector<string>& screen, int SCREEN_WIDTH, int SCREEN_HEIGHT, float playerX, float playerY, float angleVisa, const vector<string>& mapMatrix, const string& titleMap, bool /*temaFloresta*/, char iconDoPlayer, const string& colorPlayerAnsi) {
    int widthMap = mapMatrix.empty() ? 0 : mapMatrix[0].size();
    int heightMap = mapMatrix.size();

    // HUD: Mini-mapa sobreposto no topo-esquerdo
    int widthMiniMap = 31; // Aumentado a largura para visao panoramica horizontal
    int heightMiniMap = 15;  // Mantem a altura original
    int offsetX = 2;
    int offsetY = 1; // Topo

    // 1. Desenha a borda do mini-mapa
    string colorEdge = "\033[38;2;255;255;255m"; // Branco
    string bgMini = "\033[48;2;25;25;25m"; // Fundo pseudo-transparente cinza escuro
    for (int y = offsetY - 1; y <= offsetY + heightMiniMap; y++) {
        for (int x = offsetX - 1; x <= offsetX + widthMiniMap; x++) {
            if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
                string prefix = bgMini + colorEdge;
                if (y == offsetY - 1 && x == offsetX - 1) screen[y * SCREEN_WIDTH + x] = prefix + "╔\033[0m";
                else if (y == offsetY - 1 && x == offsetX + widthMiniMap) screen[y * SCREEN_WIDTH + x] = prefix + "╗\033[0m";
                else if (y == offsetY + heightMiniMap && x == offsetX - 1) screen[y * SCREEN_WIDTH + x] = prefix + "╚\033[0m";
                else if (y == offsetY + heightMiniMap && x == offsetX + widthMiniMap) screen[y * SCREEN_WIDTH + x] = prefix + "╝\033[0m";
                else if (y == offsetY - 1 || y == offsetY + heightMiniMap) screen[y * SCREEN_WIDTH + x] = prefix + "═\033[0m";
                else if (x == offsetX - 1 || x == offsetX + widthMiniMap) screen[y * SCREEN_WIDTH + x] = prefix + "║\033[0m";
            }
        }
    }

    // 2. Desenha o conteudo do mini-mapa SEM rotacao
    float angleNorm = fmod(angleVisa, 2.0f * 3.14159f);
    if (angleNorm < 0) angleNorm += 2.0f * 3.14159f;
    
    string directionArrow = "►"; // 0 radianos aponta para o Leste (+X)
    int sayX = 1, sayY = 0;
    if (angleNorm >= 0.785f && angleNorm < 2.356f) { directionArrow = "▼"; sayX = 0; sayY = 1; }
    else if (angleNorm >= 2.356f && angleNorm < 3.926f) { directionArrow = "◄"; sayX = -1; sayY = 0; }
    else if (angleNorm >= 3.926f && angleNorm < 5.497f) { directionArrow = "▲"; sayX = 0; sayY = -1; }

    for (int my = 0; my < heightMiniMap; my++) {
        for (int mx = 0; mx < widthMiniMap; mx++) {
            int mapX = (int)playerX + (mx - widthMiniMap/2);
            int mapY = (int)playerY + (my - heightMiniMap/2);
            
            int screenY = my + offsetY;
            int screenX = mx + offsetX;
            
            if (mapX >= 0 && mapX < widthMap && mapY >= 0 && mapY < heightMap) {
                char c = mapMatrix[mapY][mapX];
                
                // bool isLabel = RaycasterMundo::isMapLabel(mapX, mapY, matrizDoMapa);
                
                if (mx == widthMiniMap/2 && my == heightMiniMap/2) {
                    screen[screenY * SCREEN_WIDTH + screenX] = bgMini + colorPlayerAnsi + string(1, iconDoPlayer) + "\033[0m"; // Jogador
                } else if (mx == widthMiniMap/2 + sayX && my == heightMiniMap/2 + sayY) {
                    screen[screenY * SCREEN_WIDTH + screenX] = bgMini + "\033[1;38;2;255;255;255m" + directionArrow + "\033[0m"; // Indicador Visao Branco
                } else {
                    screen[screenY * SCREEN_WIDTH + screenX] = bgMini + MapControl::formatCell(c, mapX, mapY, titleMap, mapMatrix, true) + "\033[0m";
                }
            } else {
                if (screenY >= 0 && screenY < SCREEN_HEIGHT && screenX >= 0 && screenX < SCREEN_WIDTH) {
                    screen[screenY * SCREEN_WIDTH + screenX] = bgMini + " \033[0m"; // Preenche o espaco fora dos limites do mapa
                }
            }
        }
    }
}

void RaycasterHUD::drawBarStatus(vector<string>& screen, int SCREEN_WIDTH, int SCREEN_HEIGHT, Character* player, float angleVisa, const string& titleEdge, int framesDamagePlayer, int damageAmount, bool isCure) {
    if (screen.empty()) return;
    bool isModeLines = (screen.size() <= (size_t)SCREEN_HEIGHT); // If it's a small vector, it's a vector of lines (strings)
    
    vector<string> linesHUD = CombatScreen::getLinesBarDeStatusDoPlayer(player, Color::RESET, damageAmount, framesDamagePlayer, isCure);
    int hudHeight = linesHUD.size();
    int maxHudWidth = 0;
    for (const string& line : linesHUD) {
        int len = Appearance::getVisualLength(line);
        if (len > maxHudWidth) maxHudWidth = len;
    }
    
    int boxWidth = maxHudWidth + 1; 
    int boxHeight = hudHeight + 2;
    int hudOffsetX = (SCREEN_WIDTH - maxHudWidth) / 2; 
    if (hudOffsetX < 0) hudOffsetX = 0;
    int hudOffsetY = SCREEN_HEIGHT - boxHeight - 1; 

    string colorHudEdge = "\033[38;2;255;255;255m"; 
    string bgHud = "\033[48;2;25;25;25m"; 
    
    // Pulsing Border (Critical HP)
    if (player && player->getHealth() <= player->getMaxHealth() * 0.25) {
        auto now = std::chrono::steady_clock::now();
        int timeMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
        int pulse = (timeMs / 5) % 510;
        if (pulse > 255) pulse = 510 - pulse;
        colorHudEdge = "\033[38;2;255;" + to_string(100 - (pulse*100)/255) + ";" + to_string(100 - (pulse*100)/255) + "m";
    }
    


    string centerTop = "";
    if (titleEdge.empty()) {
        if (!CombatScreen::context.nameShiftVisible.empty()) {
            centerTop = "\033[38;2;255;255;255m[ Turno: " + CombatScreen::context.nameShiftVisible + " ]" + colorHudEdge;
        } else {
            centerTop = ""; // Sem titulo
        }
    } else {
        centerTop = titleEdge;
    }

    if (hudOffsetY >= 0 && hudOffsetY < SCREEN_HEIGHT) {
        string pixelTop = bgHud + colorHudEdge + "╔";
        int lenVisCenter = Appearance::getVisualLength(centerTop);
        int halfLine = (boxWidth - 2 - lenVisCenter) / 2;
        for(int k=0; k < halfLine; ++k) pixelTop += "═";
        pixelTop += centerTop + colorHudEdge;
        for(int k=0; k < (boxWidth - 2 - lenVisCenter - halfLine); ++k) pixelTop += "═";
        pixelTop += "╗\033[0m";
        
        if (isModeLines) {
            screen[hudOffsetY] = Appearance::superimposePanelNaLineAnsi(screen[hudOffsetY], pixelTop, hudOffsetX);
        } else {
            screen[hudOffsetY * SCREEN_WIDTH + hudOffsetX] = pixelTop;
            for(int x = 1; x < boxWidth; ++x) {
                if (hudOffsetX + x < SCREEN_WIDTH) screen[hudOffsetY * SCREEN_WIDTH + hudOffsetX + x] = "";
            }
        }
    }

    if (hudOffsetY + boxHeight - 1 >= 0 && hudOffsetY + boxHeight - 1 < SCREEN_HEIGHT) {
        string pixelBase = bgHud + colorHudEdge + "╚";
        for(int k=0; k < boxWidth - 2; ++k) pixelBase += "═";
        pixelBase += "╝\033[0m";
        
        if (isModeLines) {
            screen[hudOffsetY + boxHeight - 1] = Appearance::superimposePanelNaLineAnsi(screen[hudOffsetY + boxHeight - 1], pixelBase, hudOffsetX);
        } else {
            screen[(hudOffsetY + boxHeight - 1) * SCREEN_WIDTH + hudOffsetX] = pixelBase;
            for(int x = 1; x < boxWidth; ++x) {
                if (hudOffsetX + x < SCREEN_WIDTH) screen[(hudOffsetY + boxHeight - 1) * SCREEN_WIDTH + hudOffsetX + x] = "";
            }
        }
    }

    for (size_t i = 0; i < linesHUD.size(); ++i) {
        string line = linesHUD[i];
        size_t post = 0;
        while ((post = line.find("\033[0m", post)) != string::npos) {
            line.replace(post, 4, "\033[0m" + bgHud);
            post += 4 + bgHud.length(); 
        }
        
        size_t postLeft = line.find("║");
        if (postLeft != string::npos) line.replace(postLeft, 3, bgHud + colorHudEdge + "║" + bgHud);
        else line = bgHud + line;

        int len = Appearance::getVisualLength(line);
        int pad = maxHudWidth - len;
        if (pad > 0) line += string(pad, ' ');

        line += colorHudEdge + "║\033[0m";

        int y = hudOffsetY + 1 + i;
        if (y >= 0 && y < SCREEN_HEIGHT) {
            if (isModeLines) {
                screen[y] = Appearance::superimposePanelNaLineAnsi(screen[y], line, hudOffsetX);
            } else {
                screen[y * SCREEN_WIDTH + hudOffsetX] = line;
                for(int x = 1; x < boxWidth; ++x) {
                    if (hudOffsetX + x < SCREEN_WIDTH) screen[y * SCREEN_WIDTH + hudOffsetX + x] = "";
                }
            }
        }
    }
    
    // Floating Damage do Jogador
    if (framesDamagePlayer > 0 && damageAmount > 0) {
        int yOffset = max(0, 10 - framesDamagePlayer) / 2; // Sobe ate o final da animacao
        int fctY = hudOffsetY - 1 - yOffset;
        if (fctY >= 0 && fctY < SCREEN_HEIGHT) {
            string txtDamage = "-" + to_string(damageAmount);
            string colorDamage = "\033[1;38;2;255;50;50m";
            
            // Procura onde está o "/" do HP na primeira linha do HUD
            int hpVisX = boxWidth / 2; // default centro
            if (linesHUD.size() > 0) {
                size_t postHP = linesHUD[0].find("HP:");
                if (postHP != string::npos) {
                    size_t postSlash = linesHUD[0].find("/", postHP);
                    if (postSlash != string::npos) {
                        hpVisX = Appearance::getVisualLength(linesHUD[0].substr(0, postSlash)); 
                    } else {
                        hpVisX = Appearance::getVisualLength(linesHUD[0].substr(0, postHP)) + 5; 
                    }
                }
            }
            
            int fctX = hudOffsetX + hpVisX;
            
            // Sobreposicao simples do dano flutuante no HUD
            if (isModeLines) {
                screen[fctY] = Appearance::superimposePanelNaLineAnsi(screen[fctY], colorDamage + txtDamage + "\033[0m", fctX);
            } else {
                screen[fctY * SCREEN_WIDTH + fctX] = colorDamage + txtDamage + "\033[0m";
            }
        }
    }
}

void RaycasterHUD::drawControls(vector<string>& screen, int SCREEN_WIDTH, int SCREEN_HEIGHT) {
    string cF = "\033[38;2;200;200;200m\033[48;2;20;20;20m";
    string cK = "\033[1;38;2;255;255;255m\033[48;2;20;20;20m"; // Tecla em branco
    
    string displayHTML = cF + "[" + cK + "W,A,S,D" + cF + "] Mover   "
                       + cF + "[" + cK + "V" + cF + "] Visao   "
                       + cF + "[" + cK + "I" + cF + "] Inventario   "
                       + cF + "[" + cK + "C" + cF + "] Ficha   "
                       + cF + "[" + cK + "B" + cF + "] Diario   "
                       + cF + "[" + cK + "M" + cF + "] Mapa\033[0m";

    int lenVis = 77; // Comprimento visual hardcoded pra formatacao
    int startCtrlX = (SCREEN_WIDTH - lenVis) / 2;
    int lineControls = SCREEN_HEIGHT - 1;
    
    if (startCtrlX > 0) {
        screen[lineControls * SCREEN_WIDTH + startCtrlX] = displayHTML;
        for (int i = 1; i < lenVis; ++i) {
            screen[lineControls * SCREEN_WIDTH + startCtrlX + i] = "";
        }
    }
}
