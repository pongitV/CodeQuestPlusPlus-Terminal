/*
 * Arquivo: InteractionMap.h
 * Proposito: Interface de mapas exploraveis (IMap) e enumerador de transicao entre ambientes.
 */

#pragma once

#include <functional>
#include <string>

// Transicoes de destino ao trocar de mapa
enum class NextMapTransition {
    None,
    ReturnMenu,
    Village,
    Forest,
    KingdomBridge,
    Kingdom
};

/*
 * Interface contrato de um mapa exploravel do jogo.
 */
class IMap {
public:
    virtual ~IMap() = default;
    virtual std::string getTitle() const = 0;
    virtual NextMapTransition startExplorationLoop() = 0;
};


class Map2Forest;
struct ForestInteractionContext {
    Map2Forest* self;
    int nextPositionX;
    int nextPositionY;
    int terminalWidth;
    const std::function<void()>& restoreScreen;
    char cell;
    const std::function<void()>& animateScreen;
};
class ForestInteraction {
public:
    virtual ~ForestInteraction() = default;
    virtual void process(ForestInteractionContext& ctx) = 0;
};


class Map1Village;
struct VillageInteractionContext {
    Map1Village* self;
    int nextPositionX;
    int nextPositionY;
    int terminalWidth;
    const std::function<void()>& restoreScreen;
    char cell;
    const std::function<void()>& animateScreen;
};
class VillageInteraction {
public:
    virtual ~VillageInteraction() = default;
    virtual void process(VillageInteractionContext& ctx) = 0;
};
