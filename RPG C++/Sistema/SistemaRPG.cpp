#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

#include "SistemaRPG.h"
#include "Menu.h"

SistemaRPG::SistemaRPG(Personagem* jogador, std::vector<Personagem*> inimigos) 
    : jogador(jogador), inimigos(inimigos), contadorTurno(1) 
{
}

SistemaRPG::~SistemaRPG() 
{
    for (Personagem* inimigo : inimigos) 
    {
        delete inimigo;
    }
    inimigos.clear();
}

void SistemaRPG::iniciarCombate() 
{
    std::cout << "--- BATALHA INICIADA ---" << std::endl;

    while (jogador->obterVida() > 0 && !inimigos.empty()) 
    {
        bool acaoConsumiuTurno = false;

        while (!acaoConsumiuTurno) 
        {
            std::cout << "\n================================================" << std::endl;
            std::cout << "TURNO " << contadorTurno << " | SUA VEZ" << std::endl;
            jogador->mostrarStatus(); 
            
            std::cout << "\nINIMIGOS EM CAMPO:" << std::endl;
            for (size_t i = 0; i < inimigos.size(); i++) 
            {
                std::cout << i << ". ";
                inimigos[i]->mostrarStatus();
            }

            int acao;
            std::cout << "\n1. Atacar | 2. Habilidade | 3. Inventario | Escolha: ";
            std::cin >> acao;

            if (acao == 1) 
            {
                int alvoIndex;
                std::cout << "Escolha o alvo (0 a " << inimigos.size() - 1 << "): ";
                std::cin >> alvoIndex;

                if (alvoIndex >= 0 && alvoIndex < static_cast<int>(inimigos.size())) 
                {
                    realizarAtaqueFisico(jogador, inimigos[alvoIndex], contadorTurno);
                    
                    if (inimigos[alvoIndex]->obterVida() <= 0) 
                    {
                        int ouroGanho = inimigos[alvoIndex]->obterOuroRecompensa();
                        jogador->ganharOuro(ouroGanho);
                        std::cout << "[!] " << inimigos[alvoIndex]->obterNome() << " foi derrotado!" << std::endl;

                        delete inimigos[alvoIndex]; 
                        inimigos.erase(inimigos.begin() + alvoIndex);
                    }
                    acaoConsumiuTurno = true;
                }
            }
            else if (acao == 2) 
            {
                usarHabilidadeClasse(jogador, jogador); 
                acaoConsumiuTurno = true;
            }
            else if (acao == 3) 
            {
                Menu::exibirInventario(jogador);
        
                std::string nomeItem;
                std::cout << "\nDigite o nome do item para usar (ou 'voltar'): ";
                std::getline(std::cin, nomeItem);

                if (nomeItem != "voltar")
                {
                    bool encontrou = false;
            
                    if (nomeItem == "Pocao de Cura (30%)")
                    {
                        if (jogador->obterInventario()->possuiPocaoCura())
                        {
                            int cura = static_cast<int>(jogador->obterVidaMaxima() * 0.30);
                            jogador->modificarVida(cura); 
                            jogador->obterInventario()->removerPorNome(nomeItem);
                            std::cout << nomeItem << " utilizada! +" << cura << " HP." << std::endl;
                            std::cout << "Vida: " << jogador->obterVida() << "/" << jogador->obterVidaMaxima() << std::endl;
                            encontrou = true;
                            acaoConsumiuTurno = true; 
                        }
                    }

                    if (!encontrou)
                    {
                        std::cout << "Item nao encontrado ou nome incorreto" << std::endl;
                        Menu::esperar();
                    }
                }
            }

            if (verificarVitoria()) return; 
        }

        executarTurnoInimigos();
        contadorTurno++;
    }
}

void SistemaRPG::executarTurnoInimigos() 
{
    std::cout << "\n--- TURNO DOS INIMIGOS ---" << std::endl;
    for (Personagem* inimigo : inimigos) 
    {
        if (jogador->obterVida() > 0) 
        {
            realizarAtaqueFisico(inimigo, jogador, contadorTurno);
        }
    }
}

void SistemaRPG::realizarAtaqueFisico(Personagem* atacante, Personagem* defensor, int turnoAtual) 
{
    int danoArma = (atacante->obterArma()) ? atacante->obterArma()->obterBonusDano() : 0;
    int danoBase = atacante->obterForca() + danoArma;
    
    std::cout << atacante->obterNome() << " ataca " << defensor->obterNome() << "!" << std::endl;
    aplicarDano(defensor, danoBase, turnoAtual);
}

void SistemaRPG::aplicarDano(Personagem* alvo, int danoBruto, int turnoAtual) 
{
    if (alvo->obterNomeClasse() == "Arqueiro" && turnoAtual % 2 == 0) 
    {
        std::cout << "[ESQUIVA] O alvo desviou habilmente do ataque!" << std::endl;
        return;
    }

    int bonusArmadura = (alvo->obterArmadura()) ? alvo->obterArmadura()->obterReducaoFixa() : 0;
    double totalReducaoFixa = (0.2 * alvo->obterResistencia()) + bonusArmadura;

    double bonusEscudo = (alvo->obterEscudo()) ? alvo->obterEscudo()->obterReducaoPercentual() : 0.0;
    double totalReducaoPerc = ((alvo->obterConstituicao() / 3.0) / 100.0) + bonusEscudo;

    int danoFinal = static_cast<int>((danoBruto - totalReducaoFixa) * (1.0 - totalReducaoPerc));
    if (danoFinal < 1) danoFinal = 1;

    alvo->modificarVida(-danoFinal);
    std::cout << ">> " << alvo->obterNome() << " recebeu " << danoFinal << " de dano." << std::endl;
}

void SistemaRPG::usarHabilidadeClasse(Personagem* usuario, Personagem* alvo) 
{
    usuario->usarHabilidadeDeClasse(alvo);
}

bool SistemaRPG::verificarVitoria() 
{
    if (inimigos.empty()) 
    {
        std::cout << "\n[VITORIA] Todos os inimigos foram eliminados!" << std::endl;
        Menu::esperar(); 
        return true;
    }
    
    if (jogador->obterVida() <= 0) 
    {
        std::cout << "\n[DERROTA] O heroi sucumbiu em batalha..." << std::endl;
        Menu::esperar(); 
        return true;
    }
    return false;
}