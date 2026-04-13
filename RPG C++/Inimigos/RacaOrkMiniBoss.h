#pragma once

#include <string>
#include <vector>

#include "../Raças/RacaOrk.h"

class RacaOrkMiniBoss : public RacaOrk
{
public:
    std::string obterNomeRaca() const override;
    Atributos obterAtributosRaca() const override;
    std::vector<std::string> obterAparenciaRaca() const override;
};