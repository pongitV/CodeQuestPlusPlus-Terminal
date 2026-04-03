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
    while (jogador->obterVida() > 0 && !inimigos.empty()) 
    {
        bool acaoConsumiuTurno = false;

        while (!acaoConsumiuTurno) 
        {
            // 1. ATUALIZAÇÃO VISUAL DO CAMPO DE BATALHA
            Menu::limparTela();
            Menu::exibirLogo();
            Menu::exibirHorda(inimigos); // Mostra os goblins lado a lado com HP

            // 2. INTERFACE DE STATUS DO JOGADOR
            Menu::exibirStatusJogador(jogador); 

            std::cout << "\nTURNO " << contadorTurno << " | SUA VEZ" << std::endl;
            std::cout << "1. Atacar | 2. Habilidade | 3. Inventario | Escolha: ";
            
            int acao;
            if (!(std::cin >> acao)) 
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            // --- LÓGICA DE ATAQUE ---
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
                        int ouro = inimigos[alvoIndex]->obterOuroRecompensa();
                        jogador->ganharOuro(ouro);
                        std::cout << "\n[!] " << inimigos[alvoIndex]->obterNome() << " derrotado! +" << ouro << "G\n";
                        
                        delete inimigos[alvoIndex]; 
                        inimigos.erase(inimigos.begin() + alvoIndex);
                        Menu::esperar();
                    }
                    acaoConsumiuTurno = true;
                }
            }
            // --- LÓGICA DE HABILIDADE ---
            else if (acao == 2)
            {
                usarHabilidadeClasse(jogador, jogador); // Alvo padrão é o próprio ou o primeiro inimigo
                acaoConsumiuTurno = true;
                Menu::esperar();
            }
            // --- LÓGICA DE INVENTÁRIO (SISTEMA DE CÓDIGOS) ---
            else if (acao == 3) 
            {
                Menu::exibirInventario(jogador);
    
                std::string codigo;
                std::cout << "\nDigite o codigo do item ou '0' para voltar: ";
                std::cin >> codigo;

                if (codigo != "0")
                {
                    // Busca o item usando a etiqueta (ex: 1C para a primeira poção)
                    Item* itemEscolhido = jogador->obterInventario()->buscarItemPorCodigo(
                        codigo, jogador->obterArma(), jogador->obterEscudo(), jogador->obterArmadura()
                    );

                    if (itemEscolhido != nullptr)
                    {
                        if (dynamic_cast<PocaoCura*>(itemEscolhido))
                        {
                            // SALVAGUARDA: Guardar o nome antes de remover o item da memória
                            std::string nomeItem = itemEscolhido->obterNomeItem();
                            int cura = static_cast<int>(jogador->obterVidaMaxima() * 0.30);
                            
                            jogador->modificarVida(cura); 
                            jogador->obterInventario()->removerPorNome(nomeItem); // Deleta o ponteiro
                
                            std::cout << "\n[SISTEMA]: " << nomeItem << " usada! +" << cura << " HP.\n";
                            Menu::esperar(); // Permite que o jogador leia o resultado
                            acaoConsumiuTurno = true;
                        }
                        else 
                        {
                            std::cout << "[SISTEMA]: Este item nao pode ser usado em combate.\n";
                            Menu::esperar();
                        }
                    }
                    else 
                    {
                        std::cout << "[ERRO]: Codigo invalido!\n";
                        Menu::esperar();
                    }
                }
            }
            
            if (verificarVitoria()) return; 
        }

        // --- TURNO AUTOMÁTICO DOS INIMIGOS ---
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
    Menu::esperar();
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
    // Cálculos de Defesa
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