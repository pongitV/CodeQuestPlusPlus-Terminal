#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <ctime>
#include <chrono>
#include <thread>

#ifdef _WIN32
    #include <conio.h>
#endif

#include "SistemaRPG.h"
#include "Menu.h"

#include "../Raças/RacaBase.h"
#include "../Classes/ClasseBase.h"
#include "../Inventario/Item.h"

SistemaRPG::SistemaRPG(Personagem* jogador, std::vector<Personagem*> inimigos) 
    : jogador(jogador), inimigos(inimigos), contadorTurno(1), ouroObtido(0), xpObtido(0), danoCausadoTotal(0), danoRecebidoTotal(0)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int dif = jogador->obterDificuldade();
    double multiplicador = 1.0;
    if (dif == 2) { // Normal
        multiplicador = 1.5;
    } else if (dif == 3) { // Dificil
        multiplicador = 2.0;
    }

    for (Personagem* ini : this->inimigos) {
        ini->aplicarMultiplicadorDificuldade(multiplicador);
    }
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
            Menu::exibirLogoCombate(tituloCombate);
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
                        int xp = (*it)->obterXpRecompensa();
                        jogador->ganharOuro(ouro);
                        jogador->ganharXp(xp);
                        ouroObtido += ouro;
                        xpObtido += xp;
                        std::cout << "\n[!] " << (*it)->obterNome() << " derrotado! +" << ouro << "G | +" << xp << " XP\n";
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
                    std::string msg = "Digite o codigo do item ou [0] VOLTAR: ";
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
                    std::string msg = "[0] VOLTAR (combate) | [1] ALTERAR PARRY";
                    if (jogador->podeSubirDeNivel()) msg += " | [2] SUBIR DE NIVEL";
                    msg += ": ";
                    int largura = Menu::obterLarguraTerminal();
                    int esp = (largura - (int)msg.length()) / 2;
                    std::cout << "\n" << std::string(esp > 0 ? esp : 0, ' ') << msg;
                    std::cin >> opcao;

                    if (opcao == "1") {
                        jogador->definirParryAtivado(!jogador->obterParryAtivado());
                    } else if (opcao == "2" && jogador->podeSubirDeNivel()) {
                        std::string attr;
                        std::cout << "\nDigite o atributo para melhorar (Vida, Forca, Destreza, Resistencia, Constituicao, Inteligencia, Sabedoria): ";
                        std::cin >> attr;
                        if (jogador->subirDeNivel(attr)) {
                            std::cout << "[SISTEMA]: Nivel subiu! " << attr << " melhorado.\n";
                            Menu::esperar();
                        } else {
                            std::cout << "[ERRO]: Atributo invalido.\n";
                            Menu::esperar();
                        }
                    }
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
                // Na Dificuldade Dificil(3), os inimigos poderao ativar suas habilidades de Classe
                if (jogador->obterDificuldade() == 3) 
                {
                    std::vector<Personagem*> alvos = { jogador };
                    inimigo->obterClasse()->usarHabilidadeClasseAtiva(inimigo, alvos);
                }
                
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

    // Passivas de raca somente aplicam no Normal(2) ou Dificil(3), ou se for o Jogador
    if (atacante == jogador || jogador->obterDificuldade() >= 2) {
        danoBase = atacante->obterRaca()->processarDanoOfensivo(danoBase, atacante);
    }

    if (atacante->obterModoAtaqueArea() && atacante->obterNomeClasse() == "Mago" && !inimigos.empty()) 
    {
        int danoDiv = danoBase / static_cast<int>(inimigos.size());
        std::cout << atacante->obterNome() << " desfere um ataque em area!\n";
        for (Personagem* ini : inimigos) aplicarDano(atacante, ini, danoDiv, turnoAtual);
    } 
    else if (defensor != nullptr) 
    {
        std::cout << atacante->obterNome() << " ataca " << defensor->obterNome() << "!" << std::endl;
        aplicarDano(atacante, defensor, danoBase, turnoAtual);
    }
}

