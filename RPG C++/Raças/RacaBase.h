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
    
    virtual std::string obterNomeHabilidadeRaca() const = 0;
    virtual std::string obterDescricaoHabilidadeRaca() const = 0;

    virtual int processarDanoOfensivo(int danoBase, Personagem* atacante) { return danoBase; }
    virtual int processarDanoDefensivo(int danoFinal, Personagem* defensor) { return danoFinal; }
};