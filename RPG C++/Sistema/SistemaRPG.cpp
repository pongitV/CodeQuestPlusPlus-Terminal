#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <ctime>
#include <chrono>
#include <thread>
#include <iomanip>

#ifdef _WIN32
    #include <conio.h>
#endif

#include "SistemaRPG.h"
#include "Menu.h"

#include "../Raças/RacaBase.h"
#include "../Classes/ClasseBase.h"
#include "../Inventario/Item.h"
#include "../Inventario/Arma.h"

#include "../Interfaces/TelaVitoria.h"
#include "../Interfaces/TelaDerrota.h"
#include "../Interfaces/TelaCombate.h"
#include "../Inventario/InventarioCombate.h"
#include "../Interfaces/TelaAtributos.h"

SistemaRPG::SistemaRPG(Personagem* jogadorParaOCombate, std::vector<std::unique_ptr<Personagem>>&& inimigosParaOCombate) 
    : jogadorAtual(jogadorParaOCombate), listaDeInimigos(std::move(inimigosParaOCombate)), contadorDoTurnoAtual(1), quantidadeDeOuroObtido(0), quantidadeDeXpObtido(0), totalDeDanoCausado(0), totalDeDanoRecebido(0), danoMagicoPerfuranteAtual(0)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int nivelDeDificuldade = jogadorAtual->obterDificuldade();
    double multiplicadorDeDificuldadeDosInimigos = 1.0;
    
    if (nivelDeDificuldade == 2) { // Normal
    } 
    else if (nivelDeDificuldade == 3) { // Dificil
        multiplicadorDeDificuldadeDosInimigos = 2.0;
    }

    for (auto& inimigoAtualPtr : this->listaDeInimigos) 
    {
        inimigoAtualPtr->aplicarMultiplicadorDificuldade(multiplicadorDeDificuldadeDosInimigos);
    }
}

SistemaRPG::~SistemaRPG() 
{
    listaDeInimigos.clear();
}

