#include "RaycasterMundo.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <string_view>

#include "../Mundo/Vila/Mapa1VilaLayout.h"
#include "../Mundo/Floresta/Mapa2FlorestaLayout.h"
#include "../Mundo/Reino/Mapa3PonteReinoLayout.h"
#include "../Mundo/Reino/Mapa4ReinoLayout.h"

static thread_local size_t g_currentMapHash = 0;

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

char RaycasterMundo::obterNPCProximo(const std::string& tituloMapa, int mapX, int mapY) {
    std::vector<std::string> layout;
    std::string upper = tituloMapa;
    for (char& c : upper) c = std::toupper(static_cast<unsigned char>(c));

    if (upper == "REINO" || upper == "PATIO DO REINO") {
        layout = Mapa4ReinoLayouts::obterLayoutReino();
    } else if (upper.find("IGREJA") != std::string::npos) {
        layout = Mapa4ReinoLayouts::obterLayoutIgreja();
    } else if (upper.find("PONTE") != std::string::npos || upper == "CAMINHO DO REINO") {
        layout = Mapa3PonteReinoLayouts::obterLayoutPonteReino();
    } else if (upper.find("VILA") != std::string::npos) {
        layout = Mapa1VilaLayouts::obterLayoutVilaInicial();
    } else if (upper.find("FLORESTA") != std::string::npos) {
        layout = Mapa2FlorestaLayouts::obterLayoutFloresta();
    } else if (upper.find("CAVERNA") != std::string::npos) {
        layout = Mapa1VilaLayouts::obterLayoutCaverna(false);
    }

    if (layout.empty()) return ' ';

    int height = layout.size();
    int searchRadius = 12; // Procurar em um raio de até 12 células
    char npcEncontrado = ' ';
    int minDistSq = 9999;

    for (int dy = -searchRadius; dy <= searchRadius; ++dy) {
        for (int dx = -searchRadius; dx <= searchRadius; ++dx) {
            int ny = mapY + dy;
            int nx = mapX + dx;
            if (ny >= 0 && ny < height && nx >= 0 && nx < (int)layout[ny].size()) {
                char c = layout[ny][nx];
                if (c == 'B' || c == 'F' || c == 'Q' || c == 'A' || c == 'I' || c == 'P' || c == 'C' || c == 'T' || c == 'M') {
                    int distSq = dx * dx + dy * dy;
                    if (distSq < minDistSq) {
                        minDistSq = distSq;
                        npcEncontrado = c;
                    }
                }
            }
        }
    }
    return npcEncontrado;
}

static thread_local std::string g_currentMapTitle = "";

static const MapFlags& obterFlagsMapa(const std::string& tituloMapa) {
    static thread_local std::string lastTitulo;
    static thread_local MapFlags flags;
    if (tituloMapa != lastTitulo) {
        lastTitulo = tituloMapa;
        g_currentMapTitle = tituloMapa;
        std::string upper = tituloMapa;
        for (char& ch : upper) ch = std::toupper(static_cast<unsigned char>(ch));
        flags.tituloUpper = upper;
        flags.isReino = (upper.find("PATIO DO REINO") != std::string::npos || upper.find("REINO") != std::string::npos);
        flags.isCaverna = (upper.find("CAVERNA") != std::string::npos || upper.find("CORACAO") != std::string::npos);
        flags.isLabirinto = (upper.find("LABIRINTO") != std::string::npos);
        flags.isSalaChefe = (upper.find("CHEFE") != std::string::npos);
        flags.isSpawn = (upper.find("INICIO") != std::string::npos);
        flags.isTerra = (upper.find("FLORESTA") != std::string::npos || 
                         upper.find("BOSQUE") != std::string::npos ||
                         upper.find("VILA") != std::string::npos ||
                         upper.find("INICIO") != std::string::npos);
        
        if (flags.isCaverna || flags.isLabirinto || flags.isSalaChefe) {
            flags.temaCeu = 0;
        } else if (upper.find("CABANA") != std::string::npos) {
            flags.temaCeu = 3;
        } else if (upper.find("FLORESTA") != std::string::npos || upper.find("BOSQUE") != std::string::npos) {
            flags.temaCeu = 1;
        } else {
            flags.temaCeu = 2;
        }
    }
    return flags;
}

static Pixel3D aplicarNevoa(int r, int g, int b, float distancia, float profundidadeMaxima, int temaCeu, const std::vector<std::tuple<int, int, int>>& luzes, float hitX, float hitY, bool escurecer = false) {
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
        float maxDist = (tipoLuz == 1) ? 6.0f : 5.0f;
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
    
    Pixel3D px;
    px.r = static_cast<uint8_t>(finalR);
    px.g = static_cast<uint8_t>(finalG);
    px.b = static_cast<uint8_t>(finalB);
    px.ch = ' ';
    px.hasFg = false;
    px.isFundo = false;
    return px;
}

