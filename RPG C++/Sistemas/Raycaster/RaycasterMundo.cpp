#include "RaycasterMundo.h"
#include <algorithm>
#include <cctype>
#include <cmath>

bool RaycasterMundo::isTemaFloresta(const std::string& tituloMapa) {
    std::string tituloUpper = tituloMapa;
    for (char& c : tituloUpper) c = std::toupper(static_cast<unsigned char>(c));
    return (tituloUpper.find("FLORESTA") != std::string::npos || tituloUpper.find("BOSQUE") != std::string::npos);
}

bool RaycasterMundo::isEntity(char c) {
    return (c == 'G' || c == 'O' || c == 'B' || c == 'F' || c == 'S' || c == 'A' || c == 'M' || c == 'T' || c == 'H' || c == 'R' || c == 'P' || c == '^' || c == '*' || c == 'C');
}

bool RaycasterMundo::isTeleport(char c) { return c == '^'; }

bool RaycasterMundo::isWalkable(int mapX, int mapY, const std::vector<std::string>& matrizDoMapa) {
    char c = matrizDoMapa[mapY][mapX];
    if (c == '*') return false; // Impede atravessar arvores
    if (c == '.' || c == ' ' || c == '^' || c == '~' || isEntity(c)) return true;
    if (isMapLabel(mapX, mapY, matrizDoMapa)) return true;
    return false;
}

