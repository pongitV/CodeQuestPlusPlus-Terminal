#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <ctime>

#include "SistemaRPG.h"
#include "Menu.h"

// Inclusao de componentes para o funcionamento das mecanicas
#include "../Raças/RacaBase.h"
#include "../Classes/ClasseBase.h"
#include "../Inventario/Item.h"

SistemaRPG::SistemaRPG(Personagem* jogador, std::vector<Personagem*> inimigos) 
    : jogador(jogador), inimigos(inimigos), contadorTurno(1) 
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

SistemaRPG::~SistemaRPG() 
{
    for (Personagem* inimigo : inimigos) delete inimigo;
    inimigos.clear();
}

void SistemaRPG::iniciarCombate() 
{
    while (jogador->obterVida() > 0 && !inimigos.empty()) 
    {
        bool acaoConsumiuTurno = false;

        while (!acaoConsumiuTurno) 
        {
            Menu::limparTela();
            Menu::exibirLogo();
            Menu::exibirHorda(inimigos);
            Menu::exibirStatusJogador(jogador); 

            std::cout << "\nTURNO " << contadorTurno << " | SUA VEZ" << std::endl;
            std::cout << "1. Atacar | 2. Habilidade | 3. Inventario | 4. Jogador | Escolha: ";
            
            int acao;
            if (!(std::cin >> acao)) 
            {
                std::cin.clear();
                std::cin.ignore(1000, '\n');
                continue;
            }

            if (acao == 1) // --- 1. ATACAR ---
            {
                if (jogador->obterModoAtaqueArea() && jogador->obterNomeClasse() == "Mago") 
                {
                    realizarAtaqueFisico(jogador, nullptr, contadorTurno);
                    acaoConsumiuTurno = true;
                }
                else 
                {
                    int alvoIndex;
                    std::cout << "Escolha o alvo (0 a " << inimigos.size() - 1 << "): ";
                    std::cin >> alvoIndex;

                    if (alvoIndex >= 0 && alvoIndex < static_cast<int>(inimigos.size())) 
                    {
                        realizarAtaqueFisico(jogador, inimigos[alvoIndex], contadorTurno);
                        acaoConsumiuTurno = true;
                    }
                }

                // Limpeza de mortos apos o ataque
                for (auto it = inimigos.begin(); it != inimigos.end(); ) 
                {
                    if ((*it)->obterVida() <= 0) 
                    {
                        int ouro = (*it)->obterOuroRecompensa();
                        jogador->ganharOuro(ouro);
                        std::cout << "\n[!] " << (*it)->obterNome() << " derrotado! +" << ouro << "G\n";
                        delete *it;
                        it = inimigos.erase(it);
                        Menu::esperar();
                    } else { ++it; }
                }
            }
            else if (acao == 2) // --- 2. HABILIDADE ATIVA ---
            {
                jogador->obterClasse()->usarHabilidadeClasseAtiva(jogador, inimigos);
                
                // Mago nao gasta turno ao alternar estrategia
                if (jogador->obterNomeClasse() != "Mago") acaoConsumiuTurno = true;
                else Menu::esperar();
            }
            else if (acao == 3) // --- 3. INVENTARIO ---
            {
                Menu::exibirInventario(jogador);
                std::string codigo;
                std::cout << "\nDigite o codigo do item (ex: 1C) ou '0' para voltar: ";
                std::cin >> codigo;

                if (codigo != "0")
                {
                    Item* item = jogador->obterInventario()->buscarItemPorCodigo(
                        codigo, jogador->obterArma(), jogador->obterEscudo(), jogador->obterArmadura()
                    );

                    if (item && dynamic_cast<PocaoCura*>(item))
                    {
                        // Seguranca: Salva o nome antes de deletar o item do inventario
                        std::string nomeItem = item->obterNomeItem(); 
                        int cura = static_cast<int>(jogador->obterVidaMaxima() * 0.30);
                        
                        jogador->modificarVida(cura); 
                        jogador->obterInventario()->removerPorNome(nomeItem);
                        
                        std::cout << "\n[SISTEMA]: " << nomeItem << " usada! +" << cura << " HP.\n";
                        Menu::esperar();
                        acaoConsumiuTurno = true;
                    }
                }
            }
            else if (acao == 4) // --- 4. JOGADOR  ---
            {
                Menu::limparTela();
                std::cout << "========== FICHA DETALHADA DO JOGADOR ==========\n\n";
                std::cout << " NOME:           " << jogador->obterNome() << "\n";
                std::cout << " RACA:           " << jogador->obterRaca()->obterNomeRaca() << "\n";
                std::cout << " CLASSE:         " << jogador->obterNomeClasse() << "\n";
                std::cout << " OURO:           " << jogador->obterInventario()->obterOuro() << "G\n\n";
                
                std::cout << " PASSIVA RACA:   " << jogador->obterRaca()->obterNomeHabilidadeRaca() << "\n";
                std::cout << " -> " << jogador->obterRaca()->obterDescricaoHabilidadeRaca() << "\n\n";
                std::cout << " ATIVA CLASSE:   " << jogador->obterClasse()->obterNomeHabilidadeClasseAtiva() << "\n";
                std::cout << " -> " << jogador->obterClasse()->obterDescricaoHabilidadeClasseAtiva() << "\n\n";
                
                double m = jogador->obterMultiplicador();
                int t = jogador->obterTurnosBuff();

                // Formato: Nome: Base (Total){xMult} por T turnos
                auto exibirAtributo = [&](std::string nome, int valorBase) 
                {
                    std::cout << " - " << nome << ": " << valorBase;
                    if (t > 0) 
                    {
                        int total = static_cast<int>(valorBase * m);
                        std::cout << " (" << total << "){x" << m << "} por " << t << " turnos\n";
                    } 
                    else 
                    {
                        std::cout << " (0)\n"; // Indica ausencia de buff
                    }
                };

                std::cout << " ATRIBUTOS TOTAIS:\n";
                std::cout << " - HP: " << jogador->obterVida() << "/" << jogador->obterVidaMaxima() << " (0)\n";
                exibirAtributo("Forca", jogador->obterForca());
                exibirAtributo("Resistencia", jogador->obterResistencia());
                exibirAtributo("Constituicao", jogador->obterConstituicao());
                exibirAtributo("Inteligencia", jogador->obterInteligencia());
                exibirAtributo("Sabedoria", jogador->obterSabedoria());
                
                Menu::esperar();
            }
            
            if (verificarVitoria()) return; 
        }

        executarTurnoInimigos();
        contadorTurno++;
    }
}

