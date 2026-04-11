// Raças/RacaBase.h - Classe base abstrata para raças de personagens
// Define a interface comum para todas as implementações de raça

#ifndef RACA_BASE_H
#define RACA_BASE_H

#include <vector>
#include <string>
#include "../Sistema/Personagem.h"

class RacaBase {
public:
    // Destrutor virtual para permitir polimorfismo seguro
    virtual ~RacaBase() = default;

    // Métodos de consulta básicos da raça
    virtual std::string obterNomeRaca() const = 0;
    virtual std::vector<std::string> obterAparenciaRaca() const = 0;
    virtual Atributos obterAtributosRaca() const = 0;

    // Métodos relacionados à habilidade especial da raça
    virtual std::string obterNomeHabilidadeRaca() const = 0;
    virtual std::string obterDescricaoHabilidadeRaca() const = 0;

    // Processadores de dano com modificadores de raça
    // Por padrão, não altera o valor do dano
    // Subclasses devem implementar para adicionar modificadores específicos
    virtual int processarDanoOfensivo(int danoBase, Personagem* atacante) {
        return danoBase;
    }

    virtual int processarDanoDefensivo(int danoFinal, Personagem* defensor) {
        return danoFinal;
    }

private:
    // Se necessário, adicione dados privados para subclasses
    // Exemplo: nome da raça, dados de aparencia, etc.
};

#endif // RACA_BASE_H