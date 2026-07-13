#pragma once
#include <string>
#include <vector>
#include <tuple>
#include "RaycasterSprites.h"
#include "Iluminador.h"

class RaycasterMundo {
public:
    static bool isTemaFloresta(const std::string& tituloMapa);
    static bool isEntity(char c);
    static bool isTeleport(char c);
    static bool isWalkable(int mapX, int mapY, const std::vector<std::string>& matrizDoMapa);
    static bool isMapLabel(int mapX, int mapY, const std::vector<std::string>& matrizDoMapa);
    static void atualizarMapHash(const std::vector<std::string>& matrizDoMapa);
    static size_t obterMapHash();
    
    static char obterNPCProximo(const std::string& tituloMapa, int mapX, int mapY);
    
    static Pixel3D obterPixelParedeInternal(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao, const std::vector<std::tuple<int, int, int>>& luzes, float hitX, float hitY, bool isSideWall = false, char npcEncontrado = ' ', float nx = 0.0f, float ny = 0.0f);
    static Pixel3D obterPixelParede(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao = 0.0f, bool isSideWall = false, const std::vector<std::tuple<int, int, int>>& luzes = {}, float hitX = 0.0f, float hitY = 0.0f, char npcEncontrado = ' ', float nx = 0.0f, float ny = 0.0f);

    static Pixel3D obterPixelParedeInternal(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao, const Iluminador::InfoLuz& infoLuz, float hitX, float hitY, bool isSideWall = false, char npcEncontrado = ' ', float nx = 0.0f, float ny = 0.0f);
    static Pixel3D obterPixelParede(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao, bool isSideWall, const Iluminador::InfoLuz& infoLuz, float hitX, float hitY, char npcEncontrado = ' ', float nx = 0.0f, float ny = 0.0f);
    static Pixel3D obterPixelChao(const std::string& tituloMapa, float currentX, float currentY, float currentDist, float profundidadeMaxima, const std::vector<std::tuple<int, int, int>>& luzes = {}, const std::vector<std::string>* matrizDoMapa = nullptr, float tempoAnimacao = 0.0f);
    static Pixel3D obterPixelChao(const std::string& tituloMapa, float currentX, float currentY, float currentDist, float profundidadeMaxima, const Iluminador::InfoLuz& infoLuz);
    static Pixel3D obterPixelAgua(float currentX, float currentY, float currentDist, float profundidadeMaxima, float raioAngulo = 0.0f, float tempoAnimacao = 0.0f, int temaCeu = 0);
    static int obterTemaCeu(const std::string& tituloMapa);
    static Pixel3D obterPixelTeto(int temaCeu, float raioAngulo, float anguloSolLua, int y, int alturaTela, float tempoAnimacao = 0.0f, bool isMenu = false);

    static char obterSpriteChar(int mapX, int mapY, char c, const std::string& tituloMapa);
    static std::string obterCorMinimapaEntidade(char c, const std::string& tituloMapa);
};
