#include "GerenciadorTelasRaycaster.h"
#include "TelasRaycaster/Menu/TelaAberturaRaycaster.h"
#include "TelasRaycaster/Menu/TelaMenuRaycaster.h"
#include "TelasRaycaster/Menu/TelaNomeRaycaster.h"
#include "TelasRaycaster/Menu/TelaRacaRaycaster.h"
#include "TelasRaycaster/Menu/TelaClasseRaycaster.h"
#include "TelasRaycaster/Menu/TelaDificuldadeRaycaster.h"
#include "TelasRaycaster/Menu/TelaParryRaycaster.h"
#include "TelasRaycaster/Menu/TelaIntroducaoRaycaster.h"
#include "../TelasBase/Menu/TelaTutorial.h"

void GerenciadorTelasRaycaster::abertura() {
    TelaAberturaRaycaster::exibir();
}

void GerenciadorTelasRaycaster::painelLogo(const std::string& tituloDaTela, bool animarFadeIn) {
    TelaMenuRaycaster::exibirPainelLogoJogo(tituloDaTela, animarFadeIn);
}

bool GerenciadorTelasRaycaster::confirmacaoEscolha(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha,
    const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir) {
    return TelaMenuRaycaster::exibirConfirmacaoDeEscolhaComArteLadoALado(tipoDeEscolha, nomeDaEscolha, informacoesParaExibir, arteAsciiParaExibir);
}

std::vector<std::string> GerenciadorTelasRaycaster::quadroAtributos(const Atributos& stats,
    const std::string& tituloSecao, const std::string& tituloHabilidade,
    const std::string& nomeHab, const std::string& descHab,
    const std::string& tituloHabilidade2, const std::string& nomeHab2, const std::string& descHab2) {
    return TelaMenuRaycaster::comporQuadroDeAtributos(stats, tituloSecao, tituloHabilidade, nomeHab, descHab, tituloHabilidade2, nomeHab2, descHab2);
}

int GerenciadorTelasRaycaster::menuPrincipal() {
    return TelaMenuRaycaster::exibirOpcoesMenuPrincipal();
}

void GerenciadorTelasRaycaster::tutorialParry(const std::string& infoBox) {
    TelaTutorial::exibirTutorialDeParry(infoBox); // Tutorial não é separado ainda
}

TelaNome::Resultado GerenciadorTelasRaycaster::telaNome() {
    return TelaNomeRaycaster::exibir();
}

TelaRaca::Resultado GerenciadorTelasRaycaster::telaRaca(const std::string& nomePersonagem) {
    return TelaRacaRaycaster::exibir(nomePersonagem);
}

TelaClasse::Resultado GerenciadorTelasRaycaster::telaClasse(const std::string& nomePersonagem, const std::string& raca) {
    return TelaClasseRaycaster::exibir(nomePersonagem, raca);
}

TelaDificuldade::Resultado GerenciadorTelasRaycaster::telaDificuldade(const std::string& nomePersonagem, const std::string& raca, const std::string& classe) {
    return TelaDificuldadeRaycaster::exibir(nomePersonagem, raca, classe);
}

TelaParry::Resultado GerenciadorTelasRaycaster::telaParry(const std::string& nomePersonagem, const std::string& raca, const std::string& classe) {
    return TelaParryRaycaster::exibir(nomePersonagem, raca, classe);
}

void GerenciadorTelasRaycaster::telaIntroducao() {
    TelaIntroducaoRaycaster::exibir();
}

bool GerenciadorTelasRaycaster::confirmarSaida() {
    return TelaMenuRaycaster::exibirConfirmacaoSaida();
}
