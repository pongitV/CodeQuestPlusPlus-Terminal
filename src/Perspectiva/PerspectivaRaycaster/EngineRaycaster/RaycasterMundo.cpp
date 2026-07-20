#include "RaycasterMundo.h"
#include "GerenciadorTexturas.h"
#include "CacheMapa.h"
#include "Iluminador.h"
#include <algorithm>
#include <cmath>
#include <string_view>
#include "../../GerenciadorPerspectiva.h"

#include "../../../Mapas/Vila/Mapa1VilaLayout.h"
#include "../../../Mapas/Floresta/Mapa2FlorestaLayout.h"
#include "../../../Mapas/Reino/Mapa3PonteReinoLayout.h"
#include "../../../Mapas/Reino/Mapa4ReinoLayout.h"

#include <atomic>

static std::atomic<size_t> g_currentMapHash{0};

char RaycasterMundo::obterNPCProximo(const std::string& tituloMapa, int mapX, int mapY, const std::vector<std::string>* matrizDoMapa) {
    if (matrizDoMapa != nullptr) {
        return CacheMapa::obterNPCProximo(tituloMapa, mapX, mapY, *matrizDoMapa);
    }
    
    static thread_local std::string lastTitulo;
    static thread_local std::vector<std::string> layout;

    if (tituloMapa != lastTitulo) {
        lastTitulo = tituloMapa;
        std::string upper = tituloMapa;
        for (char& c : upper) c = std::toupper(static_cast<unsigned char>(c));

        if (upper == "REINO" || upper == "PATIO DO REINO") layout = Mapa4ReinoLayouts::obterLayoutReino();
        else if (upper.find("IGREJA") != std::string::npos) layout = Mapa4ReinoLayouts::obterLayoutIgreja();
        else if (upper.find("PONTE") != std::string::npos || upper == "CAMINHO DO REINO") layout = Mapa3PonteReinoLayouts::obterLayoutPonteReino();
        else if (upper.find("VILA") != std::string::npos) layout = Mapa1VilaLayouts::obterLayoutVilaInicial();
        else if (upper.find("FLORESTA") != std::string::npos) layout = Mapa2FlorestaLayouts::obterLayoutFloresta();
        else if (upper.find("CAVERNA") != std::string::npos) layout = Mapa1VilaLayouts::obterLayoutCaverna(false);
        else layout.clear();
    }

    return CacheMapa::obterNPCProximo(tituloMapa, mapX, mapY, layout);
}

static thread_local std::string g_currentMapTitle = "";

static const MapFlags& obterFlagsMapa(const std::string& tituloMapa) {
    static thread_local std::string lastTitle;
    if (tituloMapa != lastTitle) {
        lastTitle = tituloMapa;
        g_currentMapTitle = tituloMapa;
    }
    return CacheMapa::obterFlags(tituloMapa);
}


bool RaycasterMundo::isTemaFloresta(const std::string& tituloMapa) {
    const auto& flags = obterFlagsMapa(tituloMapa);
    return (flags.tituloUpper.find("FLORESTA") != std::string::npos || flags.tituloUpper.find("BOSQUE") != std::string::npos || flags.tituloUpper.find("CABANA") != std::string::npos);
}

