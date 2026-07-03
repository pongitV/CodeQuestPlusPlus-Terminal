#include "TelaCombate.h"
#include "../../GerenciadorPerspectiva.h"
#include "../../PerspectivaIDE/TelasIDE/TelaCombateIDE.h"
#include "../../PerspectivaRaycaster/TelasRaycaster/TelaCombateRaycaster.h"
#include "../../../Entidades/Personagem.h"
#include "../../../Core/Utilidades/Aparencia.h"

// Static data member definitions
bool TelaCombate::isModo3D = false;
bool TelaCombate::isTerminalView = false;
std::vector<std::string> TelaCombate::matrizDoMapaAtual;
float TelaCombate::jogadorPosX = 0.0f;
float TelaCombate::jogadorPosY = 0.0f;
float TelaCombate::jogadorAngulo = 0.0f;
std::string TelaCombate::tituloMapaAtual = "";

int TelaCombate::turnoAtualVisivel = 1;
std::string TelaCombate::nomeTurnoVisivel = "";
int TelaCombate::selecaoAcaoAtual = -1;
int TelaCombate::selecaoAlvoAtual = -1;
bool TelaCombate::piscarSelecao = true;
Personagem* TelaCombate::g_inimigoMortoComDrops = nullptr;
std::vector<std::string> TelaCombate::g_dropsAtivos = {};
std::vector<std::string> TelaCombate::opcoesMenuAtual = {};
Personagem* TelaCombate::personagemHUD = nullptr;

void TelaCombate::exibirLogoParaTelaDeCombate(const std::string& tituloDaTela, bool animar) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::exibirLogoParaTelaDeCombate(tituloDaTela, animar);
    } else {
        TelaCombateIDE::exibirLogoParaTelaDeCombate(tituloDaTela, animar);
    }
}

void TelaCombate::animarIntroducaoCombate(const std::string& titulo, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::animarIntroducaoCombate(titulo, inimigos, jogadorAtual);
    } else {
        TelaCombateIDE::animarIntroducaoCombate(titulo, inimigos, jogadorAtual);
    }
}

std::vector<std::string> TelaCombate::obterLinhasBarraDeStatusDoJogador(Personagem* jogadorAtual, Cor corDestaque, int danoAnimacao, int frameAnimacao, bool isCura) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        return TelaCombateRaycaster::obterLinhasBarraDeStatusDoJogador(jogadorAtual, corDestaque, danoAnimacao, frameAnimacao, isCura);
    } else {
        return TelaCombateIDE::obterLinhasBarraDeStatusDoJogador(jogadorAtual, corDestaque, danoAnimacao, frameAnimacao, isCura);
    }
}

void TelaCombate::exibirHordaDeInimigosLadoALado(const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, int frameAnimacao, bool isCura, bool animarSurgimento, bool isMorte, Item* armaAtacante, int danoAnimacao, const std::vector<std::string>& dropsAnimacao) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::exibirHordaDeInimigosLadoALado(listaDeInimigos, alvoAnimacao, frameAnimacao, isCura, animarSurgimento, isMorte, armaAtacante, danoAnimacao, dropsAnimacao);
    } else {
        TelaCombateIDE::exibirHordaDeInimigosLadoALado(listaDeInimigos, alvoAnimacao, frameAnimacao, isCura, animarSurgimento, isMorte, armaAtacante, danoAnimacao, dropsAnimacao);
    }
}

void TelaCombate::animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* atacante, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int danoAnimacao) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::animarDanoNoInimigo(tituloCombate, listaDeInimigos, alvoAnimacao, atacante, jogadorAtual, listaDeAliados, danoAnimacao);
    } else {
        TelaCombateIDE::animarDanoNoInimigo(tituloCombate, listaDeInimigos, alvoAnimacao, atacante, jogadorAtual, listaDeAliados, danoAnimacao);
    }
}

void TelaCombate::animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int curaAnimacao) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::animarCuraNoInimigo(tituloCombate, listaDeInimigos, alvoAnimacao, jogadorAtual, listaDeAliados, curaAnimacao);
    } else {
        TelaCombateIDE::animarCuraNoInimigo(tituloCombate, listaDeInimigos, alvoAnimacao, jogadorAtual, listaDeAliados, curaAnimacao);
    }
}

void TelaCombate::animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, bool isParry, int danoAnimacao) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::animarDanoNoJogador(tituloCombate, listaDeInimigos, alvoAnimacao, jogadorAtual, listaDeAliados, isParry, danoAnimacao);
    } else {
        TelaCombateIDE::animarDanoNoJogador(tituloCombate, listaDeInimigos, alvoAnimacao, jogadorAtual, listaDeAliados, isParry, danoAnimacao);
    }
}

void TelaCombate::animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int curaAnimacao) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::animarCuraNoJogador(tituloCombate, listaDeInimigos, alvoAnimacao, jogadorAtual, listaDeAliados, curaAnimacao);
    } else {
        TelaCombateIDE::animarCuraNoJogador(tituloCombate, listaDeInimigos, alvoAnimacao, jogadorAtual, listaDeAliados, curaAnimacao);
    }
}

void TelaCombate::animarMorteInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* inimigoMorto, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, const std::vector<std::string>& drops) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::animarMorteInimigo(tituloCombate, listaDeInimigos, inimigoMorto, jogadorAtual, listaDeAliados, drops);
    } else {
        TelaCombateIDE::animarMorteInimigo(tituloCombate, listaDeInimigos, inimigoMorto, jogadorAtual, listaDeAliados, drops);
    }
}

