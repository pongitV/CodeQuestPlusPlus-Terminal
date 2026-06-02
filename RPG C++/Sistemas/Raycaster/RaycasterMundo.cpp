#include "RaycasterMundo.h"
#include <algorithm>
#include <cctype>

bool RaycasterMundo::isTemaFloresta(const std::string& tituloMapa) {
    std::string tituloUpper = tituloMapa;
    for (char& c : tituloUpper) c = std::toupper(static_cast<unsigned char>(c));
    return (tituloUpper.find("FLORESTA") != std::string::npos || tituloUpper.find("BOSQUE") != std::string::npos);
}

bool RaycasterMundo::isEntity(char c) {
    return (c == 'G' || c == 'O' || c == 'B' || c == 'F' || c == 'S' || c == 'A' || c == 'M' || c == 'T' || c == 'H' || c == 'R');
}

bool RaycasterMundo::isTeleport(char c) { return c == '^'; }

bool RaycasterMundo::isWalkable(char c) {
    return (c == '.' || c == ' ' || c == '^' || (c >= 'a' && c <= 'z') || isEntity(c));
}

bool RaycasterMundo::isWordAfterTeleport(int mapX, int mapY, const std::vector<std::string>& matrizDoMapa) {
    int limiteEsq = std::max(0, mapX - 10);
    for (int tx = mapX - 1; tx >= limiteEsq; tx--) {
        char check = matrizDoMapa[mapY][tx];
        if (check == '^') return true;
        if (check == ' ' || check == '.') break; 
    }
    return false;
}

std::string RaycasterMundo::obterCorParede(bool temaFloresta, float distanciaAteParede, float profundidadeMaxima) {
    if (temaFloresta) {
        if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[38;2;101;67;33m";
        else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[38;2;80;50;20m";
        else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[38;2;60;35;15m";
        else if (distanciaAteParede < profundidadeMaxima)        return "\033[38;2;40;20;10m";
        else                                                     return "\033[38;2;0;0;0m";
    }
    if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[38;2;220;220;220m";
    else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[38;2;160;160;160m";
    else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[38;2;100;100;100m";
    else if (distanciaAteParede < profundidadeMaxima)        return "\033[38;2;50;50;50m";
    return "\033[38;2;0;0;0m";
}

std::string RaycasterMundo::obterPixelChao(bool temaFloresta) {
    return temaFloresta ? "\033[38;2;20;50;20m.\033[0m" : "\033[38;2;60;60;60m.\033[0m";
}

char RaycasterMundo::obterSpriteChar(char c, const std::string& tituloMapa) {
    std::string tituloUpper = tituloMapa;
    for (char& ch : tituloUpper) ch = std::toupper(static_cast<unsigned char>(ch));

    if (tituloUpper.find("SALA DO CHEFE") != std::string::npos && (c == 'M' || c == 'A' || c == 'H' || c == 'O' || c == 'R' || c == 'G')) {
        return 'H'; // Mahoraga
    }
    if (tituloUpper.find("CABANA") != std::string::npos && c == 'M') {
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

    if (tituloUpper.find("SALA DO CHEFE") != std::string::npos) return "\033[38;2;255;50;255m"; // Magenta para Boss
    if (tituloUpper.find("CABANA") != std::string::npos && c == 'M') return "\033[38;2;200;100;255m"; // Morgana Roxa
    if (tituloUpper.find("LABIRINTO") != std::string::npos && c == 'B') return "\033[38;2;200;150;50m"; // Bau Dourado
    
    return "\033[38;2;255;50;50m"; // Vermelho base
}
