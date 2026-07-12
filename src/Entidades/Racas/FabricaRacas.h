#pragma once

#include <memory>
#include "RacaBase.h"

class FabricaRacas {
public:
    static std::unique_ptr<RacaBase> criarRaca(TipoRaca tipo);
    static std::vector<TipoRaca> obterRacasJogaveis();
};
