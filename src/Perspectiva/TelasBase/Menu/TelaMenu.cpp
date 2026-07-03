#include "TelaMenu.h"
#include "TelaTutorial.h"
#include "../../GerenciadorPerspectiva.h"
#include "../../PerspectivaIDE/TelasIDE/TelaMenuIDE.h"
#include "../../PerspectivaRaycaster/TelasRaycaster/TelaMenuRaycaster.h"

void TelaMenu::exibirPainelLogoJogo(const std::string& tituloDaTela, bool animarFadeIn) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaMenuRaycaster::exibirPainelLogoJogo(tituloDaTela, animarFadeIn);
    } else {
        TelaMenuIDE::exibirPainelLogoJogo(tituloDaTela, animarFadeIn);
    }
}

bool TelaMenu::exibirConfirmacaoDeEscolhaComArteLadoALado(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha, const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        return TelaMenuRaycaster::exibirConfirmacaoDeEscolhaComArteLadoALado(tipoDeEscolha, nomeDaEscolha, informacoesParaExibir, arteAsciiParaExibir);
    }
    return TelaMenuIDE::exibirConfirmacaoDeEscolhaComArteLadoALado(tipoDeEscolha, nomeDaEscolha, informacoesParaExibir, arteAsciiParaExibir);
}

std::vector<std::string> TelaMenu::comporQuadroDeAtributos(const Atributos& stats, const std::string& tituloSecao, const std::string& tituloHabilidade, const std::string& nomeHab, const std::string& descHab, const std::string& tituloHabilidade2, const std::string& nomeHab2, const std::string& descHab2) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        return TelaMenuRaycaster::comporQuadroDeAtributos(stats, tituloSecao, tituloHabilidade, nomeHab, descHab, tituloHabilidade2, nomeHab2, descHab2);
    }
    return TelaMenuIDE::comporQuadroDeAtributos(stats, tituloSecao, tituloHabilidade, nomeHab, descHab, tituloHabilidade2, nomeHab2, descHab2);
}

int TelaMenu::exibirOpcoesMenuPrincipal(bool temSave, const std::vector<std::vector<std::string>>& artesClasses, const std::vector<std::vector<std::string>>& artesRacas, const std::vector<std::vector<std::string>>& artesInimigos) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        return TelaMenuRaycaster::exibirOpcoesMenuPrincipal(temSave, artesClasses, artesRacas, artesInimigos);
    }
    return TelaMenuIDE::exibirOpcoesMenuPrincipal(temSave, artesClasses, artesRacas, artesInimigos);
}

int TelaMenu::exibirMenuCarregarJogo(const std::vector<std::string>& informacoesSaves) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        return TelaMenuRaycaster::exibirMenuCarregarJogo(informacoesSaves);
    }
    return TelaMenuIDE::exibirMenuCarregarJogo(informacoesSaves);
}

void TelaMenu::exibirTutorialDeParry(const std::string& infoBox) {
    TelaTutorial::exibirTutorialDeParry(infoBox);
}
