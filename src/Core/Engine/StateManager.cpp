/*
 * Arquivo: StateManager.cpp
 * Proposito: Implementacao das transicoes de estados e loop principal de exploracao.
 */

#include "Core/Engine/StateManager.h"
#include "Domain/Characters/Classes/Archer.h"
#include "Domain/Characters/Classes/Bard.h"
#include "Domain/Characters/Classes/BaseClass.h"
#include "Domain/Characters/Classes/Warrior.h"
#include "Domain/Characters/Classes/Mage.h"
#include "Core/Engine/GameMenu.h"
#include "World/Village/Map1Village.h"
#include "World/Forest/Map2Forest.h"
#include "World/Kingdom/Map3KingdomBridge.h"
#include "World/Kingdom/Map4Kingdom.h"
#include "Domain/Characters/Races/Dwarf.h"
#include "Domain/Characters/Races/Elf.h"
#include "Domain/Characters/Races/Human.h"
#include "Domain/Characters/Races/Orc.h"
#include "Systems/Progression/Progression.h"
#include "Systems/Progression/ProgressionFlags.h"

void MenuState::execute(Game& game, GameContext& ctx) {
    auto player = GameMenu::mainMenu();
    if (!player) { 
        game.changeState(nullptr); 
        return; 
    }
    ctx.player = std::move(player);
    game.changeState(std::make_unique<ExplorationState>());
}

void ExplorationState::onExit(Game& game, GameContext& ctx) {
    // Desvincula e limpa a memoria do jogador para a proxima iteracao
    ctx.player.reset();
}

void ExplorationState::execute(Game& game, GameContext& ctx) {
    Character* player = ctx.player.get();
    if (!player) { 
        game.changeState(nullptr); 
        return; 
    }

    auto villageMap = std::make_unique<Map1Village>(player);
    auto forestMap = std::make_unique<Map2Forest>(player);
    auto kingdomBridgeMap = std::make_unique<Map3KingdomBridge>(player);
    auto kingdomMap = std::make_unique<Map4Kingdom>(player);

    IMap* currentMap = villageMap.get();
    while (currentMap) {
        NextMapTransition transition = currentMap->startExplorationLoop();
        
        if (transition == NextMapTransition::ReturnMenu || player->getHealth() <= 0 || player->getReturnToMenu()) {
            break;
        }
        else if (transition == NextMapTransition::Village) {
            currentMap = villageMap.get();
            villageMap->isExplorationActive = true;
        }
        else if (transition == NextMapTransition::Forest) {
            currentMap = forestMap.get();
            forestMap->isExplorationActive = true;
            if (!Progression::instance().getFlag(Flags::Visited_Forest)) Progression::instance().setFlag(Flags::Visited_Forest, true);
        }
        else if (transition == NextMapTransition::KingdomBridge) {
            currentMap = kingdomBridgeMap.get();
            kingdomBridgeMap->isExplorationActive = true;
            if (!Progression::instance().getFlag(Flags::Visited_KingdomBridge)) Progression::instance().setFlag(Flags::Visited_KingdomBridge, true);
        }
        else if (transition == NextMapTransition::Kingdom) {
            currentMap = kingdomMap.get();
            kingdomMap->isExplorationActive = true;
            if (!Progression::instance().getFlag(Flags::Visited_Kingdom)) Progression::instance().setFlag(Flags::Visited_Kingdom, true);
        }
        else { // Inclui NextMapTransition::None
            /*
             * Se nenhuma transicao foi solicitada, encerra o loop de exploracao por precaucao
             * e forca o retorno seguro ao menu principal.
             */
            player->setReturnToMenu(true);
            break;
        }
    }
    
    if (player->getHealth() > 0 && !player->getReturnToMenu()) { 
        game.changeState(nullptr); 
        return; 
    }

    game.changeState(std::make_unique<MenuState>());
}
