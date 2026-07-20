#include "GerenciadorPerspectiva.h"
#include "PerspectivaRaycaster/EngineRaycaster/Raycaster.h"
#include "PerspectivaRaycaster/EngineRaycaster/RaycasterRenderer.h"
#include "../Core/Utilidades/RendererProvider.h"
#include "../Core/Utilidades/Aparencia.h"
#include "PerspectivaRaycaster/GerenciadorTelasRaycaster.h"
#include "PerspectivaRaycaster/TelasRaycaster/Diario/TelaDiarioRaycaster.h"
#include "PerspectivaRaycaster/TelasRaycaster/Inventario/TelaInventarioRaycaster.h"
#include "PerspectivaRaycaster/TelasRaycaster/Atributos/TelaAtributosRaycaster.h"
#include "PerspectivaRaycaster/TelasRaycaster/Bestiario/TelaBestiarioRaycaster.h"
#include "PerspectivaRaycaster/TelasRaycaster/Combate/TelaCombateRaycaster.h"
#include "PerspectivaRaycaster/TelasRaycaster/Derrota/TelaDerrotaRaycaster.h"
#include "PerspectivaRaycaster/TelasRaycaster/Vitoria/TelaVitoriaRaycaster.h"
#include "PerspectivaRaycaster/TelasRaycaster/Pause/TelaPauseRaycaster.h"
#include "PerspectivaRaycaster/TelasRaycaster/Mapa/TelaMapaMundoRaycaster.h"

// ─── UI Adapters ────────────────────────────────────────────────────────────
// Lightweight wrappers that implement the abstract UI interfaces by delegating
// to the concrete static methods of the Raycaster (or IDE) renderers.
// These keep concrete dependencies confined to the composition root.

class AtributosUIAdapter : public IAtributosUI {
    void exibir(Personagem* jogador) override { TelaAtributosRaycaster::exibir(jogador); }
    void exibirDetalhesAtributos(Personagem* jogadorAtual) override { TelaAtributosRaycaster::exibirDetalhesAtributos(jogadorAtual); }
    void gerenciarFichaDoJogador(Personagem* jogadorAtual) override { TelaAtributosRaycaster::gerenciarFichaDoJogador(jogadorAtual); }
};

class BestiarioUIAdapter : public IBestiarioUI {
    void exibir(const std::vector<Personagem*>& inimigos) override { TelaBestiarioRaycaster::exibir(inimigos); }
    void exibirDetalhe(Personagem* inimigo) override { TelaBestiarioRaycaster::exibirDetalhe(inimigo); }
};

