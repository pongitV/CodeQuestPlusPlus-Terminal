#include "SistemaParry.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "../Utilidades/ControleDeInput.h"
#include "SistemaPersonagem.h"
#include "../Utilidades/GeradorAleatorio.h"
#include "../Utilidades/SimplificacoesAparencia.h"

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
    
    for (int indiceAtual = 0; indiceAtual < quantidadeDeNumerosParaDigitar; ++indiceAtual) 
    {
        int numeroAleatorio = GeradorAleatorio::obterInteiro(1, 9);
        sequenciaGeradaPeloSistema += std::to_string(numeroAleatorio);
        quantidadeDeDanoReduzido += numeroAleatorio;
    }

    std::cout << "\n" << SimplificacoesAparencia::margemCombate() << SimplificacoesAparencia::cor(Cor::AMARELO) << "[PARRY] O inimigo ataca! Digite a sequencia rapidamente para defender!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
    std::cout << SimplificacoesAparencia::margemCombate() << SimplificacoesAparencia::cor(Cor::CIANO) << "[PARRY] Sequencia: " << sequenciaGeradaPeloSistema << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
    std::cout << SimplificacoesAparencia::margemCombate() << SimplificacoesAparencia::cor(Cor::AMARELO) << "[PARRY] Tempo Limite: " << tempoLimiteEmSegundos << " segundos!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
    std::cout << SimplificacoesAparencia::margemCombate() << SimplificacoesAparencia::cor(Cor::CIANO) << "[PARRY] Digite: " << SimplificacoesAparencia::cor(Cor::RESET);

    std::string entradaDigitadaPeloJogador = "";
    
    auto tempoInicial = std::chrono::steady_clock::now();
    while (true) {
        auto tempoAtual = std::chrono::steady_clock::now();
        std::chrono::duration<double> tempoDecorrido = tempoAtual - tempoInicial;
        if (tempoDecorrido.count() > tempoLimiteEmSegundos) {
            std::cout << "\n" << SimplificacoesAparencia::margemCombate() << SimplificacoesAparencia::cor(Cor::FUNDO_VERMELHO) << "[PARRY] TEMPO ESGOTADO (" << tempoDecorrido.count() << "s)!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
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