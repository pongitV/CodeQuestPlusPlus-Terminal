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

std::function<void()> Parry::onUpdateScreen = nullptr;
std::string Parry::minigameMessage = "";
std::string Parry::minigameBar = "";

bool Parry::tentarParry(Personagem* atacante, Personagem* defensor, int danoMitigado, int& quantidadeDeDanoReduzido) 
{
    int destrezaDoAtacante = atacante ? std::max(1, atacante->obterDestreza()) : 1;
    int destrezaDoDefensor = defensor ? std::max(1, defensor->obterDestreza()) : 1;

    if (destrezaDoAtacante > destrezaDoDefensor * 2) 
    {
        std::string msgAgil = "O inimigo e agil demais para voce efetivar o parry!";
        if (Parry::onUpdateScreen) {
            Parry::minigameMessage = "\033[48;2;120;0;0m " + msgAgil + " \033[0m";
            Parry::onUpdateScreen();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            Parry::minigameMessage = "";
            Parry::onUpdateScreen();
        } else {
            std::cout << "\n" << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgCombate(msgAgil, Cor::FUNDO_VERMELHO) << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
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
    std::string instructions = "O inimigo ataca! Pressione [ESPACO] no momento exato!";
    Parry::minigameMessage = instructions;
    
    int tamanhoBarra = 60;
    int sweetSpotCentro = 48;
    int tamanhoSweetSpot = std::clamp(6 - (dificuldade / 3), 1, 4); 
    
    int posicaoAtual = 0;
    bool espacoPressionado = false;
    int posicaoPressionada = -1;
    
    ControleDeInput::limparBuffer();
    
    int delayMs = std::clamp(20 - dificuldade, 5, 25);
    auto minigameStart = std::chrono::steady_clock::now();

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

        Parry::minigameBar = "Parry: " + barra;
        
        if (Parry::onUpdateScreen) {
            Parry::onUpdateScreen();
        } else {
            std::cout << "\r" << Aparencia::margemCombate() << Parry::minigameBar << std::flush;
        }

        if (ControleDeInput::teclaPressionada()) {
            char tecla = ControleDeInput::lerTecla();
            auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - minigameStart).count();
            if (elapsedMs > 150) {
                if (tecla == ' ') {
                    espacoPressionado = true;
                    posicaoPressionada = posicaoAtual;
                    break;
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        posicaoAtual++;
    }

    Parry::minigameMessage = "";
    Parry::minigameBar = "";
    if (Parry::onUpdateScreen) {
        Parry::onUpdateScreen();
    } else {
        std::cout << "\n";
    }
    
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

    std::string instructions = "DIGITE RAPIDO: " + sequencia;
    Parry::minigameMessage = instructions;

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

        std::ostringstream ss;
        ss << "Parry [Tempo: " << std::fixed << std::setprecision(1) << std::max(0.0, tempoLimite - decorrido) << "s]: " << resposta;
        Parry::minigameBar = ss.str();

        if (Parry::onUpdateScreen) {
            Parry::onUpdateScreen();
        } else {
            std::cout << "\r" << Aparencia::margemCombate() << Parry::minigameBar << "\033[K" << std::flush;
        }

        if (ControleDeInput::teclaPressionada()) {
            char c = ControleDeInput::lerTecla();
            auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - inicio).count();
            if (c == '\r' || c == '\n') {
                if (elapsedMs > 150 && !resposta.empty()) {
                    concluido = true;
                    break;
                }
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

    Parry::minigameMessage = "";
    Parry::minigameBar = "";

    if (tempoEsgotado) {
        if (Parry::onUpdateScreen) {
            Parry::minigameMessage = "\033[1;38;2;255;50;50mTEMPO ESGOTADO!\033[0m";
            Parry::onUpdateScreen();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            Parry::minigameMessage = "";
            Parry::onUpdateScreen();
        } else {
            std::cout << Aparencia::margemCombate() << "\033[1;38;2;255;50;50mTEMPO ESGOTADO!\033[0m\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        quantidadeDeDanoReduzido = 0;
        return false;
    }

    if (Parry::onUpdateScreen) {
        Parry::onUpdateScreen();
    } else {
        std::cout << "\n";
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