class TelaCombateUIAdapter : public ITelaCombateUI {
    void exibirLogoParaTelaDeCombate(const std::string& tituloDaTela, bool animar) override { TelaCombateRaycaster::exibirLogoParaTelaDeCombate(tituloDaTela, animar); }
    void animarIntroducaoCombate(const std::string& titulo, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual) override { TelaCombateRaycaster::animarIntroducaoCombate(titulo, inimigos, jogadorAtual); }
    std::vector<std::string> obterLinhasBarraDeStatusDoJogador(Personagem* jogadorAtual, Cor corDestaque, int danoAnimacao, int frameAnimacao, bool isCura) override { return TelaCombateRaycaster::obterLinhasBarraDeStatusDoJogador(jogadorAtual, corDestaque, danoAnimacao, frameAnimacao, isCura); }
    void exibirHordaDeInimigosLadoALado(const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, int frameAnimacao, bool isCura, bool animarSurgimento, bool isMorte, Item* armaAtacante, int danoAnimacao, const std::vector<std::string>& dropsAnimacao) override { TelaCombateRaycaster::exibirHordaDeInimigosLadoALado(listaDeInimigos, alvoAnimacao, frameAnimacao, isCura, animarSurgimento, isMorte, armaAtacante, danoAnimacao, dropsAnimacao); }
    void animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* atacante, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int danoAnimacao) override { TelaCombateRaycaster::animarDanoNoInimigo(tituloCombate, listaDeInimigos, alvoAnimacao, atacante, jogadorAtual, listaDeAliados, danoAnimacao); }
    void animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int curaAnimacao) override { TelaCombateRaycaster::animarCuraNoInimigo(tituloCombate, listaDeInimigos, alvoAnimacao, jogadorAtual, listaDeAliados, curaAnimacao); }
    void animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, bool isParry, int danoAnimacao) override { TelaCombateRaycaster::animarDanoNoJogador(tituloCombate, listaDeInimigos, alvoAnimacao, jogadorAtual, listaDeAliados, isParry, danoAnimacao); }
    void animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int curaAnimacao) override { TelaCombateRaycaster::animarCuraNoJogador(tituloCombate, listaDeInimigos, alvoAnimacao, jogadorAtual, listaDeAliados, curaAnimacao); }
    void animarMorteInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* inimigoMorto, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, const std::vector<std::string>& drops) override { TelaCombateRaycaster::animarMorteInimigo(tituloCombate, listaDeInimigos, inimigoMorto, jogadorAtual, listaDeAliados, drops); }
    void atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, bool animarEntrada, std::function<void(std::vector<std::string>&)> callbackOverlay) override { TelaCombateRaycaster::atualizarTelaEstatica(tituloCombate, listaDeInimigos, jogadorAtual, listaDeAliados, animarEntrada, callbackOverlay); }
    void adicionarMensagemFixa(const std::string& msg) override { TelaCombateRaycaster::adicionarMensagemFixa(msg); }
    void limparMensagensFixas() override { TelaCombateRaycaster::limparMensagensFixas(); }
    void configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) override { TelaCombateRaycaster::configurarContexto3D(modo3D, matriz, posX, posY, angulo, titulo); }
    void definirTurnoVisivel(int turno, const std::string& nome) override { TelaCombateRaycaster::definirTurnoVisivel(turno, nome); }
    void selecionarHUDDeAliado(Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) override { TelaCombateRaycaster::selecionarHUDDeAliado(jogadorAtual, aliados); }
    int obterAcaoDoJogador(int turnoAtual, Personagem* personagemAgindo, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) override { return TelaCombateRaycaster::obterAcaoDoJogador(turnoAtual, personagemAgindo, inimigos, jogadorAtual, aliados); }
    int obterAlvoAtaque(const std::string& tituloCombate, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) override { return TelaCombateRaycaster::obterAlvoAtaque(tituloCombate, inimigos, jogadorAtual, aliados); }
    int obterAlvoItem(const std::string& tituloCombate, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) override { return TelaCombateRaycaster::obterAlvoItem(tituloCombate, inimigos, jogadorAtual, aliados); }
    int obterEscolhaDeEscudo(const std::string& nomePersonagem, const std::vector<Item*>& listaDeEscudos) override { return TelaCombateRaycaster::obterEscolhaDeEscudo(nomePersonagem, listaDeEscudos); }
    void notificarInimigosMaisAgeis() override { TelaCombateRaycaster::notificarInimigosMaisAgeis(); }
    void notificarTurnoExtra(int destrezaJogador, int maxDestrezaInimigos) override { TelaCombateRaycaster::notificarTurnoExtra(destrezaJogador, maxDestrezaInimigos); }
    void notificarDesprevencaoInventario() override { TelaCombateRaycaster::notificarDesprevencaoInventario(); }
    void notificarSemEscudos(const std::string& nomePersonagem) override { TelaCombateRaycaster::notificarSemEscudos(nomePersonagem); }
    void notificarDesequilibrioDefesa(const std::string& nomePersonagem) override { TelaCombateRaycaster::notificarDesequilibrioDefesa(nomePersonagem); }
    void notificarPosturaDefensiva(const std::string& nomePersonagem, const std::string& nomeEscudo) override { TelaCombateRaycaster::notificarPosturaDefensiva(nomePersonagem, nomeEscudo); }
    void notificarAcaoInvalida() override { TelaCombateRaycaster::notificarAcaoInvalida(); }
    void notificarCancelamentoItem() override { TelaCombateRaycaster::notificarCancelamentoItem(); }
    void notificarRequisitoNaoAtendido(const std::string& mensagemRequisito) override { TelaCombateRaycaster::notificarRequisitoNaoAtendido(mensagemRequisito); }
};

class DerrotaUIAdapter : public IDerrotaUI {
    void exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate) override { TelaDerrotaRaycaster::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, curaTotalRecebida, turnosCombate); }
};

class VitoriaUIAdapter : public IVitoriaUI {
    void exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate, const std::vector<std::string>& inimigosDerrotados, int parriesPerfeitos, int maiorDano, int parriesTentados, int parriesEfetivos, int itensConsumidos, const std::vector<std::pair<std::string, int>>& dropsUnicos, bool podeSubirNivel, const std::vector<std::string>& novasDescobertas, const std::string& tituloMapa) override { TelaVitoriaRaycaster::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, curaTotalRecebida, turnosCombate, inimigosDerrotados, parriesPerfeitos, maiorDano, parriesTentados, parriesEfetivos, itensConsumidos, dropsUnicos, podeSubirNivel, novasDescobertas, tituloMapa); }
};

