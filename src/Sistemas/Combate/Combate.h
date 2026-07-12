#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <memory>

#include "ICombateUI.h"
#include "../../Entidades/Personagem.h"

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

    // Contexto de Visao 3D
    bool isModo3D;
    std::vector<std::string> matrizDoMapaAtual;
    float jogadorPosX;
    float jogadorPosY;
    float jogadorAngulo;
    std::string tituloMapaAtual;    
    // Estatisticas Avancadas da Sessao
    static int stats_parriesTentados;
    static int stats_parriesEfetivos;
    static int stats_parriesPerfeitos;
    static int stats_maiorDanoCausado;
    static int stats_itensConsumidos;
    static std::vector<std::string> stats_novasDescobertas;
    static void resetarEstatisticasAvancadas();

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
    void iniciarCombate();
    void executarTurnoDeTodosOsInimigos();
    bool verificarCondicaoDeVitoriaOuDerrota();
    void realizarAtaqueFisico(Personagem* personagemAtacante, Personagem* personagemDefensor, int turnoAtualDoCombate);

    // Getters para Estatisticas Avancadas
    static int obterParriesTentados() { return stats_parriesTentados; }
    static int obterParriesEfetivos() { return stats_parriesEfetivos; }
    static int obterMaiorDanoCausado() { return stats_maiorDanoCausado; }
    static int obterItensConsumidos() { return stats_itensConsumidos; }
    static const std::vector<std::string>& obterNovasDescobertas() { return stats_novasDescobertas; }
};
