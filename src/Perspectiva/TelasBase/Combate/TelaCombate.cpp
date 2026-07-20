#include "TelaCombate.h"
#include "../../GerenciadorPerspectiva.h"

#include "../../../Entidades/Personagem.h"
#include "../../../Core/Utilidades/Aparencia.h"

ContextoCombate TelaCombate::contexto;

void TelaCombate::exibirLogoParaTelaDeCombate(const std::string& tituloDaTela, bool animar) {
    GerenciadorPerspectiva::obterTelaCombateUI().exibirLogoParaTelaDeCombate(tituloDaTela, animar);
}

void TelaCombate::animarIntroducaoCombate(const std::string& titulo, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual) {
    GerenciadorPerspectiva::obterTelaCombateUI().animarIntroducaoCombate(titulo, inimigos, jogadorAtual);
}

std::vector<std::string> TelaCombate::obterLinhasBarraDeStatusDoJogador(Personagem* jogadorAtual, Cor corDestaque, int danoAnimacao, int frameAnimacao, bool isCura) {
    return GerenciadorPerspectiva::obterTelaCombateUI().obterLinhasBarraDeStatusDoJogador(jogadorAtual, corDestaque, danoAnimacao, frameAnimacao, isCura);
}

void TelaCombate::exibirHordaDeInimigosLadoALado(const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, int frameAnimacao, bool isCura, bool animarSurgimento, bool isMorte, Item* armaAtacante, int danoAnimacao, const std::vector<std::string>& dropsAnimacao) {
    GerenciadorPerspectiva::obterTelaCombateUI().exibirHordaDeInimigosLadoALado(listaDeInimigos, alvoAnimacao, frameAnimacao, isCura, animarSurgimento, isMorte, armaAtacante, danoAnimacao, dropsAnimacao);
}

void TelaCombate::animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* atacante, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int danoAnimacao) {
    GerenciadorPerspectiva::obterTelaCombateUI().animarDanoNoInimigo(tituloCombate, listaDeInimigos, alvoAnimacao, atacante, jogadorAtual, listaDeAliados, danoAnimacao);
}

void TelaCombate::animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int curaAnimacao) {
    GerenciadorPerspectiva::obterTelaCombateUI().animarCuraNoInimigo(tituloCombate, listaDeInimigos, alvoAnimacao, jogadorAtual, listaDeAliados, curaAnimacao);
}

void TelaCombate::animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, bool isParry, int danoAnimacao) {
    GerenciadorPerspectiva::obterTelaCombateUI().animarDanoNoJogador(tituloCombate, listaDeInimigos, alvoAnimacao, jogadorAtual, listaDeAliados, isParry, danoAnimacao);
}

void TelaCombate::animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int curaAnimacao) {
    GerenciadorPerspectiva::obterTelaCombateUI().animarCuraNoJogador(tituloCombate, listaDeInimigos, alvoAnimacao, jogadorAtual, listaDeAliados, curaAnimacao);
}

void TelaCombate::animarMorteInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* inimigoMorto, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, const std::vector<std::string>& drops) {
    GerenciadorPerspectiva::obterTelaCombateUI().animarMorteInimigo(tituloCombate, listaDeInimigos, inimigoMorto, jogadorAtual, listaDeAliados, drops);
}

void TelaCombate::atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, bool animarEntrada) {
    GerenciadorPerspectiva::obterTelaCombateUI().atualizarTelaEstatica(tituloCombate, listaDeInimigos, jogadorAtual, listaDeAliados, animarEntrada);
}

void TelaCombate::adicionarMensagemFixa(const std::string& msg) {
    GerenciadorPerspectiva::obterTelaCombateUI().adicionarMensagemFixa(msg);
}

void TelaCombate::limparMensagensFixas() {
    GerenciadorPerspectiva::obterTelaCombateUI().limparMensagensFixas();
}

