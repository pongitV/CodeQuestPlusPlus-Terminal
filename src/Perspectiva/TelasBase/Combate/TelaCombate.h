#pragma once

#include <string>
#include <vector>
#include "ContextoCombate.h"
#include "../../../Core/Utilidades/Aparencia.h"

class Personagem;
class Item;

class TelaCombate {
public:
    static ContextoCombate contexto;

    static void exibirLogoParaTelaDeCombate(const std::string& tituloDaTela = "", bool animar = true);
    static void animarIntroducaoCombate(const std::string& titulo, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual = nullptr);
    static std::vector<std::string> obterLinhasBarraDeStatusDoJogador(Personagem* jogadorAtual, Cor corDestaque = Cor::RESET, int danoAnimacao = -1, int frameAnimacao = 0, bool isCura = false);
    static void exibirHordaDeInimigosLadoALado(const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao = nullptr, int frameAnimacao = 0, bool isCura = false, bool animarSurgimento = false, bool isMorte = false, Item* armaAtacante = nullptr, int danoAnimacao = -1, const std::vector<std::string>& dropsAnimacao = {});
    static void animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* atacante, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int danoAnimacao = -1);
    static void animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int curaAnimacao = 0);
    static void animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados = {}, bool isParry = false, int danoAnimacao = -1);
    static void animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados = {}, int curaAnimacao = 0);
    static void animarMorteInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* inimigoMorto, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, const std::vector<std::string>& drops = {});
    static void atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, bool animarEntrada = false);

    static void adicionarMensagemFixa(const std::string& msg);
    static void limparMensagensFixas();

    static void configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo);
    static void definirTurnoVisivel(int turno, const std::string& nome);
    static void selecionarHUDDeAliado(Personagem* jogadorAtual, const std::vector<Personagem*>& aliados);

    static int obterAcaoDoJogador(int turnoAtual, Personagem* personagemAgindo, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados);
    static int obterAlvoAtaque(const std::string& tituloCombate, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados);
    static int obterAlvoItem(const std::string& tituloCombate, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados);
    static int obterEscolhaDeEscudo(const std::string& nomePersonagem, const std::vector<Item*>& listaDeEscudos);
    static void notificarInimigosMaisAgeis();
    static void notificarTurnoExtra(int destrezaJogador, int maxDestrezaInimigos);
    static void notificarDesprevencaoInventario();
    static void notificarSemEscudos(const std::string& nomePersonagem);
    static void notificarDesequilibrioDefesa(const std::string& nomePersonagem);
    static void notificarPosturaDefensiva(const std::string& nomePersonagem, const std::string& nomeEscudo);
    static void notificarAcaoInvalida();
    static void notificarCancelamentoItem();
    static void notificarRequisitoNaoAtendido(const std::string& mensagemRequisito);

    static std::string margemCombate();
};
