#include <vector>
#include <string>

#include "../Sistema/Personagem.h"

#pragma once

class RacaBase 
{
public:
    virtual ~RacaBase() {}
    virtual std::string obterNomeRaca() const = 0;
    virtual std::vector<std::string> obterAparenciaRaca() const = 0;
    virtual Atributos obterAtributosRaca() const = 0;
    
    virtual void usarHabilidadeRaca(Personagem* u, Personagem* a) = 0;
};