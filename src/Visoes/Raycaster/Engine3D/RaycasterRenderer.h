#pragma once

#include <vector>
#include <string>
#include <map>
#include "RaycasterSprites.h"

class RaycasterRenderer {
public:
    static void renderizar3D(std::vector<Pixel3D>& tela, int LARGURA_TELA, int ALTURA_TELA, float jogadorX, float jogadorY, float anguloVisao, float horizonte, int bobbingOffset, float profundidadeMaxima, float tempoAbsoluto, const std::vector<std::string>& matrizDoMapa, const std::string& tituloMapa, bool temaFloresta, int temaCeu, const std::map<char, SpriteCache>& cacheSprites);
};