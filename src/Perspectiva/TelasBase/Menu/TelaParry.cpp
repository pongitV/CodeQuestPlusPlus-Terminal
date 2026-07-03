#include "TelaParry.h"
#include "../../GerenciadorPerspectiva.h"
#include "../../PerspectivaIDE/TelasIDE/TelaParryIDE.h"
#include "../../PerspectivaRaycaster/TelasRaycaster/TelaParryRaycaster.h"

TelaParry::Resultado TelaParry::exibir(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        return TelaParryRaycaster::exibir(nomeJogador, nomeRaca, nomeClasse);
    }
    return TelaParryIDE::exibir(nomeJogador, nomeRaca, nomeClasse);
}
