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

#include "../Interfaces/TelaVitoria.h"
#include "../Interfaces/TelaDerrota.h"

static std::vector<Personagem*> inimigosPresosComCipos;
static int danoMagicoPerfuranteAtual = 0;

SistemaRPG::SistemaRPG(Personagem* jogadorParaOCombate, std::vector<Personagem*> inimigosParaOCombate) 
    : jogadorAtual(jogadorParaOCombate), listaDeInimigos(inimigosParaOCombate), contadorDoTurnoAtual(1), quantidadeDeOuroObtido(0), quantidadeDeXpObtido(0), totalDeDanoCausado(0), totalDeDanoRecebido(0)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int nivelDeDificuldade = jogadorAtual->obterDificuldade();
    double multiplicadorDeDificuldadeDosInimigos = 1.0;
    
    if (nivelDeDificuldade == 2) { // Normal
        multiplicadorDeDificuldadeDosInimigos = 1.5;
    } 
    else if (nivelDeDificuldade == 3) { // Dificil
        multiplicadorDeDificuldadeDosInimigos = 2.0;
    }

    for (Personagem* inimigoAtual : this->listaDeInimigos) 
    {
        inimigoAtual->aplicarMultiplicadorDificuldade(multiplicadorDeDificuldadeDosInimigos);
    }
}

SistemaRPG::~SistemaRPG() 
{
    for (Personagem* inimigoAtual : listaDeInimigos) delete inimigoAtual;
    listaDeInimigos.clear();
}

