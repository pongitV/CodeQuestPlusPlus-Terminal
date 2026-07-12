#include "FabricaRacas.h"
#include "Anao.h"
#include "Elfo.h"
#include "Humano.h"
#include "Orc.h"
// Adicione includes de inimigos se necessário no futuro

std::unique_ptr<RacaBase> FabricaRacas::criarRaca(TipoRaca tipo) {
    switch (tipo) {
        case TipoRaca::Dwarf: return std::make_unique<Dwarf>();
        case TipoRaca::Elfo: return std::make_unique<Elfo>();
        case TipoRaca::Humano: return std::make_unique<Humano>();
        case TipoRaca::Ork: return std::make_unique<Ork>();
        default: return nullptr;
    }
}

std::vector<TipoRaca> FabricaRacas::obterRacasJogaveis() {
    return {TipoRaca::Dwarf, TipoRaca::Elfo, TipoRaca::Humano, TipoRaca::Ork};
}
