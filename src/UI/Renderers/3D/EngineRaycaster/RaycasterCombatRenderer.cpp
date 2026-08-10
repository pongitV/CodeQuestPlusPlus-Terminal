#include "UI/Renderers/3D/EngineRaycaster/RaycasterCombatRenderer.h"
#include "UI/Renderers/3D/EngineRaycaster/Raycaster.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterSprites.h"
#include "Core/Utils/Appearance.h"
#include "Domain/Characters/Races/BaseRace.h"
#include "UI/Screens/Combat/CombatScreen.h"
#include <algorithm>
#include <cctype>
#include <cmath>

extern Character* g_enemyAttackerParry;

/*
 * ═══════════════════════════════════════════════════════════════════
 * Arena de Combate por Bioma
 * ═══════════════════════════════════════════════════════════════════
 */
std::vector<std::string> RaycasterRendererCombat::getArenaByTitle(const std::string& title) {
    std::string upper = title;
    for (char& c : upper) c = std::toupper(static_cast<unsigned char>(c));

    char floor = '.';
    if (upper.find("VILA") != std::string::npos || upper.find("FLORESTA") != std::string::npos || upper.find("BOSQUE") != std::string::npos || upper.find("INICIO") != std::string::npos) {
        floor = ',';
    }

    auto floorStr = [&](int n) { return std::string(n, floor); };

    if (upper.find("CHEFE") != std::string::npos) {
        return {
            "  .........................  ",
            " .............................  ",
            "...............................",
            "...............................",
            "...............................",
            "...............................",
            "...............................",
            " ............................. ",
            "  ...........................  "
        };
    }
    if (upper.find("SALA DE TROFEUS") != std::string::npos || upper.find("TROFEU") != std::string::npos) {
        return {
            "#############################",
            "#.......T...T...T...T.......#",
            "#...........................#",
            "#.......*...*...*...*.......#",
            "#...........................#",
            "#############################"
        };
    }
    if (upper.find("LABIRINTO") != std::string::npos) {
        return {
            "=================================",
            "|...............................|",
            "|...............................|",
            "|...............................|",
            "|...............................|",
            "|...............................|",
            "|...............................|",
            "|...............................|",
            "================================="
        };
    }
    if (upper.find("CAVERNA") != std::string::npos || upper.find("CORACAO") != std::string::npos) {
        return {
            "################################",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "################################"
        };
    }
    if (upper.find("PATIO DO REINO") != std::string::npos || upper.find("REINO") != std::string::npos) {
        return {
            "#|||||||||||||||||||||||||||||||#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#|||||||||||||||||||||||||||||||#"
        };
    }
    if (upper.find("CEMITERIO") != std::string::npos) {
        return {
            "################################",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "################################"
        };
    }
    if (upper.find("FLORESTA") != std::string::npos || upper.find("BOSQUE") != std::string::npos) {
        return {
            "################################",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "#" + floorStr(30) + "#",
            "################################"
        };
    }
    // Vila / Inicio
    if (upper.find("VILA") != std::string::npos || upper.find("INICIO") != std::string::npos) {
        return {
            "T=====[]=======================T",
            "T" + floorStr(30) + "T",
            "T" + floorStr(30) + "T",
            "T" + floorStr(30) + "T",
            "T" + floorStr(30) + "T",
            "T" + floorStr(30) + "T",
            "T" + floorStr(30) + "T",
            "T" + floorStr(30) + "T",
            "TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT"
        };
    }
    
    // Default
    return {
        "####_[]_########################",
        "#" + floorStr(30) + "#",
        "#" + floorStr(30) + "#",
        "#" + floorStr(30) + "#",
        "#" + floorStr(30) + "#",
        "#" + floorStr(30) + "#",
        "#" + floorStr(30) + "#",
        "#" + floorStr(30) + "#",
        "################################"
    };
}

/*
 * ═══════════════════════════════════════════════════════════════════
 * Cor base do sprite do inimigo (mesmas cores do RaycasterInimigos)
 * ═══════════════════════════════════════════════════════════════════
 */
