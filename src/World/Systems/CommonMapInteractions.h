// [PT-BR] Arquivo: CommonMapInteractions.h
// [PT-BR] Proposito: Utilitario compartilhado para registro de interacoes padrao de NPCs em mapas (DRY).
// [EN-US] File: CommonMapInteractions.h
// [EN-US] Purpose: Shared utility for registering standard NPC map interactions (DRY).

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

// [PT-BR] Provedor de registro de interacoes unificadas de NPCs para evitar duplicacao entre mapas
// [EN-US] Unified NPC interaction registration provider to avoid duplication across maps
namespace CommonMapInteractions {

    inline void registerStandardNPCs(
        std::unordered_map<char, std::function<void(int, int, int)>>& interactions,
        Character* currentPlayer,
        const bool& isExplorationActive,
        const std::function<void()>& restoreScreen
    ) {
        // [PT-BR] Mercador Franchesco ('F')
        // [EN-US] Merchant Franchesco ('F')
        interactions['F'] = [currentPlayer, &isExplorationActive, restoreScreen]([[maybe_unused]] int px, [[maybe_unused]] int py, [[maybe_unused]] int width) {
            NPCMerchant franchesco;
            franchesco.interact(currentPlayer);
            Diary::instance().registerNPC("Franchesco (Mercador)");
            if (isExplorationActive && !MapControl::is3DExplorationActive()) restoreScreen();
        };

        // [PT-BR] Ferreiro Bjorn ('B')
        // [EN-US] Blacksmith Bjorn ('B')
        interactions['B'] = [currentPlayer, &isExplorationActive, restoreScreen]([[maybe_unused]] int px, [[maybe_unused]] int py, [[maybe_unused]] int width) {
            NPCBlacksmith bjorn;
            bjorn.interact(currentPlayer);
            Diary::instance().registerNPC("Bjorn (Ferreiro)");
            if (isExplorationActive && !MapControl::is3DExplorationActive()) restoreScreen();
        };

        // [PT-BR] Estilista Real Anok ('N')
        // [EN-US] Royal Stylist Anok ('N')
        interactions['N'] = [currentPlayer, &isExplorationActive, restoreScreen]([[maybe_unused]] int px, [[maybe_unused]] int py, [[maybe_unused]] int width) {
            NPCAppearance appearance;
            appearance.interact(currentPlayer);
            Diary::instance().registerNPC("Anok (Estilista)");
            if (isExplorationActive && !MapControl::is3DExplorationActive()) restoreScreen();
        };

        // [PT-BR] Alquimista Real ('Q')
        // [EN-US] Royal Alchemist ('Q')
        interactions['Q'] = [currentPlayer, &isExplorationActive, restoreScreen]([[maybe_unused]] int px, [[maybe_unused]] int py, [[maybe_unused]] int width) {
            NPCAlchemist alchemist;
            alchemist.interact(currentPlayer);
            Diary::instance().registerNPC("Alquimista Real");
            if (isExplorationActive && !MapControl::is3DExplorationActive()) restoreScreen();
        };
    }

}
