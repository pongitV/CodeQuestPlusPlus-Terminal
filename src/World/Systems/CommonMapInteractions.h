#pragma once

#include <unordered_map>
#include <functional>
#include "Domain/Characters/Character.h"
#include "Domain/NPCs/NPCMerchant.h"
#include "Domain/NPCs/NPCBlacksmith.h"
#include "Domain/NPCs/NPCPriest.h"
#include "Domain/NPCs/NPCAppearance.h"
#include "Domain/NPCs/NPCAlchemist.h"
#include "Systems/Progression/Progression.h"
#include "World/MapControl.h"

// Provedor de registro de interacoes unificadas de NPCs para evitar duplicacao entre mapas
namespace CommonMapInteractions {

    inline void registerStandardNPCs(
        std::unordered_map<char, std::function<void(int, int, int)>>& interactions,
        Character* currentPlayer,
        const bool& isExplorationActive,
        const std::function<void()>& restoreScreen
    ) {
        // Mercador Franchesco ('F')
        interactions['F'] = [currentPlayer, &isExplorationActive, restoreScreen]([[maybe_unused]] int px, [[maybe_unused]] int py, [[maybe_unused]] int width) {
            NPCMerchant franchesco;
            franchesco.interact(currentPlayer);
            Diary::instance().registerNPC("Franchesco (Mercador)");
            if (isExplorationActive && !MapControl::is3DExplorationActive()) restoreScreen();
        };

        // Ferreiro Bjorn ('B')
        interactions['B'] = [currentPlayer, &isExplorationActive, restoreScreen]([[maybe_unused]] int px, [[maybe_unused]] int py, [[maybe_unused]] int width) {
            NPCBlacksmith bjorn;
            bjorn.interact(currentPlayer);
            Diary::instance().registerNPC("Bjorn (Ferreiro)");
            if (isExplorationActive && !MapControl::is3DExplorationActive()) restoreScreen();
        };

        // Estilista Real Anok ('N')
        interactions['N'] = [currentPlayer, &isExplorationActive, restoreScreen]([[maybe_unused]] int px, [[maybe_unused]] int py, [[maybe_unused]] int width) {
            NPCAppearance appearance;
            appearance.interact(currentPlayer);
            Diary::instance().registerNPC("Anok (Estilista)");
            if (isExplorationActive && !MapControl::is3DExplorationActive()) restoreScreen();
        };

        // Alquimista Real ('Q')
        interactions['Q'] = [currentPlayer, &isExplorationActive, restoreScreen]([[maybe_unused]] int px, [[maybe_unused]] int py, [[maybe_unused]] int width) {
            NPCAlchemist alchemist;
            alchemist.interact(currentPlayer);
            Diary::instance().registerNPC("Alquimista Real");
            if (isExplorationActive && !MapControl::is3DExplorationActive()) restoreScreen();
        };
    }

}
