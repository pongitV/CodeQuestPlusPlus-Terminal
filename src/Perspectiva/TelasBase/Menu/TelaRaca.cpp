#include "TelaRaca.h"
#include "../../GerenciadorPerspectiva.h"
#include "../../PerspectivaIDE/TelasIDE/TelaRacaIDE.h"
#include "../../PerspectivaRaycaster/TelasRaycaster/TelaRacaRaycaster.h"

TelaRaca::Resultado TelaRaca::exibir(const std::string& nomeJogador) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        return TelaRacaRaycaster::exibir(nomeJogador);
    }
    return TelaRacaIDE::exibir(nomeJogador);
}
