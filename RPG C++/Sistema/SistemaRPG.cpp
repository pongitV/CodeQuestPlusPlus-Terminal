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
    int maxDestrezaInimigos = 0;
    for (Personagem* inimigo : listaDeInimigos) 
    {
        if (inimigo->obterDestreza() > maxDestrezaInimigos) maxDestrezaInimigos = inimigo->obterDestreza();
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
                        int ouroDerrubado = (*iteradorInimigos)->obterOuroRecompensa();
                        int xpDerrubado = (*iteradorInimigos)->obterXpRecompensa();
                        jogadorAtual->ganharOuro(ouroDerrubado);
                        jogadorAtual->ganharXp(xpDerrubado);
                        quantidadeDeOuroObtido += ouroDerrubado;
                        quantidadeDeXpObtido += xpDerrubado;
                        std::cout << "\n[!] " << (*iteradorInimigos)->obterNome() << " derrotado! ";
                        std::cout << "\033[43m+" << ouroDerrubado << "G\033[0m "; 
                        std::cout << "\033[44m+" << xpDerrubado << " XP\033[0m\n"; 
                    
                    if ((*iteradorInimigos)->obterArma() && (*iteradorInimigos)->obterArma()->obterNomeItem() == "Adaga artesanal de pedra") 
                    {
                        jogadorAtual->obterInventario()->adicionarItemAoInventario(new Arma("Adaga artesanal de pedra", 5, 0));
                        std::cout << "\033[37m+Adaga artesanal de pedra\033[0m\n";
                        itensObtidos.push_back("Adaga artesanal de pedra");
                    }
                    
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
                
                Item* escudoEscolhido = jogadorAtual->obterInventario()->abrirMenuParaSelecionarEscudoDeDefesa();
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
                std::string codigoDoItemDigitado;
                do 
                {
                    Menu::exibirTelaDeInventario(jogadorAtual);
                    std::string mensagemDoPrompt = "Digite o codigo do item ou [0] VOLTAR: ";
                    int espacosDaMensagem = (larguraDoTerminal - (int)mensagemDoPrompt.length()) / 2;
                    std::cout << "\n" << std::string(espacosDaMensagem > 0 ? espacosDaMensagem : 0, ' ') << mensagemDoPrompt;
                    std::cin >> codigoDoItemDigitado;
    
                    if (codigoDoItemDigitado != "0")
                    {
                        Item* itemEncontrado = jogadorAtual->obterInventario()->buscarItemPeloCodigoDigitado(
                            codigoDoItemDigitado, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura()
                        );
    
                        if (itemEncontrado && dynamic_cast<PocaoCura*>(itemEncontrado))
                        {
                            if (turnoFoiConsumido) 
                            {
                                std::cout << "\n[SISTEMA]: Voce ja usou um item neste turno!\n";
                                Menu::aguardarPressionamentoDeEnter();
                            } 
                            else 
                            {
                                std::string nomeDoItemEncontrado = itemEncontrado->obterNomeItem(); 
                                int quantidadeDeCura = static_cast<int>(jogadorAtual->obterVidaMaxima() * 0.30);
                                
                                jogadorAtual->modificarVida(quantidadeDeCura); 
                                jogadorAtual->obterInventario()->removerItemDoInventarioPeloNome(nomeDoItemEncontrado);
                                
                                std::cout << "\n[SISTEMA]: " << nomeDoItemEncontrado << " usada! +" << quantidadeDeCura << " HP.\n";
                                Menu::aguardarPressionamentoDeEnter();
                                turnoFoiConsumido = true;
                            }
                        }
                        else if (itemEncontrado)
                        {
                            std::cout << "\n[SISTEMA]: Este item nao pode ser usado em combate!\n";
                            Menu::aguardarPressionamentoDeEnter();
                        }
                    }
                } while (codigoDoItemDigitado != "0");
                break;
            }
            case 5: // --- 5. JOGADOR ---
            {
                std::string opcaoEscolhidaNoMenuJogador;
                do 
                {
                    Menu::exibirTelaDeAtributosDoJogador(jogadorAtual);
                    std::string mensagemDeOpcoes = "[0] VOLTAR (combate) | [1] ALTERAR PARRY";
                    if (jogadorAtual->podeSubirDeNivel()) mensagemDeOpcoes += " | [2] SUBIR DE NIVEL";
                    mensagemDeOpcoes += ": ";
                    int espacosDaMensagem = (larguraDoTerminal - (int)mensagemDeOpcoes.length()) / 2;
                    std::cout << "\n" << std::string(espacosDaMensagem > 0 ? espacosDaMensagem : 0, ' ') << mensagemDeOpcoes;
                    std::cin >> opcaoEscolhidaNoMenuJogador;

                    if (opcaoEscolhidaNoMenuJogador == "1") {
                        jogadorAtual->definirParryAtivado(!jogadorAtual->obterParryAtivado());
                    } else if (opcaoEscolhidaNoMenuJogador == "2" && jogadorAtual->podeSubirDeNivel()) {
                        std::string atributoEscolhidoParaEvoluir;
                        std::cout << "\nDigite o atributo para melhorar (Vida, Forca, Destreza, Resistencia, Constituicao, Inteligencia, Sabedoria): ";
                        std::cin >> atributoEscolhidoParaEvoluir;
                        if (jogadorAtual->subirDeNivel(atributoEscolhidoParaEvoluir)) {
                            std::cout << "[SISTEMA]: Nivel subiu! " << atributoEscolhidoParaEvoluir << " melhorado.\n";
                            Menu::aguardarPressionamentoDeEnter();
                        } else {
                            std::cout << "[ERRO]: Atributo invalido.\n";
                            Menu::aguardarPressionamentoDeEnter();
                        }
                    }
                } while (opcaoEscolhidaNoMenuJogador != "0");
                break;
            }
            default:
                break;
            }
            
            if (verificarCondicaoDeVitoriaOuDerrota()) return; 
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
            if (jogadorAtual->obterVida() > 0) 
            {
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

    if (personagemAtacante->obterArma()) 
    {
        danoFisicoDaArma = personagemAtacante->obterArma()->obterDanoFisico();
        danoMagicoDaArma = personagemAtacante->obterArma()->obterDanoMagico();
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

    int bonusDeDefesaDaArmadura = (personagemAlvo->obterArmadura()) ? personagemAlvo->obterArmadura()->obterReducaoFixa() : 0;
    
    // Resistencia atua como reduçao fixa base
    int totalDeReducaoFixaDeDano = personagemAlvo->obterResistencia() + bonusDeDefesaDaArmadura;
    
    // Constituicao atua como reducao percentual (produto) com limite de 50%
    double percentualDeReducao = personagemAlvo->obterConstituicao() / 100.0;
    if (percentualDeReducao > 0.50) percentualDeReducao = 0.50; // Hardcap de 50% de absorcao maxima
    
    double multiplicadorDeConstituicao = 1.0 - percentualDeReducao;

    int danoFinalAposReducoes = static_cast<int>((quantidadeDeDanoBruto - totalDeReducaoFixaDeDano) * multiplicadorDeConstituicao);
    if (danoFinalAposReducoes < 1) danoFinalAposReducoes = 1; // Impacto basico minimo

    // Logica do Parry
    if (personagemAlvo == jogadorAtual && jogadorAtual->obterParryAtivado()) 
    {
        int quantidadeDeNumerosDoParry = std::max(1, danoFinalAposReducoes / 4);
        int destrezaDoAtacante = std::max(1, personagemAtacante->obterDestreza());
        int tempoLimiteParaParryEmSegundos = std::max(1, 50 / destrezaDoAtacante);

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
            personagemAlvo->obterInventario()->removerItemDoInventarioPeloNome(personagemAlvo->obterEscudo()->obterNomeItem());
            personagemAlvo->desequiparEscudo(); // Fica sem escudo
        }
    }

    // Passivas defensivas de raca (Dwarf/Humano) somente aplicam no Normal(2) ou Dificil(3), ou se for o Jogador
    if (personagemAlvo == jogadorAtual || jogadorAtual->obterDificuldade() >= 2) 
    {
        danoFinalAposReducoes = personagemAlvo->obterRaca()->processarDanoDefensivo(danoFinalAposReducoes, personagemAlvo);
    }

    if (danoFinalAposReducoes > 0) 
    {
        personagemAlvo->modificarVida(-danoFinalAposReducoes);
        
        if (personagemAlvo == jogadorAtual) 
        {
            totalDeDanoRecebido += danoFinalAposReducoes;
            std::cout << "\033[41m>> " << personagemAlvo->obterNome() << " recebeu " << danoFinalAposReducoes << " de dano\033[0m" << std::endl;
        } else {
            totalDeDanoCausado += danoFinalAposReducoes;
            std::cout << ">> " << personagemAlvo->obterNome() << " recebeu " << danoFinalAposReducoes << " de dano" << std::endl;
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
        Menu::exibirTelaDeVitoria(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, itensObtidos);
        return true; 
    }
    if (jogadorAtual->obterVida() <= 0) 
    { 
        Menu::exibirTelaDeDerrota(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido); 
        return true; 
    }
    return false;
}