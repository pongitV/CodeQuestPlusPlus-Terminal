#pragma once

#include <string>
#include <vector>

#include "../Raças/RacaOrk.h"

class RacaOrkExilado : public RacaOrk
{
public:
    std::string obterNomeRaca() const override;
    Atributos obterAtributosRaca() const override;
    std::vector<std::string> obterAparenciaRaca() const override;
};