void SistemaRPG::iniciarCombate() 
{
    inimigosPresosComCipos.clear();
    int maxDestrezaInimigos = 0;
    for (Personagem* inimigo : listaDeInimigos) 
    {
        if (inimigo->obterDestreza() > maxDestrezaInimigos) maxDestrezaInimigos = inimigo->obterDestreza();
    }
    
    bool turnoExtraFirstTurn = false;
    if (jogadorAtual->obterDestreza() > (maxDestrezaInimigos * 2)) 
    {
        turnoExtraFirstTurn = true;
    }
    
    // Destreza decide quem ataca primeiro no combate
    if (maxDestrezaInimigos > jogadorAtual->obterDestreza()) 
    {
        Menu::limparTelaDoTerminal();
        Menu::exibirLogoParaTelaDeCombate("EM COMBATE");
        std::cout << "\n[SISTEMA]: Os inimigos sao mais ageis e atacam primeiro!\n";
        Menu::aguardarPressionamentoDeEnter();
        executarTurnoDeTodosOsInimigos();
        if (verificarCondicaoDeVitoriaOuDerrota()) return;
    }

    while (jogadorAtual->obterVida() > 0 && !listaDeInimigos.empty()) 
    {
        // --- PROCESSAMENTO DE DEBUFFS DO JOGADOR NO INICIO DO TURNO ---
        if (jogadorAtual->obterSangramento()) 
        {
            int danoSangramento = std::max(1, jogadorAtual->obterVidaMaxima() / 10);
            jogadorAtual->modificarVida(-danoSangramento);
            
            Menu::limparTelaDoTerminal();
            std::cout << "\n\033[31m[EFEITO]: Voce perdeu " << danoSangramento << " HP devido ao sangramento! (" << jogadorAtual->obterTurnosSangramento() - 1 << " turnos restantes)\033[0m\n";
            
            jogadorAtual->definirTurnosSangramento(jogadorAtual->obterTurnosSangramento() - 1);
            if (jogadorAtual->obterTurnosSangramento() <= 0) {
                jogadorAtual->definirSangramento(false);
                std::cout << "\033[31m[EFEITO]: O seu sangramento parou.\033[0m\n";
            }
            if (verificarCondicaoDeVitoriaOuDerrota()) return;
            Menu::aguardarPressionamentoDeEnter();
        }

        if (jogadorAtual->obterLentidao()) 
        {
            jogadorAtual->definirTurnosLentidao(jogadorAtual->obterTurnosLentidao() - 1);
            if (jogadorAtual->obterTurnosLentidao() <= 0) 
            {
                jogadorAtual->removerLentidaoEstatistica();
                Menu::limparTelaDoTerminal();
                std::cout << "\n\033[35m[EFEITO]: Voce se livrou da gosma e recuperou sua agilidade.\033[0m\n";
                Menu::aguardarPressionamentoDeEnter();
            }
        }
        // --- FIM DO PROCESSAMENTO DE DEBUFFS ---

        bool turnoFoiConsumido = false;

        while (!turnoFoiConsumido) 
        {
            std::string tituloDoCombate = "EM COMBATE (";
            for (size_t indice = 0; indice < listaDeInimigos.size(); ++indice) {
                tituloDoCombate += listaDeInimigos[indice]->obterNome();
                if (indice < listaDeInimigos.size() - 1) tituloDoCombate += ", ";
            }
            tituloDoCombate += ")";

            Menu::limparTelaDoTerminal();
            Menu::exibirLogoParaTelaDeCombate(tituloDoCombate);
            Menu::exibirHordaDeInimigosLadoALado(listaDeInimigos);
            Menu::exibirBarraDeStatusDoJogador(jogadorAtual); 

            int larguraDoTerminal = Menu::obterLarguraDoTerminalEmColunas();
            std::string textoDoTurno = "TURNO " + std::to_string(contadorDoTurnoAtual) + " | SUA VEZ";
            int espacosParaCentralizarTurno = (larguraDoTerminal - (int)textoDoTurno.length()) / 2;
            std::cout << "\n" << std::string(espacosParaCentralizarTurno > 0 ? espacosParaCentralizarTurno : 0, ' ') << textoDoTurno << "\n";

            std::string textoDeEscolhaDasAcoes = "1. Atacar | 2. Defender | 3. Habilidade | 4. Inventario | 5. Jogador | Escolha: ";
            int espacosParaCentralizarEscolha = (larguraDoTerminal - (int)textoDeEscolhaDasAcoes.length()) / 2;
            std::cout << std::string(espacosParaCentralizarEscolha > 0 ? espacosParaCentralizarEscolha : 0, ' ') << textoDeEscolhaDasAcoes;
            
            int acaoEscolhida;
            if (!(std::cin >> acaoEscolhida)) 
            {
                std::cin.clear();
                std::cin.ignore(1000, '\n');
                continue;
            }

            switch (acaoEscolhida) 
            {
            case 1: // --- 1. ATACAR ---
            {
                if (jogadorAtual->obterModoAtaqueArea() && jogadorAtual->obterNomeClasse() == "Mago") 
                {
                    realizarAtaqueFisico(jogadorAtual, nullptr, contadorDoTurnoAtual);
                    turnoFoiConsumido = true;
                }
                else 
                {
                    int indiceDoAlvoEscolhido;
                    std::cout << "Escolha o alvo (0 a " << listaDeInimigos.size() - 1 << "): ";
                    if (!(std::cin >> indiceDoAlvoEscolhido) || indiceDoAlvoEscolhido < 0 || indiceDoAlvoEscolhido >= static_cast<int>(listaDeInimigos.size())) 
                    {
                        std::cin.clear(); 
                        std::cin.ignore(1000, '\n');
                        std::cout << "\n[ERRO] Alvo invalido!\n";
                        Menu::aguardarPressionamentoDeEnter();
                        break; 
                    }

                    realizarAtaqueFisico(jogadorAtual, listaDeInimigos[indiceDoAlvoEscolhido], contadorDoTurnoAtual);
                    turnoFoiConsumido = true;
                }

                // Limpeza de mortos apos o ataque
                for (auto iteradorInimigos = listaDeInimigos.begin(); iteradorInimigos != listaDeInimigos.end(); ) 
                {
                    if ((*iteradorInimigos)->obterVida() <= 0) 
                    {
                        processarMorteDeInimigo(*iteradorInimigos);
                        delete *iteradorInimigos;
                        iteradorInimigos = listaDeInimigos.erase(iteradorInimigos);
                        Menu::aguardarPressionamentoDeEnter();
                    } else { ++iteradorInimigos; }
                }
                break;
            }
            case 2: // --- 2. DEFENDER ---
            {
                if (jogadorAtual->obterRecargaDefesa()) 
                {
                    std::cout << "\n[ERRO]: Voce se desequilibrou e precisa de 1 turno para poder defender novamente!\n";
                    Menu::aguardarPressionamentoDeEnter();
                    break; 
                }
                
                Item* escudoEscolhido = jogadorAtual->obterInventario()->selecionarEscudo();
                if (escudoEscolhido != nullptr) 
                {
                    jogadorAtual->equiparItem(escudoEscolhido);
                    jogadorAtual->definirDefendendo(true);
                    std::cout << "\n[SISTEMA]: " << jogadorAtual->obterNome() << " assumiu uma postura defensiva com " << escudoEscolhido->obterNomeItem() << "!\n";
                    Menu::aguardarPressionamentoDeEnter();
                    turnoFoiConsumido = true;
                }
                break;
            }
            case 3: // --- 3. HABILIDADE ATIVA ---
            {
                jogadorAtual->obterClasse()->usarHabilidadeClasse(jogadorAtual, listaDeInimigos);
                
                if (jogadorAtual->obterNomeClasse() != "Mago") turnoFoiConsumido = true;
                else Menu::aguardarPressionamentoDeEnter();
                break;
            }
            case 4: // --- 4. INVENTARIO ---
            {
                Menu::gerenciarInventario(jogadorAtual, &turnoFoiConsumido);
                if (jogadorAtual->obterItemSelecionadoParaUso() != nullptr) 
                {
                    Item* frasco = jogadorAtual->obterItemSelecionadoParaUso();
                    int indiceDoAlvoEscolhido = -1;
                    
                    std::cout << "\n--- ESCOLHA UM ALVO PARA O FRASCO ---\n";
                    for (size_t i = 0; i < listaDeInimigos.size(); ++i) {
                        std::cout << "[" << i << "] " << listaDeInimigos[i]->obterNome() << " (HP: " << listaDeInimigos[i]->obterVida() << ")\n";
                    }
                    std::cout << "Escolha (ou tecla invalida para CANCELAR): ";
                    
                    if (!(std::cin >> indiceDoAlvoEscolhido) || indiceDoAlvoEscolhido < 0 || indiceDoAlvoEscolhido >= static_cast<int>(listaDeInimigos.size())) 
                    {
                        std::cin.clear(); 
                        std::cin.ignore(1000, '\n');
                        std::cout << "\n[SISTEMA] Uso do frasco cancelado. O item voltou para a mochila.\n";
                        jogadorAtual->definirItemSelecionadoParaUso(nullptr);
                    } 
                    else 
                    {
                        Personagem* alvo = listaDeInimigos[indiceDoAlvoEscolhido];
                        std::string nomeFrasco = frasco->obterNomeItem();
                        
                        if (nomeFrasco.find("Gosma") != std::string::npos) {
                            alvo->aplicarLentidaoEstatistica();
                            alvo->definirTurnosLentidao(3);
                            std::cout << "\n\033[35m>> Voce jogou o frasco! " << alvo->obterNome() << " esta com lentidao por 3 turnos!\033[0m\n";
                        } else if (nomeFrasco.find("Fraqueza") != std::string::npos) {
                            alvo->aplicarFraquezaEstatistica();
                            alvo->definirTurnosFraqueza(3);
                            std::cout << "\n\033[31m>> Voce jogou o frasco! " << alvo->obterNome() << " teve sua forca reduzida em 25% por 3 turnos!\033[0m\n";
                        }
                        
                        jogadorAtual->obterInventario()->removerItem(nomeFrasco);
                        jogadorAtual->definirItemSelecionadoParaUso(nullptr);
                        turnoFoiConsumido = true;
                    }
                    Menu::aguardarPressionamentoDeEnter();
                }
                break;
            }
            case 5: // --- 5. JOGADOR ---
            {
                Menu::gerenciarFichaDoJogador(jogadorAtual);
                break;
            }
            default:
                break;
            }
            
            if (verificarCondicaoDeVitoriaOuDerrota()) return; 
        }
        
        if (turnoExtraFirstTurn && contadorDoTurnoAtual == 1) 
        {
            std::cout << "\n\033[36m[SISTEMA]: Sua agilidade extrema (" << jogadorAtual->obterDestreza() << " VS " << maxDestrezaInimigos << ") permite que voce aja novamente!\033[0m\n";
            Menu::aguardarPressionamentoDeEnter();
            turnoExtraFirstTurn = false;
            continue; // Pula o turno inimigo e permite que o jogador jogue o turno novamente
        }

        executarTurnoDeTodosOsInimigos();
        if (verificarCondicaoDeVitoriaOuDerrota()) return;
        contadorDoTurnoAtual++;
    }
}

