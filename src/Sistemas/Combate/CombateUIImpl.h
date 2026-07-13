#pragma once

#include "ICombateUI.h"

class CombateUIImpl : public ICombateUI {
public:
    CombateUIImpl() = default;
    ~CombateUIImpl() override = default;

    void configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) override;
    
    void animarIntroducaoCombate(const std::string& titulo, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual) override;
    void atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, bool animarEntrada = false) override;
    
    void animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* atacante, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int danoAnimacao) override;
    void animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int curaAnimacao) override;
    
    void animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, bool isParry, int danoAnimacao) override;
    void animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* alvoAnimacao, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, int curaAnimacao) override;
    
    void animarMorteInimigo(const std::string& tituloCombate, const std::vector<Personagem*>& listaDeInimigos, Personagem* inimigoMorto, Personagem* jogadorAtual, const std::vector<Personagem*>& listaDeAliados, const std::vector<std::string>& drops) override;

    void limparContextoPersonagemHUD() override;
    void limparContextoInimigoMortoEDrops() override;

    std::string margemCombate() override;

    void adicionarMensagemFixa(const std::string& msg) override;
    void limparMensagensFixas() override;
    void definirTurnoVisivel(int turno, const std::string& nome) override;
    
    int obterAcaoDoJogador(int turnoAtual, Personagem* personagemAgindo, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) override;
    int obterAlvoAtaque(const std::string& tituloCombate, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) override;
    int obterAlvoItem(const std::string& tituloCombate, const std::vector<Personagem*>& inimigos, Personagem* jogadorAtual, const std::vector<Personagem*>& aliados) override;
    int obterEscolhaDeEscudo(const std::string& nomePersonagem, const std::vector<Item*>& listaDeEscudos) override;
    
    void notificarInimigosMaisAgeis() override;
    void notificarTurnoExtra(int destrezaJogador, int maxDestrezaInimigos) override;
    void notificarDesprevencaoInventario() override;
    void notificarSemEscudos(const std::string& nomePersonagem) override;
    void notificarDesequilibrioDefesa(const std::string& nomePersonagem) override;
    void notificarPosturaDefensiva(const std::string& nomePersonagem, const std::string& nomeEscudo) override;
    void notificarAcaoInvalida() override;
    void notificarCancelamentoItem() override;
    void notificarRequisitoNaoAtendido(const std::string& mensagemRequisito) override;

    void exibirTelaVitoria(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate, const std::vector<std::string>& itensObtidos, const std::vector<std::string>& inimigosDerrotados, int parriesPerfeitos, int maiorDano, int parriesTentados, int parriesEfetivos, int itensConsumidos, const std::vector<std::string>& novasDescobertas) override;
    void exibirTelaDerrota(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate) override;

    void exibirTelaAtributos(Personagem* personagem) override;
    void exibirTelaDiario(Personagem* personagem) override;

    void limparTela() override;
};