std::tuple<int,int,int> RaycasterRendererCombat::getColorSpriteEnemy(Character* enemy) {
    if (!enemy) return {255, 255, 255};
    
    switch (enemy->getTypeRace()) {
        case TypeRace::Goblin: return {100, 200, 50};
        case TypeRace::Ork:
        case TypeRace::ExiledOrc: return {50, 150, 50};
        case TypeRace::Slime: return {50, 200, 255};
        case TypeRace::Fairy: return {255, 100, 200};
        case TypeRace::ForestAbomination: return {139, 69, 19};
        case TypeRace::Troll: return {150, 150, 160};
        case TypeRace::Mimic: return {200, 150, 50};
        case TypeRace::Mahoraga: return {255, 255, 255};
        default: return {200, 200, 200}; // Default cinza claro
    }
}

/*
 * ═══════════════════════════════════════════════════════════════════
 * Pintar texto no buffer 1D (overlay)
 * ═══════════════════════════════════════════════════════════════════
 */
void RaycasterRendererCombat::paintTextNoBuffer(std::vector<std::string>& screen, int widthScreen, int heightMax, int postX, int postY, const std::string& text, const std::string& colorFg, const std::string& colorBgOverride) {
    (void)widthScreen;
    if (postY < 0 || postY >= (int)screen.size() || postY >= heightMax) return;
    
    std::string bg = colorBgOverride.empty() ? "\033[48;2;0;0;0m" : colorBgOverride;
    std::string panelText = bg + colorFg + text + "\033[0m";
    screen[postY] = Appearance::superimposePanelNaLineAnsi(screen[postY], panelText, postX);
}

/*
 * ═══════════════════════════════════════════════════════════════════
 * Renderizar Quadro Principal
 * ═══════════════════════════════════════════════════════════════════
 */
static std::vector<std::string> s_cachedBackground;
static std::string s_cachedTitleMap;
static int s_cachedWidthScreen = 0;
static int s_cachedHeight3D = 0;

const std::vector<std::string>& RaycasterRendererCombat::getLastBackgroundRendered() {
    return s_cachedBackground;
}

std::vector<std::string> RaycasterRendererCombat::renderFrame(
    const std::string& titleMap, 
    Character* player, 
    const std::vector<Character*>& enemies,
    Character* targetAnimation,
    int frame,
    int framesDeDamagePlayer,
    int damageAmount,
    bool isCure,
    int timeMs,
    bool isDeath,
    const std::vector<std::string>& dropsAnimation,
    float spriteOpacity
) {
    // Gera uma arena dedicada ao bioma
    std::vector<std::string> arena = getArenaByTitle(titleMap);
    
    // Posicao fixa: centro da arena, olhando para Norte
    float jX = static_cast<float>(arena[0].size()) / 2.0f;
    float jY = static_cast<float>(arena.size()) - 2.0f;
    float angle = -1.57f; // Olhando pro Norte

    int widthScreen = Appearance::getTerminalWidth();
    int terminalHeight = Appearance::getTerminalHeight();
    if (widthScreen <= 0) widthScreen = 120;
    if (terminalHeight <= 0) terminalHeight = 40;

    int heightHUD = 0;
    int height3D = std::max(10, terminalHeight - heightHUD);

    if (s_cachedBackground.empty() || s_cachedTitleMap != titleMap || s_cachedWidthScreen != widthScreen || s_cachedHeight3D != height3D) {
        s_cachedBackground = Raycaster::drawFrameStatic3D(arena, jX, jY, angle, titleMap, player, height3D);
        s_cachedTitleMap = titleMap;
        s_cachedWidthScreen = widthScreen;
        s_cachedHeight3D = height3D;
    }

    std::vector<std::string> screen = s_cachedBackground;

    // Sobrepoe os inimigos
    int inaEnemies = static_cast<int>(enemies.size());
    for (int i = 0; i < inaEnemies; ++i) {
        Character* enemy = enemies[i];
        if (enemy && (enemy->getHealth() > 0 || !enemy->getDeathLively())) {
            bool isExcited = (targetAnimation != nullptr && enemy == targetAnimation);
            int framesDamage = (isExcited && damageAmount > 0 && !isDeath) ? frame : 0;
            bool isDeathIni = (isDeath && isExcited);
            int frameDeathIni = isDeathIni ? frame : 0;
            bool isSel = (CombatScreen::context.selectionTargetCurrent == i);
            
            superimposeSprite(screen, enemy, i, inaEnemies, widthScreen, height3D, framesDamage, damageAmount, isCure, timeMs, isDeathIni, frameDeathIni, dropsAnimation, isSel, spriteOpacity);
        }
    }

    /*
     * Retorna a tela com a altura correspondente a cena 3D (o HUD classico sera impresso abaixo por TelaCombate)
     * PREENCHENDO ATE A ALTURA_TELA TOTAL PARA EVITAR CRASH NO HUD!
     */
    std::vector<std::string> linesRendered(terminalHeight);
    
    int cameraOffsetX = 0;
    if (framesDeDamagePlayer > 0 && framesDeDamagePlayer % 2 == 0) {
        cameraOffsetX = 4;
    }
    
    for (int y = 0; y < height3D; y++) {
        std::string line = "";
        for (int x = 0; x < widthScreen; x++) {
            int srcX = x - cameraOffsetX;
            if (srcX >= 0 && srcX < widthScreen) {
                line += screen[y * widthScreen + srcX];
            } else {
                line += " "; // pixel vazio
            }
        }
        linesRendered[y] = std::move(line);
    }
    // Preenche o resto com espaco vazio para evitar Out-Of-Bounds quando o HUD for desenhado no fundo
    for (int y = height3D; y < terminalHeight; y++) {
        linesRendered[y] = std::string(widthScreen, ' ');
    }

    return linesRendered;
}

