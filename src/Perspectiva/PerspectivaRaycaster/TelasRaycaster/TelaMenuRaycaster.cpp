#include "TelaMenuRaycaster.h"

void TelaMenuRaycaster::exibirPainelLogoJogo(const std::string&, bool) {}

bool TelaMenuRaycaster::exibirConfirmacaoDeEscolhaComArteLadoALado(const std::string&, const std::string&, const std::vector<std::string>&, const std::vector<std::string>&) {
    return false;
}

std::vector<std::string> TelaMenuRaycaster::comporQuadroDeAtributos(const Atributos&, const std::string&, const std::string&, const std::string&, const std::string&, const std::string&, const std::string&, const std::string&) {
    return {};
}

int TelaMenuRaycaster::exibirOpcoesMenuPrincipal(bool, const std::vector<std::vector<std::string>>&, const std::vector<std::vector<std::string>>&, const std::vector<std::vector<std::string>>&) {
    return 0;
}

int TelaMenuRaycaster::exibirMenuCarregarJogo(const std::vector<std::string>&) {
    return 0;
}

void TelaMenuRaycaster::exibirPromptNome() {}

int TelaMenuRaycaster::exibirPromptRaca(const std::string&) {
    return 0;
}

int TelaMenuRaycaster::exibirPromptClasse(const std::string&, const std::string&) {
    return 0;
}

int TelaMenuRaycaster::exibirPromptParry(const std::string&, const std::string&, const std::string&) {
    return 0;
}

int TelaMenuRaycaster::exibirPromptDificuldade(const std::string&, const std::string&, const std::string&) {
    return 0;
}

void TelaMenuRaycaster::exibirTutorialDeParry(const std::string&) {}

void TelaMenuRaycaster::exibirIntroducaoJornada(const std::string&) {}
