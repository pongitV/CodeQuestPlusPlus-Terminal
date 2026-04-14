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

    static void realizarDrops(Personagem* inimigo, Personagem* jogadorAtual, std::vector<std::string>& itensObtidos);
    static std::vector<std::string> obterMapaCaverna(bool bjornResgatado);
};