bool RaycasterMundo::isEntity(char c) {
    if (c == 'T' && g_currentMapTitle.find("CORACAO") != std::string::npos) return false;
    return (c == 'G' || c == 'O' || c == 'B' || c == 'F' || c == 'S' || c == 'A' || c == 'M' || c == 'T' || c == 'H' || c == 'R' || c == 'P' || c == '^' || c == '*' || c == 'C' || c == 'I' || c == 'Q' || c == 'Y' || c == 'Z' || c == 'V' || c == 'W' || c == 'N');
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

Pixel3D RaycasterMundo::obterPixelParedeInternal(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao, const Iluminador::InfoLuz& infoLuz, float hitX, float hitY, bool isSideWall, char npcEncontrado, float nx, float ny) {
    (void)isSideWall; (void)distanciaAteParede; (void)profundidadeMaxima; (void)tempoAnimacao;
    const auto& flags = obterFlagsMapa(tituloMapa);
    int alturaParede = chao - teto;
    
    float texY = 0.0f;
    if (alturaParede > 0) texY = (float)(y - teto) / (float)alturaParede;
    if (texY > 0.999f) texY = 0.999f;
    int tx = (int)(texX * 128.0f) % 128;
    int ty = (int)(texY * 128.0f) % 128;

    bool isReino = flags.isReino;
    bool isEstrutura = (charParede == '|' || charParede == '_' || charParede == '[' || charParede == ']' || charParede == '{' || charParede == '}' || charParede == '/' || charParede == '\\' || charParede == '<' || charParede == '>' || charParede == ';' || charParede == '=' || charParede == '-' || charParede == ':' || charParede == '+');
    bool isLabyrinthArch = (!isReino && temaFloresta && charParede == '#' && hitX >= 125.0f && hitX <= 150.0f && hitY >= 5.0f && hitY <= 15.0f);

    TexID texID = TexID::ParedeInvalida;

    if (flags.isLabirinto) {
        texID = TexID::LabirintoMadeira;
    } else if (isLabyrinthArch) {
        int mapX = (int)hitX;
        int mapY = (int)hitY;
        bool isPilar = (std::abs(mapY - 12) == 1 && mapX >= 132 && mapX <= 136);
        if (isPilar) texID = TexID::LabirintoArcoPilar;
        else texID = TexID::LabirintoArcoFundo;
    } else if (isEstrutura && npcEncontrado == 'M') {
        texID = TexID::MorganaMadeira;
    } else if (isReino && (isEstrutura || charParede == '#' || charParede == '+')) {
        if (charParede == '+') {
            if (flags.isIgreja) texID = TexID::IgrejaParedeAltar;
        } else if (charParede == '|') {
            if (flags.isIgreja) texID = TexID::IgrejaVitral;
            else if (flags.isPonte) texID = TexID::PonteMadeira;
            else {
                if (npcEncontrado == 'Q') texID = TexID::Alquimista;
                else if (npcEncontrado == 'I' || npcEncontrado == 'P') texID = TexID::EntradaIgreja;
                else if (npcEncontrado == 'A' || npcEncontrado == 'N') texID = TexID::ManequimAnok;
                else if (npcEncontrado == 'F') texID = TexID::Franchesco;
                else if (npcEncontrado == 'B') texID = TexID::Bjorn;
                else if (npcEncontrado == 'C') texID = TexID::Cavaleiro;
                else texID = TexID::ReinoMadeira;
            }
        } else {
            if (flags.isIgreja) {
                if (hitX < 10.0f) texID = TexID::IgrejaAltar;
                else texID = TexID::IgrejaParede;
            } else {
                bool isBattlementGap = (ty < 12 && (tx % 32) >= 16);
                if (isBattlementGap) {
                    Pixel3D px;
                    px.isFundo = true;
                    return px;
                }
                texID = TexID::PatioMuro;
            }
        }
    } else if (isEstrutura) {
        if (temaFloresta) texID = TexID::FlorestaEstrutura;
        else texID = TexID::PadraoEstrutura;
    } else if (!isReino && (temaFloresta || flags.isTerra) && charParede == 'T') {
        if (flags.isCoracao) texID = TexID::ArvoreCoracao;
        else texID = TexID::ArvoreFloresta;
    } else if (charParede == '#') {
        if (flags.isFloresta) texID = TexID::ArvoreFloresta;
        else texID = TexID::PedraVila;
    } else if (charParede == 'T') {
        if (flags.isIgreja) texID = TexID::IgrejaTeto;
        else texID = TexID::PadraoEstrutura; // Default fallback for T
    } else if (!isReino) {
        if (flags.isSpawn) texID = TexID::PedraSpawn;
        else if (flags.isSalaChefe) texID = TexID::SalaChefeParede;
        else if (flags.isCaverna) {
            if (flags.isCoracao) texID = TexID::CavernaCoracaoParede;
            else texID = TexID::PedraVila;
        } else {
            if (flags.isFloresta) texID = TexID::ArvoreFloresta;
            else texID = TexID::PedraVila;
        }
    } else {
        if (flags.isFloresta) texID = TexID::ArvoreFloresta;
        else texID = TexID::PedraVila;
    }

    CorRGB cor = GerenciadorTexturas::obterCor(texID, tx, ty);
    return Iluminador::aplicarLuzPrecalculada(cor.r, cor.g, cor.b, infoLuz, false, true, nx, ny);
}
Pixel3D RaycasterMundo::obterPixelParedeInternal(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao, const std::vector<std::tuple<int, int, int>>& luzes, float hitX, float hitY, bool isSideWall, char npcEncontrado, float nx, float ny) {
    const auto& flags = obterFlagsMapa(tituloMapa);
    Iluminador::InfoLuz info = Iluminador::calcularInfoLuz(distanciaAteParede * 0.55f, profundidadeMaxima, flags.temaCeu, luzes, hitX, hitY, nullptr, tempoAnimacao);
    return obterPixelParedeInternal(tituloMapa, temaFloresta, distanciaAteParede, profundidadeMaxima, charParede, y, teto, chao, texX, tempoAnimacao, info, hitX, hitY, isSideWall, npcEncontrado, nx, ny);
}

Pixel3D RaycasterMundo::obterPixelParede(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao, bool isSideWall, const Iluminador::InfoLuz& infoLuz, float hitX, float hitY, char npcEncontrado, float nx, float ny) {
    return obterPixelParedeInternal(tituloMapa, temaFloresta, distanciaAteParede, profundidadeMaxima, charParede, y, teto, chao, texX, tempoAnimacao, infoLuz, hitX, hitY, isSideWall, npcEncontrado, nx, ny);
}

Pixel3D RaycasterMundo::obterPixelParede(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao, bool isSideWall, const std::vector<std::tuple<int, int, int>>& luzes, float hitX, float hitY, char npcEncontrado, float nx, float ny) {
    return obterPixelParedeInternal(tituloMapa, temaFloresta, distanciaAteParede, profundidadeMaxima, charParede, y, teto, chao, texX, tempoAnimacao, luzes, hitX, hitY, isSideWall, npcEncontrado, nx, ny);
}


Pixel3D RaycasterMundo::obterPixelChao(const std::string& tituloMapa, float currentX, float currentY, float currentDist, float profundidadeMaxima, const std::vector<std::tuple<int, int, int>>& luzes, const std::vector<std::string>* matrizDoMapa, float tempoAnimacao) {
    const auto& flags = obterFlagsMapa(tituloMapa);
    int temaCeu = flags.temaCeu;
    currentDist *= 0.55f;

    bool isTerra = flags.isTerra;
    bool isLabirinto = flags.isLabirinto;
    bool isSalaChefe = flags.isSalaChefe;
    bool isCoracao = flags.tituloUpper.find("CORACAO") != std::string::npos;

    unsigned int globX = static_cast<unsigned int>(std::abs(currentX * 128.0f));
    unsigned int globY = static_cast<unsigned int>(std::abs(currentY * 128.0f));
    int tx = globX & 127;
    int ty = globY & 127;

    TexID texID = TexID::ChaoPadrao;
    char c = ' ';
    uint8_t fgR = 0, fgG = 0, fgB = 0;
    uint8_t bgR = 0, bgG = 0, bgB = 0;
    bool isProcedural = false;

    if (isLabirinto) {
        fgR = 150; fgG = 130; fgB = 90;
        bool bordaX = ((globX & 127) < 2) || ((globX & 127) > 125);
        bool bordaY = ((globY & 31) < 2) || ((globY & 31) > 29);
        if (bordaX || bordaY) texID = TexID::ChaoLabirintoBorda;
        else texID = TexID::ChaoLabirinto;
    } else if (isSalaChefe) {
        float cx = (globX & 127) - 64.0f;
        float cy = (globY & 127) - 64.0f;
        float dist = std::sqrt(cx*cx + cy*cy);
        float angle = std::atan2(cy, cx);
        float spiral = GerenciadorTexturas::fastSin(dist * 0.4f - angle * 3.0f);
        fgR = 50; fgG = 50; fgB = 50; 
        if (spiral > 0.3f) c = '@';
        else if (spiral > 0.0f) c = '%';
        else if (spiral > -0.3f) c = '.';
        if (spiral > 0.0f) texID = TexID::ChaoSalaChefeDentro;
        else texID = TexID::ChaoSalaChefeFora;
    } else if (isCoracao) {
        float cx = (globX & 255) - 128.0f;
        float cy = (globY & 255) - 128.0f;
        float dist = std::sqrt(cx*cx + cy*cy);
        float angle = std::atan2(cy, cx);
        float spiral = GerenciadorTexturas::fastSin(dist * 0.2f + angle * 4.0f + globX * 0.1f);
        bool hasMoss = ((globX * 17 + globY * 13) % 100) < 40 || (spiral > 0.5f);
        if (hasMoss) { texID = TexID::ChaoCoracaoMusgo; fgR = 30; fgG = 80; fgB = 20; }
        else if (spiral > 0.0f) { texID = TexID::ChaoCoracaoTerra; fgR = 50; fgG = 30; fgB = 15; }
        else { texID = TexID::ChaoCoracaoEscuro; fgR = 25; fgG = 15; fgB = 10; }
    } else if (flags.isReino || flags.isPonte) {
        float cx = globX * 0.15f;
        float cy = globY * 0.15f;
        float noise = GerenciadorTexturas::fastSin(cx) * GerenciadorTexturas::fastSin(cy);
        
        int row = globY / 24;
        int offset = (row % 2 == 0) ? 0 : 12;
        float blockX = ((globX + offset) % 24) / 24.0f;
        float blockY = (globY % 24) / 24.0f;
        
        bool bordaTile = (blockX < 0.1f || blockX > 0.9f || blockY < 0.1f || blockY > 0.9f);
        
        if (bordaTile) {
            bgR = 20; bgG = 20; bgB = 22; 
        } else {
            int var = (int)(noise * 6.0f);
            bgR = 38 + var; bgG = 40 + var; bgB = 43 + var; 
        }
        c = ' ';
        isProcedural = true;
    } else if (flags.isCaverna && !isCoracao) {
        float cx = globX * 0.06f;
        float cy = globY * 0.06f;
        float cx2 = (globX - globY) * 0.03f;
        float noise = GerenciadorTexturas::fastSin(cx) + GerenciadorTexturas::fastSin(cy) + GerenciadorTexturas::fastSin(cx2);
        
        int var = (int)(noise * 5.0f);
        bgR = 30 + var; bgG = 30 + var; bgB = 30 + var;
        c = ' ';
        isProcedural = true;
    } else if (isTerra) {
        float cx = globX * 0.123f;
        float cy = globY * 0.091f;
        float cx2 = (globX + globY) * 0.054f;
        float cy2 = ((float)globX - globY) * 0.111f;
        float noise = GerenciadorTexturas::fastSin(cx) + GerenciadorTexturas::fastSin(cy) + GerenciadorTexturas::fastSin(cx2) + GerenciadorTexturas::fastSin(cy2);
        bool isGrama = (noise > -3.0f);
        if (isGrama) {
            if (flags.tituloUpper.find("FLORESTA") != std::string::npos) { fgR = 6; fgG = 35; fgB = 6; texID = TexID::ChaoGramaFloresta; }
            else { fgR = 12; fgG = 75; fgB = 12; texID = TexID::ChaoGramaVila; }
        } else {
            fgR = 45; fgG = 25; fgB = 10; texID = TexID::ChaoTerra;
        }
    } else {
        fgR = 60; fgG = 60; fgB = 60; texID = TexID::ChaoPadrao;
        if (((globX * 17 + globY * 23) & 63) < 4) c = '.';
        else if (((globX * globX + globY * 13) & 63) < 3) c = '-';
        else if (((globX * 3 + globY * 7) & 31) < 2) c = '`';
    }
    
    CorRGB cor;
    if (isProcedural) {
        cor.r = bgR; cor.g = bgG; cor.b = bgB;
    } else {
        cor = GerenciadorTexturas::obterCor(texID, tx, ty);
    }
    
    Pixel3D px = Iluminador::aplicarNevoa(cor.r, cor.g, cor.b, currentDist, profundidadeMaxima, temaCeu, luzes, currentX, currentY, false, matrizDoMapa, false, 0.0f, 0.0f, tempoAnimacao);
    if (c != ' ' && currentDist <= profundidadeMaxima * 0.5f) {
        px.ch = c;
        px.fgR = fgR;
        px.fgG = fgG;
        px.fgB = fgB;
        px.hasFg = true;
    }
    return px;
}
Pixel3D RaycasterMundo::obterPixelChao(const std::string& tituloMapa, float currentX, float currentY, float currentDist, float profundidadeMaxima, const Iluminador::InfoLuz& infoLuz) {
    const auto& flags = obterFlagsMapa(tituloMapa);
    currentDist *= 0.55f;

    bool isTerra = flags.isTerra;
    bool isLabirinto = flags.isLabirinto;
    bool isSalaChefe = flags.isSalaChefe;
    bool isCoracao = flags.tituloUpper.find("CORACAO") != std::string::npos;

    unsigned int globX = static_cast<unsigned int>(std::abs(currentX * 128.0f));
    unsigned int globY = static_cast<unsigned int>(std::abs(currentY * 128.0f));

    char c = ' ';
    int r = 0, g = 0, b = 0;
    uint8_t fgR = 0, fgG = 0, fgB = 0;

    if (isLabirinto) {
        fgR = 150; fgG = 130; fgB = 90;
        bool bordaX = ((globX & 127) < 2) || ((globX & 127) > 125);
        bool bordaY = ((globY & 31) < 2) || ((globY & 31) > 29);
        if (bordaX || bordaY) {
            r = 40; g = 40; b = 30;
            c = ' ';
        } else {
            if (((globX + globY) & 1) == 0) { r = 180; g = 160; b = 110; }
            else                          { r = 160; g = 140; b = 95; }
            c = ' ';
        }
    } else if (isSalaChefe) {
        float cx = (globX & 127) - 64.0f;
        float cy = (globY & 127) - 64.0f;
        float dist = std::sqrt(cx*cx + cy*cy);
        float angle = std::atan2(cy, cx);
        float spiral = std::sin(dist * 0.4f - angle * 3.0f);

        r = 5; g = 5; b = 5;
        fgR = 50; fgG = 50; fgB = 50; 
        if (spiral > 0.3f) c = '@';
        else if (spiral > 0.0f) c = '%';
        else if (spiral > -0.3f) c = '.';
        else c = ' ';
    } else if (isCoracao) {
        // Chão de musgo e terra para Coracao da Floresta
        float cx = (globX & 255) - 128.0f;
        float cy = (globY & 255) - 128.0f;
        float dist = std::sqrt(cx*cx + cy*cy);
        float angle = std::atan2(cy, cx);
        float spiral = std::sin(dist * 0.2f + angle * 4.0f + globX * 0.1f);
        
        bool hasMoss = ((globX * 17 + globY * 13) % 100) < 40 || (spiral > 0.5f);
        
        if (hasMoss) {
            r = 30; g = 80; b = 20; // Verde musgo
            fgR = 30; fgG = 80; fgB = 20;
        } else if (spiral > 0.0f) {
            r = 50; g = 30; b = 15; // Madeira/Terra marrom
            fgR = 50; fgG = 30; fgB = 15;
        } else {
            r = 25; g = 15; b = 10; // Madeira/Terra escura
            fgR = 25; fgG = 15; fgB = 10;
        }
        c = ' ';
    } else if (flags.isReino || flags.isPonte) {
        float cx = globX * 0.15f;
        float cy = globY * 0.15f;
        float noise = std::sin(cx) * std::sin(cy);
        
        int row = globY / 24;
        int offset = (row % 2 == 0) ? 0 : 12;
        float blockX = ((globX + offset) % 24) / 24.0f;
        float blockY = (globY % 24) / 24.0f;
        
        bool bordaTile = (blockX < 0.1f || blockX > 0.9f || blockY < 0.1f || blockY > 0.9f);
        
        if (bordaTile) {
            r = 20; g = 20; b = 22; 
        } else {
            int var = (int)(noise * 6.0f);
            r = 38 + var; g = 40 + var; b = 43 + var; 
        }
        c = ' ';
    } else if (flags.isCaverna && !isCoracao) {
        float cx = globX * 0.06f;
        float cy = globY * 0.06f;
        float cx2 = (globX - globY) * 0.03f;
        float noise = std::sin(cx) + std::sin(cy) + std::sin(cx2);
        
        int var = (int)(noise * 5.0f);
        r = 30 + var; g = 30 + var; b = 30 + var;
        c = ' ';
    } else if (isTerra) {
        // Mistura de terra e grama para Vila e Floresta
        // Formula caotica sem padrao repetitivo
        float cx = globX * 0.123f;
        float cy = globY * 0.091f;
        float cx2 = (globX + globY) * 0.054f;
        float cy2 = ((float)globX - globY) * 0.111f;
        float noise = std::sin(cx) + std::sin(cy) + std::sin(cx2) + std::sin(cy2);
        
        bool isGrama = (noise > -3.0f); // Mais grama do que terra
        
        if (isGrama) {
            if (flags.tituloUpper.find("FLORESTA") != std::string::npos) { fgR = 6; fgG = 35; fgB = 6; }
            else { fgR = 12; fgG = 75; fgB = 12; }
            
            if (flags.tituloUpper.find("FLORESTA") != std::string::npos) {
                bool isPatch = ((globX / 16) + (globY / 16)) % 2 == 0;
                if (isPatch) { r = 5; g = 28; b = 5; } else { r = 4; g = 24; b = 4; }
            } else {
                bool isPatch = ((globX / 16) + (globY / 16)) % 2 == 0;
                if (isPatch) { r = 12; g = 65; b = 12; } else { r = 10; g = 58; b = 10; }
            }
            c = ' ';
        } else {
            fgR = 45; fgG = 25; fgB = 10; // Marrom terra
            if (((globX + globY) & 1) == 0) { r = 28; g = 18; b = 8; }
            else if (((globX * 3 + globY * 5) & 7) < 2) { r = 22; g = 12; b = 4; }
            else { r = 25; g = 15; b = 5; }
            c = ' ';
        }
    } else {
        fgR = 60; fgG = 60; fgB = 60;
        if (((globX + globY) & 1) == 0) { r = 24; g = 24; b = 24; }
        else if (((globX * 3 + globY * 5) & 7) < 2) { r = 16; g = 16; b = 16; }
        else { r = 20; g = 20; b = 20; }
        
        if (((globX * 17 + globY * 23) & 63) < 4) c = '.';
        else if (((globX * globX + globY * 13) & 63) < 3) c = '-';
        else if (((globX * 3 + globY * 7) & 31) < 2) c = '`';
    }
    
    Iluminador::InfoLuz infoLocal = infoLuz;
    infoLocal.nevoaPercent = std::min(1.0f, (currentDist / (profundidadeMaxima * 0.8f)) *
                                             (currentDist / (profundidadeMaxima * 0.8f)));
    Pixel3D px = Iluminador::aplicarLuzPrecalculada(r, g, b, infoLocal);
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

    float waveX = GerenciadorTexturas::fastSin(currentX * 4.0f + tempoAnimacao * 2.0f);
    float waveY = GerenciadorTexturas::fastCos(currentY * 4.0f + tempoAnimacao * 1.5f);
    float wave = (waveX + waveY) * 0.5f; 

    if (wave > 0.3f) {
        baseR = 100; baseG = 200; baseB = 255;
    } else if (wave > -0.3f) {
        baseR = 60; baseG = 160; baseB = 235;
    } else {
        baseR = 30; baseG = 130; baseB = 215;
    }
    
    float angleOffset = wave * 0.2f; 
    float angReflexo = raioAngulo + angleOffset;
    while (angReflexo >= 2.0f * 3.14159f) angReflexo -= 2.0f * 3.14159f;
    while (angReflexo < 0) angReflexo += 2.0f * 3.14159f;
    
    if (angReflexo < 0.3f || angReflexo > (2.0f * 3.14159f - 0.3f)) {
        float dif = (angReflexo < 0.3f) ? angReflexo : ((2.0f * 3.14159f) - angReflexo);
        float intensidadeReflexo = 1.0f - (dif / 0.3f);
        intensidadeReflexo *= (0.5f + (wave + 1.0f) * 0.25f); 
        
        baseR = std::min(255, baseR + (int)(155 * intensidadeReflexo));
        baseG = std::min(255, baseG + (int)(95 * intensidadeReflexo));
        if (temaCeu != 1 && temaCeu != 2) baseB = std::min(255, baseB + (int)(255 * intensidadeReflexo)); 
    }

    std::vector<std::tuple<int, int, int>> noLuzes;
    Iluminador::InfoLuz info = Iluminador::calcularInfoLuz(currentDist, profundidadeMaxima, temaCeu, noLuzes, currentX, currentY, nullptr, tempoAnimacao);
    
    float nightFactor = std::max(0.4f, info.solIntensidade);
    baseR = (int)(baseR * nightFactor);
    baseG = (int)(baseG * nightFactor);
    baseB = (int)(baseB * nightFactor);
    
    float nf = info.nevoaPercent;
    float finalR = baseR * (1.0f - nf) + info.fogR * nf;
    float finalG = baseG * (1.0f - nf) + info.fogG * nf;
    float finalB = baseB * (1.0f - nf) + info.fogB * nf;
    
    Pixel3D px;
    px.r = (uint8_t)std::min(255, std::max(0, (int)finalR));
    px.g = (uint8_t)std::min(255, std::max(0, (int)finalG));
    px.b = (uint8_t)std::min(255, std::max(0, (int)finalB));
    px.ch = ' ';
    return px;
}
int RaycasterMundo::obterTemaCeu(const std::string& tituloMapa) {
    const auto& flags = obterFlagsMapa(tituloMapa);
    return flags.temaCeu;
}

Pixel3D RaycasterMundo::obterPixelTeto(int temaCeu, float raioAngulo, float anguloCeu, int y, int alturaTela, float tempoAnimacao, bool isMenu) {
    (void)anguloCeu; (void)tempoAnimacao; (void)isMenu;
    int horizonte = alturaTela / 2;
    if (temaCeu == 3) { 
        Pixel3D px;
        float ratioY = (horizonte > 0) ? (float)y / (float)horizonte : 1.0f;
        int tx = (int)(raioAngulo * 60.0f) % 128;
        int ty = (int)(ratioY * 60.0f) % 128;
        if (tx < 0) tx += 128;
        if (ty < 0) ty += 128;
        
        TexID texID = TexID::TetoIndoorsPadrao;
        bool isCoracao = g_currentMapTitle.find("CORACAO") != std::string::npos;
        bool isSalaChefe = g_currentMapTitle.find("CAVERNA") != std::string::npos || g_currentMapTitle.find("CHEFE") != std::string::npos;
        
        if (isCoracao) {
            float cx = (tx - 64.0f);
            float cy = (ty - 64.0f);
            float dist = std::sqrt(cx*cx + cy*cy);
            float angle = std::atan2(cy, cx);
            float spiral = GerenciadorTexturas::fastSin(dist * 0.2f + angle * 4.0f + tx * 0.1f);
            
            bool hasMoss = ((tx * 17 + ty * 13) % 100) < 20 || (spiral > 0.8f);
            
            if (hasMoss) texID = TexID::TetoIndoorsCoracaoMusgo;
            else if (spiral > 0.0f) texID = TexID::TetoIndoorsCoracaoMadeira;
            else texID = TexID::TetoIndoorsCoracaoEscuro;
        } else if (isSalaChefe) {
            texID = TexID::SalaChefeParede;
        }
        CorRGB cor = GerenciadorTexturas::obterCor(texID, tx, ty);
        px.r = cor.r; px.g = cor.g; px.b = cor.b;
        px.ch = ' '; px.isFundo = false;
        return px;
    }


    float ratio = (horizonte > 0) ? (float)y / (float)horizonte : 1.0f;
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;

    struct CorCeu { float topR, topG, topB, botR, botG, botB; };
    auto misturarCor = [](CorCeu a, CorCeu b, float t) -> CorCeu {
        return { a.topR + (b.topR - a.topR) * t, a.topG + (b.topG - a.topG) * t, a.topB + (b.topB - a.topB) * t,
                 a.botR + (b.botR - a.botR) * t, a.botG + (b.botG - a.botG) * t, a.botB + (b.botB - a.botB) * t };
    };

    CorCeu nascer = { 20, 20, 60,   255, 120, 50 };
    CorCeu dia =    { 10, 60, 150,  70, 150, 230 };
    CorCeu porSol = { 40, 10, 60,   255, 80, 20 };
    CorCeu noite =  { 2, 2, 5,      5, 25, 45 };

    float t = std::fmod(tempoAnimacao, 120.0f) / 120.0f;
    CorCeu corAtual;
    if (t < 0.1f)      corAtual = misturarCor(nascer, dia, t / 0.1f);
    else if (t < 0.4f) corAtual = dia;
    else if (t < 0.5f) corAtual = misturarCor(dia, porSol, (t - 0.4f) / 0.1f);
    else if (t < 0.6f) corAtual = misturarCor(porSol, noite, (t - 0.5f) / 0.1f);
    else if (t < 0.9f) corAtual = noite;
    else               corAtual = misturarCor(noite, nascer, (t - 0.9f) / 0.1f);

    int r = (int)(corAtual.topR * (1.0f - ratio) + corAtual.botR * ratio);
    int g = (int)(corAtual.topG * (1.0f - ratio) + corAtual.botG * ratio);
    int b = (int)(corAtual.topB * (1.0f - ratio) + corAtual.botB * ratio);

    float divHorizonte = (horizonte > 0) ? (float)horizonte : 1.0f;
    float ratioY = (float)y / divHorizonte;
    float globalRotation = tempoAnimacao * 0.05f;

    float diffAnguloSol;
    if (isMenu) {
        // No menu o Sol nasce na esquerda (colAng=-0.8) as t=0.0 e se poe na direita (colAng=+0.8) as t=0.5
        float colAng = raioAngulo;
        diffAnguloSol = colAng - (t - 0.25f) * 3.2f;
    } else {
        diffAnguloSol = std::fmod(raioAngulo - globalRotation, 2.0f * 3.14159f);
        if (diffAnguloSol < -3.14159f) diffAnguloSol += 2.0f * 3.14159f;
        if (diffAnguloSol > 3.14159f) diffAnguloSol -= 2.0f * 3.14159f;
    }
    
    float sunPhase = (t - 0.25f) * 2.0f * 3.14159f;
    float sunElevation = std::cos(sunPhase); 
    float distYSol = isMenu ? (ratioY - (0.5f - 0.2f * sunElevation)) : (ratioY - (0.9f - 1.1f * sunElevation));
    float distSol = std::sqrt(diffAnguloSol * diffAnguloSol * 6.0f + distYSol * distYSol);
    
    float diffAnguloLua;
    if (isMenu) {
        float colAng = raioAngulo;
        diffAnguloLua = colAng - (t - 0.75f) * 3.2f;
    } else {
        diffAnguloLua = std::fmod(raioAngulo - globalRotation + 3.14159f, 2.0f * 3.14159f);
        if (diffAnguloLua < -3.14159f) diffAnguloLua += 2.0f * 3.14159f;
        if (diffAnguloLua > 3.14159f) diffAnguloLua -= 2.0f * 3.14159f;
    }
    
    float moonPhase = (t - 0.75f) * 2.0f * 3.14159f;
    float moonElevation = std::cos(moonPhase);
    float distYLua = isMenu ? (ratioY - (0.5f - 0.2f * moonElevation)) : (ratioY - (0.9f - 1.1f * moonElevation));
    float distLua = std::sqrt(diffAnguloLua * diffAnguloLua * 6.0f + distYLua * distYLua);

    Pixel3D px;
    px.ch = ' '; px.hasFg = false; px.isFundo = false;

    // Draw Moon
    float moonAlpha = 1.0f;
    float moonGlowRadius = 0.11f;

    if (distLua < 0.10f) {
        float shadowOffset = 0.04f + std::sin(tempoAnimacao * 0.5f) * 0.01f; 
        float shadowDist = std::sqrt((diffAnguloLua - shadowOffset) * (diffAnguloLua - shadowOffset) * 6.0f + distYLua * distYLua);
        if (shadowDist < 0.10f) {
            px.r = 25; px.g = 30; px.b = 40; return px;
        } else {
            float lunarX = diffAnguloLua * 20.0f;
            float lunarY = distYLua * 20.0f;
            float rotation = tempoAnimacao * 0.15f; 
            float mariaNoise = std::sin(lunarX * 3.0f + lunarY + rotation) * std::cos(lunarY * 4.0f - lunarX - rotation) 
                             + std::sin(lunarX * 7.0f - rotation);
            if (mariaNoise > 0.4f) {
                px.r = 140; px.g = 150; px.b = 180;
            } else {
                px.r = 240; px.g = 245; px.b = 255;
            }
            return px;
        }
    } else if (distLua < moonGlowRadius) {
        float coronaLerp = (distLua - 0.10f) / (moonGlowRadius - 0.10f);
        px.r = 240 - (int)(40 * coronaLerp); 
        px.g = 245 - (int)(35 * coronaLerp); 
        px.b = 255 - (int)(15 * coronaLerp); 
        return px;
    } else if (distLua < 0.45f) {
        float glowPulse = std::sin(tempoAnimacao * 1.5f) * 0.02f;
        float glowDist = (distLua - moonGlowRadius) / (0.45f - moonGlowRadius + glowPulse);
        float glow = std::max(0.0f, 1.0f - glowDist);
        glow = glow * glow; 
        
        if (glow > 0.0f) {
            r = std::min(255, r + (int)(120 * glow * moonAlpha));
            g = std::min(255, g + (int)(150 * glow * moonAlpha));
            b = std::min(255, b + (int)(220 * glow * moonAlpha));
        }
    }

    // Draw Sun
    float sunAlpha = 1.0f;
    float angleSol = std::atan2(distYSol, diffAnguloSol * 2.449f);
    float rays = std::sin(angleSol * 12.0f + tempoAnimacao * 1.5f) * 0.5f 
               + std::sin(angleSol * 7.0f - tempoAnimacao * 0.8f) * 0.5f;
    float glowRadius = 0.12f + rays * 0.025f;
    
    if (distSol < 0.08f) {
        px.r = 255; px.g = 255; px.b = 255; return px;
    } else if (distSol < glowRadius) {
        float coronaLerp = (distSol - 0.08f) / (glowRadius - 0.08f);
        px.r = 255; 
        px.g = 255 - (int)(35 * coronaLerp); 
        px.b = 255 - (int)(205 * coronaLerp); 
        return px;
    } else if (distSol < 0.45f) {
        float glowPulse = std::sin(tempoAnimacao * 2.0f) * 0.03f;
        float glowDist = (distSol - glowRadius) / (0.45f - glowRadius + glowPulse);
        float glow = std::max(0.0f, 1.0f - glowDist);
        glow = glow * glow; 
        
        if (glow > 0.0f) {
            r = std::min(255, r + (int)(180 * glow * sunAlpha));
            g = std::min(255, g + (int)(110 * glow * sunAlpha));
            b = std::min(255, b + (int)(20 * glow * sunAlpha));
        }
    }

    // Clouds
    float wind = tempoAnimacao * 0.05f; 
    float angleBase = raioAngulo + wind;
    float cloudNoise = std::sin(angleBase * 5.0f) * std::sin((y + wind*10.0f) * 0.1f) 
                     + 0.5f * std::sin(angleBase * 11.0f + (y - wind*5.0f) * 0.13f)
                     + 0.25f * std::sin(angleBase * 23.0f - (y + wind*15.0f) * 0.21f);
    if (cloudNoise > 0.7f) {
        float cloudIntensity = std::min(1.0f, (cloudNoise - 0.7f) * 2.0f);
        cloudIntensity *= (0.5f + (ratio * 0.5f));
        r = r + (int)((255 - r) * cloudIntensity);
        g = g + (int)((255 - g) * cloudIntensity);
        b = b + (int)((255 - b) * cloudIntensity);
    }

    // Stars (only at night)
    if (t > 0.5f && t < 0.95f) {
        float starAlpha = (t > 0.6f && t < 0.85f) ? 1.0f : 0.5f;
        unsigned int starX = static_cast<unsigned int>(raioAngulo * 150.0f);
        unsigned int hash = starX * 374761393U + y * 668265263U;
        hash = (hash ^ (hash >> 13)) * 1274126177U;
        int noise = hash % 1000;
        
        if (noise == 0 && y <= horizonte) { px.ch = '*'; px.fgR = 255; px.fgG = 255; px.fgB = 255; px.hasFg = true; }
        else if (noise < 3 && y <= horizonte) { px.ch = '+'; px.fgR = 200; px.fgG = 200; px.fgB = 255; px.hasFg = true; }
        else if (noise < 10 && y <= horizonte) { px.ch = '.'; px.fgR = 255; px.fgG = 255; px.fgB = 255; px.hasFg = true; }
        else if (noise < 17 && y <= horizonte) { px.ch = '.'; px.fgR = 200; px.fgG = 200; px.fgB = 200; px.hasFg = true; }
        else if (noise == 20 && y <= horizonte) { px.ch = '\''; px.fgR = 255; px.fgG = 255; px.fgB = 150; px.hasFg = true; }
        
        if (px.ch != ' ') {
            px.fgR = (uint8_t)(px.fgR * starAlpha);
            px.fgG = (uint8_t)(px.fgG * starAlpha);
            px.fgB = (uint8_t)(px.fgB * starAlpha);
            px.r = r; px.g = g; px.b = b;
            return px;
        }
    }

    px.r = r; px.g = g; px.b = b;
    return px;
}

char RaycasterMundo::obterSpriteChar(int /*mapX*/, int mapY, char c, const std::string& tituloMapa) {
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

    if (c == '!' || c == '%') {
        return c; // Retorna ! ou % para serem desenhados como sprite pelo RaycasterRenderizador (IDE)
    }
    if (c == '@') {
        return '@'; // Terminal hackeável
    }
    if (c == 'Y' || c == '*') {
        return c;
    }
    const auto& flags = obterFlagsMapa(tituloMapa);
    if (flags.tituloUpper.find("IGREJA") != std::string::npos) {
        if (c == 'P') return 'J'; // Padre Benedito
    }

    if ((flags.tituloUpper.find("VILA") != std::string::npos || flags.tituloUpper.find("CASA") != std::string::npos) && c == 'F') {
        return 'V';
    }
    if (flags.tituloUpper.find("SALA DO CHEFE") != std::string::npos && (c == 'M' || c == 'A' || c == 'H' || c == 'O' || c == 'R' || c == 'G')) {
        return 'H';
    }
    if ((flags.tituloUpper.find("REINO") != std::string::npos || flags.tituloUpper.find("PATIO") != std::string::npos) && c == 'N') {
        return 'Z';
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
        if (c == 'N') return 'Z'; // Anok (Manequim)
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
