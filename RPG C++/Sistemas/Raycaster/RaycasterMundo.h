#pragma once
#include <string>
#include <vector>

class RaycasterMundo {
public:
    static bool isTemaFloresta(const std::string& tituloMapa);
    static bool isEntity(char c);
    static bool isTeleport(char c);
    static bool isWalkable(int mapX, int mapY, const std::vector<std::string>& matrizDoMapa);
    static bool isMapLabel(int mapX, int mapY, const std::vector<std::string>& matrizDoMapa);
    
    static std::string obterPixelParede(bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX);
    static std::string obterPixelChao(const std::string& tituloMapa, int x, int y);
    static std::string obterPixelAgua(float currentX, float currentY, float currentDist, float profundidadeMaxima);
    static int obterTemaCeu(const std::string& tituloMapa);
    static std::string obterPixelTeto(int temaCeu, int y, int alturaTela);

    static char obterSpriteChar(char c, const std::string& tituloMapa);
    static std::string obterCorMinimapaEntidade(char c, const std::string& tituloMapa);
};
