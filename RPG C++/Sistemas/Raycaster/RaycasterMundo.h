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
    
    static std::string obterPixelParedeInternal(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao);
    static std::string obterPixelParede(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao = 0.0f, bool isSideWall = false);
    static std::string obterPixelChao(const std::string& tituloMapa, float currentX, float currentY, float currentDist, float profundidadeMaxima);
    static std::string obterPixelAgua(float currentX, float currentY, float currentDist, float profundidadeMaxima);
    static int obterTemaCeu(const std::string& tituloMapa);
    static std::string obterPixelTeto(int temaCeu, float raioAngulo, int y, int alturaTela, float tempoAnimacao = 0.0f);

    static char obterSpriteChar(char c, const std::string& tituloMapa);
    static std::string obterCorMinimapaEntidade(char c, const std::string& tituloMapa);
};
