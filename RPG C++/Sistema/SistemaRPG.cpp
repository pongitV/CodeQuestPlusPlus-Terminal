#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

#include "SistemaRPG.h"
#include "Menu.h"

// Inclusao necessaria para acessar as passivas e nomes
#include "../Raças/RacaBase.h"
#include "../Classes/ClasseBase.h"

SistemaRPG::SistemaRPG(Personagem* jogador, std::vector<Personagem*> inimigos) 
    : jogador(jogador), inimigos(inimigos), contadorTurno(1) 
{
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
            // Opcao 4 definida como Jogador
            std::cout << "1. Atacar | 2. Habilidade | 3. Inventario | 4. Jogador | Escolha: ";
            
            int acao;
            if (!(std::cin >> acao)) 
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if (acao == 1) // --- ATACAR ---
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
            else if (acao == 2) // --- HABILIDADE ---
            {
                usarHabilidadeClasse(jogador, jogador);
                acaoConsumiuTurno = true;
                Menu::esperar();
            }
            else if (acao == 3) // --- INVENTARIO ---
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
                        std::string nome = item->obterNomeItem();
                        int cura = static_cast<int>(jogador->obterVidaMaxima() * 0.30);
                        jogador->modificarVida(cura); 
                        jogador->obterInventario()->removerPorNome(nome);
                        std::cout << "\n[SISTEMA]: " << nome << " usada! +" << cura << " HP.\n";
                        Menu::esperar();
                        acaoConsumiuTurno = true;
                    }
                    else { std::cout << "[ERRO]: Item invalido para combate.\n"; Menu::esperar(); }
                }
            }
            else if (acao == 4) // --- FICHA DO JOGADOR ---
            {
                Menu::limparTela();
                std::cout << "========== FICHA DETALHADA DO JOGADOR ==========\n\n";
                std::cout << " NOME:           " << jogador->obterNome() << "\n";
                std::cout << " RACA:           " << jogador->obterRaca()->obterNomeRaca() << "\n";
                std::cout << " CLASSE:         " << jogador->obterNomeClasse() << "\n";
                std::cout << " OURO:           " << jogador->obterInventario()->obterOuro() << "G\n\n";
                std::cout << " PASSIVA:        " << jogador->obterRaca()->obterNomeHabilidade() << "\n";
                std::cout << " EFEITO:         " << jogador->obterRaca()->obterDescricaoHabilidade() << "\n\n";
                std::cout << " ATRIBUTOS:\n";
                std::cout << " - HP:           " << jogador->obterVida() << "/" << jogador->obterVidaMaxima() << "\n";
                std::cout << " - Forca:        " << jogador->obterForca() << "\n";
                std::cout << " - Resistencia:  " << jogador->obterResistencia() << "\n";
                std::cout << " - Constituicao: " << jogador->obterConstituicao() << "\n";
                std::cout << " - Inteligencia: " << jogador->obterInteligencia() << "\n";
                std::cout << " - Sabedoria:    " << jogador->obterSabedoria() << "\n";
                Menu::esperar();
            }
            if (verificarVitoria()) return; 
        }
        executarTurnoInimigos();
        contadorTurno++;
    }
}

// Resolucao do erro de Linker
void SistemaRPG::executarTurnoInimigos() 
{
    std::cout << "\n--- TURNO DOS INIMIGOS ---" << std::endl;
    for (Personagem* inimigo : inimigos) 
    {
        if (jogador->obterVida() > 0) realizarAtaqueFisico(inimigo, jogador, contadorTurno);
    }
    Menu::esperar();
}

void SistemaRPG::realizarAtaqueFisico(Personagem* atacante, Personagem* defensor, int turnoAtual) 
{
    int dArma = (atacante->obterArma()) ? atacante->obterArma()->obterBonusDano() : 0;
    double multiplicador = 1.0;

    // PASSIVA ELFO: Agil e preciso (33% chance de 1.5x dano)
    if (atacante->obterRaca()->obterNomeRaca() == "Elfo") 
    {
        if ((rand() % 100) < 33) 
        {
            multiplicador = 1.5;
            std::cout << "[PASSIVA]: Agil e preciso! Golpe critico.\n";
        }
    }

    int danoBase = static_cast<int>((atacante->obterForca() + dArma) * multiplicador);

    // PASSIVA ORK: Furia cega (Dano extra por vida perdida)
    if (atacante->obterRaca()->obterNomeRaca() == "Ork") 
    {
        double vidaPerdidaPerc = 1.0 - (static_cast<double>(atacante->obterVida()) / atacante->obterVidaMaxima());
        int danoExtra = static_cast<int>(danoBase * vidaPerdidaPerc);
        if (danoExtra > 0) 
        {
            danoBase += danoExtra;
            std::cout << "[PASSIVA]: Furia cega aumentou o dano em " << danoExtra << "!\n";
        }
    }

    std::cout << atacante->obterNome() << " ataca " << defensor->obterNome() << "!" << std::endl;
    aplicarDano(defensor, danoBase, turnoAtual);
}

void SistemaRPG::aplicarDano(Personagem* alvo, int danoBruto, int turnoAtual) 
{
    int bArmadura = (alvo->obterArmadura()) ? alvo->obterArmadura()->obterReducaoFixa() : 0;
    double tRedFixa = (0.2 * alvo->obterResistencia()) + bArmadura;
    
    double bEscudo = (alvo->obterEscudo()) ? alvo->obterEscudo()->obterReducaoPercentual() : 0.0;

    // PASSIVA DWARF: Forjado com determinacao (Dobro de eficacia de escudos)
    if (alvo->obterRaca()->obterNomeRaca() == "Dwarf" && bEscudo > 0) 
    {
        bEscudo *= 2.0;
        if (bEscudo > 0.9) bEscudo = 0.9; // Limite de protecao
        std::cout << "[PASSIVA]: Forjado com determinacao absorveu mais dano!\n";
    }

    double tRedPerc = ((alvo->obterConstituicao() / 3.0) / 100.0) + bEscudo;

    int dFinal = static_cast<int>((danoBruto - tRedFixa) * (1.0 - tRedPerc));
    if (dFinal < 1) dFinal = 1;

    alvo->modificarVida(-dFinal);
    std::cout << ">> " << alvo->obterNome() << " recebeu " << dFinal << " de dano." << std::endl;

    // PASSIVA HUMANO: Espirito indomavel (Revive uma vez com 1/2 HP)
    if (alvo->obterVida() <= 0 && alvo->obterRaca()->obterNomeRaca() == "Humano " && alvo->podeUsarRessurreicao()) 
    {
        int curaReviver = alvo->obterVidaMaxima() / 2;
        alvo->modificarVida(curaReviver);
        alvo->consumirRessurreicao();
        std::cout << "[PASSIVA]: Espirito indomavel! O humano reviveu com metade do HP!\n";
    }
}

void SistemaRPG::usarHabilidadeClasse(Personagem* usuario, Personagem* alvo) 
{ 
    usuario->usarHabilidadeDeClasse(alvo); 
}

bool SistemaRPG::verificarVitoria() 
{
    if (inimigos.empty()) { std::cout << "\n[VITORIA] Inimigos eliminados!\n"; Menu::esperar(); return true; }
    if (jogador->obterVida() <= 0) { std::cout << "\n[DERROTA] O heroi caiu...\n"; Menu::esperar(); return true; }
    return false;
}