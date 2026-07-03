#include "TelaDificuldade.h"
#include "../../GerenciadorPerspectiva.h"
#include "../../PerspectivaIDE/TelasIDE/TelaDificuldadeIDE.h"
#include "../../PerspectivaRaycaster/TelasRaycaster/TelaDificuldadeRaycaster.h"

TelaDificuldade::Resultado TelaDificuldade::exibir(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        return TelaDificuldadeRaycaster::exibir(nomeJogador, nomeRaca, nomeClasse);
    }
    return TelaDificuldadeIDE::exibir(nomeJogador, nomeRaca, nomeClasse);
}