class PauseUIAdapter : public IPauseUI {
    int renderizarMenuPause() override { return TelaPauseRaycaster::renderizarMenuPause(); }
    int renderizarMenuConfiguracoes(Personagem* jogador) override { return TelaPauseRaycaster::renderizarMenuConfiguracoes(jogador); }
    int renderizarMenuAparencia(Personagem* jogador) override { return TelaPauseRaycaster::renderizarMenuAparencia(jogador); }
    int renderizarMenuFundo(int corFundoAtualIndex) override { return TelaPauseRaycaster::renderizarMenuFundo(corFundoAtualIndex); }
    int renderizarMenuSensibilidade(int percX, int percY) override { return TelaPauseRaycaster::renderizarMenuSensibilidade(percX, percY); }
};

class MapaMundoUIAdapter : public IMapaMundoUI {
    void renderizarPopup(const std::vector<std::string>& arte, const std::vector<std::string>& lugares, int selecao, bool redesenhoCompleto) override { TelaMapaMundoRaycaster::renderizarPopup(arte, lugares, selecao, redesenhoCompleto); }
};

GerenciadorPerspectiva::GerenciadorPerspectiva() : m_visao3DAtiva(true) {
}

void GerenciadorPerspectiva::inicializar() {
    m_renderer3D = std::make_unique<RaycasterRenderer>();
    m_telas3D = std::make_unique<GerenciadorTelasRaycaster>();
    m_visao3DAtiva = true;
    RendererProvider::set(m_renderer3D.get());
}

void GerenciadorPerspectiva::alternarVisao() {
    Aparencia::exibirPopup(
        "PERSPECTIVA IDE",
        {"A perspective IDE esta em construcao!",
         "",
         "Em breve voce podera explorar o jogo",
         "no estilo de um terminal de programacao.",
         "Por enquanto, apenas a visao 3D esta disponivel."},
        Cor::AMARELO
    );
}

bool GerenciadorPerspectiva::isVisao3DAtiva() const {
    return true;
}

RenderizadorPerspectiva* GerenciadorPerspectiva::obterRendererAtivo() const {
    return m_renderer3D.get();
}

IGerenciadorTelas* GerenciadorPerspectiva::obterGerenciadorTelas() const {
    return m_telas3D.get();
}

float GerenciadorPerspectiva::obterSensibilidadeMouseX() {
    return Raycaster::sensibilidadeX;
}

float GerenciadorPerspectiva::obterSensibilidadeMouseY() {
    return Raycaster::sensibilidadeY;
}

void GerenciadorPerspectiva::definirSensibilidadeMouse(float x, float y) {
    Raycaster::sensibilidadeX = x;
    Raycaster::sensibilidadeY = y;
}

IDiarioUI& GerenciadorPerspectiva::obterDiarioUI() {
    static TelaDiarioRaycaster diarioUI;
    return diarioUI;
}

IInventarioUI& GerenciadorPerspectiva::obterInventarioUI() {
    static TelaInventarioRaycaster inventarioUI;
    return inventarioUI;
}

IAtributosUI& GerenciadorPerspectiva::obterAtributosUI() {
    static AtributosUIAdapter adapter;
    return adapter;
}

IBestiarioUI& GerenciadorPerspectiva::obterBestiarioUI() {
    static BestiarioUIAdapter adapter;
    return adapter;
}

ITelaCombateUI& GerenciadorPerspectiva::obterTelaCombateUI() {
    static TelaCombateUIAdapter adapter;
    return adapter;
}

IDerrotaUI& GerenciadorPerspectiva::obterDerrotaUI() {
    static DerrotaUIAdapter adapter;
    return adapter;
}

IVitoriaUI& GerenciadorPerspectiva::obterVitoriaUI() {
    static VitoriaUIAdapter adapter;
    return adapter;
}

IPauseUI& GerenciadorPerspectiva::obterPauseUI() {
    static PauseUIAdapter adapter;
    return adapter;
}

IMapaMundoUI& GerenciadorPerspectiva::obterMapaMundoUI() {
    static MapaMundoUIAdapter adapter;
    return adapter;
}
