#include "UI/Renderers/3D/EngineRaycaster/RaycasterEnemies.h"
#include "Core/Engine/EnemyCreator.h"
#include "Domain/Characters/Races/BaseRace.h"
#include "Domain/Characters/Character.h"
#include <vector>
#include <string>
#include <memory>

namespace {

    std::vector<std::string> getArtFull(std::vector<std::unique_ptr<Character>> (*func)(int)) {
        auto vec = func(1);
        if (!vec.empty() && vec[0]) return vec[0]->getRace()->getAppearanceRace();
        return {"?"};
    }
}

void RaycasterEnemies::bootSprites(std::map<char, SpriteCache>& cache) {
    cache['G'] = RaycasterSprites::parseSprite(getArtFull(EnemyCreator::createGoblinEnemy), 100, 200, 50); // Verde Goblin
    cache['O'] = RaycasterSprites::parseSprite(getArtFull(EnemyCreator::createExiledOrcEnemy), 50, 150, 50); // Verde Escuro Orc
    cache['S'] = RaycasterSprites::parseSprite(getArtFull(EnemyCreator::createSlimeEnemy), 50, 200, 255); // Ciano Slime
    cache['F'] = RaycasterSprites::parseSprite(getArtFull(EnemyCreator::createFairyEnemy), 255, 100, 200); // Rosa Fada
    cache['A'] = RaycasterSprites::parseSprite(getArtFull(EnemyCreator::createForestAbominationEnemy), 139, 69, 19); // Marrom Abominacao
    cache['T'] = RaycasterSprites::parseSprite(getArtFull(EnemyCreator::createTrollEnemy), 150, 150, 160); // Cinza Troll
    cache['M'] = RaycasterSprites::parseSprite(getArtFull(EnemyCreator::createMimicEnemy), 200, 150, 50); // Dourado Mimico
    
    cache['H'] = RaycasterSprites::parseSprite(getArtFull(EnemyCreator::createMahoragaEnemy), 255, 255, 255, true); // Branco Mahoraga
}
