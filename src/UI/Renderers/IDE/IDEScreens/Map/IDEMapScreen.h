#pragma once

#include <string>

enum class MapLocation;

class IDEMapScreen {
public:
    // Renderiza o mapa e retorna o indice selecionado (0-3) ou -1 para cancelar
    static int display(MapLocation locationCurrent, int villageProgress, int forestProgress, int kingdomBridgeProgress, int kingdomProgress, const std::string& msgExtra = "");
};
