#include "TelaNome.h"
#include "../../GerenciadorPerspectiva.h"
#include "../../PerspectivaIDE/TelasIDE/TelaNomeIDE.h"
#include "../../PerspectivaRaycaster/TelasRaycaster/TelaNomeRaycaster.h"

TelaNome::Resultado TelaNome::exibir() {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        return TelaNomeRaycaster::exibir();
    }
    return TelaNomeIDE::exibir();
}
