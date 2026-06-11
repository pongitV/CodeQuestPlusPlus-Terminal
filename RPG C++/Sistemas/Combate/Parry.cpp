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

bool Parry::tentarParry(Personagem* atacante, Personagem* defensor, int danoMitigado, int& quantidadeDeDanoReduzido) 
{
    int destrezaDoAtacante = atacante ? std::max(1, atacante->obterDestreza()) : 1;
    int destrezaDoDefensor = defensor ? std::max(1, defensor->obterDestreza()) : 1;

    if (destrezaDoAtacante > destrezaDoDefensor) 
    {
        std::string msgAgil = "O inimigo e agil demais para voce efetivar o parry!";
        std::cout << "\n" << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgCombate(msgAgil, Cor::FUNDO_VERMELHO) << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        quantidadeDeDanoReduzido = 0;
        return false;
    }

    // Quantidade de numeros: Relacionado puramente ao dano do monstro (1/3 do dano)
    // Limitamos a 15 digitos no maximo para o minigame nao se tornar humanamente impossivel
    int quantidadeDeNumerosDoParry = std::clamp(danoMitigado / 3, 3, 15); 
    
    // Tempo em segundos: Tempo humano base + Relacao estrita com a diferenca de Destreza
    int tempoBaseParaDigitar = std::max(2, quantidadeDeNumerosDoParry / 2);
    int bonusDeDestreza = (destrezaDoDefensor - destrezaDoAtacante) / 5;
    int tempoLimiteParaParryEmSegundos = std::max(1, tempoBaseParaDigitar + bonusDeDestreza);

    bool sucesso = executarMinigame(quantidadeDeNumerosDoParry, tempoLimiteParaParryEmSegundos, quantidadeDeDanoReduzido);
    if (sucesso) 
    {
        // Balanceamento da recompensa: Multiplicamos a soma dos numeros pelo nivel do ataque
        // Isso garante que o parry continue eficiente e recompensador contra grandes danos (Chefes)
        int fatorMultiplicador = std::max(1, (danoMitigado / 15) + 1);
        quantidadeDeDanoReduzido *= fatorMultiplicador;
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
    
    std::cout << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgCombate("Sequencia: ", Cor::VERMELHO) << Aparencia::cor(Cor::AMARELO) << sequenciaGeradaPeloSistema << Aparencia::cor(Cor::RESET) << "\n";

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
            std::string timeoutMsg = "TEMPO ESGOTADO (" + std::to_string(static_cast<int>(tempoDecorrido.count())) + "s)!";
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
