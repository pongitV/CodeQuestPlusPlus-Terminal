#include "CombateRaycasterUIImpl.h"

#include "../../Perspectiva/PerspectivaRaycaster/TelasRaycaster/Combate/TelaCombateRaycaster.h"
#include "../../Perspectiva/PerspectivaRaycaster/TelasRaycaster/Vitoria/TelaVitoriaRaycaster.h"
#include "../../Perspectiva/PerspectivaRaycaster/TelasRaycaster/Derrota/TelaDerrotaRaycaster.h"
#include "../../Perspectiva/TelasBase/Atributos/TelaAtributos.h"
#include "../../Perspectiva/TelasBase/Diario/TelaDiario.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Perspectiva/TelasBase/Combate/TelaCombate.h"

void CombateRaycasterUIImpl::configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) {
    TelaCombateRaycaster::configurarContexto3D(modo3D, matriz, posX, posY, angulo, titulo);
}

void CombateRaycasterUIImpl::animarIntroducaoCombate(const std::string& titulo, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual) {
    TelaCombateRaycaster::animarIntroducaoCombate(titulo, inimigos, jogadorAtual);
}

void CombateRaycasterUIImpl::atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, bool animarEntrada) {
    TelaCombateRaycaster::atualizarTelaEstatica(tituloCombate, listaDeInimigos, jogadorAtual, listaDeAliados, animarEntrada, nullptr);
}

void CombateRaycasterUIImpl::animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* atacante, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int danoAnimacao) {
    TelaCombateRaycaster::animarDanoNoInimigo(tituloCombate, listaDeInimigos, alvoAnimacao, atacante, jogadorAtual, listaDeAliados, danoAnimacao);
}

void CombateRaycasterUIImpl::animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int curaAnimacao) {
    TelaCombateRaycaster::animarCuraNoInimigo(tituloCombate, listaDeInimigos, alvoAnimacao, jogadorAtual, listaDeAliados, curaAnimacao);
}

void CombateRaycasterUIImpl::animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, bool isParry, int danoAnimacao) {
    TelaCombateRaycaster::animarDanoNoJogador(tituloCombate, listaDeInimigos, alvoAnimacao, jogadorAtual, listaDeAliados, isParry, danoAnimacao);
}

void CombateRaycasterUIImpl::animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int curaAnimacao) {
    TelaCombateRaycaster::animarCuraNoJogador(tituloCombate, listaDeInimigos, alvoAnimacao, jogadorAtual, listaDeAliados, curaAnimacao);
}

void CombateRaycasterUIImpl::animarMorteInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* inimigoMorto, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, const std::vector<std::string>& drops) {
    TelaCombateRaycaster::animarMorteInimigo(tituloCombate, listaDeInimigos, inimigoMorto, jogadorAtual, listaDeAliados, drops);
}

void CombateRaycasterUIImpl::limparContextoPersonagemHUD() {
    TelaCombate::contexto.personagemHUD = nullptr;
}

void CombateRaycasterUIImpl::limparContextoInimigoMortoEDrops() {
    TelaCombate::contexto.inimigoMortoComDrops = nullptr;
    TelaCombate::contexto.dropsAtivos.clear();
}

std::string CombateRaycasterUIImpl::margemCombate() {
    return ""; // TelaCombateRaycaster usually uses its own formatting or empty margin for fixed messages in 3D
}

void CombateRaycasterUIImpl::adicionarMensagemFixa(const std::string& msg) {
    TelaCombateRaycaster::adicionarMensagemFixa(msg);
}

void CombateRaycasterUIImpl::limparMensagensFixas() {
    TelaCombateRaycaster::limparMensagensFixas();
}

void CombateRaycasterUIImpl::definirTurnoVisivel(int turno, const std::string& nome) {
    TelaCombateRaycaster::definirTurnoVisivel(turno, nome);
}

