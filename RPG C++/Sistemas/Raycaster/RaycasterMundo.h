#pragma once
#include <string>
#include <vector>

class RaycasterMundo {
public:
    static bool isTemaFloresta(const std::string& tituloMapa);
    static bool isEntity(char c);
    static bool isTeleport(char c);
    static bool isWalkable(char c);
    static bool isWordAfterTeleport(int mapX, int mapY, const std::vector<std::string>& matrizDoMapa);
    
    static std::string obterPixelParede(bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao);
    static std::string obterPixelChao(bool temaFloresta);
    static int obterTemaCeu(const std::string& tituloMapa);
    static std::string obterPixelTeto(int temaCeu, int y, int alturaTela);

    static char obterSpriteChar(char c, const std::string& tituloMapa);
    static std::string obterCorMinimapaEntidade(char c, const std::string& tituloMapa);
};
