#include "RaycasterInimigos.h"
#include "../../Core/Controladores/CriadorInimigos.h"
#include "../../Entidades/Racas/RacaBase.h"
#include "../../Entidades/Personagem.h"
#include <vector>
#include <string>
#include <memory>

namespace {
    std::vector<std::string> getArte(std::vector<std::unique_ptr<Personagem>> (*func)(int)) {
        auto vec = func(1);
        if (!vec.empty() && vec[0]) return vec[0]->obterRaca()->obterAparenciaCombate();
        return {"?"};
    }

    std::vector<std::string> getArteFull(std::vector<std::unique_ptr<Personagem>> (*func)(int)) {
        auto vec = func(1);
        if (!vec.empty() && vec[0]) return vec[0]->obterRaca()->obterAparenciaRaca();
        return {"?"};
    }
}

void RaycasterInimigos::inicializarSprites(std::map<char, SpriteCache>& cache) {
    cache['G'] = RaycasterSprites::parseSprite(getArteFull(CriadorInimigos::criarInimigoGoblin), 100, 200, 50); // Verde Goblin
    cache['O'] = RaycasterSprites::parseSprite(getArteFull(CriadorInimigos::criarInimigoOrkExilado), 50, 150, 50); // Verde Escuro Orc
    cache['S'] = RaycasterSprites::parseSprite(getArteFull(CriadorInimigos::criarInimigoSlime), 50, 200, 255); // Ciano Slime
    cache['F'] = RaycasterSprites::parseSprite(getArteFull(CriadorInimigos::criarInimigoFada), 255, 100, 200); // Rosa Fada
    cache['A'] = RaycasterSprites::parseSprite(getArteFull(CriadorInimigos::criarInimigoAbominacaoFloresta), 139, 69, 19); // Marrom Abominacao
    cache['T'] = RaycasterSprites::parseSprite(getArteFull(CriadorInimigos::criarInimigoTroll), 150, 150, 160); // Cinza Troll
    cache['M'] = RaycasterSprites::parseSprite(getArteFull(CriadorInimigos::criarInimigoMimico), 200, 150, 50); // Dourado Mimico
    
    cache['H'] = RaycasterSprites::parseSprite(getArteFull(CriadorInimigos::criarInimigoMahoraga), 255, 255, 255); // Branco Mahoraga
}
