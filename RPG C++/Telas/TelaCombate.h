#pragma once

#include <string>
#include <vector>

#include "../Utilidades/Aparencia.h"

class SistemaPersonagem;
class Item;

class TelaCombate 
{
public:
    static void exibirLogoParaTelaDeCombate(const std::string& tituloDaTela = "");
    static void exibirBarraDeStatusDoJogador(SistemaPersonagem* jogadorAtual, Cor corDestaque = Cor::RESET, int danoAnimacao = -1, int frameAnimacao = 0);
    static void exibirHordaDeInimigosLadoALado(const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao = nullptr, int frameAnimacao = 0, bool isCura = false, bool animarSurgimento = false, bool isMorte = false, Item* armaAtacante = nullptr, int danoAnimacao = -1, const std::vector<std::string>& dropsAnimacao = {});
    static void animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, SistemaPersonagem* atacante, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados, int danoAnimacao = -1);
    static void animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados = {});
    static void animarDanoNoJogador(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* alvoAnimacao, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados = {}, bool isParry = false, int danoAnimacao = -1);
    static void animarMorteInimigo(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* inimigoMorto, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados, const std::vector<std::string>& drops = {});
    static void atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<SistemaPersonagem*>& listaDeInimigos, SistemaPersonagem* jogadorAtual, const std::vector<SistemaPersonagem*>& listaDeAliados);

    static void adicionarMensagemFixa(const std::string& msg);
    static void limparMensagensFixas();

    // Menus e interacoes de texto do combate
    static void exibirCabecalhoDoTurno(int turnoAtual, const std::string& nomePersonagem);
    static int obterAcaoDoJogador();
    static int obterAlvoAtaque(int maxIndice);
    static int obterAlvoItem(const std::vector<SistemaPersonagem*>& listaDeInimigos);
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
};
