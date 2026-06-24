#include "Parry.h"
#include <iomanip>
#include <cctype>
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

bool Parry::tentarParry(Personagem* atacante, Personagem* defensor, int danoMitigado, int& quantidadeDeDanoReduzido) 
{
    int destrezaDoAtacante = atacante ? std::max(1, atacante->obterDestreza()) : 1;
    int destrezaDoDefensor = defensor ? std::max(1, defensor->obterDestreza()) : 1;

    if (destrezaDoAtacante > destrezaDoDefensor * 2) 
    {
        std::string msgAgil = "O inimigo e agil demais para voce efetivar o parry!";
        std::cout << "\n" << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgCombate(msgAgil, Cor::FUNDO_VERMELHO) << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        quantidadeDeDanoReduzido = 0;
        return false;
    }

    int dificuldade = std::clamp(danoMitigado / 5 + (destrezaDoAtacante / 10), 1, 20);

    bool sucesso = false;
    if (defensor && defensor->obterParryModerno()) {
        sucesso = executarMinigameMovimento(dificuldade, danoMitigado, quantidadeDeDanoReduzido);
    } else {
        sucesso = executarMinigameDigitacao(dificuldade, danoMitigado, quantidadeDeDanoReduzido);
    }
    return sucesso;
}

bool Parry::executarMinigameMovimento(int dificuldade, int danoMitigado, int& quantidadeDeDanoReduzido) 
{
    std::cout << "\n" << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgCombate("O inimigo ataca! Pressione [ESPACO] no momento exato!", Cor::AMARELO) << "\n";
    
    int tamanhoBarra = 60;
    int sweetSpotCentro = 48;
    int tamanhoSweetSpot = std::clamp(6 - (dificuldade / 3), 1, 4); 
    
    int posicaoAtual = 0;
    bool espacoPressionado = false;
    int posicaoPressionada = -1;
    
    ControleDeInput::limparBuffer();
    
    int delayMs = std::clamp(20 - dificuldade, 5, 25);

    while (posicaoAtual <= tamanhoBarra) {
        std::string barra = "[";
        for (int i = 0; i < tamanhoBarra; i++) {
            bool noSweetSpot = (i >= sweetSpotCentro - tamanhoSweetSpot/2 && i <= sweetSpotCentro + tamanhoSweetSpot/2);
            
            if (i == posicaoAtual) {
                barra += "\033[48;2;255;255;255m>\033[0m"; // Cursor branco
            } else if (noSweetSpot) {
                barra += "\033[38;2;50;255;50m█\033[0m"; // Zona Verde
            } else if (i < posicaoAtual) {
                barra += "░"; // Rastro
            } else {
                barra += " ";
            }
        }
        barra += "]";

        std::cout << "\r" << Aparencia::margemCombate() << "Parry: " << barra << std::flush;

        if (ControleDeInput::teclaPressionada()) {
            char tecla = ControleDeInput::lerTecla();
            if (tecla == ' ' || tecla == '\n' || tecla == '\r') {
                espacoPressionado = true;
                posicaoPressionada = posicaoAtual;
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        posicaoAtual++;
    }

    std::cout << "\n";
    
    if (espacoPressionado) {
        bool noSweetSpot = (posicaoPressionada >= sweetSpotCentro - tamanhoSweetSpot/2 && posicaoPressionada <= sweetSpotCentro + tamanhoSweetSpot/2);
        if (noSweetSpot) {
            int distancia = std::abs(posicaoPressionada - sweetSpotCentro);
            if (distancia <= 1) {
                // Parry Perfeito!
                quantidadeDeDanoReduzido = danoMitigado; 
            } else {
                // Parry Efetivo
                quantidadeDeDanoReduzido = std::max(1, danoMitigado / 2); 
            }
            return true;
        }
    }
    
    quantidadeDeDanoReduzido = 0;
    return false;
}

bool Parry::executarMinigameDigitacao(int dificuldade, int danoMitigado, int& quantidadeDeDanoReduzido) 
{
    int tamanhoSequencia = std::clamp(4 + dificuldade / 5, 4, 8);
    std::string sequencia = "";
    for (int i = 0; i < tamanhoSequencia; ++i) {
        sequencia += std::to_string(GeradorAleatorio::obterInteiro(0, 9));
    }

    double tempoLimite = std::max(1.5, 4.0 - (dificuldade / 6.0));

    std::cout << "\n" << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgCombate("DIGITE RAPIDO: " + sequencia, Cor::AMARELO) << "\n";
    std::cout << Aparencia::margemCombate() << "Sua resposta: " << std::flush;

    std::string resposta = "";
    auto inicio = std::chrono::steady_clock::now();
    bool tempoEsgotado = false;
    bool concluido = false;

    ControleDeInput::limparBuffer();

    while (true) {
        auto agora = std::chrono::steady_clock::now();
        double decorrido = std::chrono::duration<double>(agora - inicio).count();

        if (decorrido >= tempoLimite) {
            tempoEsgotado = true;
            break;
        }

        std::cout << "\r" << Aparencia::margemCombate() << "Parry [Tempo: " 
                  << std::fixed << std::setprecision(1) << (tempoLimite - decorrido) << "s]: " 
                  << resposta << "\033[K" << std::flush;

        if (ControleDeInput::teclaPressionada()) {
            char c = ControleDeInput::lerTecla();
            if (c == '\r' || c == '\n') {
                concluido = true;
                break;
            } else if (c == '\b' || c == 127) { // Backspace
                if (!resposta.empty()) {
                    resposta.pop_back();
                }
            } else if (std::isdigit(static_cast<unsigned char>(c))) {
                resposta += c;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    std::cout << "\n";

    if (tempoEsgotado) {
        std::cout << Aparencia::margemCombate() << "\033[1;38;2;255;50;50mTEMPO ESGOTADO!\033[0m\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        quantidadeDeDanoReduzido = 0;
        return false;
    }

    auto fim = std::chrono::steady_clock::now();
    double tempoTotal = std::chrono::duration<double>(fim - inicio).count();

    if (concluido && resposta == sequencia) {
        if (tempoTotal <= tempoLimite * 0.5) {
            // Parry Perfeito!
            quantidadeDeDanoReduzido = danoMitigado;
        } else {
            // Parry Efetivo!
            quantidadeDeDanoReduzido = std::max(1, danoMitigado / 2);
        }
        return true;
    }

    quantidadeDeDanoReduzido = 0;
    return false;
}
