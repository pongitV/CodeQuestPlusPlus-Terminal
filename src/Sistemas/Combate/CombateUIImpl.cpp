#include "CombateUIImpl.h"

#include "../../Perspectiva/TelasBase/Combate/TelaCombate.h"
#include "../../Perspectiva/TelasBase/Vitoria/TelaVitoria.h"
#include "../../Perspectiva/TelasBase/Derrota/TelaDerrota.h"
#include "../../Perspectiva/TelasBase/Atributos/TelaAtributos.h"
#include "../../Perspectiva/TelasBase/Diario/TelaDiario.h"
#include "../../Core/Utilidades/Aparencia.h"

void CombateUIImpl::configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) {
    TelaCombate::configurarContexto3D(modo3D, matriz, posX, posY, angulo, titulo);
}

void CombateUIImpl::animarIntroducaoCombate(const std::string& titulo, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual) {
    TelaCombate::animarIntroducaoCombate(titulo, inimigos, jogadorAtual);
}

void CombateUIImpl::atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, bool animarEntrada) {
    TelaCombate::atualizarTelaEstatica(tituloCombate, listaDeInimigos, jogadorAtual, listaDeAliados, animarEntrada);
}

void CombateUIImpl::animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* atacante, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int danoAnimacao) {
    TelaCombate::animarDanoNoInimigo(tituloCombate, listaDeInimigos, alvoAnimacao, atacante, jogadorAtual, listaDeAliados, danoAnimacao);
}

void CombateUIImpl::animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int curaAnimacao) {
    TelaCombate::animarCuraNoInimigo(tituloCombate, listaDeInimigos, alvoAnimacao, jogadorAtual, listaDeAliados, curaAnimacao);
}

void CombateUIImpl::animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, bool isParry, int danoAnimacao) {
    TelaCombate::animarDanoNoJogador(tituloCombate, listaDeInimigos, alvoAnimacao, jogadorAtual, listaDeAliados, isParry, danoAnimacao);
}

void CombateUIImpl::animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int curaAnimacao) {
    TelaCombate::animarCuraNoJogador(tituloCombate, listaDeInimigos, alvoAnimacao, jogadorAtual, listaDeAliados, curaAnimacao);
}

void CombateUIImpl::animarMorteInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* inimigoMorto, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, const std::vector<std::string>& drops) {
    TelaCombate::animarMorteInimigo(tituloCombate, listaDeInimigos, inimigoMorto, jogadorAtual, listaDeAliados, drops);
}

void CombateUIImpl::limparContextoPersonagemHUD() {
    TelaCombate::contexto.personagemHUD = nullptr;
}

void CombateUIImpl::limparContextoInimigoMortoEDrops() {
    TelaCombate::contexto.inimigoMortoComDrops = nullptr;
    TelaCombate::contexto.dropsAtivos.clear();
}

std::string CombateUIImpl::margemCombate() {
    return TelaCombate::margemCombate();
}

void CombateUIImpl::adicionarMensagemFixa(const std::string& msg) {
    TelaCombate::adicionarMensagemFixa(msg);
}

void CombateUIImpl::limparMensagensFixas() {
    TelaCombate::limparMensagensFixas();
}

void CombateUIImpl::definirTurnoVisivel(int turno, const std::string& nome) {
    TelaCombate::definirTurnoVisivel(turno, nome);
}

int CombateUIImpl::obterAcaoDoJogador(int turnoAtual, Personagem* personagemAgindo, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {
    return TelaCombate::obterAcaoDoJogador(turnoAtual, personagemAgindo, inimigos, jogadorAtual, aliados);
}

int CombateUIImpl::obterAlvoAtaque(const std::string& tituloCombate, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {
    return TelaCombate::obterAlvoAtaque(tituloCombate, inimigos, jogadorAtual, aliados);
}

int CombateUIImpl::obterAlvoItem(const std::string& tituloCombate, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {
    return TelaCombate::obterAlvoItem(tituloCombate, inimigos, jogadorAtual, aliados);
}

int CombateUIImpl::obterEscolhaDeEscudo(const std::string& nomePersonagem, const std::vector<Item*>& listaDeEscudos) {
    return TelaCombate::obterEscolhaDeEscudo(nomePersonagem, listaDeEscudos);
}

void CombateUIImpl::notificarInimigosMaisAgeis() {
    TelaCombate::notificarInimigosMaisAgeis();
}

void CombateUIImpl::notificarTurnoExtra(int destrezaJogador, int maxDestrezaInimigos) {
    TelaCombate::notificarTurnoExtra(destrezaJogador, maxDestrezaInimigos);
}

void CombateUIImpl::notificarDesprevencaoInventario() {
    TelaCombate::notificarDesprevencaoInventario();
}

void CombateUIImpl::notificarSemEscudos(const std::string& nomePersonagem) {
    TelaCombate::notificarSemEscudos(nomePersonagem);
}

void CombateUIImpl::notificarDesequilibrioDefesa(const std::string& nomePersonagem) {
    TelaCombate::notificarDesequilibrioDefesa(nomePersonagem);
}

void CombateUIImpl::notificarPosturaDefensiva(const std::string& nomePersonagem, const std::string& nomeEscudo) {
    TelaCombate::notificarPosturaDefensiva(nomePersonagem, nomeEscudo);
}

void CombateUIImpl::notificarAcaoInvalida() {
    TelaCombate::notificarAcaoInvalida();
}

void CombateUIImpl::notificarCancelamentoItem() {
    TelaCombate::notificarCancelamentoItem();
}

void CombateUIImpl::notificarRequisitoNaoAtendido(const std::string& mensagemRequisito) {
    TelaCombate::notificarRequisitoNaoAtendido(mensagemRequisito);
}

void CombateUIImpl::exibirTelaVitoria(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate, const std::vector<std::string>& itensObtidos, const std::vector<std::string>& inimigosDerrotados, int parriesPerfeitos, int maiorDano) {
    TelaVitoria::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, curaTotalRecebida, turnosCombate, itensObtidos, inimigosDerrotados, parriesPerfeitos, maiorDano);
}

void CombateUIImpl::exibirTelaDerrota(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate) {
    TelaDerrota::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, curaTotalRecebida, turnosCombate);
}

void CombateUIImpl::exibirTelaAtributos(Personagem* personagem) {
    TelaAtributos::gerenciarFichaDoJogador(personagem);
}

void CombateUIImpl::exibirTelaDiario(Personagem* personagem) {
    TelaDiario::exibir(personagem);
}

void CombateUIImpl::limparTela() {
    Aparencia::limparTela();
}