void SistemaRPG::executarTurnoDeTodosOsInimigos() 
{
    if (jogadorAtual->obterPularTurnoInimigo()) 
    {
        std::cout << "\n[EFEITO]: Os inimigos estao atordoados e nao podem agir!\n";
        jogadorAtual->definirPularTurnoInimigo(false); 
    }
    else
    {
        std::cout << "\n--- TURNO DOS INIMIGOS ---" << std::endl;
        for (Personagem* inimigoAtual : listaDeInimigos) 
        {
            if (inimigoAtual->obterSangramento() && inimigoAtual->obterVida() > 0)
            {
                int danoSangramento = std::max(1, inimigoAtual->obterVida() / 10);
                inimigoAtual->modificarVida(-danoSangramento);
                std::cout << "\033[31m[EFEITO]: " << inimigoAtual->obterNome() << " perdeu " << danoSangramento << " HP devido ao sangramento! (" << inimigoAtual->obterTurnosSangramento() - 1 << " turnos restantes)\033[0m\n";
                
                inimigoAtual->definirTurnosSangramento(inimigoAtual->obterTurnosSangramento() - 1);
                if (inimigoAtual->obterTurnosSangramento() <= 0) {
                    inimigoAtual->definirSangramento(false);
                    std::cout << "\033[31m[EFEITO]: O sangramento em " << inimigoAtual->obterNome() << " parou.\033[0m\n";
                }

                if (inimigoAtual->obterVida() <= 0) continue; // Pula o ataque se o inimigo morreu para o sangramento
            }

            if (inimigoAtual->obterLentidao())
            {
                inimigoAtual->definirTurnosLentidao(inimigoAtual->obterTurnosLentidao() - 1);
                if (inimigoAtual->obterTurnosLentidao() <= 0)
                {
                    inimigoAtual->removerLentidaoEstatistica();
                    std::cout << "\033[35m[EFEITO]: " << inimigoAtual->obterNome() << " se livrou da gosma e recuperou sua agilidade.\033[0m\n";
                }
            }

            if (inimigoAtual->obterFraqueza()) 
            {
                inimigoAtual->definirTurnosFraqueza(inimigoAtual->obterTurnosFraqueza() - 1);
                if (inimigoAtual->obterTurnosFraqueza() <= 0) 
                {
                    inimigoAtual->removerFraquezaEstatistica();
                    std::cout << "\033[31m[EFEITO]: " << inimigoAtual->obterNome() << " recuperou sua forca original.\033[0m\n";
                }
            }

            if (jogadorAtual->obterVida() > 0) 
            {
                auto itCipos = std::find(inimigosPresosComCipos.begin(), inimigosPresosComCipos.end(), inimigoAtual);
                if (itCipos != inimigosPresosComCipos.end()) {
                    std::cout << "\033[32m[EFEITO]: " << inimigoAtual->obterNome() << " esta preso em cipos e nao pode atacar neste turno!\033[0m\n";
                    inimigosPresosComCipos.erase(itCipos);
                } else {
                    // Na Dificuldade Dificil(3), os inimigos poderao ativar suas habilidades de Classe
                    if (jogadorAtual->obterDificuldade() == 3) 
                    {
                        std::vector<Personagem*> listaDeAlvosDoInimigo = { jogadorAtual };
                        inimigoAtual->obterClasse()->usarHabilidadeClasse(inimigoAtual, listaDeAlvosDoInimigo);
                    }
                    
                    realizarAtaqueFisico(inimigoAtual, jogadorAtual, contadorDoTurnoAtual);
                }
            }
        }
    }

    // Reseta o estado da defesa ativada APOS os inimigos atacarem
    if (jogadorAtual->obterDefendendo()) 
    {
        jogadorAtual->definirDefendendo(false);
        jogadorAtual->definirRecargaDefesa(true); // Entra em Cooldown
    } 
    else if (jogadorAtual->obterRecargaDefesa()) 
    {
        jogadorAtual->definirRecargaDefesa(false); // Resfria o Cooldown
    }

    // Gerencia a duracao do buff acumulativo
    if (jogadorAtual->obterTurnosBuff() > 0) 
    {
        jogadorAtual->definirTurnosBuff(jogadorAtual->obterTurnosBuff() - 1);
        
        if (jogadorAtual->obterTurnosBuff() <= 0) 
        {
            jogadorAtual->definirMultiplicador(1.0); // Reseta multiplicador ao expirar
            std::cout << "\n[SISTEMA]: O efeito da habilidade expirou!\n";
        }
    }

    if (jogadorAtual->obterRecarga()) jogadorAtual->definirRecarga(false);
    Menu::aguardarPressionamentoDeEnter();
}