void SistemaRPG::iniciarCombate() 
{
    int maxDestrezaInimigos = 0; // Declare this variable
    for (auto& inimigoPtr : listaDeInimigos)
    {
        if (inimigoPtr->obterDestreza() > maxDestrezaInimigos) maxDestrezaInimigos = inimigoPtr->obterDestreza();
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
        TelaCombate::exibirLogoParaTelaDeCombate("EM COMBATE");
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
    
    jogadorAtual->processarEfeitosInicioTurno();
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
            TelaCombate::exibirLogoParaTelaDeCombate(tituloDoCombate);
            std::vector<Personagem*> inimigosRaw;
            for (auto& ini : listaDeInimigos) inimigosRaw.push_back(ini.get());
            exibirHordaDeInimigosLadoALado(inimigosRaw);
            TelaCombate::exibirBarraDeStatusDoJogador(jogadorAtual); 

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
                if (jogadorAtual->obterTipoAtaque() == TipoAtaque::AREA) 
                {
                    realizarAtaqueFisico(*jogadorAtual, nullptr, contadorDoTurnoAtual);
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

                    realizarAtaqueFisico(*jogadorAtual, listaDeInimigos[indiceDoAlvoEscolhido].get(), contadorDoTurnoAtual);
                    turnoFoiConsumido = true;
                }

                // Limpeza de mortos apos o ataque
                for (auto iteradorInimigos = listaDeInimigos.begin(); iteradorInimigos != listaDeInimigos.end(); ) 
                {
                    if ((*iteradorInimigos)->obterVida() <= 0) 
                    {
                        processarMorteDeInimigo(iteradorInimigos->get());
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
                std::vector<Personagem*> alvosRaw;
                for (auto& ini : listaDeInimigos) alvosRaw.push_back(ini.get());

                jogadorAtual->obterClasse()->usarHabilidadeClasse(*jogadorAtual, alvosRaw);
                
                if (jogadorAtual->habilidadeDaClasseConsomeTurno()) turnoFoiConsumido = true;
                else Menu::aguardarPressionamentoDeEnter();
                break;
            }
            case 4: // --- 4. INVENTARIO ---
            {
                InventarioCombate::gerenciarInventario(jogadorAtual, &turnoFoiConsumido);
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
                        Personagem* alvo = listaDeInimigos[indiceDoAlvoEscolhido].get();
                        std::string nomeFrasco = frasco->obterNomeItem();
                        
                        if (frasco->temPropriedade(Propriedade::ConsumivelDebuffLentidao)) {
                            alvo->aplicarLentidaoEstatistica();
                            alvo->definirTurnosLentidao(3);
                            std::cout << "\n\033[35m>> Voce jogou o frasco! " << alvo->obterNome() << " esta com lentidao por 3 turnos!\033[0m\n";
                        } else if (frasco->temPropriedade(Propriedade::ConsumivelDebuffFraqueza)) {
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
                TelaAtributos::gerenciarFichaDoJogador(jogadorAtual);
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
        for (auto& inimigoAtualPtr : listaDeInimigos) 
        {
            Personagem* inimigoAtual = inimigoAtualPtr.get();
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

        inimigoAtual->processarEfeitosInicioTurno();

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
                if (inimigoAtual->podeAgir()) {
                    // Na Dificuldade Dificil(3), os inimigos poderao ativar suas habilidades de Classe
                    if (jogadorAtual->obterDificuldade() == 3) 
                    {
                        std::vector<Personagem*> listaDeAlvosDoInimigo = { jogadorAtual };
                        inimigoAtual->obterClasse()->usarHabilidadeClasse(*inimigoAtual, listaDeAlvosDoInimigo);
                    }
                    
                    realizarAtaqueFisico(*inimigoAtual, jogadorAtual, contadorDoTurnoAtual);
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

void SistemaRPG::realizarAtaqueFisico(Personagem& personagemAtacante, Personagem* personagemDefensor, int turnoAtualDoCombate) 
{
    double multiplicadorDeAtributos = personagemAtacante.obterMultiplicador(); // Aplica o multiplicador acumulado

    if (personagemAtacante.obterInviolavel()) 
    {
        multiplicadorDeAtributos *= 2.0; // Bonus fixo do Arqueiro
        personagemAtacante.definirInviolavel(false);
    }

    int danoFisicoDaArma = 1; // Dano base desarmado
    int danoMagicoDaArma = 0;
    danoMagicoPerfuranteAtual = 0;

    if (personagemAtacante.obterArma()) 
    {
        if (Arma* arma = dynamic_cast<Arma*>(personagemAtacante.obterArma())) {
            danoFisicoDaArma = arma->obterDanoFisico();
            danoMagicoDaArma = arma->obterDanoMagico();
        }

        if (personagemAtacante.obterArma()->temPropriedade(Propriedade::Magica)) {
            int bonusMagico = danoFisicoDaArma / 2;
            
            double bonusEscalado = bonusMagico * (1.0 + (personagemAtacante.obterSabedoria() / 100.0));
            danoMagicoPerfuranteAtual = static_cast<int>(bonusEscalado * multiplicadorDeAtributos);
        }
    }

    double forcaEfetiva = personagemAtacante.obterForca();
    double destrezaEfetiva = personagemAtacante.obterDestreza();
    double inteligenciaEfetiva = personagemAtacante.obterInteligencia();
    double sabedoriaEfetiva = personagemAtacante.obterSabedoria();

    if (danoFisicoDaArma == 0 && danoMagicoDaArma > 0) {
        forcaEfetiva *= 0.1;
        destrezaEfetiva *= 0.1;
    } else if (danoFisicoDaArma > 0 && danoMagicoDaArma == 0) {
        inteligenciaEfetiva *= 0.1;
        sabedoriaEfetiva *= 0.1;
    }

    // Força soma dano fisico, Destreza multiplica dano fisico
    double danoFisicoCalculado = (danoFisicoDaArma + forcaEfetiva) * (1.0 + (destrezaEfetiva / 100.0));
    if (danoFisicoCalculado < 0) danoFisicoCalculado = 0; // Evita que atributos negativos reduzam o dano total
    
    // Inteligencia soma dano magico, Sabedoria multiplica dano magico
    double danoMagicoCalculado = (danoMagicoDaArma + inteligenciaEfetiva) * (1.0 + (sabedoriaEfetiva / 100.0));
    if (danoMagicoCalculado < 0) danoMagicoCalculado = 0; // Evita que magias negativas curem ou enfraquecam o ataque fisico

    double danoTotalCalculado = danoFisicoCalculado + danoMagicoCalculado;
    if (danoTotalCalculado < 1.0) danoTotalCalculado = 1.0;

    int danoBaseCalculado = static_cast<int>(danoTotalCalculado * multiplicadorDeAtributos);

    if (&personagemAtacante == jogadorAtual || jogadorAtual->obterDificuldade() >= 2) 
    {
        danoBaseCalculado = personagemAtacante.obterRaca()->processarDanoOfensivo(danoBaseCalculado, &personagemAtacante);
    }

    if (personagemAtacante.obterTipoAtaque() == TipoAtaque::AREA && !listaDeInimigos.empty()) 
    {
        int danoDivididoParaArea = danoBaseCalculado / static_cast<int>(listaDeInimigos.size());
        danoMagicoPerfuranteAtual /= static_cast<int>(listaDeInimigos.size());
        std::cout << personagemAtacante.obterNome() << " desfere um ataque em area!\n";
        for (auto& inimigoAtualPtr : listaDeInimigos) aplicarDanoAoAlvo(personagemAtacante, *inimigoAtualPtr, danoDivididoParaArea, turnoAtualDoCombate);
    } 
    else if (personagemDefensor != nullptr) 
    {
        std::cout << personagemAtacante.obterNome() << " ataca " << personagemDefensor->obterNome() << "!" << std::endl;
        aplicarDanoAoAlvo(personagemAtacante, *personagemDefensor, danoBaseCalculado, turnoAtualDoCombate);
    }
}

void SistemaRPG::aplicarDanoAoAlvo(Personagem& personagemAtacante, Personagem& personagemAlvo, int quantidadeDeDanoBruto, int turnoAtualDoCombate) 
{
    if (personagemAlvo.obterInviolavel()) 
    {
        std::cout << ">> " << personagemAlvo.obterNome() << " esquivou totalmente!\n";
        return;
    }

    // Logica da Quebra de Resistencia (Pó Mágico)
    if (personagemAtacante.obterArma()) personagemAtacante.obterArma()->antesDeCausarDano(personagemAtacante, personagemAlvo);

    int danoBaseMitigado = personagemAlvo.calcularDefesaBase(quantidadeDeDanoBruto, danoMagicoPerfuranteAtual);
    int quantidadeDeDanoReduzidoPeloParry = 0;

    // Logica do Parry
    if (&personagemAlvo == jogadorAtual && jogadorAtual->obterParryAtivado()) 
    {
        int quantidadeDeNumerosDoParry = std::max(1, danoBaseMitigado / 4);
        int destrezaDoAtacante = std::max(1, personagemAtacante.obterDestreza());
        int tempoLimiteParaParryEmSegundos = std::max(1, 60 / destrezaDoAtacante);

        bool parryFoiBemSucedido = executarSistemaDeParry(quantidadeDeNumerosDoParry, tempoLimiteParaParryEmSegundos, quantidadeDeDanoReduzidoPeloParry);
        if (parryFoiBemSucedido) 
        {
            // Aplica o limite de 50% de reducao do dano (com um minimo de 1 para danos baixos)
            int limiteMaximoDeReducaoPeloParry = std::max(1, danoBaseMitigado / 2);
            if (quantidadeDeDanoReduzidoPeloParry > limiteMaximoDeReducaoPeloParry) {
                quantidadeDeDanoReduzidoPeloParry = limiteMaximoDeReducaoPeloParry;
            }

            if (danoBaseMitigado - quantidadeDeDanoReduzidoPeloParry <= 0) {
                std::cout << ">> [PARRY]: Parry perfeito, reducao total!\n";
            } else {
                std::cout << ">> [PARRY]: Parry efetivo, dano reduzido em " << quantidadeDeDanoReduzidoPeloParry << "\n";
            }
        }
        else 
        {
            quantidadeDeDanoReduzidoPeloParry = 0;
            std::cout << ">> [PARRY]: FALHA! A defesa falhou!\n";
        }
    }

    bool aplicarPassivas = (&personagemAlvo == jogadorAtual || jogadorAtual->obterDificuldade() >= 2);
    int danoFinalAposReducoes = personagemAlvo.receberDano(quantidadeDeDanoBruto, danoMagicoPerfuranteAtual, quantidadeDeDanoReduzidoPeloParry, &personagemAtacante, aplicarPassivas);

    if (danoFinalAposReducoes > 0) 
    {
        if (&personagemAlvo == jogadorAtual) 
        {
            totalDeDanoRecebido += danoFinalAposReducoes;
            std::cout << "\033[41m>> " << personagemAlvo.obterNome() << " recebeu " << danoFinalAposReducoes << " de dano\033[0m" << std::endl;
        } else {
            totalDeDanoCausado += danoFinalAposReducoes;
            std::cout << ">> " << personagemAlvo.obterNome() << " recebeu " << danoFinalAposReducoes << " de dano" << std::endl;
        }
        
        // Aplicação dos efeitos no acerto
        if (personagemAtacante.obterArma()) 
        {
            personagemAtacante.obterArma()->aoCausarDano(personagemAtacante, personagemAlvo, danoFinalAposReducoes);
        }

        personagemAtacante.obterRaca()->aoCausarDano(personagemAtacante, personagemAlvo, danoFinalAposReducoes);
    }
    else if (danoFinalAposReducoes == 0 && personagemAlvo.obterDefendendo()) 
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

void SistemaRPG::processarMorteDeInimigo(Personagem* inimigo)
{
    std::cout << "\n[!] " << inimigo->obterNome() << " derrotado! ";

    inimigo->executarDrops(jogadorAtual, itensObtidos, quantidadeDeOuroObtido, quantidadeDeXpObtido);
}

void SistemaRPG::exibirHordaDeInimigosLadoALado(const std::vector<Personagem*>& listaDeInimigos) 
{
    if (listaDeInimigos.empty()) return;
    int larguraTerminal = Menu::obterLarguraDoTerminalEmColunas();
    // Pega a arte dinamicamente da raca do inimigo que esta sendo enfrentado
    std::vector<std::string> arteDoInimigo = listaDeInimigos[0]->obterRaca()->obterAparenciaRaca();
    int quantidadeTotalDeInimigosNaHorda = static_cast<int>(listaDeInimigos.size());
    int larguraSeparadaParaCadaColuna = larguraTerminal / quantidadeTotalDeInimigosNaHorda; 

    std::cout << std::string(larguraTerminal, '-') << "\n";
    for (size_t indiceInimigo = 0; indiceInimigo < listaDeInimigos.size(); indiceInimigo++) 
    {
        std::string tagIdentificadoraDoInimigo = listaDeInimigos[indiceInimigo]->obterNome() + " [" + std::to_string(indiceInimigo) + "]";
        int espacosParaCentralizarOId = (larguraSeparadaParaCadaColuna - (int)tagIdentificadoraDoInimigo.length()) / 2;
        std::cout << std::string(espacosParaCentralizarOId > 0 ? espacosParaCentralizarOId : 0, ' ') << std::left << std::setw(larguraSeparadaParaCadaColuna - espacosParaCentralizarOId) << tagIdentificadoraDoInimigo;
    }
    std::cout << "\n";
    for (size_t indiceInimigo = 0; indiceInimigo < listaDeInimigos.size(); indiceInimigo++) 
    {
        std::string valorDePontosDeVidaDoInimigo = "HP: " + std::to_string(listaDeInimigos[indiceInimigo]->obterVida()) + "/" + std::to_string(listaDeInimigos[indiceInimigo]->obterVidaMaxima());
        int espacosParaCentralizarOHp = (larguraSeparadaParaCadaColuna - (int)valorDePontosDeVidaDoInimigo.length()) / 2;
        std::cout << std::string(espacosParaCentralizarOHp > 0 ? espacosParaCentralizarOHp : 0, ' ') << std::left << std::setw(larguraSeparadaParaCadaColuna - espacosParaCentralizarOHp) << valorDePontosDeVidaDoInimigo;
    }
    std::cout << "\n";
    
    for (size_t indiceInimigo = 0; indiceInimigo < listaDeInimigos.size(); indiceInimigo++) 
    {
        std::vector<std::pair<std::string, std::string>> debuffs;
        if (listaDeInimigos[indiceInimigo]->obterSangramento()) debuffs.push_back({"[Sangramento]", "\033[31m[Sangramento]\033[0m"});
        if (listaDeInimigos[indiceInimigo]->obterLentidao()) debuffs.push_back({"[Lentidao]", "\033[35m[Lentidao]\033[0m"});
        if (listaDeInimigos[indiceInimigo]->obterFraqueza()) debuffs.push_back({"[Fraqueza]", "\033[33m[Fraqueza]\033[0m"});
        if (listaDeInimigos[indiceInimigo]->obterQuebraResistencia()) debuffs.push_back({"[Quebra Def.]", "\033[36m[Quebra Def.]\033[0m"});

        std::string visualStr = "";
        std::string printStr = "";
        for (size_t i = 0; i < debuffs.size(); ++i) {
            visualStr += debuffs[i].first;
            printStr += debuffs[i].second;
            if (i < debuffs.size() - 1) {
                visualStr += " ";
                printStr += " ";
            }
        }

        int espacosEsquerda = (larguraSeparadaParaCadaColuna - (int)visualStr.length()) / 2;
        if (espacosEsquerda < 0) espacosEsquerda = 0;
        int espacosDireita = larguraSeparadaParaCadaColuna - espacosEsquerda - (int)visualStr.length();
        if (espacosDireita < 0) espacosDireita = 0;
        
        std::cout << std::string(espacosEsquerda, ' ') << printStr << std::string(espacosDireita, ' ');
    }
    std::cout << "\n\n";
    
    for (size_t indiceDaLinhaDaArte = 0; indiceDaLinhaDaArte < arteDoInimigo.size(); indiceDaLinhaDaArte++) 
    {
        for (size_t indiceDoInimigoParaDesenhar = 0; indiceDoInimigoParaDesenhar < listaDeInimigos.size(); indiceDoInimigoParaDesenhar++) 
        {
            int espacosParaCentralizarAArte = (larguraSeparadaParaCadaColuna - (int)arteDoInimigo[indiceDaLinhaDaArte].length()) / 2;
            std::cout << std::string(espacosParaCentralizarAArte > 0 ? espacosParaCentralizarAArte : 0, ' ') << std::left << std::setw(larguraSeparadaParaCadaColuna - espacosParaCentralizarAArte) << arteDoInimigo[indiceDaLinhaDaArte];
        }
        std::cout << "\n";
    }
    std::cout << std::string(larguraTerminal, '-') << "\n\n";
}