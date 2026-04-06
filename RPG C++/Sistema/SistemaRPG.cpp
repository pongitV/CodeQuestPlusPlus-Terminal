#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <ctime>

#include "SistemaRPG.h"
#include "Menu.h"

#include "../Raças/RacaBase.h"
#include "../Classes/ClasseBase.h"
#include "../Inventario/Item.h"

SistemaRPG::SistemaRPG(Personagem* jogador, std::vector<Personagem*> inimigos) 
    : jogador(jogador), inimigos(inimigos), contadorTurno(1), ouroObtido(0), danoCausadoTotal(0), danoRecebidoTotal(0)
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
            std::string tituloCombate = "EM COMBATE (";
            for (size_t i = 0; i < inimigos.size(); ++i) {
                tituloCombate += inimigos[i]->obterNome();
                if (i < inimigos.size() - 1) tituloCombate += ", ";
            }
            tituloCombate += ")";

            Menu::limparTela();
            Menu::exibirLogo(tituloCombate);
            Menu::exibirHorda(inimigos);
            Menu::exibirStatusJogador(jogador); 

            int largura = Menu::obterLarguraTerminal();
            std::string textoTurno = "TURNO " + std::to_string(contadorTurno) + " | SUA VEZ";
            int espTurno = (largura - (int)textoTurno.length()) / 2;
            std::cout << "\n" << std::string(espTurno > 0 ? espTurno : 0, ' ') << textoTurno << "\n";

            std::string textoEscolha = "1. Atacar | 2. Defender | 3. Habilidade | 4. Inventario | 5. Jogador | Escolha: ";
            int espEscolha = (largura - (int)textoEscolha.length()) / 2;
            std::cout << std::string(espEscolha > 0 ? espEscolha : 0, ' ') << textoEscolha;
            
            int acao;
            if (!(std::cin >> acao)) 
            {
                std::cin.clear();
                std::cin.ignore(1000, '\n');
                continue;
            }

            switch (acao) 
            {
            case 1: // --- 1. ATACAR ---
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
                    if (!(std::cin >> alvoIndex) || alvoIndex < 0 || alvoIndex >= static_cast<int>(inimigos.size())) 
                    {
                        std::cin.clear(); 
                        std::cin.ignore(1000, '\n');
                        std::cout << "\n[ERRO] Alvo invalido!\n";
                        Menu::esperar();
                        break; // Volta para o menu de acoes sem gastar o turno
                    }

                    realizarAtaqueFisico(jogador, inimigos[alvoIndex], contadorTurno);
                    acaoConsumiuTurno = true;
                }

                // Limpeza de mortos apos o ataque
                for (auto it = inimigos.begin(); it != inimigos.end(); ) 
                {
                    if ((*it)->obterVida() <= 0) 
                    {
                        int ouro = (*it)->obterOuroRecompensa();
                        jogador->ganharOuro(ouro);
                        ouroObtido += ouro;
                        std::cout << "\n[!] " << (*it)->obterNome() << " derrotado! +" << ouro << "G\n";
                        delete *it;
                        it = inimigos.erase(it);
                        Menu::esperar();
                    } else { ++it; }
                }
                break;
            }
            case 2: // --- 2. DEFENDER ---
            {
                if (jogador->obterRecargaDefesa()) 
                {
                    std::cout << "\n[ERRO]: Voce se desequilibrou e precisa de 1 turno para poder defender novamente!\n";
                    Menu::esperar();
                    break; // Volta pro loop de acao sem gastar turno
                }
                
                Item* esc = jogador->obterInventario()->escolherEscudoParaDefesa();
                if (esc != nullptr) 
                {
                    jogador->equiparItem(esc);
                    jogador->definirDefendendo(true);
                    std::cout << "\n[SISTEMA]: " << jogador->obterNome() << " assumiu uma postura defensiva com " << esc->obterNomeItem() << "!\n";
                    Menu::esperar();
                    acaoConsumiuTurno = true;
                }
                break;
            }
            case 3: // --- 3. HABILIDADE ATIVA ---
            {
                jogador->obterClasse()->usarHabilidadeClasseAtiva(jogador, inimigos);
                
                if (jogador->obterNomeClasse() != "Mago") acaoConsumiuTurno = true;
                else Menu::esperar();
                break;
            }
            case 4: // --- 4. INVENTARIO ---
            {
                std::string codigo;
                do 
                {
                    Menu::exibirInventario(jogador);
                    std::string msg = "Digite o codigo do item ou '0' para voltar: ";
                    int largura = Menu::obterLarguraTerminal();
                    int esp = (largura - (int)msg.length()) / 2;
                    std::cout << "\n" << std::string(esp > 0 ? esp : 0, ' ') << msg;
                    std::cin >> codigo;
    
                    if (codigo != "0")
                    {
                        Item* item = jogador->obterInventario()->buscarItemPorCodigo(
                            codigo, jogador->obterArma(), jogador->obterEscudo(), jogador->obterArmadura()
                        );
    
                        if (item && dynamic_cast<PocaoCura*>(item))
                        {
                            if (acaoConsumiuTurno) 
                            {
                                std::cout << "\n[SISTEMA]: Voce ja usou um item neste turno!\n";
                                Menu::esperar();
                            } 
                            else 
                            {
                                std::string nomeItem = item->obterNomeItem(); 
                                int cura = static_cast<int>(jogador->obterVidaMaxima() * 0.30);
                                
                                jogador->modificarVida(cura); 
                                jogador->obterInventario()->removerPorNome(nomeItem);
                                
                                std::cout << "\n[SISTEMA]: " << nomeItem << " usada! +" << cura << " HP.\n";
                                Menu::esperar();
                                acaoConsumiuTurno = true;
                            }
                        }
                        else if (item)
                        {
                            std::cout << "\n[SISTEMA]: Este item nao pode ser usado em combate!\n";
                            Menu::esperar();
                        }
                    }
                } while (codigo != "0");
                break;
            }
            case 5: // --- 5. JOGADOR ---
            {
                std::string opcao;
                do 
                {
                    Menu::exibirFichaJogador(jogador);
                    std::string msg = "Digite '0' para voltar: ";
                    int largura = Menu::obterLarguraTerminal();
                    int esp = (largura - (int)msg.length()) / 2;
                    std::cout << "\n" << std::string(esp > 0 ? esp : 0, ' ') << msg;
                    std::cin >> opcao;
                } while (opcao != "0");
                break;
            }
            default:
                break;
            }
            
            if (verificarVitoria()) return; 
        }

        executarTurnoInimigos();
        if (verificarVitoria()) return;
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
    }
    else
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

    // Reseta o estado da defesa APOS os inimigos atacarem
    if (jogador->obterDefendendo()) {
        jogador->definirDefendendo(false);
        jogador->definirRecargaDefesa(true); // Entra em Cooldown
    } else if (jogador->obterRecargaDefesa()) {
        jogador->definirRecargaDefesa(false); // Resfria o Cooldown
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
    
    int atributoBase = atacante->obterForca();
    if (atacante->obterNomeClasse() == "Mago") {
        atributoBase = atacante->obterInteligencia();
    } else if (atacante->obterNomeClasse() == "Bardo") {
        atributoBase = atacante->obterSabedoria();
    } else if (atacante->obterNomeClasse() == "Arqueiro") {
        atributoBase = atacante->obterDestreza();
    }

    int danoBase = static_cast<int>((atributoBase + dArma) * multBuff);

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
    double tRedPerc = ((alvo->obterConstituicao() / 3.0) / 100.0);

    int dFinal = static_cast<int>((danoBruto - tRedFixa) * (1.0 - tRedPerc));
    if (dFinal < 1) dFinal = 1; // Impacto basico minimo

    // Logica Ativa do Escudo
    if (alvo->obterDefendendo() && alvo->obterEscudo() != nullptr) 
    {
        int reducaoFixo = alvo->obterEscudo()->obterReducaoFixaEscudo();
        std::cout << ">> [DEFESA]: O escudo bloqueou " << reducaoFixo << " de dano!\n";
        dFinal -= reducaoFixo;
        if (dFinal < 0) dFinal = 0; // Escudo pode zerar completamente o dano
        
        alvo->obterEscudo()->reduzirDurabilidade(1);
        if (alvo->obterEscudo()->obterDurabilidade() <= 0) 
        {
            std::cout << "[!] ALERTA: O escudo " << alvo->obterEscudo()->obterNomeItem() << " foi DESTRUIDO em pedacos!\n";
            alvo->obterInventario()->removerPorNome(alvo->obterEscudo()->obterNomeItem());
            alvo->desequiparEscudo(); // Fica sem escudo
        }
    }

    // Passivas defensivas de raca (Dwarf/Humano)
    dFinal = alvo->obterRaca()->processarDanoDefensivo(dFinal, alvo);

    if (dFinal > 0) 
    {
        alvo->modificarVida(-dFinal);
        
        if (alvo == jogador) {
            danoRecebidoTotal += dFinal;
        } else {
            danoCausadoTotal += dFinal;
        }
        
        std::cout << ">> " << alvo->obterNome() << " recebeu " << dFinal << " de dano." << std::endl;
    }
    else if (dFinal == 0 && alvo->obterDefendendo()) 
    {
        std::cout << ">> O dano foi totalmente absorvido pela sua defesa!" << std::endl;
    }
}

bool SistemaRPG::verificarVitoria() 
{
    if (inimigos.empty()) 
    { 
        Menu::exibirTelaVitoria(jogador, ouroObtido, danoCausadoTotal, danoRecebidoTotal);
        return true; 
    }
    if (jogador->obterVida() <= 0) 
    { 
        Menu::exibirTelaDerrota(jogador, ouroObtido, danoCausadoTotal, danoRecebidoTotal); 
        return true; 
    }
    return false;
}