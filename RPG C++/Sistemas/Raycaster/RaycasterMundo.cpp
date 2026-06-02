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

std::string RaycasterMundo::obterPixelParede(bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao) {
    char sombraParede = ' ';
    if (distanciaAteParede <= profundidadeMaxima / 4.0f)     sombraParede = (char)219; // Bloco cheio
    else if (distanciaAteParede < profundidadeMaxima / 3.0f) sombraParede = (char)178; // Bloco escuro
    else if (distanciaAteParede < profundidadeMaxima / 2.0f) sombraParede = (char)177; // Bloco medio
    else if (distanciaAteParede < profundidadeMaxima)        sombraParede = (char)176; // Bloco claro

    if (charParede == '~') {
        sombraParede = '~'; // Substitui o bloco solido pelo efeito de onda (agua)
        if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[38;2;50;150;255m" + std::string(1, sombraParede) + "\033[0m";
        else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[38;2;40;120;200m" + std::string(1, sombraParede) + "\033[0m";
        else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[38;2;30;90;150m" + std::string(1, sombraParede) + "\033[0m";
        else if (distanciaAteParede < profundidadeMaxima)        return "\033[38;2;20;60;100m" + std::string(1, sombraParede) + "\033[0m";
        else                                                     return "\033[38;2;0;0;0m \033[0m";
    }

    int alturaParede = chao - teto;
    int metadeParede = teto + (alturaParede / 2);

    bool isArvore = false;
    if (charParede == '*') isArvore = true;

    if (isArvore) {
        if (y < metadeParede) { // Topo (Folhas)
            if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[38;2;34;139;34m" + std::string(1, sombraParede) + "\033[0m";
            else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[38;2;28;115;28m" + std::string(1, sombraParede) + "\033[0m";
            else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[38;2;22;89;22m" + std::string(1, sombraParede) + "\033[0m";
            else if (distanciaAteParede < profundidadeMaxima)        return "\033[38;2;14;60;14m" + std::string(1, sombraParede) + "\033[0m";
            else                                                     return "\033[38;2;0;0;0m \033[0m";
        }
        // Base da arvore (Marrom)
        if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[38;2;101;67;33m" + std::string(1, sombraParede) + "\033[0m";
        else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[38;2;80;50;20m" + std::string(1, sombraParede) + "\033[0m";
        else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[38;2;60;35;15m" + std::string(1, sombraParede) + "\033[0m";
        else if (distanciaAteParede < profundidadeMaxima)        return "\033[38;2;40;20;10m" + std::string(1, sombraParede) + "\033[0m";
        else                                                     return "\033[38;2;0;0;0m \033[0m";
    }
    
    bool isEstrutura = false;
    std::string charsEstrutura = "|_[]{}/\\<>;=-:+";
    if (charsEstrutura.find(charParede) != std::string::npos) isEstrutura = true;

    if (isEstrutura) {
        // Azul Claro (Casas da Vila / Cabana da Bruxa / Interiores)
        if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[38;2;150;200;255m" + std::string(1, sombraParede) + "\033[0m";
        else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[38;2;100;150;200m" + std::string(1, sombraParede) + "\033[0m";
        else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[38;2;50;100;150m" + std::string(1, sombraParede) + "\033[0m";
        else if (distanciaAteParede < profundidadeMaxima)        return "\033[38;2;25;50;100m" + std::string(1, sombraParede) + "\033[0m";
        else                                                     return "\033[38;2;0;0;0m \033[0m";
    }

    // Default / Bordas do Mapa '#' (Branco / Cinza)
    if (distanciaAteParede <= profundidadeMaxima / 4.0f)     return "\033[38;2;220;220;220m" + std::string(1, sombraParede) + "\033[0m";
    else if (distanciaAteParede < profundidadeMaxima / 3.0f) return "\033[38;2;160;160;160m" + std::string(1, sombraParede) + "\033[0m";
    else if (distanciaAteParede < profundidadeMaxima / 2.0f) return "\033[38;2;100;100;100m" + std::string(1, sombraParede) + "\033[0m";
    else if (distanciaAteParede < profundidadeMaxima)        return "\033[38;2;50;50;50m" + std::string(1, sombraParede) + "\033[0m";
    return "\033[38;2;0;0;0m \033[0m";
}

std::string RaycasterMundo::obterPixelChao(bool temaFloresta) {
    return "\033[38;2;60;60;60m.\033[0m";
}

int RaycasterMundo::obterTemaCeu(const std::string& tituloMapa) {
    std::string tituloUpper = tituloMapa;
    for (char& ch : tituloUpper) ch = std::toupper(static_cast<unsigned char>(ch));

    if (tituloUpper.find("CAVERNA") != std::string::npos ||
        tituloUpper.find("LABIRINTO") != std::string::npos ||
        tituloUpper.find("CHEFE") != std::string::npos ||
        tituloUpper.find("CORACAO") != std::string::npos ||
        tituloUpper.find("FORJA") != std::string::npos ||
        tituloUpper.find("LOJA") != std::string::npos ||
        tituloUpper.find("CABANA") != std::string::npos) {
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

    if (tituloUpper.find("VILA") != std::string::npos && c == 'F') return "\033[38;2;255;200;50m"; // Amarelo para Franchesco
    if (tituloUpper.find("SALA DO CHEFE") != std::string::npos) return "\033[38;2;255;255;255m"; // Branco para Boss
    if (tituloUpper.find("CABANA") != std::string::npos && c == 'M') return "\033[38;2;200;100;255m"; // Morgana Roxa
    if (tituloUpper.find("LABIRINTO") != std::string::npos && c == 'B') return "\033[38;2;200;150;50m"; // Bau Dourado
    
    return "\033[38;2;255;50;50m"; // Vermelho base
}
