#include "GerenciadorTelasIDE.h"
#include "TelasIDE/Menu/TelaAberturaIDE.h"
#include "TelasIDE/Menu/TelaMenuIDE.h"
#include "TelasIDE/Menu/TelaNomeIDE.h"
#include "TelasIDE/Menu/TelaRacaIDE.h"
#include "TelasIDE/Menu/TelaClasseIDE.h"
#include "TelasIDE/Menu/TelaDificuldadeIDE.h"
#include "TelasIDE/Menu/TelaParryIDE.h"
#include "TelasIDE/Menu/TelaIntroducaoIDE.h"
#include "../TelasBase/Menu/TelaTutorial.h"

void GerenciadorTelasIDE::abertura() {
    TelaAberturaIDE::exibir();
}

void GerenciadorTelasIDE::painelLogo(const std::string& tituloDaTela, bool animarFadeIn) {
    TelaMenuIDE::exibirPainelLogoJogo(tituloDaTela, animarFadeIn);
}

bool GerenciadorTelasIDE::confirmacaoEscolha(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha,
    const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir) {
    return TelaMenuIDE::exibirConfirmacaoDeEscolhaComArteLadoALado(tipoDeEscolha, nomeDaEscolha, informacoesParaExibir, arteAsciiParaExibir);
}

std::vector<std::string> GerenciadorTelasIDE::quadroAtributos(const Atributos& stats,
    const std::string& tituloSecao, const std::string& tituloHabilidade,
    const std::string& nomeHab, const std::string& descHab,
    const std::string& tituloHabilidade2, const std::string& nomeHab2, const std::string& descHab2) {
    return TelaMenuIDE::comporQuadroDeAtributos(stats, tituloSecao, tituloHabilidade, nomeHab, descHab, tituloHabilidade2, nomeHab2, descHab2);
}

int GerenciadorTelasIDE::menuPrincipal() {
    return TelaMenuIDE::exibirOpcoesMenuPrincipal();
}

void GerenciadorTelasIDE::tutorialParry(const std::string& infoBox) {
    TelaTutorial::exibirTutorialDeParry(infoBox);
}

TelaNome::Resultado GerenciadorTelasIDE::telaNome() {
    return TelaNomeIDE::exibir();
}

TelaRaca::Resultado GerenciadorTelasIDE::telaRaca(const std::string& nomePersonagem) {
    return TelaRacaIDE::exibir(nomePersonagem);
}

TelaClasse::Resultado GerenciadorTelasIDE::telaClasse(const std::string& nomePersonagem, const std::string& raca) {
    return TelaClasseIDE::exibir(nomePersonagem, raca);
}

TelaDificuldade::Resultado GerenciadorTelasIDE::telaDificuldade(const std::string& nomePersonagem, const std::string& raca, const std::string& classe) {
    return TelaDificuldadeIDE::exibir(nomePersonagem, raca, classe);
}

TelaParry::Resultado GerenciadorTelasIDE::telaParry(const std::string& nomePersonagem, const std::string& raca, const std::string& classe) {
    return TelaParryIDE::exibir(nomePersonagem, raca, classe);
}

void GerenciadorTelasIDE::telaIntroducao() {
    TelaIntroducaoIDE::exibir();
}

bool GerenciadorTelasIDE::confirmarSaida() {
    return TelaMenuIDE::exibirConfirmacaoSaida();
}
