#include "TelaBestiario.h"
#include "../../GerenciadorPerspectiva.h"
#include "../../PerspectivaIDE/TelasIDE/TelaBestiarioIDE.h"
#include "../../PerspectivaRaycaster/TelasRaycaster/TelaBestiarioRaycaster.h"

void TelaBestiario::exibirLista(Personagem* jogadorAtual) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaBestiarioRaycaster::exibir({});
    } else {
        TelaBestiarioIDE::exibirLista(jogadorAtual);
    }
}

void TelaBestiario::exibirFicha(Personagem* jogadorAtual, const std::string& nomeInimigo, int indiceDescoberto, const std::vector<std::string>& descobertos) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaBestiarioRaycaster::exibirDetalhe(nullptr);
    } else {
        TelaBestiarioIDE::exibirFicha(jogadorAtual, nomeInimigo, indiceDescoberto, descobertos);
    }
}
