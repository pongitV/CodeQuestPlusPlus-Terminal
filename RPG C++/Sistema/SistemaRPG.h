#include <vector>
#include "Personagem.h"

#pragma once

class SistemaRPG 
{
private:
    // Referencias aos participantes do combate ativo
    Personagem* jogadorAtual;
    std::vector<Personagem*> listaDeInimigos;

    // Estatisticas gerais e controle da sessao de combate
    int quantidadeDeOuroObtido;
    int quantidadeDeXpObtido;
    int totalDeDanoCausado;
    int totalDeDanoRecebido;
    int contadorDoTurnoAtual;
    
    void aplicarDanoAoAlvo(Personagem* personagemAtacante, Personagem* personagemAlvo, int quantidadeDeDanoBruto, int turnoAtualDoCombate);
    bool executarSistemaDeParry(int quantidadeDeNumerosParaDigitar, int tempoLimiteEmSegundos, int& quantidadeDeDanoReduzido);

public:
    SistemaRPG(Personagem* jogadorParaOCombate, std::vector<Personagem*> inimigosParaOCombate);
    virtual ~SistemaRPG();

    void iniciarCombate();
    void executarTurnoDeTodosOsInimigos();
    bool verificarCondicaoDeVitoriaOuDerrota();
    void realizarAtaqueFisico(Personagem* personagemAtacante, Personagem* personagemDefensor, int turnoAtualDoCombate);
};