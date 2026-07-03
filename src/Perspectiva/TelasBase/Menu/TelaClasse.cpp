#include "TelaClasse.h"
#include "../../GerenciadorPerspectiva.h"
#include "../../PerspectivaIDE/TelasIDE/TelaClasseIDE.h"
#include "../../PerspectivaRaycaster/TelasRaycaster/TelaClasseRaycaster.h"

TelaClasse::Resultado TelaClasse::exibir(const std::string& nomeJogador, const std::string& nomeRaca) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        return TelaClasseRaycaster::exibir(nomeJogador, nomeRaca);
    }
    return TelaClasseIDE::exibir(nomeJogador, nomeRaca);
}
