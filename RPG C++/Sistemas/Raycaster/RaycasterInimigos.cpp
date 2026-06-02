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
    cache['G'] = RaycasterSprites::parseArte(getArte(CriadorInimigos::criarInimigoGoblin));
    cache['O'] = RaycasterSprites::parseArte(getArte(CriadorInimigos::criarInimigoOrkExilado));
    cache['S'] = RaycasterSprites::parseArte(getArte(CriadorInimigos::criarInimigoSlime));
    cache['F'] = RaycasterSprites::parseArte(getArte(CriadorInimigos::criarInimigoFada));
    cache['A'] = RaycasterSprites::parseArte(getArte(CriadorInimigos::criarInimigoAbominacaoFloresta));
    cache['T'] = RaycasterSprites::parseArte(getArte(CriadorInimigos::criarInimigoTroll));
    cache['M'] = RaycasterSprites::parseArte(getArte(CriadorInimigos::criarInimigoMimico));
    
    cache['H'] = RaycasterSprites::parseArte(getArteFull(CriadorInimigos::criarInimigoMahoraga));
}