void SistemaRPG::realizarAtaqueFisico(Personagem* personagemAtacante, Personagem* personagemDefensor, int turnoAtualDoCombate) 
{
    double multiplicadorDeAtributos = personagemAtacante->obterMultiplicador(); // Aplica o multiplicador acumulado

    if (personagemAtacante->obterInviolavel() && personagemAtacante->obterNomeClasse() == "Arqueiro") 
    {
        multiplicadorDeAtributos *= 2.0; // Bonus fixo do Arqueiro
        personagemAtacante->definirInviolavel(false);
    }

    int danoFisicoDaArma = 1; // Dano base desarmado
    int danoMagicoDaArma = 0;
    danoMagicoPerfuranteAtual = 0;

    if (personagemAtacante->obterArma()) 
    {
        danoFisicoDaArma = personagemAtacante->obterArma()->obterDanoFisico();
        danoMagicoDaArma = personagemAtacante->obterArma()->obterDanoMagico();

        if (personagemAtacante->obterArma()->obterNomeItem().find("enfeiticada") != std::string::npos) {
            int bonusMagico = danoFisicoDaArma / 2;
            
            double bonusEscalado = bonusMagico * (1.0 + (personagemAtacante->obterSabedoria() / 100.0));
            danoMagicoPerfuranteAtual = static_cast<int>(bonusEscalado * multiplicadorDeAtributos);
        }
    }

    // Força soma dano fisico, Destreza multiplica dano fisico
    double danoFisicoCalculado = (danoFisicoDaArma + personagemAtacante->obterForca()) * (1.0 + (personagemAtacante->obterDestreza() / 100.0));
    if (danoFisicoCalculado < 0) danoFisicoCalculado = 0; // Evita que atributos negativos reduzam o dano total
    
    // Inteligencia soma dano magico, Sabedoria multiplica dano magico
    double danoMagicoCalculado = (danoMagicoDaArma + personagemAtacante->obterInteligencia()) * (1.0 + (personagemAtacante->obterSabedoria() / 100.0));
    if (danoMagicoCalculado < 0) danoMagicoCalculado = 0; // Evita que magias negativas curem ou enfraquecam o ataque fisico

    double danoTotalCalculado = danoFisicoCalculado + danoMagicoCalculado;
    if (danoTotalCalculado < 1.0) danoTotalCalculado = 1.0;

    int danoBaseCalculado = static_cast<int>(danoTotalCalculado * multiplicadorDeAtributos);

    if (personagemAtacante == jogadorAtual || jogadorAtual->obterDificuldade() >= 2) 
    {
        danoBaseCalculado = personagemAtacante->obterRaca()->processarDanoOfensivo(danoBaseCalculado, personagemAtacante);
    }

    if (personagemAtacante->obterModoAtaqueArea() && personagemAtacante->obterNomeClasse() == "Mago" && !listaDeInimigos.empty()) 
    {
        int danoDivididoParaArea = danoBaseCalculado / static_cast<int>(listaDeInimigos.size());
        danoMagicoPerfuranteAtual /= static_cast<int>(listaDeInimigos.size());
        std::cout << personagemAtacante->obterNome() << " desfere um ataque em area!\n";
        for (Personagem* inimigoAtual : listaDeInimigos) aplicarDanoAoAlvo(personagemAtacante, inimigoAtual, danoDivididoParaArea, turnoAtualDoCombate);
    } 
    else if (personagemDefensor != nullptr) 
    {
        std::cout << personagemAtacante->obterNome() << " ataca " << personagemDefensor->obterNome() << "!" << std::endl;
        aplicarDanoAoAlvo(personagemAtacante, personagemDefensor, danoBaseCalculado, turnoAtualDoCombate);
    }
}