int CombateRaycasterUIImpl::obterAcaoDoJogador(int turnoAtual, Personagem* personagemAgindo, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {
    return TelaCombateRaycaster::obterAcaoDoJogador(turnoAtual, personagemAgindo, inimigos, jogadorAtual, aliados);
}

int CombateRaycasterUIImpl::obterAlvoAtaque(const std::string& tituloCombate, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {
    return TelaCombateRaycaster::obterAlvoAtaque(tituloCombate, inimigos, jogadorAtual, aliados);
}

int CombateRaycasterUIImpl::obterAlvoItem(const std::string& tituloCombate, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {
    return TelaCombateRaycaster::obterAlvoItem(tituloCombate, inimigos, jogadorAtual, aliados);
}

int CombateRaycasterUIImpl::obterEscolhaDeEscudo(const std::string& nomePersonagem, const std::vector<Item*>& listaDeEscudos) {
    return TelaCombateRaycaster::obterEscolhaDeEscudo(nomePersonagem, listaDeEscudos);
}

void CombateRaycasterUIImpl::notificarInimigosMaisAgeis() {
    TelaCombateRaycaster::notificarInimigosMaisAgeis();
}

void CombateRaycasterUIImpl::notificarTurnoExtra(int destrezaJogador, int maxDestrezaInimigos) {
    TelaCombateRaycaster::notificarTurnoExtra(destrezaJogador, maxDestrezaInimigos);
}

void CombateRaycasterUIImpl::notificarDesprevencaoInventario() {
    TelaCombateRaycaster::notificarDesprevencaoInventario();
}

void CombateRaycasterUIImpl::notificarSemEscudos(const std::string& nomePersonagem) {
    TelaCombateRaycaster::notificarSemEscudos(nomePersonagem);
}

void CombateRaycasterUIImpl::notificarDesequilibrioDefesa(const std::string& nomePersonagem) {
    TelaCombateRaycaster::notificarDesequilibrioDefesa(nomePersonagem);
}

void CombateRaycasterUIImpl::notificarPosturaDefensiva(const std::string& nomePersonagem, const std::string& nomeEscudo) {
    TelaCombateRaycaster::notificarPosturaDefensiva(nomePersonagem, nomeEscudo);
}

void CombateRaycasterUIImpl::notificarAcaoInvalida() {
    TelaCombateRaycaster::notificarAcaoInvalida();
}

void CombateRaycasterUIImpl::notificarCancelamentoItem() {
    TelaCombateRaycaster::notificarCancelamentoItem();
}

void CombateRaycasterUIImpl::notificarRequisitoNaoAtendido(const std::string& mensagemRequisito) {
    TelaCombateRaycaster::notificarRequisitoNaoAtendido(mensagemRequisito);
}

void CombateRaycasterUIImpl::exibirTelaVitoria(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate, const std::vector<std::string>& itensObtidos, const std::vector<std::string>& inimigosDerrotados, int parriesPerfeitos, int maiorDano) {
    std::unordered_map<std::string, int> frequenciaDrops;
    for (const auto& item : itensObtidos) {
        frequenciaDrops[item]++;
    }
    std::vector<std::pair<std::string, int>> dropsUnicos;
    for (const auto& par : frequenciaDrops) {
        dropsUnicos.push_back(par);
    }

    bool podeSubirNivel = jogadorAtual->obterXpAtual() + quantidadeDeXpObtido >= jogadorAtual->obterXpParaSubir();

    TelaVitoriaRaycaster::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, curaTotalRecebida, turnosCombate, inimigosDerrotados, parriesPerfeitos, maiorDano, dropsUnicos, podeSubirNivel, {}, "");
}

void CombateRaycasterUIImpl::exibirTelaDerrota(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate) {
    TelaDerrotaRaycaster::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, curaTotalRecebida, turnosCombate);
}

void CombateRaycasterUIImpl::exibirTelaAtributos(Personagem* personagem) {
    Aparencia::limparTela();
    TelaAtributos::gerenciarFichaDoJogador(personagem);
    Aparencia::limparTela();
}

void CombateRaycasterUIImpl::exibirTelaDiario(Personagem* personagem) {
    Aparencia::limparTela();
    TelaDiario::exibir(personagem);
    Aparencia::limparTela();
}

void CombateRaycasterUIImpl::limparTela() {
    Aparencia::limparTela();
}