void SistemaRPG::aplicarDano(Personagem* atacante, Personagem* alvo, int danoBruto, int turnoAtual) 
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

    // Logica do Parry
    if (alvo == jogador && jogador->obterParryAtivado()) 
    {
        int qtdNumeros = std::max(1, dFinal / 4);
        int destrezaAtacante = std::max(1, atacante->obterDestreza());
        int tempoLimite = std::max(1, 50 / destrezaAtacante);

        int reducaoParry = 0;
        bool sucesso = executarParry(qtdNumeros, tempoLimite, reducaoParry);
        if (sucesso) 
        {
            // Aplica o limite de 50% de reducao do dano (com um minimo de 1 para danos baixos)
            int limiteReducao = std::max(1, dFinal / 2);
            if (reducaoParry > limiteReducao) {
                reducaoParry = limiteReducao;
            }

            dFinal -= reducaoParry;
            if (dFinal <= 0) 
            {
                dFinal = 0;
                std::cout << ">> [PARRY]: Parry perfeito, reducao total!\n";
            } 
            else 
            {
                std::cout << ">> [PARRY]: Parry efetivo, dano reduzido em " << reducaoParry << "\n";
            }
        }
        else 
        {
            std::cout << ">> [PARRY]: FALHA! A defesa falhou!\n";
        }
    }

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

    // Passivas defensivas de raca (Dwarf/Humano) somente aplicam no Normal(2) ou Dificil(3), ou se for o Jogador
    if (alvo == jogador || jogador->obterDificuldade() >= 2) {
        dFinal = alvo->obterRaca()->processarDanoDefensivo(dFinal, alvo);
    }

    if (dFinal > 0) 
    {
        alvo->modificarVida(-dFinal);
        
        if (alvo == jogador) {
            danoRecebidoTotal += dFinal;
        } else {
            danoCausadoTotal += dFinal;
        }
        
        if (alvo == jogador) {
            danoRecebidoTotal += dFinal;
            std::cout << "\033[31m>> " << alvo->obterNome() << " recebeu " << dFinal << " de dano.\033[0m" << std::endl;
        } else {
            danoCausadoTotal += dFinal;
            std::cout << "\033[33m>> " << alvo->obterNome() << " recebeu " << dFinal << " de dano.\033[0m" << std::endl;
        }
    }
    else if (dFinal == 0 && alvo->obterDefendendo()) 
    {
        std::cout << ">> O dano foi totalmente absorvido pela sua defesa!" << std::endl;
    }
}

bool SistemaRPG::executarParry(int qtdNumeros, int tempoLimite, int& reducao) 
{
    std::string sequencia = "";
    reducao = 0;
    for (int i = 0; i < qtdNumeros; ++i) {
        int num = (std::rand() % 9) + 1; // 1 to 9
        sequencia += std::to_string(num);
        reducao += num;
    }

    std::cout << "\n[PARRY] O inimigo ataca! Digite a sequencia rapidamente para defender!\n";
    std::cout << "[PARRY] Sequencia: " << sequencia << "\n";
    std::cout << "[PARRY] Tempo Limite: " << tempoLimite << " segundos!\n";
    std::cout << "[PARRY] Digite: ";

    std::string entrada = "";
    
#ifdef _WIN32
    auto start = std::chrono::steady_clock::now();
    while (true) {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - start;
        if (elapsed.count() > tempoLimite) {
            std::cout << "\n[PARRY] TEMPO ESGOTADO!\n";
            return false;
        }
        if (_kbhit()) 
        {
            char c = _getch();
            if (c == '\r' || c == '\n') {
                std::cout << "\n";
                break;
            } else if (c == '\b' || c == 127) {
                if (!entrada.empty()) 
                {
                    entrada.pop_back();
                    std::cout << "\b \b";
                }
            } else {
                entrada += c;
                std::cout << c;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
#else
    auto start = std::chrono::steady_clock::now();
    std::cin >> entrada;
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = now - start;
    if (elapsed.count() > tempoLimite) {
        std::cout << "\n[PARRY] TEMPO ESGOTADO (" << elapsed.count() << "s)!\n";
        return false;
    }
#endif

    return entrada == sequencia;
}

bool SistemaRPG::verificarVitoria() 
{
    if (inimigos.empty()) 
    { 
        Menu::exibirTelaVitoria(jogador, ouroObtido, xpObtido, danoCausadoTotal, danoRecebidoTotal);
        return true; 
    }
    if (jogador->obterVida() <= 0) 
    { 
        Menu::exibirTelaDerrota(jogador, ouroObtido, xpObtido, danoCausadoTotal, danoRecebidoTotal); 
        return true; 
    }
    return false;
}