void SistemaRPG::aplicarDanoAoAlvo(Personagem* personagemAtacante, Personagem* personagemAlvo, int quantidadeDeDanoBruto, int turnoAtualDoCombate) 
{
    if (personagemAlvo->obterInviolavel()) 
    {
        std::cout << ">> " << personagemAlvo->obterNome() << " esquivou totalmente!\n";
        return;
    }

    // Logica da Quebra de Resistencia (Pó Mágico)
    if (personagemAtacante->obterArma() && personagemAtacante->obterArma()->obterNomeItem().find("(Penetrante)") != std::string::npos && !personagemAlvo->obterQuebraResistencia()) {
        personagemAlvo->aplicarQuebraResistenciaEstatistica();
        std::cout << "\033[36m>> A arma de " << personagemAtacante->obterNome() << " perfurou a armadura, reduzindo a resistencia de " << personagemAlvo->obterNome() << " pela metade e a constituicao em um terco!\033[0m\n";
    }

    int quantidadeDeDanoBrutoSemPerfuracao = quantidadeDeDanoBruto - danoMagicoPerfuranteAtual;
    if (quantidadeDeDanoBrutoSemPerfuracao < 0) quantidadeDeDanoBrutoSemPerfuracao = 0;

    int bonusDeDefesaDaArmadura = (personagemAlvo->obterArmadura()) ? personagemAlvo->obterArmadura()->obterReducaoFixa() : 0;
    
    // Resistencia atua como reduçao fixa base
    int totalDeReducaoFixaDeDano = personagemAlvo->obterResistencia() + bonusDeDefesaDaArmadura;
    
    // Constituicao atua como reducao percentual (produto) com limite de 50%
    double percentualDeReducao = personagemAlvo->obterConstituicao() / 100.0;
    if (percentualDeReducao > 0.50) percentualDeReducao = 0.50; // Hardcap de 50% de absorcao maxima
    
    double multiplicadorDeConstituicao = 1.0 - percentualDeReducao;

    int danoFinalAposReducoes = static_cast<int>((quantidadeDeDanoBrutoSemPerfuracao - totalDeReducaoFixaDeDano) * multiplicadorDeConstituicao);
    if (danoFinalAposReducoes < 1 && quantidadeDeDanoBrutoSemPerfuracao > 0) danoFinalAposReducoes = 1; // Impacto basico minimo
    else if (quantidadeDeDanoBrutoSemPerfuracao == 0) danoFinalAposReducoes = 0;

    danoFinalAposReducoes += danoMagicoPerfuranteAtual;

    // Logica do Parry
    if (personagemAlvo == jogadorAtual && jogadorAtual->obterParryAtivado()) 
    {
        int quantidadeDeNumerosDoParry = std::max(1, danoFinalAposReducoes / 4);
        int destrezaDoAtacante = std::max(1, personagemAtacante->obterDestreza());
        int tempoLimiteParaParryEmSegundos = std::max(1, 60 / destrezaDoAtacante);

        int quantidadeDeDanoReduzidoPeloParry = 0;
        bool parryFoiBemSucedido = executarSistemaDeParry(quantidadeDeNumerosDoParry, tempoLimiteParaParryEmSegundos, quantidadeDeDanoReduzidoPeloParry);
        if (parryFoiBemSucedido) 
        {
            // Aplica o limite de 50% de reducao do dano (com um minimo de 1 para danos baixos)
            int limiteMaximoDeReducaoPeloParry = std::max(1, danoFinalAposReducoes / 2);
            if (quantidadeDeDanoReduzidoPeloParry > limiteMaximoDeReducaoPeloParry) {
                quantidadeDeDanoReduzidoPeloParry = limiteMaximoDeReducaoPeloParry;
            }

            danoFinalAposReducoes -= quantidadeDeDanoReduzidoPeloParry;
            if (danoFinalAposReducoes <= 0) 
            {
                danoFinalAposReducoes = 0;
                std::cout << ">> [PARRY]: Parry perfeito, reducao total!\n";
            } 
            else 
            {
                std::cout << ">> [PARRY]: Parry efetivo, dano reduzido em " << quantidadeDeDanoReduzidoPeloParry << "\n";
            }
        }
        else 
        {
            std::cout << ">> [PARRY]: FALHA! A defesa falhou!\n";
        }
    }

    // Logica Ativa do Escudo
    if (personagemAlvo->obterDefendendo() && personagemAlvo->obterEscudo() != nullptr) 
    {
        int bloqueioFixoDoEscudo = personagemAlvo->obterEscudo()->obterReducaoDanoFixaEscudo();
        std::cout << ">> [DEFESA]: O escudo bloqueou " << bloqueioFixoDoEscudo << " de dano!\n";
        danoFinalAposReducoes -= bloqueioFixoDoEscudo;
        if (danoFinalAposReducoes < 0) danoFinalAposReducoes = 0; // Escudo pode zerar completamente o dano
        
        personagemAlvo->obterEscudo()->reduzirDurabilidade(1);
        if (personagemAlvo->obterEscudo()->obterDurabilidadeAtualEscudo() <= 0) 
        {
            std::cout << "[!] ALERTA: O escudo " << personagemAlvo->obterEscudo()->obterNomeItem() << " foi DESTRUIDO em pedacos!\n";
            personagemAlvo->obterInventario()->removerItem(personagemAlvo->obterEscudo()->obterNomeItem());
            personagemAlvo->desequiparEscudo(); // Fica sem escudo
        }
    }

    // Passivas defensivas de raca (Dwarf/Humano) somente aplicam no Normal(2) ou Dificil(3), ou se for o Jogador
    if (personagemAlvo == jogadorAtual || jogadorAtual->obterDificuldade() >= 2) 
    {
        danoFinalAposReducoes = personagemAlvo->obterRaca()->processarDanoDefensivo(danoFinalAposReducoes, personagemAlvo);
    }

    // Logica do Dano Minimo Magico dos Violoes
    if (personagemAtacante->obterArma() && personagemAtacante->obterArma()->obterNomeItem().find("Violao") != std::string::npos) {
        int danoMagicoBase = personagemAtacante->obterArma()->obterDanoMagico();
        if (danoFinalAposReducoes < danoMagicoBase) {
            danoFinalAposReducoes = danoMagicoBase; // Garante o dano mágico como base da agressão
        }
    }

    if (danoFinalAposReducoes > 0) 
    {
        if (personagemAtacante->obterArma() && personagemAtacante->obterArma()->obterNomeItem().find("Violao enfeiticado") != std::string::npos) {
            int danoRaizes = personagemAlvo->obterVida() * 0.20;
            if (danoRaizes > 0) {
                personagemAlvo->modificarVida(-danoRaizes);
                personagemAtacante->modificarVida(danoRaizes);
                std::cout << "\033[32m>> [Raizes sangue suga]: O violao drenou " << danoRaizes << " de HP de " << personagemAlvo->obterNome() << " e curou " << personagemAtacante->obterNome() << "!\033[0m\n";
            }
        }

        personagemAlvo->modificarVida(-danoFinalAposReducoes);
        
        if (personagemAlvo == jogadorAtual) 
        {
            totalDeDanoRecebido += danoFinalAposReducoes;
            std::cout << "\033[41m>> " << personagemAlvo->obterNome() << " recebeu " << danoFinalAposReducoes << " de dano\033[0m" << std::endl;
        } else {
            totalDeDanoCausado += danoFinalAposReducoes;
            std::cout << ">> " << personagemAlvo->obterNome() << " recebeu " << danoFinalAposReducoes << " de dano" << std::endl;
        }
        
        // Aplicação dos efeitos no acerto
        if (personagemAtacante->obterArma()) 
        {
            if (personagemAtacante->obterArma()->possuiEfeitoSangramento() && !personagemAlvo->obterSangramento()) {
                personagemAlvo->definirSangramento(true);
                personagemAlvo->definirTurnosSangramento(3);
                std::cout << "\033[31m>> " << personagemAlvo->obterNome() << " comecou a sangrar profundamente! (3 turnos)\033[0m\n";
            }
            if (personagemAtacante->obterArma()->possuiEfeitoLentidao() && !personagemAlvo->obterLentidao()) {
                personagemAlvo->aplicarLentidaoEstatistica();
                personagemAlvo->definirTurnosLentidao(3);
                std::cout << "\033[35m>> " << personagemAlvo->obterNome() << " foi coberto por gosma e sua destreza caiu pela metade! (3 turnos)\033[0m\n";
            }
            if (personagemAtacante->obterArma()->obterNomeItem().find("Cajado de cipos") != std::string::npos) {
                if ((std::rand() % 100) < 30) {
                    if (std::find(inimigosPresosComCipos.begin(), inimigosPresosComCipos.end(), personagemAlvo) == inimigosPresosComCipos.end()) {
                        inimigosPresosComCipos.push_back(personagemAlvo);
                        std::cout << "\033[32m>> " << personagemAlvo->obterNome() << " foi preso por cipos e perdera seu proximo ataque!\033[0m\n";
                    }
                }
            }
        }

        if (personagemAtacante->obterRaca()->obterNomeRaca() == "Slime" && !personagemAlvo->obterLentidao()) {
            if ((std::rand() % 100) < 20) {
                personagemAlvo->aplicarLentidaoEstatistica();
                personagemAlvo->definirTurnosLentidao(3);
                std::cout << "\033[35m>> O ataque de " << personagemAtacante->obterNome() << " espalhou gosma! " << personagemAlvo->obterNome() << " perdeu destreza! (3 turnos)\033[0m\n";
            }
        }
    }
    else if (danoFinalAposReducoes == 0 && personagemAlvo->obterDefendendo()) 
    {
        std::cout << ">> O dano foi totalmente absorvido pela sua defesa!" << std::endl;
    }
}

