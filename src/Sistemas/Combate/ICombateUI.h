#pragma once

#include <string>
#include <vector>
#include <memory>

class Personagem;
class Item;

class ICombateUI {
public:
    virtual ~ICombateUI() = default;

    virtual void configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) = 0;
    
    virtual void animarIntroducaoCombate(const std::string& titulo, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual) = 0;
    virtual void atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, bool animarEntrada = false) = 0;
    
    virtual void animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* atacante, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int danoAnimacao) = 0;
    virtual void animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int curaAnimacao) = 0;
    
    virtual void animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, bool isParry, int danoAnimacao) = 0;
    virtual void animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int curaAnimacao) = 0;
    
    virtual void animarMorteInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* inimigoMorto, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, const std::vector<std::string>& drops) = 0;

    virtual void limparContextoPersonagemHUD() = 0;
    virtual void limparContextoInimigoMortoEDrops() = 0;

    virtual std::string margemCombate() = 0;

    virtual void adicionarMensagemFixa(const std::string& msg) = 0;
    virtual void limparMensagensFixas() = 0;
    virtual void definirTurnoVisivel(int turno, const std::string& nome) = 0;
    
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

    virtual void exibirTelaVitoria(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate, const std::vector<std::string>& itensObtidos, const std::vector<std::string>& inimigosDerrotados, int parriesPerfeitos, int maiorDano) = 0;
    virtual void exibirTelaDerrota(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate) = 0;
    
    virtual void exibirTelaAtributos(Personagem* personagem) = 0;
    virtual void exibirTelaDiario(Personagem* personagem) = 0;

    virtual void limparTela() = 0;
};
