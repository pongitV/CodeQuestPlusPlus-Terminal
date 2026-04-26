#include <vector>
#include <string>
#include <memory>
#include <utility>
#include "Personagem.h"

#pragma once

class SistemaRPG 
{
private:
    // Referencias aos participantes do combate ativo
    Personagem* jogadorAtual;
    std::vector<std::unique_ptr<Personagem>> listaDeInimigos;

    // Estatisticas gerais e controle da sessao de combate
    int quantidadeDeOuroObtido;
    int quantidadeDeXpObtido;
    int totalDeDanoCausado;
    int totalDeDanoRecebido;
    int contadorDoTurnoAtual;
    std::vector<std::string> itensObtidos;
    
    void aplicarDanoAoAlvo(Personagem* personagemAtacante, Personagem* personagemAlvo, int quantidadeDeDanoBruto, int danoPerfurante, int turnoAtualDoCombate);
    void processarMorteDeInimigo(Personagem* inimigo);
    void exibirResultadoDoAtaque(Personagem* alvo, int danoFinal, bool tentouParry, bool parrySucesso);

    void processarMenuDeAcoesDoJogador(bool& turnoFoiConsumido, bool& usouInventarioNoTurno);
    void processarAcaoAtacar(bool& turnoFoiConsumido);
    void processarAcaoDefender(bool& turnoFoiConsumido);
    void processarAcaoHabilidade(bool& turnoFoiConsumido);
    void processarAcaoInventario(bool& turnoFoiConsumido, bool& usouInventarioNoTurno);
    void limparInimigosMortos();

    std::pair<int, int> calcularDanoBase(Personagem* atacante);

    std::string obterTituloDoCombate() const;
    std::vector<Personagem*> obterInimigosRaw() const;
    void exibirTelaDeCombate() const;

public:
    SistemaRPG(Personagem* jogadorParaOCombate, std::vector<std::unique_ptr<Personagem>>&& inimigosParaOCombate);
    virtual ~SistemaRPG();

    void iniciarCombate();
    void executarTurnoDeTodosOsInimigos();
    bool verificarCondicaoDeVitoriaOuDerrota();
    void realizarAtaqueFisico(Personagem* personagemAtacante, Personagem* personagemDefensor, int turnoAtualDoCombate);
};