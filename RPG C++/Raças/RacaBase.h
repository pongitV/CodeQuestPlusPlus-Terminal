#include <string>

#include "../Sistema/Personagem.h"

#pragma once

class Personagem;

class RacaBase 
{
public:
    virtual ~RacaBase() {}
    virtual std::string obterNomeRaca() const = 0;
    virtual Atributos obterAtributosRaca() const = 0;
    virtual void usarHabilidadeRaca(Personagem* usuario, Personagem* alvo) = 0;
};