bool RaycasterMundo::isMapLabel(int mapX, int mapY, const std::vector<std::string>& matrizDoMapa) {
    char c = matrizDoMapa[mapY][mapX];
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
        if (isEntity(c)) {
            bool hasAdjacentText = false;
            // Checa Horizontal (procura ^ ou letras minusculas do resto da palavra)
            for (int dx = -1; dx <= 1; dx += 2) {
                int nx = mapX + dx;
                if (mapY >= 0 && mapY < (int)matrizDoMapa.size() && nx >= 0 && nx < (int)matrizDoMapa[mapY].size()) {
                    char adj = matrizDoMapa[mapY][nx];
                    if (adj == '^' || (adj >= 'a' && adj <= 'z')) {
                        hasAdjacentText = true;
                        break;
                    }
                }
            }
            // Checa Vertical (procura APENAS ^, para evitar bugar NPCs acima de textos)
            if (!hasAdjacentText) {
                for (int dy = -1; dy <= 1; dy += 2) {
                    int ny = mapY + dy;
                    if (ny >= 0 && ny < (int)matrizDoMapa.size() && mapX >= 0 && mapX < (int)matrizDoMapa[ny].size()) {
                        char adj = matrizDoMapa[ny][mapX];
                        if (adj == '^') {
                            hasAdjacentText = true;
                            break;
                        }
                    }
                }
            }
            if (!hasAdjacentText) return false;
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

std::string RaycasterMundo::obterPixelParede(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao) {
    // Reduz o peso da distancia para empurrar a escuridao (fog) mais para longe
    // Isso melhora drasticamente a clareza visual a medias e longas distancias
    distanciaAteParede *= 0.55f;

    int alturaParede = chao - teto;
    int metadeParede = teto + (alturaParede / 2);
    
    float texY = 0.0f;
    if (alturaParede > 0) texY = (float)(y - teto) / (float)alturaParede;
    if (texY > 0.999f) texY = 0.999f; // Impede o "wrapping" (loop) da textura no ultimo pixel tocando o chao
    int tx = (int)(texX * 64.0f) % 64;
    int ty = (int)(texY * 64.0f) % 64;

    std::string tituloUpper = tituloMapa;
    for (char& ch : tituloUpper) ch = std::toupper(static_cast<unsigned char>(ch));
    bool isReino = (tituloUpper.find("CASTELO") != std::string::npos || tituloUpper.find("REINO") != std::string::npos);

    bool isEstrutura = false;
    std::string charsEstrutura = "|_[]{}/\\<>;=-:+";
    if (charsEstrutura.find(charParede) != std::string::npos) isEstrutura = true;

    if (isReino && (isEstrutura || charParede == '#')) {
        if (charParede == '|') {
            // Textura de Madeira HD para o Portao do Reino
            bool isTabua = (tx % 8 == 0); 
            if (isTabua) {
                if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;45;25;10m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;30;15;5m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;20;10;0m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;10;5;0m \033[0m";
                else                                                     return "\033[48;2;0;0;0m \033[0m";
            } else {
                bool hasGrain = ((tx * 3 + ty * 7) % 5) == 0;
                if (hasGrain) {
                    if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;80;50;20m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;60;35;15m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;40;20;10m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;20;10;5m \033[0m";
                    else                                                     return "\033[48;2;0;0;0m \033[0m";
                } else {
                    if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;101;67;33m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;80;50;20m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;60;35;15m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;40;20;10m \033[0m";
                    else                                                     return "\033[48;2;0;0;0m \033[0m";
                }
            }
        } else {
            // Muros do Castelo com Degraus (Battlements)
            bool isBattlementGap = (ty < 12 && (tx % 32) >= 16);
            if (isBattlementGap) {
                return "FUNDO";
            }
            // Tijolos Cinzas
            bool isJunta = (ty % 4 == 0) || (((ty / 4) % 2 == 0) && tx % 8 == 0) || (((ty / 4) % 2 == 1) && (tx + 4) % 8 == 0); 
            if (isJunta) {
                if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;60;60;60m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;40;40;40m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;25;25;25m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;10;10;10m \033[0m";
                else                                                     return "\033[48;2;0;0;0m \033[0m";
            } else {
                bool hasGrain = ((tx * 7 + ty * 13) % 10) < 3;
                if (hasGrain) {
                    if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;100;100;100m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;70;70;70m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;45;45;45m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;20;20;20m \033[0m";
                    else                                                     return "\033[48;2;0;0;0m \033[0m";
                } else {
                    if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;120;120;120m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;90;90;90m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;60;60;60m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;30;30;30m \033[0m";
                    else                                                     return "\033[48;2;0;0;0m \033[0m";
                }
            }
        }
    } else if (isEstrutura) {
        if (temaFloresta) {
            // Textura de Madeira HD para a Cabana da Bruxa
            bool isTabua = (tx % 8 == 0); 
            if (isTabua) {
                if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;45;25;10m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;30;15;5m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;20;10;0m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;10;5;0m \033[0m";
                else                                                     return "\033[48;2;0;0;0m \033[0m";
            } else {
                bool hasGrain = ((tx * 3 + ty * 7) % 5) == 0;
                if (hasGrain) {
                    if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;80;50;20m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;60;35;15m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;40;20;10m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;20;10;5m \033[0m";
                    else                                                     return "\033[48;2;0;0;0m \033[0m";
                } else {
                    if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;101;67;33m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;80;50;20m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;60;35;15m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;40;20;10m \033[0m";
                    else                                                     return "\033[48;2;0;0;0m \033[0m";
                }
            }
        } else {
            // Vermelho/Marrom com Textura de Tijolos HD (O dobro de tijolos na malha 32x32)
            bool isJunta = (ty % 4 == 0) || (((ty / 4) % 2 == 0) && tx % 8 == 0) || (((ty / 4) % 2 == 1) && (tx + 4) % 8 == 0); 
            if (isJunta) {
                if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;120;120;120m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;80;80;80m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;50;50;50m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;25;25;25m \033[0m";
                else                                                     return "\033[48;2;0;0;0m \033[0m";
            } else {
                bool hasGrain = ((tx * 7 + ty * 13) % 10) < 3; // Ruido denso para textura HD
                if (hasGrain) {
                    if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;140;50;30m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;100;35;20m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;60;20;10m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;30;10;5m \033[0m";
                    else                                                     return "\033[48;2;0;0;0m \033[0m";
                } else {
                    if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;160;60;40m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;120;45;30m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;80;30;20m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;40;15;10m \033[0m";
                    else                                                     return "\033[48;2;0;0;0m \033[0m";
                }
            }
        }
    }

    if (!isReino && temaFloresta && charParede == '#') {
        int folhaTx = tx;
        int limiteFolhas = 28 + ((tx * 7) % 10); // Borda organica variando de 28 a 37 de altura

        if (ty < limiteFolhas) {
            // Animacao de vento suave com seno nas folhas do topo
            int animOffset = (int)(std::sin(tempoAnimacao * 1.5f + texX * 10.0f) * 4.0f);
            folhaTx = (tx + animOffset) % 64;
            if (folhaTx < 0) folhaTx += 64;
            
            bool sombraFolha = ((folhaTx * 7 + ty * 13) % 11) < 4; 
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
        } else {
            // Tronco macico de arvore antiga cobrindo todo o bloco na base
            bool isBordaEscura = (tx < 6 || tx > 57);
            bool isSombra = (tx >= 6 && tx <= 12) || (tx >= 51 && tx <= 57);
            bool hasWoodGrain = ((tx * 3 + ty * 7) % 5) == 0;

            if (isBordaEscura) {
                if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;15;10;5m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;10;7;3m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;5;3;0m \033[0m";
                else                                                     return "\033[48;2;0;0;0m \033[0m";
            } else if (isSombra) {
                if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;40;25;10m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;30;18;5m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;20;10;0m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;10;5;0m \033[0m";
                else                                                     return "\033[48;2;0;0;0m \033[0m";
            } else {
                if (hasWoodGrain) {
                    if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;60;40;20m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;45;30;15m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;30;20;10m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;15;10;5m \033[0m";
                    else                                                     return "\033[48;2;0;0;0m \033[0m";
                } else {
                    if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;80;55;25m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;60;40;20m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;40;25;10m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;20;12;5m \033[0m";
                    else                                                     return "\033[48;2;0;0;0m \033[0m";
                }
            }
        }
    } else if (!isReino) {
        bool isSpawn = (tituloUpper.find("INICIO") != std::string::npos);
        bool isCaverna = (tituloUpper.find("CAVERNA") != std::string::npos || tituloUpper.find("LABIRINTO") != std::string::npos || tituloUpper.find("CHEFE") != std::string::npos || tituloUpper.find("CORACAO") != std::string::npos);
        
        if (isSpawn) {
            // Tijolos brancos originais para o Spawn
            bool isJuntaBranca = (ty % 4 == 0) || (((ty / 4) % 2 == 0) && tx % 8 == 0) || (((ty / 4) % 2 == 1) && (tx + 4) % 8 == 0); 
            if (isJuntaBranca) {
                if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;140;140;140m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;100;100;100m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;70;70;70m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;40;40;40m \033[0m";
                return "\033[48;2;0;0;0m \033[0m";
            } else {
                bool hasGrain = ((tx * 11 + ty * 17) % 10) < 3;
                if (hasGrain) {
                    if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;210;210;210m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;150;150;150m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;100;100;100m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;50;50;50m \033[0m";
                    return "\033[48;2;0;0;0m \033[0m";
                } else {
                    if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;240;240;240m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;180;180;180m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;120;120;120m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;60;60;60m \033[0m";
                    return "\033[48;2;0;0;0m \033[0m";
                }
            }
        } else if (isCaverna) {
            // Textura de Rochas Escuras e Umidas para Cavernas
            bool isJuntaPedra = ((tx * 3 + ty * 7) % 9) < 2 || ((tx * 11 + ty * 5) % 13) < 2;
            if (isJuntaPedra) {
                if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;30;30;30m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;20;20;20m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;10;10;10m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;5;5;5m \033[0m";
                return "\033[48;2;0;0;0m \033[0m";
            } else {
                bool hasGrain = ((tx * 17 + ty * 23) % 7) < 3;
                if (hasGrain) {
                    if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;55;50;45m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;40;35;30m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;25;20;15m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;15;10;5m \033[0m";
                    return "\033[48;2;0;0;0m \033[0m";
                } else {
                    if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;75;70;65m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;55;50;45m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;35;30;25m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;20;15;10m \033[0m";
                    return "\033[48;2;0;0;0m \033[0m";
                }
            }
        } else {
            // Textura natural de Rochas Irregulares para a Vila
            bool isJuntaPedra = ((tx * 3 + ty * 7) % 9) < 2 || ((tx * 11 + ty * 5) % 13) < 2;
            if (isJuntaPedra) {
                if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;50;50;50m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;40;40;40m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;25;25;25m \033[0m";
                else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;15;15;15m \033[0m";
                return "\033[48;2;0;0;0m \033[0m";
            } else {
                bool hasGrain = ((tx * 17 + ty * 23) % 7) < 3;
                if (hasGrain) {
                    if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;90;90;90m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;70;70;70m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;50;50;50m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;30;30;30m \033[0m";
                    return "\033[48;2;0;0;0m \033[0m";
                } else {
                    if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[48;2;110;110;110m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[48;2;85;85;85m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[48;2;60;60;60m \033[0m";
                    else if (distanciaAteParede < profundidadeMaxima)        return "\033[48;2;35;35;35m \033[0m";
                    return "\033[48;2;0;0;0m \033[0m";
                }
            }
        }
    }
    return "\033[48;2;0;0;0m \033[0m";
}

std::string RaycasterMundo::obterPixelChao(const std::string& tituloMapa, float currentX, float currentY, float currentDist, float profundidadeMaxima) {
    currentDist *= 0.55f; // Empurra o sombreamento do chao mais para o fundo

    std::string tituloUpper = tituloMapa;
    for (char& ch : tituloUpper) ch = std::toupper(static_cast<unsigned char>(ch));

    bool isTerra = (tituloUpper.find("FLORESTA") != std::string::npos || 
                    tituloUpper.find("BOSQUE") != std::string::npos ||
                    tituloUpper.find("VILA") != std::string::npos ||
                    tituloUpper.find("INICIO") != std::string::npos);

    unsigned int globX = static_cast<unsigned int>(std::abs(currentX * 32.0f));
    unsigned int globY = static_cast<unsigned int>(std::abs(currentY * 32.0f));

    std::string bg;
    std::string fg;
    char c = ' ';

    // Textura HD no Background (Mesclando tons baseados na grade 32x32)
    if (isTerra) {
        fg = "\033[38;2;45;25;10m";
        if ((globX + globY) % 2 == 0) bg = "\033[48;2;28;18;8m";
        else if ((globX * 3 + globY * 5) % 7 < 2) bg = "\033[48;2;22;12;4m";
        else bg = "\033[48;2;25;15;5m";
    } else {
        fg = "\033[38;2;60;60;60m";
        if ((globX + globY) % 2 == 0) bg = "\033[48;2;24;24;24m";
        else if ((globX * 3 + globY * 5) % 7 < 2) bg = "\033[48;2;16;16;16m";
        else bg = "\033[48;2;20;20;20m";
    }

    // Padrao HD para os detalhes em caracteres
    if ((globX * 17 + globY * 23) % 47 < 4) c = '.'; // Manchas
    else if ((globX * globX + globY * 13) % 53 < 3) c = '-'; // Rachaduras/ranhuras
    else if ((globX * 3 + globY * 7) % 31 < 2) c = '`'; // Pedregulhos
    
    // Aplicando sombreamento de distancia (Nevoeiro) ao chao
    if (currentDist > profundidadeMaxima / 2.0f) {
        bg = "\033[48;2;8;8;8m"; fg = "\033[38;2;20;20;20m";
    } else if (currentDist > profundidadeMaxima / 3.0f) {
        bg = "\033[48;2;12;12;12m"; fg = "\033[38;2;30;30;30m";
    }
    
    if (c == ' ') return bg + " \033[0m";
    return bg + fg + std::string(1, c) + "\033[0m";
}

std::string RaycasterMundo::obterPixelAgua(float currentX, float currentY, float currentDist, float profundidadeMaxima) {
    currentDist *= 0.55f; // Empurra o sombreamento da agua mais para o fundo

    int tx = (int)(currentX * 32.0f) % 32;
    int ty = (int)(currentY * 32.0f) % 32;

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

std::string RaycasterMundo::obterPixelTeto(int temaCeu, float raioAngulo, int y, int alturaTela, float tempoAnimacao) {
    if (temaCeu == 0) return " "; // Interior

    int horizonte = alturaTela / 2;
    float ratio = (horizonte > 0) ? (float)y / (float)horizonte : 1.0f;
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;

    if (temaCeu == 1) { // Floresta (Noite Estrelada)
        int r = 5 + (int)(15 * ratio);  // 5 -> 20 (Ceu bem escuro)
        int g = 5 + (int)(20 * ratio);  // 5 -> 25
        int b = 15 + (int)(30 * ratio); // 15 -> 45
        
        // Lua (Fixada no angulo 0.0 radianos)
        float divHorizonte = (horizonte > 0) ? (float)horizonte : 1.0f;
        float diffAnguloLua = std::fmod(raioAngulo - 0.0f, 2.0f * 3.14159f);
        if (diffAnguloLua < -3.14159f) diffAnguloLua += 2.0f * 3.14159f;
        if (diffAnguloLua > 3.14159f) diffAnguloLua -= 2.0f * 3.14159f;
        
        float distXLua = diffAnguloLua;
        float distYLua = (y - horizonte * 0.25f) / divHorizonte;
        float distLua = std::sqrt(distXLua * distXLua * 6.0f + distYLua * distYLua); // 6.0f ajusta a proporcao dos caracteres
        
        if (distLua < 0.10f) {
            // Sombra da iluminacao parcial (crescente) que pulsa com o tempo
            float shadowOffset = 0.04f + std::sin(tempoAnimacao * 1.5f) * 0.015f; 
            float shadowDist = std::sqrt((distXLua - shadowOffset) * (distXLua - shadowOffset) * 6.0f + distYLua * distYLua);
            
            if (shadowDist < 0.10f) {
                return "\033[48;2;60;60;70m \033[0m"; // Parte escura (sombra da lua)
            }
            int craterNoise = ((int)(distXLua * 100) * 17 + y * 23) % 47;
            if (craterNoise < 8) return "\033[48;2;180;180;190m \033[0m"; // Cratera
            return "\033[48;2;230;230;240m \033[0m"; // Lua brilhante
        } else if (distLua < 0.12f) {
            return "\033[48;2;120;120;130m \033[0m"; // Borda da lua mais escura
        } else if (distLua < 0.25f) {
            // Aura da lua (brilho suave misturado no ceu escuro, pulsando)
            float glowPulse = std::sin(tempoAnimacao * 2.0f) * 0.02f;
            float glow = 1.0f - ((distLua - 0.12f) / (0.13f + glowPulse));
            if (glow > 0.0f) {
                r = std::min(255, r + (int)(40 * glow));
                g = std::min(255, g + (int)(45 * glow));
                b = std::min(255, (int)(b + 60 * glow));
            }
        }

        std::string bg = "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";

        // Normaliza o angulo para nunca ser negativo (evita o bug da "parede" de estrelas)
        float anguloNorm = std::fmod(raioAngulo, 2.0f * 3.14159f);
        if (anguloNorm < 0.0f) anguloNorm += 2.0f * 3.14159f;

        // Ruido espacial nao-linear para criar as estrelas (espalhadas aleatoriamente, sem diagonais)
        unsigned int starX = static_cast<unsigned int>(anguloNorm * 150.0f); // Densidade horizontal de estrelas em 360 graus
        unsigned int starY = static_cast<unsigned int>(y);
        
        unsigned int hash = starX * 374761393U + starY * 668265263U;
        hash = (hash ^ (hash >> 13)) * 1274126177U;
        int noise = hash % 1000;
        
        if (noise == 0 && y < horizonte * 0.8f) return bg + "\033[38;2;255;255;255m*\033[0m"; 
        else if (noise < 4 && y < horizonte * 0.8f) return bg + "\033[38;2;200;200;255m+\033[0m"; 
        else if (noise < 12 && y < horizonte * 0.8f) return bg + "\033[38;2;255;255;255m.\033[0m"; 
        else if (noise < 20 && y < horizonte * 0.8f) return bg + "\033[38;2;200;200;200m.\033[0m"; 
        else if (noise == 25 && y < horizonte * 0.8f) return bg + "\033[38;2;255;255;150m'\033[0m"; 
        
        return bg + " \033[0m";
    }

    // Default (Azul Dia)
    int r = 10 + (int)(60 * ratio);  // 10 -> 70
    int g = 60 + (int)(90 * ratio);  // 60 -> 150
    int b = 150 + (int)(80 * ratio); // 150 -> 230

    // Sol (Fixado no angulo 0.0 radianos / Mesma direcao da lua)
    float divHorizonte = (horizonte > 0) ? (float)horizonte : 1.0f;
    float diffAnguloSol = std::fmod(raioAngulo - 0.0f, 2.0f * 3.14159f);
    if (diffAnguloSol < -3.14159f) diffAnguloSol += 2.0f * 3.14159f;
    if (diffAnguloSol > 3.14159f) diffAnguloSol -= 2.0f * 3.14159f;
    
    float distXSol = diffAnguloSol;
    float distYSol = (y - horizonte * 0.35f) / divHorizonte;
    float distSol = std::sqrt(distXSol * distXSol * 6.0f + distYSol * distYSol);
    
    // Calcula os raios solares rotacionando com o tempo
    float angleSol = std::atan2(distYSol, distXSol * 2.449f); // 2.449f = aprox sqrt(6.0f)
    float rays = std::sin(angleSol * 8.0f + tempoAnimacao * 2.0f); // 8 pontas girando
    float glowRadius = 0.12f + rays * 0.02f;
    
    if (distSol < 0.08f) {
        return "\033[48;2;255;255;255m \033[0m"; // Centro branco e incandescente
    } else if (distSol < glowRadius) {
        return "\033[48;2;255;220;50m \033[0m"; // Coroa amarela forte com raios solares
    } else if (distSol < 0.35f) {
        // Brilho solar clareando e mesclando com o ceu azul (tbm pulsando levemente)
        float glowPulse = std::sin(tempoAnimacao * 1.5f) * 0.03f;
        float glow = 1.0f - ((distSol - glowRadius) / (0.35f - glowRadius + glowPulse)); 
        if (glow > 0.0f) {
            r = std::min(255, r + (int)(150 * glow));
            g = std::min(255, g + (int)(100 * glow));
        }
    }

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

    bool isReino = (tituloUpper.find("CASTELO") != std::string::npos || tituloUpper.find("REINO") != std::string::npos);
    if (isReino && (c == 'C' || c == 'G')) return 'C'; // Cavaleiro Real
    return c; // Default
}

std::string RaycasterMundo::obterCorMinimapaEntidade(char c, const std::string& tituloMapa) {
    std::string tituloUpper = tituloMapa;
    for (char& ch : tituloUpper) ch = std::toupper(static_cast<unsigned char>(ch));

    if (tituloUpper.find("VILA") != std::string::npos && c == 'F') return "\033[1;38;2;255;200;50m"; // Amarelo para Franchesco (Brilhante)
    if ((tituloUpper.find("VILA") != std::string::npos || tituloUpper.find("CAVERNA") != std::string::npos) && c == 'B') return "\033[1;38;2;100;200;255m"; // Ciano para Bjorn
    if (tituloUpper.find("SALA DO CHEFE") != std::string::npos) return "\033[1;38;2;255;255;255m"; // Branco para Boss
    if ((tituloUpper.find("CABANA") != std::string::npos || tituloUpper.find("FLORESTA") != std::string::npos) && c == 'M') return "\033[1;38;2;200;100;255m"; // Morgana Roxa
    if (tituloUpper.find("LABIRINTO") != std::string::npos && c == 'B') return "\033[1;38;2;200;150;50m"; // Bau Dourado
    if (c == 'P') return "\033[1;38;2;139;69;19m"; // Placa (Brilhante)
    if (c == '*') return "\033[38;2;34;139;34m"; // Verde para arvore
    
    bool isReino = (tituloUpper.find("CASTELO") != std::string::npos || tituloUpper.find("REINO") != std::string::npos);
    if (isReino && c == 'C') return "\033[1;38;2;200;200;220m"; // Prata para Cavaleiro
    if (isReino && c == 'G') return "\033[1;38;2;255;200;50m"; // Amarelo para Guarda do Reino
    return "\033[1;38;2;255;50;50m"; // Vermelho base para Inimigos (Brilhante)
}