void TelaCombate::atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, bool animarEntrada) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::atualizarTelaEstatica(tituloCombate, listaDeInimigos, jogadorAtual, listaDeAliados, animarEntrada);
    } else {
        TelaCombateIDE::atualizarTelaEstatica(tituloCombate, listaDeInimigos, jogadorAtual, listaDeAliados, animarEntrada);
    }
}

void TelaCombate::adicionarMensagemFixa(const std::string& msg) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::adicionarMensagemFixa(msg);
    } else {
        TelaCombateIDE::adicionarMensagemFixa(msg);
    }
}

void TelaCombate::limparMensagensFixas() {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::limparMensagensFixas();
    } else {
        TelaCombateIDE::limparMensagensFixas();
    }
}

void TelaCombate::configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) {
    isModo3D = modo3D;
    matrizDoMapaAtual = matriz;
    jogadorPosX = posX;
    jogadorPosY = posY;
    jogadorAngulo = angulo;
    tituloMapaAtual = titulo;
}

void TelaCombate::selecionarHUDDeAliado(Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::selecionarHUDDeAliado(jogadorAtual, aliados);
    } else {
        TelaCombateIDE::selecionarHUDDeAliado(jogadorAtual, aliados);
    }
}

void TelaCombate::definirTurnoVisivel(int turno, const std::string& nome) {
    turnoAtualVisivel = turno;
    nomeTurnoVisivel = nome;
}

int TelaCombate::obterAcaoDoJogador(int turnoAtual, Personagem* personagemAgindo, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        return TelaCombateRaycaster::obterAcaoDoJogador(turnoAtual, personagemAgindo, inimigos, jogadorAtual, aliados);
    } else {
        return TelaCombateIDE::obterAcaoDoJogador(turnoAtual, personagemAgindo, inimigos, jogadorAtual, aliados);
    }
}

int TelaCombate::obterAlvoAtaque(const std::string& tituloCombate, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        return TelaCombateRaycaster::obterAlvoAtaque(tituloCombate, inimigos, jogadorAtual, aliados);
    } else {
        return TelaCombateIDE::obterAlvoAtaque(tituloCombate, inimigos, jogadorAtual, aliados);
    }
}

int TelaCombate::obterAlvoItem(const std::string& tituloCombate, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        return TelaCombateRaycaster::obterAlvoItem(tituloCombate, inimigos, jogadorAtual, aliados);
    } else {
        return TelaCombateIDE::obterAlvoItem(tituloCombate, inimigos, jogadorAtual, aliados);
    }
}

int TelaCombate::obterEscolhaDeEscudo(const std::string& nomePersonagem, const std::vector<Item*>& listaDeEscudos) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        return TelaCombateRaycaster::obterEscolhaDeEscudo(nomePersonagem, listaDeEscudos);
    } else {
        return TelaCombateIDE::obterEscolhaDeEscudo(nomePersonagem, listaDeEscudos);
    }
}

void TelaCombate::notificarInimigosMaisAgeis() {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::notificarInimigosMaisAgeis();
    } else {
        TelaCombateIDE::notificarInimigosMaisAgeis();
    }
}

void TelaCombate::notificarTurnoExtra(int destrezaJogador, int maxDestrezaInimigos) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::notificarTurnoExtra(destrezaJogador, maxDestrezaInimigos);
    } else {
        TelaCombateIDE::notificarTurnoExtra(destrezaJogador, maxDestrezaInimigos);
    }
}

void TelaCombate::notificarDesprevencaoInventario() {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::notificarDesprevencaoInventario();
    } else {
        TelaCombateIDE::notificarDesprevencaoInventario();
    }
}

void TelaCombate::notificarSemEscudos(const std::string& nomePersonagem) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::notificarSemEscudos(nomePersonagem);
    } else {
        TelaCombateIDE::notificarSemEscudos(nomePersonagem);
    }
}

void TelaCombate::notificarDesequilibrioDefesa(const std::string& nomePersonagem) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::notificarDesequilibrioDefesa(nomePersonagem);
    } else {
        TelaCombateIDE::notificarDesequilibrioDefesa(nomePersonagem);
    }
}

void TelaCombate::notificarPosturaDefensiva(const std::string& nomePersonagem, const std::string& nomeEscudo) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::notificarPosturaDefensiva(nomePersonagem, nomeEscudo);
    } else {
        TelaCombateIDE::notificarPosturaDefensiva(nomePersonagem, nomeEscudo);
    }
}

void TelaCombate::notificarAcaoInvalida() {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::notificarAcaoInvalida();
    } else {
        TelaCombateIDE::notificarAcaoInvalida();
    }
}

void TelaCombate::notificarCancelamentoItem() {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::notificarCancelamentoItem();
    } else {
        TelaCombateIDE::notificarCancelamentoItem();
    }
}

void TelaCombate::notificarRequisitoNaoAtendido(const std::string& mensagemRequisito) {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaCombateRaycaster::notificarRequisitoNaoAtendido(mensagemRequisito);
    } else {
        TelaCombateIDE::notificarRequisitoNaoAtendido(mensagemRequisito);
    }
}

std::string TelaCombate::margemCombate() {
    int larguraHUD = 91;
    int larguraTerminal = Aparencia::obterLarguraTerminal();
    int larguraRef = std::min(larguraHUD, larguraTerminal);
    return Aparencia::espacosParaCentralizar(larguraRef);
}
