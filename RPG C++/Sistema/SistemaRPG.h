#include <vector>
#include <string>
#include <memory>
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
    int danoMagicoPerfuranteAtual;
    
    void aplicarDanoAoAlvo(Personagem* personagemAtacante, Personagem* personagemAlvo, int quantidadeDeDanoBruto, int turnoAtualDoCombate);
    bool executarSistemaDeParry(int quantidadeDeNumerosParaDigitar, int tempoLimiteEmSegundos, int& quantidadeDeDanoReduzido);
    void processarMorteDeInimigo(Personagem* inimigo);

public:
    SistemaRPG(Personagem* jogadorParaOCombate, std::vector<std::unique_ptr<Personagem>>&& inimigosParaOCombate);
    virtual ~SistemaRPG();

    void iniciarCombate();
    void executarTurnoDeTodosOsInimigos();
    bool verificarCondicaoDeVitoriaOuDerrota();
    void realizarAtaqueFisico(Personagem* personagemAtacante, Personagem* personagemDefensor, int turnoAtualDoCombate);
};