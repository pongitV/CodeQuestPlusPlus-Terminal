#include "TelaMenuRaycaster.h"

void TelaMenuRaycaster::exibirPainelLogoJogo(const std::string&, bool) {}

bool TelaMenuRaycaster::exibirConfirmacaoDeEscolhaComArteLadoALado(const std::string&, const std::string&, const std::vector<std::string>&, const std::vector<std::string>&) {
    return false;
}

std::vector<std::string> TelaMenuRaycaster::comporQuadroDeAtributos(const Atributos&, const std::string&, const std::string&, const std::string&, const std::string&, const std::string&, const std::string&, const std::string&) {
    return {};
}

int TelaMenuRaycaster::exibirOpcoesMenuPrincipal(bool, const std::vector<std::vector<std::string>>&, const std::vector<std::vector<std::string>>&, const std::vector<std::vector<std::string>>&) {
    return -1;
}

int TelaMenuRaycaster::exibirMenuCarregarJogo(const std::vector<std::string>&) {
    return -1;
}

void TelaMenuRaycaster::exibirTutorialDeParry(const std::string&) {}
