// [PT-BR] Arquivo: InteractionMap.h
// [PT-BR] Proposito: Interface de mapas exploraveis (IMap) e enumerador de transicao entre ambientes.
// [EN-US] File: InteractionMap.h
// [EN-US] Purpose: Exploratory maps interface (IMap) and destination transition enumerator between areas.

#pragma once

#include <functional>
#include <string>

// [PT-BR] Transicoes de destino ao trocar de mapa
// [EN-US] Destination transitions when switching maps
enum class NextMapTransition {
    None,
    ReturnMenu,
    Village,
    Forest,
    KingdomBridge,
    Kingdom
};

// [PT-BR] Interface de contrato para um mapa exploravel do jogo.
// [EN-US] Contract interface for an exploratory game map.
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
