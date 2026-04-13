#pragma once

#include <vector>
#include <string>
#include "../Sistema/Personagem.h"

class RacaBase {
public:
    // Destrutor virtual para permitir polimorfismo seguro
    virtual ~RacaBase() = default;

    // INFORMACOES DA RACA
    virtual std::string obterNomeRaca() const = 0;
    virtual std::vector<std::string> obterAparenciaRaca() const = 0;
    virtual Atributos obterAtributosRaca() const = 0;

    // HABILIDADE PASSIVA DA RACA
    virtual std::string obterNomeHabilidadeRaca() const = 0;
    virtual std::string obterDescricaoHabilidadeRaca() const = 0;

    // --- PROCESSADORES DE DANO ---
    // Modificadores que atuam no momento de ataque e defesa.
    // Por padrão, não altera o valor do dano
    // Subclasses devem implementar para adicionar modificadores específicos
    virtual int processarDanoOfensivo(int danoBase, Personagem* /*atacante*/) {
        return danoBase;
    }

    virtual int processarDanoDefensivo(int danoFinal, Personagem* /*defensor*/) {
        return danoFinal;
    }

private:
};
