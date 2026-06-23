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

void RaycasterRenderer::renderizar3D(vector<string>& tela, int LARGURA_TELA, int ALTURA_TELA, float jogadorX, float jogadorY, float anguloVisao, float horizonte, int bobbingOffset, float profundidadeMaxima, float tempoAbsoluto, const vector<string>& matrizDoMapa, const string& tituloMapa, bool temaFloresta, int temaCeu, const map<char, SpriteCache>& cacheSprites) {
    float campoVisao = 3.14159f / 4.0f; // FOV 45 graus
    int larguraMapa = matrizDoMapa.empty() ? 0 : matrizDoMapa[0].size();
    int alturaMapa = matrizDoMapa.size();

    std::vector<std::tuple<int, int, int>> luzes;
    for (int ly = 0; ly < alturaMapa; ly++) {
        for (int lx = 0; lx < larguraMapa; lx++) {
            char c = matrizDoMapa[ly][lx];
            if (RaycasterMundo::isMapLabel(lx, ly, matrizDoMapa)) continue;

            if (c == '^') {
                luzes.push_back({lx, ly, 1}); // White
            } else if (c == 'P' || c == 'F' || c == 'B') {
                luzes.push_back({lx, ly, 0}); // Orange
            }
        }
    }

    std::string tituloUpper = tituloMapa;
    for (char& ch : tituloUpper) ch = std::toupper(static_cast<unsigned char>(ch));
    bool isReino = (tituloUpper.find("CASTELO") != std::string::npos || tituloUpper.find("REINO") != std::string::npos);

        unsigned int numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 4;
        
        std::vector<std::thread> threads;
        int chunk = LARGURA_TELA / numThreads;

        for (unsigned int t = 0; t < numThreads; ++t) {
            int startX = t * chunk;
            int endX = (t == numThreads - 1) ? LARGURA_TELA : (t + 1) * chunk;

            threads.emplace_back([&, startX, endX, luzes]() {
                for (int x = startX; x < endX; x++) {
                    float raioAngulo = (anguloVisao - campoVisao / 2.0f) + ((float)x / (float)LARGURA_TELA) * campoVisao;
                    float distanciaAteParede = 0.0f;
                    bool bateuNaParede = false;

                    float olhoX = cosf(raioAngulo);
                    float olhoY = sinf(raioAngulo);
                    
                    vector<EntidadeAtingida> entidadesAtingidas;
                    int lastEntX = -1, lastEntY = -1;
                    char charParede = '#';

                    while (!bateuNaParede && distanciaAteParede < profundidadeMaxima) {
                        distanciaAteParede += 0.1f;

                        int testeX = (int)(jogadorX + olhoX * distanciaAteParede);
                        int testeY = (int)(jogadorY + olhoY * distanciaAteParede);
                        if (testeX < 0 || testeX >= larguraMapa || testeY < 0 || testeY >= alturaMapa) {
                            bateuNaParede = true;
                            distanciaAteParede = profundidadeMaxima;
                        } else {
                            char c = matrizDoMapa[testeY][testeX];
                            if (c != '.' && c != ' ' && c != '~') {
                                bool isLabel = RaycasterMundo::isMapLabel(testeX, testeY, matrizDoMapa);
                                if (!isLabel) {
                                    bool isEntity = RaycasterMundo::isEntity(c);
                                    if (isEntity) {
                                        if (testeX != lastEntX || testeY != lastEntY) {
                                            lastEntX = testeX;
                                            lastEntY = testeY;
                                            char spriteChar = RaycasterMundo::obterSpriteChar(testeX, testeY, c, tituloMapa);

                                            float centerX = testeX + 0.5f;
                                            float centerY = testeY + 0.5f;

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
                    float hitXAnterior = jogadorX + olhoX * (distanciaAteParede - 0.1f);

                    float texXParede = 0.0f;
                    bool isSideWall = false;
                    if ((int)hitXAnterior != (int)hitX) {
                        texXParede = hitY - floorf(hitY); 
                        isSideWall = true;
                    } else {
                        texXParede = hitX - floorf(hitX); 
                        isSideWall = false;
                    }

                    int teto = (int)(horizonte - ALTURA_TELA / ((float)distanciaAteParede));
                    int chao = (int)(horizonte + ALTURA_TELA / ((float)distanciaAteParede));

                    if (temaFloresta && charParede == '#') teto -= (int)(ALTURA_TELA / ((float)distanciaAteParede) * 1.5f); 
                    if (charParede == '*') teto -= (int)(ALTURA_TELA / ((float)distanciaAteParede) * 1.5f); 
                    if (isReino && charParede == '|') teto -= (int)(ALTURA_TELA / ((float)distanciaAteParede) * 3.0f); // Portao super alto

                    for (int y = 0; y < ALTURA_TELA; y++) {
                        bool drawFloor = false;

                        if (y < teto) {
                            tela[y * LARGURA_TELA + x] = RaycasterMundo::obterPixelTeto(temaCeu, raioAngulo, y - bobbingOffset, ALTURA_TELA, tempoAbsoluto);
                        } else if (y >= teto && y <= chao) {
                            std::string pixel = RaycasterMundo::obterPixelParede(tituloMapa, temaFloresta, distanciaAteParede, profundidadeMaxima, charParede, y, teto, chao, texXParede, tempoAbsoluto, isSideWall, luzes, hitX, hitY);
                            if (pixel == "FUNDO") {
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
                                            string p = sc.pixels[spriteY][spriteX];
                                            if (p == " \033[0m" && (ent.c == '^' || (ent.c >= '1' && ent.c <= '5'))) {
                                                float wave = sinf(tempoAbsoluto * 6.0f + y * 0.2f + spriteX * 0.2f);
                                                int r = 210 + (int)(wave * 45); // Oscila
                                                int g = 190 + (int)(wave * 65); // Oscila
                                                int b = 255;
                                                p = "\033[48;2;" + to_string(r) + ";" + to_string(g) + ";" + to_string(b) + "m \033[0m";
                                            }
                                            if (p != "") tela[y * LARGURA_TELA + x] = p; 
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            });
        }

        for (auto& th : threads) {
            th.join();
        }
}