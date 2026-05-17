#include "SistemaParry.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "../Utilidades/ControleDeInput.h"
#include "SistemaPersonagem.h"
#include "../Utilidades/GeradorAleatorio.h"
#include "../Utilidades/Aparencia.h"

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

    std::cout << "\n" << Aparencia::margemCombate() << Aparencia::cor(Cor::VERMELHO) << "[PARRY] O inimigo ataca! Digite a sequencia rapidamente para defender!" << Aparencia::cor(Cor::RESET) << "\n";
    
    std::cout << Aparencia::margemCombate() << Aparencia::cor(Cor::VERMELHO) << "[PARRY] Sequencia: " << Aparencia::cor(Cor::AMARELO) << std::flush;
    for (char c : sequenciaGeradaPeloSistema) {
        std::cout << c << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
    std::cout << Aparencia::cor(Cor::RESET) << "\n";

    std::cout << Aparencia::margemCombate() << Aparencia::cor(Cor::VERMELHO) << "[PARRY] Tempo Limite: " << Aparencia::cor(Cor::BRANCO) << tempoLimiteEmSegundos << Aparencia::cor(Cor::VERMELHO) << " segundos!" << Aparencia::cor(Cor::RESET) << "\n";
    std::cout << Aparencia::margemCombate() << Aparencia::cor(Cor::VERMELHO) << "[PARRY] Digite: " << Aparencia::cor(Cor::BRANCO) << std::flush;

    std::string entradaDigitadaPeloJogador = "";
    
    ControleDeInput::limparBuffer(); // Evita que o jogador trapaceie digitando antes da animacao terminar
    auto tempoInicial = std::chrono::steady_clock::now();
    while (true) {
        auto tempoAtual = std::chrono::steady_clock::now();
        std::chrono::duration<double> tempoDecorrido = tempoAtual - tempoInicial;
        if (tempoDecorrido.count() > tempoLimiteEmSegundos) {
            std::cout << "\n" << Aparencia::margemCombate() << Aparencia::cor(Cor::FUNDO_VERMELHO) << "[PARRY] TEMPO ESGOTADO (" << tempoDecorrido.count() << "s)!" << Aparencia::cor(Cor::RESET) << "\n";
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