void TelaCombate::configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) {
    contexto.isModo3D = modo3D;
    contexto.matrizDoMapaAtual = matriz;
    contexto.jogadorPosX = posX;
    contexto.jogadorPosY = posY;
    contexto.jogadorAngulo = angulo;
    contexto.tituloMapaAtual = titulo;
}

void TelaCombate::selecionarHUDDeAliado(Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {
    GerenciadorPerspectiva::obterTelaCombateUI().selecionarHUDDeAliado(jogadorAtual, aliados);
}

void TelaCombate::definirTurnoVisivel(int turno, const std::string& nome) {
    contexto.turnoAtualVisivel = turno;
    contexto.nomeTurnoVisivel = nome;
}

int TelaCombate::obterAcaoDoJogador(int turnoAtual, Personagem* personagemAgindo, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {
    return GerenciadorPerspectiva::obterTelaCombateUI().obterAcaoDoJogador(turnoAtual, personagemAgindo, inimigos, jogadorAtual, aliados);
}

int TelaCombate::obterAlvoAtaque(const std::string& tituloCombate, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {
    return GerenciadorPerspectiva::obterTelaCombateUI().obterAlvoAtaque(tituloCombate, inimigos, jogadorAtual, aliados);
}

int TelaCombate::obterAlvoItem(const std::string& tituloCombate, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {
    return GerenciadorPerspectiva::obterTelaCombateUI().obterAlvoItem(tituloCombate, inimigos, jogadorAtual, aliados);
}

int TelaCombate::obterEscolhaDeEscudo(const std::string& nomePersonagem, const std::vector<Item*>& listaDeEscudos) {
    return GerenciadorPerspectiva::obterTelaCombateUI().obterEscolhaDeEscudo(nomePersonagem, listaDeEscudos);
}

void TelaCombate::notificarInimigosMaisAgeis() {
    GerenciadorPerspectiva::obterTelaCombateUI().notificarInimigosMaisAgeis();
}

void TelaCombate::notificarTurnoExtra(int destrezaJogador, int maxDestrezaInimigos) {
    GerenciadorPerspectiva::obterTelaCombateUI().notificarTurnoExtra(destrezaJogador, maxDestrezaInimigos);
}

void TelaCombate::notificarDesprevencaoInventario() {
    GerenciadorPerspectiva::obterTelaCombateUI().notificarDesprevencaoInventario();
}

void TelaCombate::notificarSemEscudos(const std::string& nomePersonagem) {
    GerenciadorPerspectiva::obterTelaCombateUI().notificarSemEscudos(nomePersonagem);
}

void TelaCombate::notificarDesequilibrioDefesa(const std::string& nomePersonagem) {
    GerenciadorPerspectiva::obterTelaCombateUI().notificarDesequilibrioDefesa(nomePersonagem);
}

void TelaCombate::notificarPosturaDefensiva(const std::string& nomePersonagem, const std::string& nomeEscudo) {
    GerenciadorPerspectiva::obterTelaCombateUI().notificarPosturaDefensiva(nomePersonagem, nomeEscudo);
}

void TelaCombate::notificarAcaoInvalida() {
    GerenciadorPerspectiva::obterTelaCombateUI().notificarAcaoInvalida();
}

void TelaCombate::notificarCancelamentoItem() {
    GerenciadorPerspectiva::obterTelaCombateUI().notificarCancelamentoItem();
}

void TelaCombate::notificarRequisitoNaoAtendido(const std::string& mensagemRequisito) {
    GerenciadorPerspectiva::obterTelaCombateUI().notificarRequisitoNaoAtendido(mensagemRequisito);
}

std::string TelaCombate::margemCombate() {
    int larguraHUD = 91;
    int larguraTerminal = Aparencia::obterLarguraTerminal();
    int larguraRef = std::min(larguraHUD, larguraTerminal);
    return Aparencia::espacosParaCentralizar(larguraRef);
}
