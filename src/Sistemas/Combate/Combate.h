#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <memory>

#include "ICombateUI.h"
#include "../../Entidades/Personagem.h"

/**
 * @brief Classe responsavel por gerenciar o fluxo de combate do jogo.
 * Controla turnos, vida, acoes e UI do combate.
 */
class Combate 
{
public:
    enum class AcaoCombate 
    { 
        Atacar = 1, 
        Defender = 2, 
        Habilidade = 3, 
        Inventario = 4, 
        Jogador = 5, 
        Bestiario = 6 
    };

private:
    // Referencias aos participantes do combate ativo
    Personagem* jogadorAtual;
    std::vector<std::unique_ptr<Personagem>> listaDeInimigos;
    std::vector<std::unique_ptr<Personagem>> listaDeAliados;

    // Interface visual de combate (Injecao de Dependencia)
    std::unique_ptr<ICombateUI> ui;

    // Estatisticas gerais e controle da sessao de combate
    int quantidadeDeOuroObtido;
    int quantidadeDeXpObtido;
    int totalDeDanoCausado;
    int totalDeDanoRecebido;
    int contadorDoTurnoAtual;
    std::vector<std::string> itensObtidos;
    std::vector<std::string> inimigosDerrotados;

    // Estatisticas Avancadas da Sessao
    int stats_parriesTentados;
    int stats_parriesEfetivos;
    int stats_parriesPerfeitos;
    int stats_maiorDanoCausado;
    int stats_itensConsumidos;
    std::vector<std::string> stats_novasDescobertas;
    void resetarEstatisticasAvancadas();

    void aplicarDanoAoAlvo(Personagem* personagemAtacante, Personagem* personagemAlvo, int quantidadeDeDanoBruto, int danoPerfurante, int turnoAtualDoCombate);
    void processarMorteDeInimigo(Personagem* inimigo);
    void exibirResultadoDoAtaque(Personagem* alvo, int danoFinal, bool tentouParry, bool parrySucesso, int danoBloqueado, bool escudoQuebrou, const std::string& nomeEscudoQuebrado);

    void prepararTurnoPersonagem(Personagem* personagem);
    void processarPosDano(Personagem* atacante, Personagem* alvo, int danoFinal, bool tentouParry, bool parrySucesso);
    bool isPersonagemJogadorOuAliado(Personagem* personagem) const;
    void processarMenuDeAcoesDoJogador(Personagem* personagemAgindo, bool& turnoFoiConsumido, bool& usouInventarioNoTurno);
    void processarAcaoAtacar(Personagem* personagemAgindo, bool& turnoFoiConsumido);
    void processarAcaoDefender(Personagem* personagemAgindo, bool& turnoFoiConsumido);
    void processarAcaoHabilidade(Personagem* personagemAgindo, bool& turnoFoiConsumido);
    void processarAcaoInventario(Personagem* personagemAgindo, bool& turnoFoiConsumido, bool& usouInventarioNoTurno);
    void limparInimigosMortos();
    Item* selecionarEscudo(Personagem* personagemAgindo);

    std::string obterTituloDoCombate() const;
    std::vector<Personagem*> obterInimigosRaw() const;
    void exibirTelaDeCombate(bool animarEntrada = false) const;

public:
    Combate(Personagem* jogadorParaOCombate, std::vector<std::unique_ptr<Personagem>>&& inimigosParaOCombate, std::unique_ptr<ICombateUI> interfaceVisual = nullptr);
    virtual ~Combate();

    void setContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo);

    std::vector<Personagem*> obterAliadosVivosRaw() const;
    bool executarTurnoJogadorOuAliado(Personagem* personagem, bool& primeiraRenderizacao, bool processarEfeitosInicio = true);
    void adicionarAliadoEmCombate(std::unique_ptr<Personagem> aliado);
    void adicionarAliados(std::vector<std::unique_ptr<Personagem>> aliados);
    /**
     * @brief Inicia o laco principal de combate.
     */
    void iniciarCombate();

    /**
     * @brief Executa a inteligencia e as acoes de todos os inimigos presentes.
     */
    void executarTurnoDeTodosOsInimigos();

    /**
     * @brief Verifica se todos os inimigos estao mortos ou se o jogador morreu.
     * @return true se o combate deve acabar.
     */
    bool verificarCondicaoDeVitoriaOuDerrota();

    /**
     * @brief Aplica o fluxo completo de dano fisico de um personagem a outro.
     * @param personagemAtacante Ponteiro para quem ataca.
     * @param personagemDefensor Ponteiro para quem defende.
     * @param turnoAtualDoCombate Turno em que a acao ocorre.
     */
    void realizarAtaqueFisico(Personagem* personagemAtacante, Personagem* personagemDefensor, int turnoAtualDoCombate);

    // Getters para Estatisticas Avancadas
    int obterParriesTentados() const { return stats_parriesTentados; }
    int obterParriesEfetivos() const { return stats_parriesEfetivos; }
    int obterMaiorDanoCausado() const { return stats_maiorDanoCausado; }
    int obterItensConsumidos() const { return stats_itensConsumidos; }
    const std::vector<std::string>& obterNovasDescobertas() const { return stats_novasDescobertas; }
};
