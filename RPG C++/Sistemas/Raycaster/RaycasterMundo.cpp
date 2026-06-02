#include "RaycasterMundo.h"
#include <algorithm>
#include <cctype>

bool RaycasterMundo::isTemaFloresta(const std::string& tituloMapa) {
    std::string tituloUpper = tituloMapa;
    for (char& c : tituloUpper) c = std::toupper(static_cast<unsigned char>(c));
    return (tituloUpper.find("FLORESTA") != std::string::npos || tituloUpper.find("BOSQUE") != std::string::npos);
}

bool RaycasterMundo::isEntity(char c) {
    return (c == 'G' || c == 'O' || c == 'B' || c == 'F' || c == 'S' || c == 'A' || c == 'M' || c == 'T' || c == 'H' || c == 'R' || c == 'P');
}

bool RaycasterMundo::isTeleport(char c) { return c == '^'; }

bool RaycasterMundo::isWalkable(int mapX, int mapY, const std::vector<std::string>& matrizDoMapa) {
    char c = matrizDoMapa[mapY][mapX];
    if (c == '.' || c == ' ' || c == '^' || c == '~' || isEntity(c)) return true;
    if (isMapLabel(mapX, mapY, matrizDoMapa)) return true;
    return false;
}

bool RaycasterMundo::isMapLabel(int mapX, int mapY, const std::vector<std::string>& matrizDoMapa) {
    char c = matrizDoMapa[mapY][mapX];
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
        if (isEntity(c)) {
            bool hasHorizontalText = false;
            for (int dx = -1; dx <= 1; dx += 2) {
                int nx = mapX + dx;
                if (mapY >= 0 && mapY < (int)matrizDoMapa.size() && nx >= 0 && nx < (int)matrizDoMapa[mapY].size()) {
                    char adj = matrizDoMapa[mapY][nx];
                    if (adj == '^' || (adj >= 'a' && adj <= 'z')) {
                        hasHorizontalText = true;
                        break;
                    }
                }
            }
            if (!hasHorizontalText) return false;
            return true;
        } else {
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = mapX + dx;
                    int ny = mapY + dy;
                    if (ny >= 0 && ny < (int)matrizDoMapa.size() && nx >= 0 && nx < (int)matrizDoMapa[ny].size()) {
                        char adj = matrizDoMapa[ny][nx];
                        if (adj == '^' || (adj >= 'a' && adj <= 'z')) return true;
                    }
                }
            }
        }
    }
    return false;
}

