#include "TelaBestiario.h"
#include "../../GerenciadorPerspectiva.h"

void TelaBestiario::exibirLista(Personagem* jogadorAtual) {
    GerenciadorPerspectiva::obterBestiarioUI().exibir({});
}

void TelaBestiario::exibirFicha(Personagem* jogadorAtual, const std::string& nomeInimigo, int indiceDescoberto, const std::vector<std::string>& descobertos) {
    GerenciadorPerspectiva::obterBestiarioUI().exibirDetalhe(nullptr);
}
