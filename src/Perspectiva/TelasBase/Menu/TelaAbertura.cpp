#include "TelaAbertura.h"
#include "../../GerenciadorPerspectiva.h"
#include "../../PerspectivaIDE/TelasIDE/TelaAberturaIDE.h"
#include "../../PerspectivaRaycaster/TelasRaycaster/TelaAberturaRaycaster.h"

void TelaAbertura::exibir() {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaAberturaRaycaster::exibir();
    } else {
        TelaAberturaIDE::exibir();
    }
}