void SistemaRPG::executarTurnoInimigos() 
{
    // Bardo: Flashing Lights pula o turno
    if (jogador->obterPularTurnoInimigo()) 
    {
        std::cout << "\n[EFEITO]: Os inimigos estao atordoados e nao podem agir!\n";
        jogador->definirPularTurnoInimigo(false); 
        Menu::esperar();
        return;
    }

    std::cout << "\n--- TURNO DOS INIMIGOS ---" << std::endl;
    for (Personagem* inimigo : inimigos) 
    {
        if (jogador->obterVida() > 0) 
        {
            realizarAtaqueFisico(inimigo, jogador, contadorTurno);
        }
    }

    // Gerencia a duracao do buff acumulativo
    if (jogador->obterTurnosBuff() > 0) 
    {
        jogador->definirTurnosBuff(jogador->obterTurnosBuff() - 1);
        
        if (jogador->obterTurnosBuff() <= 0) 
        {
            jogador->definirMultiplicador(1.0); // Reseta multiplicador ao expirar
            std::cout << "\n[SISTEMA]: O efeito da habilidade expirou!\n";
        }
    }

    if (jogador->obterRecarga()) jogador->definirRecarga(false);
    Menu::esperar();
}

void SistemaRPG::realizarAtaqueFisico(Personagem* atacante, Personagem* defensor, int turnoAtual) 
{
    double multBuff = atacante->obterMultiplicador(); // Aplica o multiplicador acumulado

    if (atacante->obterInviolavel() && atacante->obterNomeClasse() == "Arqueiro") 
    {
        multBuff *= 2.0; // Bonus fixo do Arqueiro
        atacante->definirInviolavel(false);
    }

    int dArma = (atacante->obterArma()) ? atacante->obterArma()->obterBonusDano() : 0;
    int danoBase = static_cast<int>((atacante->obterForca() + dArma) * multBuff);

    // Passivas de raca
    danoBase = atacante->obterRaca()->processarDanoOfensivo(danoBase, atacante);

    if (atacante->obterModoAtaqueArea() && atacante->obterNomeClasse() == "Mago" && !inimigos.empty()) 
    {
        int danoDiv = danoBase / static_cast<int>(inimigos.size());
        std::cout << atacante->obterNome() << " desfere um ataque em area!\n";
        for (Personagem* ini : inimigos) aplicarDano(ini, danoDiv, turnoAtual);
    } 
    else if (defensor != nullptr) 
    {
        std::cout << atacante->obterNome() << " ataca " << defensor->obterNome() << "!" << std::endl;
        aplicarDano(defensor, danoBase, turnoAtual);
    }
}

void SistemaRPG::aplicarDano(Personagem* alvo, int danoBruto, int turnoAtual) 
{
    if (alvo->obterInviolavel()) 
    {
        std::cout << ">> " << alvo->obterNome() << " esquivou totalmente!\n";
        return;
    }

    int bArmadura = (alvo->obterArmadura()) ? alvo->obterArmadura()->obterReducaoFixa() : 0;
    double tRedFixa = (0.2 * alvo->obterResistencia()) + bArmadura;
    double bEscudo = (alvo->obterEscudo()) ? alvo->obterEscudo()->obterReducaoPercentual() : 0.0;
    double tRedPerc = ((alvo->obterConstituicao() / 3.0) / 100.0) + bEscudo;

    int dFinal = static_cast<int>((danoBruto - tRedFixa) * (1.0 - tRedPerc));
    if (dFinal < 1) dFinal = 1;

    // Passivas defensivas de raca (Dwarf/Humano)
    dFinal = alvo->obterRaca()->processarDanoDefensivo(dFinal, alvo);

    if (dFinal > 0) 
    {
        alvo->modificarVida(-dFinal);
        std::cout << ">> " << alvo->obterNome() << " recebeu " << dFinal << " de dano." << std::endl;
    }
}

bool SistemaRPG::verificarVitoria() 
{
    if (inimigos.empty()) { std::cout << "\n[VITORIA] Inimigos eliminados!\n"; Menu::esperar(); return true; }
    if (jogador->obterVida() <= 0) { std::cout << "\n[DERROTA] O heroi caiu...\n"; Menu::esperar(); return true; }
    return false;
}