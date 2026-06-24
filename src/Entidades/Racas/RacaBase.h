#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../../Sistemas/Inventario/Item.h"
#include "../Personagem.h"

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
    AbominacaoFloresta,
    Mimico,
    Troll,
    Mahoraga
};

struct InfoBestiario {
    std::string mapa;
    std::string habitat;
    std::string lore;
    std::string fatoCurioso;
    std::vector<std::string> drops;
    int dificuldade;
};

class RacaBase {
public:
    virtual ~RacaBase() = default;

    virtual std::string obterNomeRaca() const = 0;
    virtual TipoRaca obterTipoRaca() const = 0;
    virtual const std::vector<std::string>& obterAparenciaRaca() const = 0;
    virtual const std::vector<std::string>& obterAparenciaCombate() const { return obterAparenciaRaca(); }
    virtual Atributos obterAtributosRaca() const = 0;

    virtual std::string obterNomeHabilidadeRaca() const = 0;
    virtual std::string obterDescricaoHabilidadeRaca() const = 0;

    virtual InfoBestiario obterInfoBestiario() const { return {"Desconhecido", "Desconhecido", "", "", {}, 1}; }

    virtual std::vector<std::unique_ptr<Item>> obterEquipamentoRaca() const { return {}; }

    virtual int processarDanoOfensivo(int danoBase, Personagem* /*atacante*/) {
        return danoBase;
    }

    virtual int processarDanoDefensivo(int danoFinal, Personagem* /*defensor*/) {
        return danoFinal;
    }
    
    virtual void aoSofrerParryPerfeito() {}
    virtual bool ignoraParry() const { return false; }

    virtual void realizarDrops(Personagem* /*inimigo*/, Personagem* /*jogadorAtual*/, std::vector<std::string>& /*itensObtidos*/, int& /*ouroTotal*/, int& /*xpTotal*/) {
        // Implementação padrão vazia (sem drops)
    }

    virtual void aoCausarDano(Personagem* /*atacante*/, Personagem* /*alvo*/, int /*danoCausado*/) {}

    virtual bool tentarUsarHabilidadeAtiva(Personagem* /*esteInimigo*/, Personagem* /*alvo*/, int /*dificuldade*/) {
        return false; // Por padrao, inimigos nao possuem habilidades ativas que consomem o turno
    }

private:
};
