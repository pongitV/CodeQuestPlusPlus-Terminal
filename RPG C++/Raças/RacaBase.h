#pragma once

#include <vector>
#include <string>
#include "../Sistema/Personagem.h"

class RacaBase {
public:
    virtual ~RacaBase() = default;

    virtual std::string obterNomeRaca() const = 0;
    virtual std::vector<std::string> obterAparenciaRaca() const = 0;
    virtual Atributos obterAtributosRaca() const = 0;

    virtual std::string obterNomeHabilidadeRaca() const = 0;
    virtual std::string obterDescricaoHabilidadeRaca() const = 0;

    virtual int processarDanoOfensivo(int danoBase, Personagem* /*atacante*/) {
        return danoBase;
    }

    virtual int processarDanoDefensivo(int danoFinal, Personagem* /*defensor*/) {
        return danoFinal;
    }

    virtual void realizarDrops(Personagem* /*inimigo*/, Personagem* /*jogadorAtual*/, std::vector<std::string>& /*itensObtidos*/, int& /*ouroTotal*/, int& /*xpTotal*/) {
        // Implementação padrão vazia (sem drops)
    }

    virtual void aoCausarDano(Personagem& atacante, Personagem& alvo, int danoCausado) {}

private:
};
