#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../Inventario/Item.h"
#include "../Sistemas/SistemaPersonagem.h"

enum class TipoRaca 
{
    Nenhum,
    Dwarf,
    Elfo,
    Humano,
    Ork,
    OrkExilado,
    Goblin,
    Fada,
    Slime,
    AbominacaoFloresta
};

class RacaBase {
public:
    virtual ~RacaBase() = default;

    virtual std::string obterNomeRaca() const = 0;
    virtual TipoRaca obterTipoRaca() const = 0;
    virtual std::vector<std::string> obterAparenciaRaca() const = 0;
    virtual Atributos obterAtributosRaca() const = 0;

    virtual std::string obterNomeHabilidadeRaca() const = 0;
    virtual std::string obterDescricaoHabilidadeRaca() const = 0;

    virtual std::vector<std::unique_ptr<Item>> obterEquipamentoRaca() const { return {}; }

    virtual int processarDanoOfensivo(int danoBase, SistemaPersonagem* /*atacante*/) {
        return danoBase;
    }

    virtual int processarDanoDefensivo(int danoFinal, SistemaPersonagem* /*defensor*/) {
        return danoFinal;
    }

    virtual void realizarDrops(SistemaPersonagem* /*inimigo*/, SistemaPersonagem* /*jogadorAtual*/, std::vector<std::string>& /*itensObtidos*/, int& /*ouroTotal*/, int& /*xpTotal*/) {
        // Implementação padrão vazia (sem drops)
    }

    virtual void aoCausarDano(SistemaPersonagem* atacante, SistemaPersonagem* alvo, int danoCausado) {}

    virtual bool tentarUsarHabilidadeAtiva(SistemaPersonagem* /*esteInimigo*/, SistemaPersonagem* /*alvo*/, int /*dificuldade*/) {
        return false; // Por padrao, inimigos nao possuem habilidades ativas que consomem o turno
    }

private:
};