/*
 * ═══════════════════════════════════════════════════════════════════
 * Sobrepor Sprite do Inimigo (com arte 3D texturizada)
 * ═══════════════════════════════════════════════════════════════════
 */
void RaycasterRendererCombat::superimposeSprite(
    std::vector<std::string>& screen, 
    Character* enemy, 
    int enemyIdx,
    int totalEnemies,
    int widthScreen, 
    int heightVisible, 
    int flashDamageEnemy, 
    int damageAmount, 
    bool isCure, 
    int timeMs, 
    bool isDeath, 
    int frameDeath, 
    const std::vector<std::string>& dropsAnimation, 
    bool isSelected,
    float spriteOpacity
) {
    // Usa a arte de MAPA (mesma do raycaster) em vez da arte de combate 2D
    const std::vector<std::string>& artOriginalEnemy = enemy->getRace()->getAppearanceRace();
    if (artOriginalEnemy.empty()) return;

    (void)dropsAnimation;

    double progress = 0.0;
    int totalFramesDeath = 0;
    if (isDeath) {
        totalFramesDeath = 12; // 12 frames em 3D
        progress = std::min(1.0, static_cast<double>(frameDeath) / totalFramesDeath);
    }

    // Obtem cor base para texturizacao estilo raycaster
    auto [baseR, baseG, baseB] = getColorSpriteEnemy(enemy);

    std::vector<std::string> artUsed = artOriginalEnemy;
    
    int heightArt = static_cast<int>(artUsed.size());
    float factor = static_cast<float>(Appearance::FACTOR_COMPRESSION_GLOBAL);
    
    TypeRace type = enemy->getTypeRace();
    bool isBoss = enemy->isBoss();
    
    switch (type) {
        case TypeRace::Ork:
        case TypeRace::ExiledOrc:
            factor = 2.7f;
            break;
        case TypeRace::Goblin:
            factor = 2.5f;
            break;
        case TypeRace::Slime:
            factor = 2.5f;
            break;
        case TypeRace::Mahoraga:
            factor = 3.0f;
            break;
        case TypeRace::ForestAbomination:
            factor = 1.5f;
            break;
        case TypeRace::Troll:
            factor = 1.9f;
            break;
        case TypeRace::Mimic:
            factor = 2.5f;
            break;
        case TypeRace::Fairy:
            factor = 3.2f;
            break;
        default:
            break;
    }
    
    if (!isBoss) {
        // Variacao de escala deterministica em ate 5%
        size_t h = reinterpret_cast<size_t>(enemy);
        float pct = ((h % 101) - 50.0f) / 1000.0f; // Varia de -0.05 a +0.05
        factor *= (1.0f + pct);
    }
    
    if (heightArt > 10) {
        artUsed = Appearance::reduceScaleAscii(artOriginalEnemy, factor, factor);
        heightArt = static_cast<int>(artUsed.size());
    }
    
    int reservedBottom = 14;

    int artWidth = 0;
    for (const auto& line : artUsed) {
        int comp = Appearance::getVisualLength(line);
        if (comp > artWidth) artWidth = comp;
    }

    // Sway horizontal based on time (apenas se nao estiver no meio da animacao de morte)
    int swayOff = 0;
    if (!isDeath) {
        int stepSway = (timeMs / 200) % 8;
        if (stepSway < 0) stepSway += 8;
        int swayPattern[] = {0, 1, 2, 1, 0, -1, -2, -1};
        swayOff = swayPattern[stepSway] * 2;
    }
    
    int maxStartY = heightVisible - heightArt - reservedBottom;
    int startY = (heightVisible - heightArt) / 2;
    startY += heightVisible / 10;
    if (startY > maxStartY) startY = maxStartY;
    if (startY < 0) startY = 0;
    
    int startX = 0; // Sera recalculado apos obter o croppedWidth

    // Helper para extrair o background da celula do Raycaster
    auto getBg = [](const std::string& s) {
        size_t post = s.find("\033[48;2;");
        if (post != std::string::npos) {
            size_t end = s.find('m', post);
            if (end != std::string::npos) return s.substr(post, end - post + 1);
        }
        return std::string("\033[48;2;0;0;0m");
    };

    auto parsiAnsiRGB = [](const std::string& str) -> std::tuple<int,int,int> {
        int r = 0, g = 0, b = 0;
        size_t post = str.find("48;2;");
        if (post == std::string::npos) {
            post = str.find("38;2;");
        }
        if (post != std::string::npos) {
            post += 5;
            while (post < str.size() && str[post] >= '0' && str[post] <= '9') {
                r = r * 10 + (str[post] - '0');
                post++;
            }
            if (post < str.size() && str[post] == ';') {
                post++;
                while (post < str.size() && str[post] >= '0' && str[post] <= '9') {
                    g = g * 10 + (str[post] - '0');
                    post++;
                }
                if (post < str.size() && str[post] == ';') {
                    post++;
                    while (post < str.size() && str[post] >= '0' && str[post] <= '9') {
                        b = b * 10 + (str[post] - '0');
                        post++;
                    }
                }
            }
        }
        return {r, g, b};
    };

    // Texturiza cada caractere da arte com cores baseadas no caractere
    auto getTgtRGB = [&](char c, int /*rx*/, int ry) -> std::tuple<int,int,int> {
        int currentBaseR = baseR;
        int currentBaseG = baseG;
        int currentBaseB = baseB;
        if (enemy->getTypeRace() == TypeRace::Mahoraga && (ry * factor) < 24) {
            currentBaseR = 255;
            currentBaseG = 215;
            currentBaseB = 0; // Amarelo/Dourado
        }
        int rMod = currentBaseR, gMod = currentBaseG, bMod = currentBaseB;
        if (c == '@' || c == 'M' || c == 'W' || c == '#' || c == '&' || c == '8') { rMod = currentBaseR * 0.4; gMod = currentBaseG * 0.4; bMod = currentBaseB * 0.4; }
        else if (c == '%' || c == 'O' || c == 'X' || c == 'S' || c == 'Q') { rMod = currentBaseR * 0.6; gMod = currentBaseG * 0.6; bMod = currentBaseB * 0.6; }
        else if (c == '*' || c == '+' || c == 'x' || c == 'o' || c == '=' || c == 'H') { rMod = currentBaseR * 0.8; gMod = currentBaseG * 0.8; bMod = currentBaseB * 0.8; }
        else if (c == '-' || c == '~' || c == ':' || c == ';') { rMod = std::min(255, (int)(currentBaseR * 1.2)); gMod = std::min(255, (int)(currentBaseG * 1.2)); bMod = std::min(255, (int)(currentBaseB * 1.2)); }
        else if (c == '.' || c == ',' || c == '\'') { rMod = std::min(255, (int)(currentBaseR * 1.5)); gMod = std::min(255, (int)(currentBaseG * 1.5)); bMod = std::min(255, (int)(currentBaseB * 1.5)); }
        else if (c == '_' || c == '|' || c == '\\' || c == '/' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || c == '<' || c == '>') { rMod = currentBaseR * 0.5; gMod = currentBaseG * 0.5; bMod = currentBaseB * 0.5; }
        
        if (flashDamageEnemy > 0 && flashDamageEnemy % 2 == 0) {
            if (isCure) {
                rMod = 50; gMod = 255; bMod = 50;
            } else {
                rMod = 255; gMod = 50; bMod = 50;
            }
        } else if (enemy == g_enemyAttackerParry) {
            rMod = 255; gMod = 140; bMod = 0;
        } else if (isSelected) {
            if (CombatScreen::context.blinkSelection) {
                rMod = (rMod + 255) / 2;
                gMod = (gMod + 255) / 2;
                bMod = bMod / 2;
            } else {
                int gray = (rMod + gMod + bMod) / 3;
                rMod = (rMod + gray) / 2;
                gMod = (gMod + gray) / 2;
                bMod = (bMod + gray) / 2;
            }
        }

        if (isDeath) {
            double fade = 1.0 - progress;
            rMod = static_cast<int>(rMod * fade);
            gMod = static_cast<int>(gMod * fade);
            bMod = static_cast<int>(bMod * fade);
        }
        
        return {rMod, gMod, bMod};
    };

    // Auto-crop horizontal
    int minX = artWidth, maxX = 0;
    for (const auto& line : artUsed) {
        std::string clean = Appearance::removeANSIColors(line);
        for (int i = 0; i < static_cast<int>(clean.length()); ++i) {
            if (clean[i] != ' ') {
                if (i < minX) minX = i;
                if (i > maxX) maxX = i;
            }
        }
    }
    if (minX > maxX) { minX = 0; maxX = artWidth - 1; }
    int croppedWidth = maxX - minX + 1;
    
    int widthColumn = widthScreen / totalEnemies;
    int centerColumnX = enemyIdx * widthColumn + widthColumn / 2;
    startX = centerColumnX - croppedWidth / 2 + swayOff;

    bool drawBody = (enemy->getHealth() > 0 || !enemy->getDeathLively());

    if (drawBody) {
        // Desenha contorno preto (borda do sprite) + corpo texturizado (mesclando fundo via spriteOpacity)
        for (int y = 0; y < heightArt; y++) {
            int screenY = startY + y;
            if (screenY >= 0 && screenY < heightVisible) {
                std::string lineWithoutColor = Appearance::removeANSIColors(artUsed[y]);
                
                for (int rawX = minX; rawX <= maxX; rawX++) {
                    int x = rawX - minX;
                    int screenX = startX + x;
                    if (screenX >= 0 && screenX < widthScreen && rawX < static_cast<int>(lineWithoutColor.length())) {
                        char c = lineWithoutColor[rawX];
                        
                        if (c != ' ') {
                            if (isDeath) {
                                // Efeito de desintegracao dithered (virando poeira)
                                int hash = (rawX * 37 + y * 57) % 100;
                                if (hash < progress * 100) {
                                    // Renderiza particulas de poeira '.' flutuantes ou some o pixel
                                    if (progress < 0.8 && (hash % 3 == 0)) {
                                        std::string bg = getBg(screen[screenY * widthScreen + screenX]);
                                        int dustIntensity = static_cast<int>(100 * (1.0 - progress));
                                        std::string colorDust = "\033[38;2;" + std::to_string(dustIntensity) + ";" + std::to_string(dustIntensity) + ";" + std::to_string(dustIntensity) + "m";
                                        screen[screenY * widthScreen + screenX] = bg + colorDust + "." + "\033[0m";
                                    }
                                    continue;
                                }
                            }

                            // Checa se eh borda (pixel adjacente a espaco ou borda da arte)
                            bool isEdge = false;
                            for (int dy = -1; dy <= 1; ++dy) {
                                for (int dx = -1; dx <= 1; ++dx) {
                                      if (dx == 0 && dy == 0) continue;
                                      int ny = y + dy;
                                      int nx = rawX + dx;
                                      if (ny < 0 || ny >= heightArt || nx < minX || nx > maxX) { isEdge = true; }
                                      else {
                                          std::string adjLine = Appearance::removeANSIColors(artUsed[ny]);
                                          if (nx >= static_cast<int>(adjLine.length()) || adjLine[nx] == ' ') isEdge = true;
                                      }
                                }
                            }
                            
                            std::string bgStr = getBg(screen[screenY * widthScreen + screenX]);
                            auto [bgR, bgG, bgB] = parsiAnsiRGB(bgStr);
                            int tgtR = 0, tgtG = 0, tgtB = 0;
                            
                            if (isEdge) {
                                tgtR = 0; tgtG = 0; tgtB = 0;
                            } else {
                                auto [tr, tg, tb] = getTgtRGB(c, rawX, y);
                                tgtR = tr; tgtG = tg; tgtB = tb;
                                
                                // Effect Overlays
                                if (flashDamageEnemy > 0 && damageAmount > 0) {
                                    int pulse = (timeMs / 3) % 255;
                                    if ((timeMs / 765) % 2 == 1) pulse = 255 - pulse;
                                    
                                    if (isCure) {
                                        tgtR = tgtR / 2;
                                        tgtG = (tgtG + 255) / 2 + pulse / 2;
                                        tgtB = tgtB / 2;
                                        if (tgtG > 255) tgtG = 255;
                                    } else {
                                        tgtR = (tgtR + 255) / 2 + pulse / 2;
                                        tgtG = tgtG / 2;
                                        tgtB = tgtB / 2;
                                        if (tgtR > 255) tgtR = 255;
                                    }
                                } else if (enemy == g_enemyAttackerParry) {
                                    int pulse = (timeMs / 10) % 255;
                                    if ((timeMs / 2550) % 2 == 1) pulse = 255 - pulse;
                                    tgtR = (tgtR + 255) / 2 + pulse / 5;
                                    tgtG = (tgtG + 140) / 2 + pulse / 10;
                                    tgtB = tgtB / 2;
                                    if (tgtR > 255) tgtR = 255;
                                    if (tgtG > 255) tgtG = 255;
                                } else if (isSelected) {
                                    int pulse = (timeMs / 10) % 255;
                                    if ((timeMs / 2550) % 2 == 1) pulse = 255 - pulse;
                                    tgtR = (tgtR + 255) / 2 + pulse / 5;
                                    tgtG = (tgtG + 255) / 2 + pulse / 5;
                                    tgtB = tgtB / 2;
                                    if (tgtR > 255) tgtR = 255;
                                    if (tgtG > 255) tgtG = 255;
                                }
                            }
                            
                            int blendedR = static_cast<int>(bgR + (tgtR - bgR) * spriteOpacity);
                            int blendedG = static_cast<int>(bgG + (tgtG - bgG) * spriteOpacity);
                            int blendedB = static_cast<int>(bgB + (tgtB - bgB) * spriteOpacity);
                            
                            screen[screenY * widthScreen + screenX] = "\033[48;2;" + std::to_string(blendedR) + ";" + std::to_string(blendedG) + ";" + std::to_string(blendedB) + "m \033[0m";
                        }
                    }
                }
            }
        }
    }

    // Helper local para dividir string UTF-8
    auto splitUTF8 = [](const std::string& s) {
        std::vector<std::string> chars;
        for (size_t i = 0; i < s.length(); ) {
            int len = 1;
            unsigned char c = static_cast<unsigned char>(s[i]);
            if ((c & 0x80) == 0) len = 1;
            else if ((c & 0xE0) == 0xC0) len = 2;
            else if ((c & 0xF0) == 0xE0) len = 3;
            else if ((c & 0xF8) == 0xF0) len = 4;
            chars.push_back(s.substr(i, len));
            i += len;
        }
        return chars;
    };

    // Helper para desenhar strings no buffer (com suporte a UTF-8)
    auto paintStr = [&](int postX, int postY, const std::string& txt, const std::string& color, const std::string& forcedBg = "") {
        if (postY < 0 || postY >= heightVisible) return;
        std::vector<std::string> chars = splitUTF8(txt);
        int len = static_cast<int>(chars.size());
        int drawX = postX - len/2;
        for (int i = 0; i < len; ++i) {
            int tx = drawX + i;
            if (tx >= 0 && tx < widthScreen) {
                std::string bg = forcedBg.empty() ? getBg(screen[postY * widthScreen + tx]) : forcedBg;
                screen[postY * widthScreen + tx] = bg + color + chars[i] + "\033[0m";
            }
        }
    };

    // Helper para desenhar strings no buffer (com suporte a UTF-8, alinhado a esquerda)
    auto paintStrLeft = [&](int postX, int postY, const std::string& txt, const std::string& color, const std::string& forcedBg = "") {
        if (postY < 0 || postY >= heightVisible) return;
        std::vector<std::string> chars = splitUTF8(txt);
        int len = static_cast<int>(chars.size());
        for (int i = 0; i < len; ++i) {
            int tx = postX + i;
            if (tx >= 0 && tx < widthScreen) {
                std::string bg = forcedBg.empty() ? getBg(screen[postY * widthScreen + tx]) : forcedBg;
                screen[postY * widthScreen + tx] = bg + color + chars[i] + "\033[0m";
            }
        }
    };

    // So desenha nameplate/HP bar se nao estiver morrendo e se spriteOpacity >= 1.0f
    if (drawBody && !isDeath && spriteOpacity >= 1.0f) {
        int nameY = startY - 2;
        if (nameY >= 0) {
            std::string nameplate = enemy->getName();
            if (totalEnemies > 1) {
                nameplate += " (" + std::to_string(enemyIdx + 1) + ")";
            }
            std::string nameColor = "\033[1;38;2;255;100;100m"; // Default vermelho claro
            
            if (isSelected) {
                nameplate = "> " + nameplate + " <";
                if (CombatScreen::context.blinkSelection) {
                    nameColor = "\033[1;38;2;255;255;0m"; // Amarelo
                } else {
                    nameColor = "\033[1;38;2;120;120;120m"; // Cinza
                }
            } else if (enemy == g_enemyAttackerParry) {
                nameColor = "\033[1;38;2;255;140;0m"; // Laranja ao atacar
            }
            
            paintStr(startX + croppedWidth/2, nameY, nameplate, nameColor);
            
            int hpY = startY - 1;
            if (hpY >= 0) {
                double pct = static_cast<double>(enemy->getHealth()) / std::max(1, enemy->getMaxHealth());
                std::string hpValStr = std::to_string(enemy->getHealth()) + "/" + std::to_string(enemy->getMaxHealth());
                
                int totalLen = 5 + 8 + 2 + (int)hpValStr.length(); // "HP: [" (5) + 8 blocks + "] " (2) + hpValStr
                int drawX = (startX + croppedWidth / 2) - totalLen / 2;
                
                // 1. "HP: ["
                paintStrLeft(drawX, hpY, "HP: [", "\033[38;2;180;180;180m");
                drawX += 5;
                
                // 2. Blocos com gradiente
                int blocks = 8;
                int qtyReal = static_cast<int>(pct * blocks * 8);
                
                Color baseColorLife = (pct > 0.70) ? Color::GREEN : (pct > 0.30) ? Color::YELLOW : Color::RED;
                if (flashDamageEnemy > 0 && flashDamageEnemy % 2 != 0) {
                    baseColorLife = Color::WHITE; // Pisca branco/vermelho
                }
                
                for (int i = 0; i < blocks; ++i) {
                    int intensity = 130 + (125 * i) / std::max(1, blocks - 1);
                    std::string colorCurrent = Appearance::getColorRGBFade(baseColorLife, intensity);
                    int charIdx = i * 8;
                    if (qtyReal >= charIdx + 4) {
                        paintStrLeft(drawX + i, hpY, "█", colorCurrent);
                    } else {
                        paintStrLeft(drawX + i, hpY, "░", "\033[38;2;140;140;140m"); // Cor::CINZA e 140, 140, 140
                    }
                }
                drawX += blocks;
                
                // 3. "] "
                paintStrLeft(drawX, hpY, "] ", "\033[38;2;180;180;180m");
                drawX += 2;
                
                // 4. hpValStr (Orange if attacker, otherwise color based on life percentage)
                std::string hpColor = "\033[38;2;255;100;100m";
                if (enemy == g_enemyAttackerParry) {
                    hpColor = "\033[38;2;255;140;0m"; // Laranja ao atacar
                } else {
                    hpColor = Appearance::getColorRGBFade(baseColorLife, 200);
                }
                paintStrLeft(drawX, hpY, hpValStr, hpColor);
            }
        }

        // Floating Combat Text (FCT)
        if (damageAmount > 0 && flashDamageEnemy > 0) {
            int fctY = startY - 3;
            if (fctY < 0) fctY = 0;
            
            std::string textFCT = isCure ? ("+" + std::to_string(damageAmount)) : ("-" + std::to_string(damageAmount));
            std::string colorFCT = isCure ? "\033[1;38;2;50;255;50m" : "\033[1;38;2;255;50;50m";
            
            int tremble = (!isCure && flashDamageEnemy % 2 == 0) ? 1 : -1;
            
            paintStr(startX + croppedWidth/2 + tremble, fctY, textFCT, colorFCT);
        }
    }


}
