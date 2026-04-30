#include <vector>
#include <string>
#include <memory>
#include <utility>
#include "../Sistemas/SistemaPersonagem.h"

#pragma once

class GerenciadorCombate 
{
private:
    // Referencias aos participantes do combate ativo
    SistemaPersonagem* jogadorAtual;
    std::vector<std::unique_ptr<SistemaPersonagem>> listaDeInimigos;

    // Estatisticas gerais e controle da sessao de combate
    int quantidadeDeOuroObtido;
    int quantidadeDeXpObtido;
    int totalDeDanoCausado;
    int totalDeDanoRecebido;
    int contadorDoTurnoAtual;
    std::vector<std::string> itensObtidos;
    
    void aplicarDanoAoAlvo(SistemaPersonagem* personagemAtacante, SistemaPersonagem* personagemAlvo, int quantidadeDeDanoBruto, int danoPerfurante, int turnoAtualDoCombate);
    void processarMorteDeInimigo(SistemaPersonagem* inimigo);
    void exibirResultadoDoAtaque(SistemaPersonagem* alvo, int danoFinal, bool tentouParry, bool parrySucesso);

    void processarMenuDeAcoesDoJogador(bool& turnoFoiConsumido, bool& usouInventarioNoTurno);
    void processarAcaoAtacar(bool& turnoFoiConsumido);
    void processarAcaoDefender(bool& turnoFoiConsumido);
    void processarAcaoHabilidade(bool& turnoFoiConsumido);
    void processarAcaoInventario(bool& turnoFoiConsumido, bool& usouInventarioNoTurno);
    void limparInimigosMortos();

    std::pair<int, int> calcularDanoBase(SistemaPersonagem* atacante);

    std::string obterTituloDoCombate() const;
    std::vector<SistemaPersonagem*> obterInimigosRaw() const;
    void exibirTelaDeCombate() const;

public:
    GerenciadorCombate(SistemaPersonagem* jogadorParaOCombate, std::vector<std::unique_ptr<SistemaPersonagem>>&& inimigosParaOCombate);
    virtual ~GerenciadorCombate();

    void iniciarCombate();
    void executarTurnoDeTodosOsInimigos();
    bool verificarCondicaoDeVitoriaOuDerrota();
    void realizarAtaqueFisico(SistemaPersonagem* personagemAtacante, SistemaPersonagem* personagemDefensor, int turnoAtualDoCombate);
};