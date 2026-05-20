#include "Parry.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "../../Core/Utilidades/ControleDeInput.h"
#include "../../Entidades/Personagem.h"
#include "../../Core/Utilidades/GeradorAleatorio.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Core/Utilidades/FuncoesDialogo.h"

bool Parry::tentarParry(Personagem* atacante, int danoMitigado, int& quantidadeDeDanoReduzido) 
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

bool Parry::executarMinigame(int quantidadeDeNumerosParaDigitar, int tempoLimiteEmSegundos, int& quantidadeDeDanoReduzido) 
{
    std::string sequenciaGeradaPeloSistema = "";
    quantidadeDeDanoReduzido = 0;
    
    for (int indiceAtual = 0; indiceAtual < quantidadeDeNumerosParaDigitar; ++indiceAtual) 
    {
        int numeroAleatorio = GeradorAleatorio::obterInteiro(1, 9);
        sequenciaGeradaPeloSistema += std::to_string(numeroAleatorio);
        quantidadeDeDanoReduzido += numeroAleatorio;
    }

    std::cout << "\n" << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgCombate("O inimigo ataca! Digite a sequencia rapidamente para defender!", Cor::VERMELHO) << "\n";
    
    std::cout << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgCombate("Sequencia: ", Cor::VERMELHO) << Aparencia::cor(Cor::AMARELO) << std::flush;
    for (char c : sequenciaGeradaPeloSistema) {
        std::cout << c << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
    std::cout << Aparencia::cor(Cor::RESET) << "\n";

    std::string tempoMsg = "Tempo Limite: " + Aparencia::cor(Cor::BRANCO) + std::to_string(tempoLimiteEmSegundos) + Aparencia::cor(Cor::VERMELHO) + " segundos!";
    std::cout << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgCombate(tempoMsg, Cor::VERMELHO) << "\n";
    std::cout << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgCombate("Digite: ", Cor::VERMELHO) << Aparencia::cor(Cor::BRANCO) << std::flush;

    std::string entradaDigitadaPeloJogador = "";
    
    ControleDeInput::limparBuffer(); // Evita que o jogador trapaceie digitando antes da animacao terminar
    auto tempoInicial = std::chrono::steady_clock::now();
    while (true) {
        auto tempoAtual = std::chrono::steady_clock::now();
        std::chrono::duration<double> tempoDecorrido = tempoAtual - tempoInicial;
        if (tempoDecorrido.count() > tempoLimiteEmSegundos) {
            std::string timeoutMsg = "TEMPO ESGOTADO (" + std::to_string(tempoDecorrido.count()) + "s)!";
            std::cout << "\n" << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgCombate(timeoutMsg, Cor::FUNDO_VERMELHO) << "\n";
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