bool RaycasterMundo::isTemaFloresta(const std::string& tituloMapa) {
    const auto& flags = obterFlagsMapa(tituloMapa);
    return (flags.tituloUpper.find("FLORESTA") != std::string::npos || flags.tituloUpper.find("BOSQUE") != std::string::npos || flags.tituloUpper.find("CABANA") != std::string::npos);
}

bool RaycasterMundo::isEntity(char c) {
    return (c == 'G' || c == 'O' || c == 'B' || c == 'F' || c == 'S' || c == 'A' || c == 'M' || c == 'T' || c == 'H' || c == 'R' || c == 'P' || c == '^' || c == '*' || c == 'C' || c == 'I' || c == 'Q');
}

bool RaycasterMundo::isTeleport(char c) { return c == '^'; }

bool RaycasterMundo::isWalkable(int mapX, int mapY, const std::vector<std::string>& matrizDoMapa) {
    char c = matrizDoMapa[mapY][mapX];
    if (c == '*') return false;
    
    if (c == '=' || c == '|' || c == '\'' || c == '+') return false;

    if (c == '.' || c == ' ' || c == '^' || c == '~' || isEntity(c)) return true;
    if (isMapLabel(mapX, mapY, matrizDoMapa)) return true;
    return false;
}

void RaycasterMundo::atualizarMapHash(const std::vector<std::string>& matrizDoMapa) {
    size_t hash = 0;
    for (const auto& r : matrizDoMapa) {
        hash ^= std::hash<std::string_view>{}(r) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    g_currentMapHash = hash;
}

size_t RaycasterMundo::obterMapHash() {
    return g_currentMapHash;
}

bool RaycasterMundo::isMapLabel(int mapX, int mapY, const std::vector<std::string>& matrizDoMapa) {
    static thread_local size_t lastMapHash = 0;
    static thread_local std::vector<std::vector<char>> cachedLabels;

    int height = matrizDoMapa.size();
    if (height == 0) return false;
    int width = matrizDoMapa[0].size();
    if (mapY < 0 || mapY >= height || mapX < 0 || mapX >= width) return false;

    size_t hash = g_currentMapHash;
    if (hash == 0) {
        for (const auto& r : matrizDoMapa) {
            hash ^= std::hash<std::string_view>{}(r) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
    }

    if (hash != lastMapHash) {
        lastMapHash = hash;
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

Pixel3D RaycasterMundo::obterPixelParedeInternal(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao, const std::vector<std::tuple<int, int, int>>& luzes, float hitX, float hitY, bool isSideWall) {
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

    char npcEncontrado = obterNPCProximo(tituloMapa, (int)hitX, (int)hitY);

    if (npcEncontrado == 'M') {
        // Morgana ('M') -> Cabana de madeira escura com pequenos cogumelos
        bool isTabua = (tx % 10 == 0);
        if (isTabua) { 
            baseR = 30; baseG = 15; baseB = 5; 
        } else { 
            bool hasGrain = ((tx * 3 + ty * 7) % 5) == 0;
            if (hasGrain) {
                baseR = 50; baseG = 25; baseB = 10;
            } else {
                baseR = 40; baseG = 20; baseB = 8;
            }
        }

        // Pequenos cogumelos espalhados na base da parede
        if (ty >= 50 && ty <= 60) {
            int posCogumeloX = ((ty * 13) % 64);
            if (std::abs(tx - posCogumeloX) < 3) {
                if (ty < 54) { // Chapéu do cogumelo
                    baseR = 150; baseG = 30; baseB = 150; // Roxo místico
                    if ((tx + ty) % 2 == 0) { // Pontinhos brilhantes
                        baseR = 200; baseG = 100; baseB = 255;
                    }
                } else { // Caule
                    baseR = 180; baseG = 180; baseB = 150;
                }
            }
        }
    } else if (flags.isLabirinto) {
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
            // Se estivermos dentro da Igreja (submapa da Igreja)
            if (flags.tituloUpper.find("IGREJA") != std::string::npos) {
                // Vitrais geométricos coloridos da igreja
                bool isVitral = (tx % 32 >= 8 && tx % 32 <= 24 && ty >= 12 && ty <= 52);
                if (isVitral) {
                    int padrao = (tx / 4 + ty / 4) % 4;
                    if (padrao == 0) { baseR = 230; baseG = 30; baseB = 30; }
                    else if (padrao == 1) { baseR = 30; baseG = 120; baseB = 230; }
                    else if (padrao == 2) { baseR = 230; baseG = 200; baseB = 20; }
                    else { baseR = 30; baseG = 200; baseB = 100; }
                } else {
                    baseR = 210; baseG = 205; baseB = 195; // Mármore claro
                }
            }
            // Se estivermos na Ponte do Reino (Mapa 3) -> Sempre Madeira Pura
            else if (flags.tituloUpper.find("PONTE") != std::string::npos) {
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
            }
            // Outros mapas (PATIO DO REINO, Vila, etc.)
            else {
                char npcEncontrado = obterNPCProximo(tituloMapa, (int)hitX, (int)hitY);

                // Em volta do Alquimista ('Q') -> Prateleiras de poções
                if (npcEncontrado == 'Q') {
                    bool isPrateleira = (ty == 12 || ty == 28 || ty == 44);
                    if (isPrateleira) {
                        baseR = 80; baseG = 40; baseB = 15;
                    } else {
                        int vidroCol = (tx % 12);
                        if (vidroCol >= 3 && vidroCol <= 8 && ((ty % 16) > 4 && (ty % 16) < 12)) {
                            int corPocao = (tx / 12) % 3;
                            if (corPocao == 0) { baseR = 0; baseG = 220; baseB = 255; }
                            else if (corPocao == 1) { baseR = 255; baseG = 0; baseB = 128; }
                            else { baseR = 50; baseG = 255; baseB = 50; }
                        } else {
                            baseR = 40; baseG = 30; baseB = 20;
                        }
                    }
                }
                // Em volta da Igreja ('I' ou 'P') -> Entrada da Igreja (Mármore e porta de madeira com arco)
                else if (npcEncontrado == 'I' || npcEncontrado == 'P') {
                    // Pilares de mármore nas bordas
                    if (tx < 8 || tx > 55) {
                        baseR = 210; baseG = 205; baseB = 195;
                    } 
                    // Arco de pedra sobre a porta
                    else if (ty < 16) {
                        // Formato de arco
                        float arcX = (tx - 32.0f) / 24.0f;
                        float arcY = (ty - 16.0f) / 16.0f;
                        if (arcX*arcX + arcY*arcY > 1.0f) {
                            baseR = 190; baseG = 185; baseB = 175; // Pedra do arco
                        } else {
                            // Vitral acima da porta (meio círculo)
                            int padrao = (tx / 4 + ty / 4) % 3;
                            if (padrao == 0) { baseR = 230; baseG = 30; baseB = 30; }
                            else if (padrao == 1) { baseR = 30; baseG = 120; baseB = 230; }
                            else { baseR = 230; baseG = 200; baseB = 20; }
                        }
                    }
                    // Porta dupla de madeira pesada
                    else {
                        // Linha no meio dividindo as portas
                        if (tx >= 31 && tx <= 32) {
                            baseR = 20; baseG = 10; baseB = 5;
                        } 
                        // Dobradiças/maçanetas de ferro/ouro
                        else if (ty >= 38 && ty <= 42 && (tx == 28 || tx == 36)) {
                            baseR = 218; baseG = 165; baseB = 32; // Ouro
                        }
                        // Madeira escura da porta
                        else {
                            bool isVerticalLine = (tx % 4 == 0);
                            if (isVerticalLine) {
                                baseR = 40; baseG = 20; baseB = 10;
                            } else {
                                baseR = 60; baseG = 30; baseB = 15;
                            }
                        }
                    }
                }
                // Anok ('A') -> Manequins detalhados com manto degradê e pedestal (Mantido perfeito)
                else if (npcEncontrado == 'A') {
                    // Fundo de madeira elegante do ateliê
                    bool isVerticalJoint = (tx == 0 || tx == 63);
                    bool isHorizontalJoint = (ty % 16 == 0);
                    if (isVerticalJoint || isHorizontalJoint) {
                        baseR = 50; baseG = 30; baseB = 15;
                    } else {
                        int grain = (tx * 3 + ty * 7) % 8;
                        baseR = 110 - grain * 2;
                        baseG = 75 - grain;
                        baseB = 45;
                    }

                    // Cabeça do manequim (elipsoide no topo central)
                    float dx = (tx - 31.5f) / 4.0f;
                    float dy = (ty - 10.5f) / 5.0f;
                    if (dx*dx + dy*dy <= 1.0f) {
                        baseR = 222; baseG = 184; baseB = 135; // Madeira clara/polida
                    }
                    // Pescoço
                    else if (tx >= 30 && tx <= 33 && ty >= 13 && ty <= 15) {
                        baseR = 202; baseG = 164; baseB = 115;
                    }
                    // Corpo vestido com Manto Real
                    else if (ty >= 16 && ty <= 45) {
                        int larguraVestido = 10;
                        if (ty <= 22) {
                            larguraVestido = 12 - (ty - 16); // Ombros
                        } else if (ty <= 30) {
                            larguraVestido = 6 + (ty - 22) / 2; // Cintura fina
                        } else {
                            larguraVestido = 10 + (ty - 30) / 2; // Saia do manto
                        }
                        
                        if (tx >= 32 - larguraVestido && tx <= 32 + larguraVestido) {
                            // Cinto dourado
                            if (ty >= 28 && ty <= 30) {
                                baseR = 218; baseG = 165; baseB = 32;
                            } 
                            // Colarinho gola dourada
                            else if (ty >= 16 && ty <= 18 && tx >= 27 && tx <= 37) {
                                baseR = 218; baseG = 165; baseB = 32;
                            }
                            // Tecido carmesim com relevos verticais
                            else {
                                int dobra = (tx % 6 < 3) ? 20 : 0;
                                baseR = 160 + dobra - (ty - 16);
                                baseG = 20;
                                baseB = 40;
                            }
                        }
                    }
                    // Haste preta/metálica do pedestal
                    else if (tx >= 30 && tx <= 33 && ty > 45 && ty <= 56) {
                        baseR = 80; baseG = 80; baseB = 80;
                    }
                    // Base de madeira escura do pedestal
                    else if (ty > 56 && ty <= 62) {
                        int largBase = (ty - 56) * 3;
                        if (tx >= 32 - largBase && tx <= 32 + largBase) {
                            baseR = 50; baseG = 25; baseB = 10;
                        }
                    }
                }
                else if (npcEncontrado == 'F') {
                    bool isPrateleira = (ty == 16 || ty == 32 || ty == 48);
                    if (isPrateleira) {
                        baseR = 60; baseG = 35; baseB = 15; // Madeira da prateleira
                    } else if (ty > 16 && ty < 24 && (tx % 16 > 2 && tx % 16 < 14)) { // Livros/Caixas
                        baseR = 120; baseG = 40; baseB = 40; // Vermelho
                        if (tx % 4 == 0) { baseR = 200; baseG = 180; baseB = 120; } // Páginas/Detalhes
                    } else if (ty > 32 && ty < 40 && (tx % 12 > 2 && tx % 12 < 10)) { // Sacos de ouro/itens
                        baseR = 160; baseG = 140; baseB = 100; // Tecido cru
                        if (ty < 35) { baseR = 100; baseG = 80; baseB = 50; } // Corda de amarração
                    } else if (ty > 48 && ty < 56 && (tx % 20 > 4 && tx % 20 < 16)) { // Baús pequenos
                        baseR = 100; baseG = 60; baseB = 20; // Madeira
                        if (tx % 20 < 6 || tx % 20 > 14 || ty == 52) { baseR = 80; baseG = 80; baseB = 80; } // Ferro do baú
                    } else {
                        // Fundo da parede de madeira
                        bool isTabua = (tx % 8 == 0); 
                        if (isTabua) { baseR = 40; baseG = 20; baseB = 10; }
                        else { baseR = 50; baseG = 30; baseB = 15; }
                    }
                }
                // Bjorn ('B') -> Fundo com bigorna, espadas, machados e arcos
                else if (npcEncontrado == 'B') {
                    // Fundo da parede (madeira rústica)
                    bool isTabua = (tx % 16 == 0); 
                    if (isTabua) { baseR = 35; baseG = 20; baseB = 10; }
                    else { baseR = 45; baseG = 25; baseB = 15; }

                    // Espada na esquerda (tx=8 a 12, ty=10 a 40)
                    if (tx >= 8 && tx <= 12 && ty >= 10 && ty <= 40) {
                        if (tx == 10 && ty >= 10 && ty <= 30) { baseR = 190; baseG = 195; baseB = 200; } // Lâmina
                        else if (ty >= 30 && ty <= 32 && tx >= 8 && tx <= 12) { baseR = 150; baseG = 130; baseB = 40; } // Guarda de bronze
                        else if (tx == 10 && ty > 32 && ty <= 38) { baseR = 80; baseG = 40; baseB = 10; } // Cabo de couro
                        else if (tx == 10 && ty > 38 && ty <= 40) { baseR = 150; baseG = 130; baseB = 40; } // Pomo
                    }
                    // Machado na direita (tx=52 a 58, ty=15 a 45)
                    else if (tx >= 52 && tx <= 58 && ty >= 15 && ty <= 45) {
                        if (tx == 55 && ty >= 15 && ty <= 45) { baseR = 80; baseG = 50; baseB = 20; } // Cabo de madeira
                        else if (ty >= 18 && ty <= 26 && tx >= 52 && tx <= 58) {
                            if (tx < 55) { baseR = 180; baseG = 180; baseB = 185; } // Lâmina do machado
                        }
                    }
                    // Arco no topo (curva ty = 10 a 20, tx = 20 a 44)
                    else if (ty >= 10 && ty <= 22 && tx >= 20 && tx <= 44) {
                        float dx = (tx - 32.0f) / 12.0f;
                        float dy = (ty - 16.0f) / 6.0f;
                        if (dx*dx + dy*dy > 0.8f && dx*dx + dy*dy < 1.2f && ty < 16) {
                            baseR = 120; baseG = 70; baseB = 30; // Arco de madeira
                        }
                        if (ty == 16 && tx >= 20 && tx <= 44) {
                            baseR = 200; baseG = 200; baseB = 200; // Corda do arco
                        }
                    }
                    // Bigorna no centro-inferior (tx = 22 a 42, ty = 48 a 62)
                    else if (tx >= 22 && tx <= 42 && ty >= 48 && ty <= 62) {
                        if (ty >= 48 && ty <= 52 && tx >= 22 && tx <= 42) {
                            baseR = 70; baseG = 70; baseB = 75; // Topo da bigorna (largo)
                        } else if (ty > 52 && ty <= 58 && tx >= 28 && tx <= 36) {
                            baseR = 60; baseG = 60; baseB = 65; // Corpo da bigorna (estreito)
                        } else if (ty > 58 && ty <= 62 && tx >= 26 && tx <= 38) {
                            baseR = 50; baseG = 50; baseB = 55; // Base da bigorna
                        }
                    }
                }
                // Morgana ('M') -> Cabana de madeira escura com pequenos cogumelos
                else if (npcEncontrado == 'M') {
                    // Fundo de madeira escura
                    bool isTabua = (tx % 10 == 0);
                    if (isTabua) { 
                        baseR = 30; baseG = 15; baseB = 5; 
                    } else { 
                        bool hasGrain = ((tx * 3 + ty * 7) % 5) == 0;
                        if (hasGrain) {
                            baseR = 50; baseG = 25; baseB = 10;
                        } else {
                            baseR = 40; baseG = 20; baseB = 8;
                        }
                    }

                    // Pequenos cogumelos espalhados na base da parede
                    if (ty >= 50 && ty <= 60) {
                        int posCogumeloX = ((ty * 13) % 64);
                        if (std::abs(tx - posCogumeloX) < 3) {
                            if (ty < 54) { // Chapéu do cogumelo
                                baseR = 150; baseG = 30; baseB = 150; // Roxo místico
                                if ((tx + ty) % 2 == 0) { // Pontinhos brilhantes
                                    baseR = 200; baseG = 100; baseB = 255;
                                }
                            } else { // Caule
                                baseR = 180; baseG = 180; baseB = 150;
                            }
                        }
                    }
                }
                // Demais NPCs -> Madeira Pura
                else {
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
                }
            }
        }
        // Se for pedra (#) ou outra estrutura
        else {
            if (flags.tituloUpper.find("IGREJA") != std::string::npos) {
                if (hitX < 10.0f) {
                    // Altar de ouro brilhante
                    baseR = 255; baseG = 215; baseB = 0;
                    if ((tx + ty) % 4 == 0) {
                        baseR = 255; baseG = 240; baseB = 150;
                    }
                } else {
                    // Vitrais coloridos
                    bool isVitral = (tx % 32 >= 8 && tx % 32 <= 24 && ty >= 12 && ty <= 52);
                    if (isVitral) {
                        int padrao = (tx / 4 + ty / 4) % 4;
                        if (padrao == 0) { baseR = 230; baseG = 30; baseB = 30; }
                        else if (padrao == 1) { baseR = 30; baseG = 120; baseB = 230; }
                        else if (padrao == 2) { baseR = 230; baseG = 200; baseB = 20; }
                        else { baseR = 30; baseG = 200; baseB = 100; }
                    } else {
                        baseR = 80; baseG = 80; baseB = 85;
                    }
                }
            } else {
                // Muros de pedra do PATIO DO REINO normais (incluindo pilares e colunas centrais)
                bool isBattlementGap = (ty < 12 && (tx % 32) >= 16);
                if (isBattlementGap) {
                    Pixel3D px;
                    px.isFundo = true;
                    return px;
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
            
            bool isCogumelo = ((tx * 11 + ty * 13) % 47) < 2;
            if (isCogumelo) {
                baseR = 255; baseG = 50; baseB = 50; // Cogumelos vermelhos
            } else if (((tx * 17 + ty * 19) % 53) < 2) {
                baseR = 200; baseG = 200; baseB = 255; // Cogumelos azulados/brancos
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

    if (!isReino && temaFloresta && charParede == '#' && npcEncontrado != 'M') {
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

Pixel3D RaycasterMundo::obterPixelParede(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao, bool isSideWall, const std::vector<std::tuple<int, int, int>>& luzes, float hitX, float hitY) {
    return obterPixelParedeInternal(tituloMapa, temaFloresta, distanciaAteParede, profundidadeMaxima, charParede, y, teto, chao, texX, tempoAnimacao, luzes, hitX, hitY, isSideWall);
}

Pixel3D RaycasterMundo::obterPixelChao(const std::string& tituloMapa, float currentX, float currentY, float currentDist, float profundidadeMaxima, const std::vector<std::tuple<int, int, int>>& luzes) {
    const auto& flags = obterFlagsMapa(tituloMapa);
    int temaCeu = flags.temaCeu;
    currentDist *= 0.55f;

    bool isTerra = flags.isTerra;
    bool isLabirinto = flags.isLabirinto;
    bool isSalaChefe = flags.isSalaChefe;

    unsigned int globX = static_cast<unsigned int>(std::abs(currentX * 32.0f));
    unsigned int globY = static_cast<unsigned int>(std::abs(currentY * 32.0f));

    char c = ' ';
    int r = 0, g = 0, b = 0;
    uint8_t fgR = 0, fgG = 0, fgB = 0;

    if (isLabirinto) {
        fgR = 150; fgG = 130; fgB = 90;
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
        fgR = 50; fgG = 50; fgB = 50; 
        if (spiral > 0.3f) c = '@';
        else if (spiral > 0.0f) c = '%';
        else if (spiral > -0.3f) c = '.';
        else c = ' ';
    } else if (isTerra) {
        fgR = 45; fgG = 25; fgB = 10;
        if ((globX + globY) % 2 == 0) { r = 28; g = 18; b = 8; }
        else if ((globX * 3 + globY * 5) % 7 < 2) { r = 22; g = 12; b = 4; }
        else { r = 25; g = 15; b = 5; }
        
        if ((globX * 17 + globY * 23) % 47 < 4) c = '.';
        else if ((globX * globX + globY * 13) % 53 < 3) c = '-';
        else if ((globX * 3 + globY * 7) % 31 < 2) c = '`';
    } else {
        fgR = 60; fgG = 60; fgB = 60;
        if ((globX + globY) % 2 == 0) { r = 24; g = 24; b = 24; }
        else if ((globX * 3 + globY * 5) % 7 < 2) { r = 16; g = 16; b = 16; }
        else { r = 20; g = 20; b = 20; }
        
        if ((globX * 17 + globY * 23) % 47 < 4) c = '.';
        else if ((globX * globX + globY * 13) % 53 < 3) c = '-';
        else if ((globX * 3 + globY * 7) % 31 < 2) c = '`';
    }
    
    Pixel3D px = aplicarNevoa(r, g, b, currentDist, profundidadeMaxima, temaCeu, luzes, currentX, currentY);
    if (c != ' ' && currentDist <= profundidadeMaxima * 0.5f) {
        px.ch = c;
        px.fgR = fgR;
        px.fgG = fgG;
        px.fgB = fgB;
        px.hasFg = true;
    }
    return px;
}

Pixel3D RaycasterMundo::obterPixelAgua(float currentX, float currentY, float currentDist, float profundidadeMaxima, float raioAngulo, float tempoAnimacao, int temaCeu) {
    int baseR=0, baseG=0, baseB=0;
    currentDist *= 0.55f;

    int tx = (int)(currentX * 32.0f) % 32;
    int ty = (int)(currentY * 32.0f) % 32;

    bool isOnda = ((tx + ty) % 4 == 0) || (((tx / 2) + ty) % 5 == 0);
    if (isOnda) {
        baseR = 80; baseG = 180; baseB = 255;
    } else {
        baseR = 50; baseG = 150; baseB = 255;
    }
    
    float angleOffset = std::sin(tempoAnimacao + currentX * 0.5f) * 0.1f;
    float diffAnguloLua = std::fmod(raioAngulo + angleOffset, 2.0f * 3.14159f);
    if (diffAnguloLua < 0) diffAnguloLua += 2.0f * 3.14159f;
    if (diffAnguloLua < 0.2f || diffAnguloLua > 6.0f) {
        baseR += 40; baseG += 40; baseB += 40;
    }
    std::vector<std::tuple<int, int, int>> noLuzes;
    return aplicarNevoa(baseR, baseG, baseB, currentDist, profundidadeMaxima, temaCeu, noLuzes, currentX, currentY);
}

int RaycasterMundo::obterTemaCeu(const std::string& tituloMapa) {
    const auto& flags = obterFlagsMapa(tituloMapa);
    return flags.temaCeu;
}

Pixel3D RaycasterMundo::obterPixelTeto(int temaCeu, float raioAngulo, int y, int alturaTela, float tempoAnimacao) {
    int horizonte = alturaTela / 2;
    if (temaCeu == 3) {
        Pixel3D px;
        float ratioY = (horizonte > 0) ? (float)y / (float)horizonte : 1.0f;
        int tx = (int)(raioAngulo * 30.0f) % 64;
        int ty = (int)(ratioY * 30.0f) % 64;
        if (tx < 0) tx += 64;
        if (ty < 0) ty += 64;
        
        bool isJunta = (ty % 10 == 0) || (tx % 8 == 0);
        if (isJunta) {
            px.r = 30; px.g = 15; px.b = 5;
        } else {
            px.r = 45; px.g = 25; px.b = 10;
        }
        px.ch = ' ';
        px.isFundo = false;
        return px;
    }
    if (temaCeu == 0) {
        Pixel3D px;
        px.ch = ' ';
        if (g_currentMapTitle.find("IGREJA") != std::string::npos) {
            float divHorizonte = (horizonte > 0) ? (float)horizonte : 1.0f;
            float ratioY = (float)y / divHorizonte;
            int colorIndex = (int)(raioAngulo * 5.0f + ratioY * 8.0f) % 5;
            px.isFundo = false;
            px.hasFg = true;
            px.ch = '#';
            if (colorIndex == 0) { px.r = 150; px.g = 50; px.b = 50; px.fgR = 100; px.fgG = 30; px.fgB = 30; }
            else if (colorIndex == 1) { px.r = 50; px.g = 100; px.b = 150; px.fgR = 30; px.fgG = 70; px.fgB = 100; }
            else if (colorIndex == 2) { px.r = 180; px.g = 150; px.b = 50; px.fgR = 130; px.fgG = 110; px.fgB = 30; }
            else if (colorIndex == 3) { px.r = 50; px.g = 120; px.b = 80; px.fgR = 30; px.fgG = 80; px.fgB = 50; }
            else { px.r = 120; px.g = 80; px.b = 120; px.fgR = 80; px.fgG = 50; px.fgB = 80; }
            return px;
        }
        return px;
    }

    float ratio = (horizonte > 0) ? (float)y / (float)horizonte : 1.0f;
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;

    if (temaCeu == 1) {
        int r = 5 + (int)(15 * ratio);
        int g = 5 + (int)(20 * ratio);
        int b = 15 + (int)(30 * ratio);
        
        float divHorizonte = (horizonte > 0) ? (float)horizonte : 1.0f;
        float diffAnguloLua = std::fmod(raioAngulo - 0.0f, 2.0f * 3.14159f);
        if (diffAnguloLua < -3.14159f) diffAnguloLua += 2.0f * 3.14159f;
        if (diffAnguloLua > 3.14159f) diffAnguloLua -= 2.0f * 3.14159f;
        
        float distXLua = diffAnguloLua;
        float distYLua = (y - horizonte * 0.25f) / divHorizonte;
        float distLua = std::sqrt(distXLua * distXLua * 6.0f + distYLua * distYLua);
        
        if (distLua < 0.10f) {
            float shadowOffset = 0.04f + std::sin(tempoAnimacao * 1.5f) * 0.015f; 
            float shadowDist = std::sqrt((distXLua - shadowOffset) * (distXLua - shadowOffset) * 6.0f + distYLua * distYLua);
            
            Pixel3D px;
            px.ch = ' ';
            px.isFundo = false;
            px.hasFg = false;
            if (shadowDist < 0.10f) {
                px.r = 60; px.g = 60; px.b = 70;
            } else {
                int craterNoise = ((int)(distXLua * 100) * 17 + y * 23) % 47;
                if (craterNoise < 8) {
                    px.r = 180; px.g = 180; px.b = 190;
                } else {
                    px.r = 230; px.g = 230; px.b = 240;
                }
            }
            return px;
        } else if (distLua < 0.12f) {
            Pixel3D px;
            px.ch = ' ';
            px.r = 120; px.g = 120; px.b = 130;
            return px;
        } else if (distLua < 0.25f) {
            float glowPulse = std::sin(tempoAnimacao * 2.0f) * 0.02f;
            float glow = 1.0f - ((distLua - 0.12f) / (0.13f + glowPulse));
            if (glow > 0.0f) {
                r = std::min(255, r + (int)(40 * glow));
                g = std::min(255, g + (int)(45 * glow));
                b = std::min(255, (int)(b + 60 * glow));
            }
        }

        float anguloNorm = std::fmod(raioAngulo, 2.0f * 3.14159f);
        if (anguloNorm < 0.0f) anguloNorm += 2.0f * 3.14159f;

        unsigned int starX = static_cast<unsigned int>(anguloNorm * 150.0f);
        unsigned int starY = static_cast<unsigned int>(y);
        
        unsigned int hash = starX * 374761393U + starY * 668265263U;
        hash = (hash ^ (hash >> 13)) * 1274126177U;
        int noise = hash % 1000;
        
        Pixel3D px;
        px.r = r; px.g = g; px.b = b;
        px.ch = ' ';
        px.hasFg = false;
        px.isFundo = false;
        
        if (noise == 0 && y < horizonte * 0.8f) { px.ch = '*'; px.fgR = 255; px.fgG = 255; px.fgB = 255; px.hasFg = true; }
        else if (noise < 4 && y < horizonte * 0.8f) { px.ch = '+'; px.fgR = 200; px.fgG = 200; px.fgB = 255; px.hasFg = true; }
        else if (noise < 12 && y < horizonte * 0.8f) { px.ch = '.'; px.fgR = 255; px.fgG = 255; px.fgB = 255; px.hasFg = true; }
        else if (noise < 20 && y < horizonte * 0.8f) { px.ch = '.'; px.fgR = 200; px.fgG = 200; px.fgB = 200; px.hasFg = true; }
        else if (noise == 25 && y < horizonte * 0.8f) { px.ch = '\''; px.fgR = 255; px.fgG = 255; px.fgB = 150; px.hasFg = true; }
        
        return px;
    }

    int r = 10 + (int)(60 * ratio);
    int g = 60 + (int)(90 * ratio);
    int b = 150 + (int)(80 * ratio);

    float divHorizonte = (horizonte > 0) ? (float)horizonte : 1.0f;
    float diffAnguloSol = std::fmod(raioAngulo - 0.0f, 2.0f * 3.14159f);
    if (diffAnguloSol < -3.14159f) diffAnguloSol += 2.0f * 3.14159f;
    if (diffAnguloSol > 3.14159f) diffAnguloSol -= 2.0f * 3.14159f;
    
    float distXSol = diffAnguloSol;
    float distYSol = (y - horizonte * 0.35f) / divHorizonte;
    float distSol = std::sqrt(distXSol * distXSol * 6.0f + distYSol * distYSol);
    
    float angleSol = std::atan2(distYSol, distXSol * 2.449f);
    float rays = std::sin(angleSol * 8.0f + tempoAnimacao * 2.0f);
    float glowRadius = 0.12f + rays * 0.02f;
    
    Pixel3D px;
    px.ch = ' ';
    px.hasFg = false;
    px.isFundo = false;
    
    if (distSol < 0.08f) {
        px.r = 255; px.g = 255; px.b = 255;
        return px;
    } else if (distSol < glowRadius) {
        px.r = 255; px.g = 220; px.b = 50;
        return px;
    } else if (distSol < 0.35f) {
        float glowPulse = std::sin(tempoAnimacao * 1.5f) * 0.03f;
        float glow = 1.0f - ((distSol - glowRadius) / (0.35f - glowRadius + glowPulse)); 
        if (glow > 0.0f) {
            r = std::min(255, r + (int)(150 * glow));
            g = std::min(255, g + (int)(100 * glow));
        }
    }

    px.r = r; px.g = g; px.b = b;
    return px;
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
        return 'V';
    }
    if (flags.tituloUpper.find("SALA DO CHEFE") != std::string::npos && (c == 'M' || c == 'A' || c == 'H' || c == 'O' || c == 'R' || c == 'G')) {
        return 'H';
    }
    if ((flags.tituloUpper.find("CABANA") != std::string::npos || flags.tituloUpper.find("FLORESTA") != std::string::npos) && c == 'M') {
        return 'W';
    }
    if (flags.tituloUpper.find("LABIRINTO") != std::string::npos && c == 'B') {
        return 'X';
    }

    // Customizações para o PATIO DO REINO (e REINO) e Igreja
    if (flags.tituloUpper.find("PATIO DO REINO") != std::string::npos || flags.tituloUpper == "REINO") {
        if (c == 'F') return 'V'; // Franchesco
        if (c == 'A') return 'A'; // Anok (Manequim)
        if (c == 'Q') return 'Q'; // Alquimista
        if (c == 'C') return 'C'; // Cavaleiro de Treino
        if (c == 'I') return 'Y'; // Igreja (Capela)
        if (c == 'B') return 'B'; // Bjorn
    }
    if (flags.tituloUpper.find("IGREJA") != std::string::npos) {
        if (c == 'P') return 'J'; // Padre Benedito
    }

    bool isReino = flags.isReino;
    if (isReino && flags.tituloUpper.find("PATIO DO REINO") == std::string::npos) {
        if (c == 'C' || c == 'G') return 'C';
    }
    return c;
}

std::string RaycasterMundo::obterCorMinimapaEntidade(char c, const std::string& tituloMapa) {
    const auto& flags = obterFlagsMapa(tituloMapa);

    if (flags.tituloUpper.find("VILA") != std::string::npos && c == 'F') return "\033[1;38;2;255;200;50m";
    if ((flags.tituloUpper.find("VILA") != std::string::npos || flags.tituloUpper.find("CAVERNA") != std::string::npos) && c == 'B') return "\033[1;38;2;100;200;255m";
    if (flags.tituloUpper.find("SALA DO CHEFE") != std::string::npos) return "\033[1;38;2;255;255;255m";
    if ((flags.tituloUpper.find("CABANA") != std::string::npos || flags.tituloUpper.find("FLORESTA") != std::string::npos) && c == 'M') return "\033[1;38;2;200;100;255m";
    if (flags.tituloUpper.find("LABIRINTO") != std::string::npos && c == 'B') return "\033[1;38;2;200;150;50m";
    if (c == 'P') return "\033[1;38;2;139;69;19m";
    if (c == '*') return "\033[38;2;34;139;34m";
    
    bool isReino = flags.isReino;
    if (isReino && c == 'C') return "\033[1;38;2;200;200;220m";
    if (isReino && c == 'G') return "\033[1;38;2;255;200;50m";
    return "\033[1;38;2;255;50;50m";
}
