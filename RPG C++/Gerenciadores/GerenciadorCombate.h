#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../Sistemas/SistemaPersonagem.h"

class GerenciadorCombate 
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
    SistemaPersonagem* jogadorAtual;
    std::vector<std::unique_ptr<SistemaPersonagem>> listaDeInimigos;
    std::vector<std::unique_ptr<SistemaPersonagem>> listaDeAliados;

    // Estatisticas gerais e controle da sessao de combate
    int quantidadeDeOuroObtido;
    int quantidadeDeXpObtido;
    int totalDeDanoCausado;
    int totalDeDanoRecebido;
    int contadorDoTurnoAtual;
    std::vector<std::string> itensObtidos;
    
    void aplicarDanoAoAlvo(SistemaPersonagem* personagemAtacante, SistemaPersonagem* personagemAlvo, int quantidadeDeDanoBruto, int danoPerfurante, int turnoAtualDoCombate);
    void processarMorteDeInimigo(SistemaPersonagem* inimigo);
    void exibirResultadoDoAtaque(SistemaPersonagem* alvo, int danoFinal, bool tentouParry, bool parrySucesso, int danoBloqueado, bool escudoQuebrou, const std::string& nomeEscudoQuebrado);

    bool isPersonagemJogadorOuAliado(SistemaPersonagem* personagem) const;
    void processarMenuDeAcoesDoJogador(SistemaPersonagem* personagemAgindo, bool& turnoFoiConsumido, bool& usouInventarioNoTurno);
    void processarAcaoAtacar(SistemaPersonagem* personagemAgindo, bool& turnoFoiConsumido);
    void processarAcaoDefender(SistemaPersonagem* personagemAgindo, bool& turnoFoiConsumido);
    void processarAcaoHabilidade(SistemaPersonagem* personagemAgindo, bool& turnoFoiConsumido);
    void processarAcaoInventario(SistemaPersonagem* personagemAgindo, bool& turnoFoiConsumido, bool& usouInventarioNoTurno);
    void limparInimigosMortos();
    Item* selecionarEscudo(SistemaPersonagem* personagemAgindo);

    std::pair<int, int> calcularDanoBase(SistemaPersonagem* atacante);

    std::string obterTituloDoCombate() const;
    std::vector<SistemaPersonagem*> obterInimigosRaw() const;
    void exibirTelaDeCombate(bool animarEntrada = false) const;

public:
    GerenciadorCombate(SistemaPersonagem* jogadorParaOCombate, std::vector<std::unique_ptr<SistemaPersonagem>>&& inimigosParaOCombate);
    virtual ~GerenciadorCombate();

    void adicionarAliados(std::vector<std::unique_ptr<SistemaPersonagem>> aliados);
    void iniciarCombate();
    void executarTurnoDeTodosOsInimigos();
    bool verificarCondicaoDeVitoriaOuDerrota();
    void realizarAtaqueFisico(SistemaPersonagem* personagemAtacante, SistemaPersonagem* personagemDefensor, int turnoAtualDoCombate);
};