#include "TelaDiario.h"
#include "../../GerenciadorPerspectiva.h"
#include "../../PerspectivaIDE/TelasIDE/TelaDiarioIDE.h"
#include "../../PerspectivaRaycaster/TelasRaycaster/TelaDiarioRaycaster.h"

void TelaDiario::exibir(Personagem* jogadorAtual) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaDiarioRaycaster::exibir(jogadorAtual);
    } else {
        TelaDiarioIDE::exibir(jogadorAtual);
    }
}