std::string RaycasterMundo::obterPixelParede(bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX) {
    int alturaParede = chao - teto;
    int metadeParede = teto + (alturaParede / 2);
    
    float texY = 0.0f;
    if (alturaParede > 0) texY = (float)(y - teto) / (float)alturaParede;
    int tx = (int)(texX * 16.0f) % 16;
    int ty = (int)(texY * 16.0f) % 16;

    bool isArvore = false;
    if (charParede == '*') isArvore = true;

    if (isArvore) {
        if (y < metadeParede) { // Topo (Folhas)
            bool sombraFolha = ((tx + ty) % 3 == 0); // Textura quadriculada para simular volume nas folhas
            if (sombraFolha) {
                if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;22;89;22m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;16;70;16m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;10;50;10m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;6;30;6m \033[0m";
                else                                                     return "\033[48;2;0;0;0m \033[0m";
            } else {
                if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;34;139;34m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;28;115;28m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;22;89;22m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;14;60;14m \033[0m";
                else                                                     return "\033[48;2;0;0;0m \033[0m";
            }
        }
        // Base da arvore (Marrom)
        bool isLinhaTronco = (tx % 4 == 0); // Linhas verticais simulando casca
        if (isLinhaTronco) {
            if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;60;35;15m \033[0m";
            else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;45;25;10m \033[0m";
            else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;30;15;5m \033[0m";
            else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;15;5;0m \033[0m";
            else                                                     return "\033[48;2;0;0;0m \033[0m";
        } else {
            if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;101;67;33m \033[0m";
            else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;80;50;20m \033[0m";
            else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;60;35;15m \033[0m";
            else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;40;20;10m \033[0m";
            else                                                     return "\033[48;2;0;0;0m \033[0m";
        }
    }
    
    bool isEstrutura = false;
    std::string charsEstrutura = "|_[]{}/\\<>;=-:+";
    if (charsEstrutura.find(charParede) != std::string::npos) isEstrutura = true;

    if (isEstrutura) {
        // Vermelho/Marrom com Textura de Tijolos Reais (Bricks)
        bool isJunta = (ty % 4 == 0) || (((ty / 4) % 2 == 0) && tx % 8 == 0) || (((ty / 4) % 2 == 1) && (tx + 4) % 8 == 0); 
        if (isJunta) {
            if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;120;120;120m \033[0m";
            else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;80;80;80m \033[0m";
            else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;50;50;50m \033[0m";
            else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;25;25;25m \033[0m";
            else                                                     return "\033[48;2;0;0;0m \033[0m";
        } else {
            if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;160;60;40m \033[0m";
            else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;120;45;30m \033[0m";
            else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;80;30;20m \033[0m";
            else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;40;15;10m \033[0m";
            else                                                     return "\033[48;2;0;0;0m \033[0m";
        }
    }

    // Default / Bordas do Mapa '#' com textura de Tijolos Brancos
    bool isJuntaBranca = (ty % 4 == 0) || (((ty / 4) % 2 == 0) && tx % 8 == 0) || (((ty / 4) % 2 == 1) && (tx + 4) % 8 == 0); 
    if (isJuntaBranca) {
        if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;140;140;140m \033[0m";
        else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;100;100;100m \033[0m";
        else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;70;70;70m \033[0m";
        else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;40;40;40m \033[0m";
        return "\033[48;2;0;0;0m \033[0m";
    } else {
        if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;240;240;240m \033[0m";
        else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;180;180;180m \033[0m";
        else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;120;120;120m \033[0m";
        else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;60;60;60m \033[0m";
        return "\033[48;2;0;0;0m \033[0m";
    }
}

std::string RaycasterMundo::obterPixelChao(const std::string& tituloMapa, int x, int y) {
    std::string tituloUpper = tituloMapa;
    for (char& ch : tituloUpper) ch = std::toupper(static_cast<unsigned char>(ch));

    bool isTerra = (tituloUpper.find("FLORESTA") != std::string::npos || 
                    tituloUpper.find("BOSQUE") != std::string::npos ||
                    tituloUpper.find("VILA") != std::string::npos ||
                    tituloUpper.find("INICIO") != std::string::npos);

    std::string bg = isTerra ? "\033[48;2;25;15;5m" : "\033[48;2;20;20;20m"; // Marrom escuro para terra / Cinza para dungeon
    std::string fg = isTerra ? "\033[38;2;45;25;10m" : "\033[38;2;60;60;60m"; // Detalhes da terra / Detalhes de pedra
    char c = ' ';

    // Padrão de ruído matemático ("hash" simples) para criar manchas e rachaduras no chão
    int noise = (x * 17 + y * 23) % 47;
    if (noise < 3) c = '.'; // Manchas
    else if ((x * x + y * 13) % 53 < 2) c = '-'; // Rachaduras/ranhuras
    else if ((x * 3 + y * 7) % 31 < 1) c = '`'; // Pedregulhos
    
    if (c == ' ') return bg + " \033[0m";
    return bg + fg + std::string(1, c) + "\033[0m";
}

std::string RaycasterMundo::obterPixelAgua(float currentX, float currentY, float currentDist, float profundidadeMaxima) {
    int tx = (int)(currentX * 16.0f) % 16;
    int ty = (int)(currentY * 16.0f) % 16;

    bool isOnda = ((tx + ty) % 4 == 0) || (((tx / 2) + ty) % 5 == 0);
    if (isOnda) {
        if (currentDist <= profundidadeMaxima / 4.0f)     return "\033[48;2;80;180;255m \033[0m";
        else if (currentDist < profundidadeMaxima / 3.0f) return "\033[48;2;60;140;210m \033[0m";
        else if (currentDist < profundidadeMaxima / 2.0f) return "\033[48;2;40;100;160m \033[0m";
        else if (currentDist < profundidadeMaxima)        return "\033[48;2;20;60;100m \033[0m";
        else                                              return "\033[48;2;0;0;0m \033[0m";
    } else {
        if (currentDist <= profundidadeMaxima / 4.0f)     return "\033[48;2;50;150;255m \033[0m";
        else if (currentDist < profundidadeMaxima / 3.0f) return "\033[48;2;40;120;200m \033[0m";
        else if (currentDist < profundidadeMaxima / 2.0f) return "\033[48;2;30;90;150m \033[0m";
        else if (currentDist < profundidadeMaxima)        return "\033[48;2;20;60;100m \033[0m";
        else                                              return "\033[48;2;0;0;0m \033[0m";
    }
}

int RaycasterMundo::obterTemaCeu(const std::string& tituloMapa) {
    std::string tituloUpper = tituloMapa;
    for (char& ch : tituloUpper) ch = std::toupper(static_cast<unsigned char>(ch));

    if (tituloUpper.find("CAVERNA") != std::string::npos ||
        tituloUpper.find("LABIRINTO") != std::string::npos ||
        tituloUpper.find("CHEFE") != std::string::npos ||
        tituloUpper.find("CORACAO") != std::string::npos) {
        return 0; // Preto normal para cavernas e interiores
    }

    if (tituloUpper.find("FLORESTA") != std::string::npos || 
        tituloUpper.find("BOSQUE") != std::string::npos) {
        return 1; // Laranja/Fim de tarde
    }
    
    // Default: Vila, Caminho do Inicio, Reino (Azul Dia)
    return 2; 
}

std::string RaycasterMundo::obterPixelTeto(int temaCeu, int y, int alturaTela) {
    if (temaCeu == 0) return " "; // Interior

    int horizonte = alturaTela / 2;
    float ratio = (horizonte > 0) ? (float)y / (float)horizonte : 1.0f;
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;

    if (temaCeu == 1) { // Floresta (Por do Sol)
        int r = 80 + (int)(130 * ratio); // 80 -> 210
        int g = 20 + (int)(90 * ratio);  // 20 -> 110
        int b = 10 + (int)(30 * ratio);  // 10 -> 40
        return "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m \033[0m";
    }

    // Default (Azul Dia)
    int r = 10 + (int)(60 * ratio);  // 10 -> 70
    int g = 60 + (int)(90 * ratio);  // 60 -> 150
    int b = 150 + (int)(80 * ratio); // 150 -> 230
    return "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m \033[0m"; 
}

char RaycasterMundo::obterSpriteChar(char c, const std::string& tituloMapa) {
    std::string tituloUpper = tituloMapa;
    for (char& ch : tituloUpper) ch = std::toupper(static_cast<unsigned char>(ch));

    if (tituloUpper.find("VILA") != std::string::npos && c == 'F') {
        return 'V'; // Franchesco
    }
    if (tituloUpper.find("SALA DO CHEFE") != std::string::npos && (c == 'M' || c == 'A' || c == 'H' || c == 'O' || c == 'R' || c == 'G')) {
        return 'H'; // Mahoraga
    }
    if ((tituloUpper.find("CABANA") != std::string::npos || tituloUpper.find("FLORESTA") != std::string::npos) && c == 'M') {
        return 'W'; // Morgana
    }
    if (tituloUpper.find("LABIRINTO") != std::string::npos && c == 'B') {
        return 'X'; // Bau
    }
    return c; // Default
}

std::string RaycasterMundo::obterCorMinimapaEntidade(char c, const std::string& tituloMapa) {
    std::string tituloUpper = tituloMapa;
    for (char& ch : tituloUpper) ch = std::toupper(static_cast<unsigned char>(ch));

    if (tituloUpper.find("VILA") != std::string::npos && c == 'F') return "\033[38;2;255;200;50m"; // Amarelo para Franchesco
    if ((tituloUpper.find("VILA") != std::string::npos || tituloUpper.find("CAVERNA") != std::string::npos) && c == 'B') return "\033[38;2;100;200;255m"; // Ciano para Bjorn
    if (tituloUpper.find("SALA DO CHEFE") != std::string::npos) return "\033[38;2;255;255;255m"; // Branco para Boss
    if ((tituloUpper.find("CABANA") != std::string::npos || tituloUpper.find("FLORESTA") != std::string::npos) && c == 'M') return "\033[38;2;200;100;255m"; // Morgana Roxa
    if (tituloUpper.find("LABIRINTO") != std::string::npos && c == 'B') return "\033[38;2;200;150;50m"; // Bau Dourado
    if (c == 'P') return "\033[38;2;139;69;19m"; // Placa
    
    return "\033[38;2;255;50;50m"; // Vermelho base
}
