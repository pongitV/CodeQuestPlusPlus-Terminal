#pragma once
#include <string>
#include <vector>
#include <tuple>

class RaycasterMundo {
public:
    static bool isTemaFloresta(const std::string& tituloMapa);
    static bool isEntity(char c);
    static bool isTeleport(char c);
    static bool isWalkable(int mapX, int mapY, const std::vector<std::string>& matrizDoMapa);
    static bool isMapLabel(int mapX, int mapY, const std::vector<std::string>& matrizDoMapa);
    
    static std::string obterPixelParedeInternal(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao, const std::vector<std::tuple<int, int, int>>& luzes, float hitX, float hitY, bool isSideWall = false);
    static std::string obterPixelParede(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao = 0.0f, bool isSideWall = false, const std::vector<std::tuple<int, int, int>>& luzes = {}, float hitX = 0.0f, float hitY = 0.0f);
    static std::string obterPixelChao(const std::string& tituloMapa, float currentX, float currentY, float currentDist, float profundidadeMaxima, const std::vector<std::tuple<int, int, int>>& luzes = {});
    static std::string obterPixelAgua(float currentX, float currentY, float currentDist, float profundidadeMaxima, float raioAngulo = 0.0f, float tempoAnimacao = 0.0f, int temaCeu = 0);
    static int obterTemaCeu(const std::string& tituloMapa);
    static std::string obterPixelTeto(int temaCeu, float raioAngulo, int y, int alturaTela, float tempoAnimacao = 0.0f);

    static char obterSpriteChar(int mapX, int mapY, char c, const std::string& tituloMapa);
    static std::string obterCorMinimapaEntidade(char c, const std::string& tituloMapa);
};
