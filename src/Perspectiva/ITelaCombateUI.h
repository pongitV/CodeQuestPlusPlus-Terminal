#pragma once
#include <string>
#include <vector>
#include <functional>
#include "../Entidades/Personagem.h"
#include "../Sistemas/Inventario/Item.h"

class ITelaCombateUI {
public:
    virtual ~ITelaCombateUI() = default;

    virtual void exibirLogoParaTelaDeCombate(const std::string& tituloDaTela = "", bool animar = true) = 0;
    virtual void animarIntroducaoCombate(const std::string& titulo, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual = nullptr) = 0;
    virtual std::vector<std::string> obterLinhasBarraDeStatusDoJogador(Personagem* jogadorAtual, Cor corDestaque = Cor::RESET, int danoAnimacao = -1, int frameAnimacao = 0, bool isCura = false) = 0;
    virtual void exibirHordaDeInimigosLadoALado(const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao = nullptr, int frameAnimacao = 0, bool isCura = false, bool animarSurgimento = false, bool isMorte = false, Item* armaAtacante = nullptr, int danoAnimacao = -1, const std::vector<std::string>& dropsAnimacao = {}) = 0;
    virtual void animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* atacante, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int danoAnimacao = -1) = 0;
    virtual void animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int curaAnimacao = 0) = 0;
    virtual void animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados = {}, bool isParry = false, int danoAnimacao = -1) = 0;
    virtual void animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados = {}, int curaAnimacao = 0) = 0;
    virtual void animarMorteInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* inimigoMorto, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, const std::vector<std::string>& drops = {}) = 0;
    virtual void atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, bool animarEntrada = false, std::function<void(std::vector<std::string>&)> callbackOverlay = nullptr) = 0;
    virtual void adicionarMensagemFixa(const std::string& msg) = 0;
    virtual void limparMensagensFixas() = 0;
    virtual void configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) = 0;
    virtual void definirTurnoVisivel(int turno, const std::string& nome) = 0;
    virtual void selecionarHUDDeAliado(Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) = 0;
    virtual int obterAcaoDoJogador(int turnoAtual, Personagem* personagemAgindo, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) = 0;
    virtual int obterAlvoAtaque(const std::string& tituloCombate, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) = 0;
    virtual int obterAlvoItem(const std::string& tituloCombate, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) = 0;
    virtual int obterEscolhaDeEscudo(const std::string& nomePersonagem, const std::vector<Item*>& listaDeEscudos) = 0;
    virtual void notificarInimigosMaisAgeis() = 0;
    virtual void notificarTurnoExtra(int destrezaJogador, int maxDestrezaInimigos) = 0;
    virtual void notificarDesprevencaoInventario() = 0;
    virtual void notificarSemEscudos(const std::string& nomePersonagem) = 0;
    virtual void notificarDesequilibrioDefesa(const std::string& nomePersonagem) = 0;
    virtual void notificarPosturaDefensiva(const std::string& nomePersonagem, const std::string& nomeEscudo) = 0;
    virtual void notificarAcaoInvalida() = 0;
    virtual void notificarCancelamentoItem() = 0;
    virtual void notificarRequisitoNaoAtendido(const std::string& mensagemRequisito) = 0;
};
