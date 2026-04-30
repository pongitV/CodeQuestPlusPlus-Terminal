#include "SistemaParry.h"
#include "SistemaPersonagem.h"
#include "ControleDeInput.h"
#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <thread>
#include <algorithm>

bool SistemaParry::tentarParry(SistemaPersonagem* atacante, int danoMitigado, int& quantidadeDeDanoReduzido) 
{
    int quantidadeDeNumerosDoParry = std::max(1, danoMitigado / 4);
    int destrezaDoAtacante = std::max(1, atacante->obterDestreza());
    int tempoLimiteParaParryEmSegundos = std::max(1, 60 / destrezaDoAtacante);

    bool sucesso = executarMinigame(quantidadeDeNumerosDoParry, tempoLimiteParaParryEmSegundos, quantidadeDeDanoReduzido);
    if (sucesso) 
    {
        int limiteMaximoDeReducao = std::max(1, danoMitigado / 2);
        quantidadeDeDanoReduzido = std::min(quantidadeDeDanoReduzido, limiteMaximoDeReducao);
    }
    else 
    {
        quantidadeDeDanoReduzido = 0;
    }
    return sucesso;
}

bool SistemaParry::executarMinigame(int quantidadeDeNumerosParaDigitar, int tempoLimiteEmSegundos, int& quantidadeDeDanoReduzido) 
{
    std::string sequenciaGeradaPeloSistema = "";
    quantidadeDeDanoReduzido = 0;
    
    static std::random_device rd;
    static std::mt19937 gerador(rd());
    std::uniform_int_distribution<int> distribuicao(1, 9);
    
    for (int indiceAtual = 0; indiceAtual < quantidadeDeNumerosParaDigitar; ++indiceAtual) 
    {
        int numeroAleatorio = distribuicao(gerador);
        sequenciaGeradaPeloSistema += std::to_string(numeroAleatorio);
        quantidadeDeDanoReduzido += numeroAleatorio;
    }

    std::cout << "\n[PARRY] O inimigo ataca! Digite a sequencia rapidamente para defender!\n";
    std::cout << "[PARRY] Sequencia: " << sequenciaGeradaPeloSistema << "\n";
    std::cout << "[PARRY] Tempo Limite: " << tempoLimiteEmSegundos << " segundos!\n";
    std::cout << "[PARRY] Digite: ";

    std::string entradaDigitadaPeloJogador = "";
    
    auto tempoInicial = std::chrono::steady_clock::now();
    while (true) {
        auto tempoAtual = std::chrono::steady_clock::now();
        std::chrono::duration<double> tempoDecorrido = tempoAtual - tempoInicial;
        if (tempoDecorrido.count() > tempoLimiteEmSegundos) {
            std::cout << "\n[PARRY] TEMPO ESGOTADO (" << tempoDecorrido.count() << "s)!\n";
            return false;
        }
        if (ControleDeInput::teclaPressionada()) 
        {
            char teclaPressionada = ControleDeInput::lerTecla();
            if (teclaPressionada == '\r' || teclaPressionada == '\n') 
            {
                std::cout << "\n";
                break;
            } 
            else if (teclaPressionada == '\b' || teclaPressionada == 127) 
            {
                if (!entradaDigitadaPeloJogador.empty()) { entradaDigitadaPeloJogador.pop_back(); std::cout << "\b \b"; }
            } 
            else { entradaDigitadaPeloJogador += teclaPressionada; std::cout << teclaPressionada; }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return entradaDigitadaPeloJogador == sequenciaGeradaPeloSistema;
}