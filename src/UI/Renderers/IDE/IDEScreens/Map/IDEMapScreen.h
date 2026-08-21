#pragma once

#include <string>

enum class MapLocation;

class IDEMapScreen {
public:
    // [PT-BR] Renderiza o mapa e retorna o indice selecionado (0-3) ou -1 para cancelar
// [EN-US] Renders the map and returns selected index (0-3) or -1 to cancel
    static int display(MapLocation locationCurrent, int villageProgress, int forestProgress, int kingdomBridgeProgress, int kingdomProgress, const std::string& msgExtra = "");
};
