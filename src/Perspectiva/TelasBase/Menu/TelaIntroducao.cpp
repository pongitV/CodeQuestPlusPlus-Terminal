#include "TelaIntroducao.h"
#include "../../GerenciadorPerspectiva.h"
#include "../../PerspectivaIDE/TelasIDE/TelaIntroducaoIDE.h"
#include "../../PerspectivaRaycaster/TelasRaycaster/TelaIntroducaoRaycaster.h"

void TelaIntroducao::exibir() {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaIntroducaoRaycaster::exibir();
    } else {
        TelaIntroducaoIDE::exibir();
    }
}