bool SistemaRPG::executarSistemaDeParry(int quantidadeDeNumerosParaDigitar, int tempoLimiteEmSegundos, int& quantidadeDeDanoReduzido) 
{
    std::string sequenciaGeradaPeloSistema = "";
    quantidadeDeDanoReduzido = 0;
    for (int indiceAtual = 0; indiceAtual < quantidadeDeNumerosParaDigitar; ++indiceAtual) 
    {
        int numeroAleatorio = (std::rand() % 9) + 1; // 1 to 9
        sequenciaGeradaPeloSistema += std::to_string(numeroAleatorio);
        quantidadeDeDanoReduzido += numeroAleatorio;
    }

    std::cout << "\n[PARRY] O inimigo ataca! Digite a sequencia rapidamente para defender!\n";
    std::cout << "[PARRY] Sequencia: " << sequenciaGeradaPeloSistema << "\n";
    std::cout << "[PARRY] Tempo Limite: " << tempoLimiteEmSegundos << " segundos!\n";
    std::cout << "[PARRY] Digite: ";

    std::string entradaDigitadaPeloJogador = "";
    
#ifdef _WIN32
    auto tempoInicial = std::chrono::steady_clock::now();
    while (true) {
        auto tempoAtual = std::chrono::steady_clock::now();
        std::chrono::duration<double> tempoDecorrido = tempoAtual - tempoInicial;
        if (tempoDecorrido.count() > tempoLimiteEmSegundos) {
            std::cout << "\n[PARRY] TEMPO ESGOTADO!\n";
            return false;
        }
        if (_kbhit()) 
        {
            char teclaPressionada = _getch();
            if (teclaPressionada == '\r' || teclaPressionada == '\n') 
            {
                std::cout << "\n";
                break;
            } 
            else if (teclaPressionada == '\b' || teclaPressionada == 127) 
            {
                if (!entradaDigitadaPeloJogador.empty()) 
                {
                    entradaDigitadaPeloJogador.pop_back();
                    std::cout << "\b \b";
                }
            } 
            else 
            {
                entradaDigitadaPeloJogador += teclaPressionada;
                std::cout << teclaPressionada;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
#else
    auto tempoInicial = std::chrono::steady_clock::now();
    std::cin >> entradaDigitadaPeloJogador;
    auto tempoAtual = std::chrono::steady_clock::now();
    std::chrono::duration<double> tempoDecorrido = tempoAtual - tempoInicial;
    if (tempoDecorrido.count() > tempoLimiteEmSegundos) {
        std::cout << "\n[PARRY] TEMPO ESGOTADO (" << tempoDecorrido.count() << "s)!\n";
        return false;
    }
#endif

    return entradaDigitadaPeloJogador == sequenciaGeradaPeloSistema;
}

bool SistemaRPG::verificarCondicaoDeVitoriaOuDerrota() 
{
    if (listaDeInimigos.empty()) 
    { 
        TelaVitoria::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, itensObtidos);
        return true; 
    }
    if (jogadorAtual->obterVida() <= 0) 
    { 
        TelaDerrota::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido); 
        return true; 
    }
    return false;
}

void realizarDropsGoblin(Personagem* inimigo, Personagem* jogadorAtual, std::vector<std::string>& itensObtidos);
void realizarDropsSlime(Personagem* inimigo, Personagem* jogadorAtual, std::vector<std::string>& itensObtidos);
void realizarDropsOrkExilado(Personagem* inimigo, Personagem* jogadorAtual, std::vector<std::string>& itensObtidos);
void realizarDropsFada(Personagem* inimigo, Personagem* jogadorAtual, std::vector<std::string>& itensObtidos);
void realizarDropsAbominacaoFloresta(Personagem* inimigo, Personagem* jogadorAtual, std::vector<std::string>& itensObtidos);

void SistemaRPG::processarMorteDeInimigo(Personagem* inimigo)
{
    int ouroDerrubado = inimigo->obterOuroRecompensa();
    int xpDerrubado = inimigo->obterXpRecompensa();
    jogadorAtual->ganharOuro(ouroDerrubado);
    jogadorAtual->ganharXp(xpDerrubado);
    quantidadeDeOuroObtido += ouroDerrubado;
    quantidadeDeXpObtido += xpDerrubado;
    std::cout << "\n[!] " << inimigo->obterNome() << " derrotado! ";
    std::cout << "\033[43m+" << ouroDerrubado << "G\033[0m "; 
    std::cout << "\033[44m+" << xpDerrubado << " XP\033[0m\n"; 

    auto itCipos = std::find(inimigosPresosComCipos.begin(), inimigosPresosComCipos.end(), inimigo);
    if (itCipos != inimigosPresosComCipos.end()) inimigosPresosComCipos.erase(itCipos);

    if (inimigo->obterNome() == "Goblin") 
    {
        realizarDropsGoblin(inimigo, jogadorAtual, itensObtidos);
    }
    else if (inimigo->obterNome() == "Slime") 
    {
        realizarDropsSlime(inimigo, jogadorAtual, itensObtidos);
    }
    else if (inimigo->obterNome() == "Ork [mini-boss]" || inimigo->obterRaca()->obterNomeRaca() == "Ork [exilado]") 
    {
        realizarDropsOrkExilado(inimigo, jogadorAtual, itensObtidos);
    }
    else if (inimigo->obterRaca()->obterNomeRaca() == "Fada") 
    {
        realizarDropsFada(inimigo, jogadorAtual, itensObtidos);
    }
    else if (inimigo->obterNome() == "Abominacao da Floresta" || inimigo->obterRaca()->obterNomeRaca() == "Abominacao da Floresta") 
    {
        realizarDropsAbominacaoFloresta(inimigo, jogadorAtual, itensObtidos);
    }
}