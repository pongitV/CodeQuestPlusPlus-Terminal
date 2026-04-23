#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <map> 
#include <chrono>
#include <thread>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

#include "Menu.h"
#include "../Inventario/Item.h"
#include "../Inventario/ItemConsumivel.h"
#include "../Sistema/GeradorInimigos.h"

#include "../Interfaces/TelaInventario.h"
#include "../Interfaces/TelaAtributos.h"

// Inclusao das Racas
#include "../Raças/RacaBase.h"
#include "../Raças/RacaDwarf.h"
#include "../Raças/RacaElfo.h" 
#include "../Raças/RacaHumano.h"
#include "../Raças/RacaOrk.h"

// Inclusao das Classes
#include "../Classes/ClasseBase.h"
#include "../Classes/ClasseArqueiro.h"
#include "../Classes/ClasseBardo.h"
#include "../Classes/ClasseGuerreiro.h"
#include "../Classes/ClasseMago.h"

void Menu::maximizarJanelaDoTerminal()
{
#ifdef _WIN32
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_MAXIMIZE);
#endif
}

int Menu::obterLarguraDoTerminalEmColunas() 
{
    int largura = 120; // Tamanho padrão de fallback

#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) 
    {
        largura = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
#endif
    return largura;
}

void Menu::limparTelaDoTerminal() 
{
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void Menu::aguardarPressionamentoDeEnter() 
{
    std::cout << "\nPressione Enter para continuar...";
#ifdef _WIN32
    // Limpa teclas residuais pressionadas antes da hora e aguarda estritamente pelo Enter (ASCII 13)
    while (_kbhit()) _getch();
    while (_getch() != 13) {}
#else
    std::cin.clear(); 
    if (std::cin.rdbuf()->in_avail() > 0) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::string temp;
    std::getline(std::cin, temp);
#endif
}

void Menu::imprimirTextoComEfeitoDeDigitacao(const std::string& textoParaImprimir, int tempoDeEsperaEmMilissegundos)
{
    // 1. Salva a posição atual do cursor para não perder o lugar do texto
    // 2. Move para a linha 24, coluna 1 (canto inferior esquerdo)
    // 3. Imprime a mensagem em cinza para não distrair
    // 4. Volta para a posição original
    std::cout << "\033[s\033[80;1H\033[1;90m[Pressione 'k' para pular]\033[u";

    for (size_t i = 0; i < textoParaImprimir.length(); ++i)
    {
        // Verifica se o usuário pressionou a tecla 'k'
        #ifdef _WIN32
            if (_kbhit()) 
            {
                char tecla = _getch();
                if (tecla == 'k' || tecla == 'K') 
                {
                    // Limpa a linha do "pular" antes de sair para não deixar lixo
                    std::cout << "\033[s\033[24;1H\033[K\033[u";
                    
                    // Pula o restante do texto
                    std::cout << textoParaImprimir.substr(i) << std::flush;
                    return; 
                }
            }
        #endif

        // Imprime o caractere atual do diálogo
        std::cout << textoParaImprimir[i] << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(tempoDeEsperaEmMilissegundos));
    }
    std::cout << std::endl;
}

bool Menu::exibirConfirmacaoDeEscolhaComArteLadoALado(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha, const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir) 
{
    limparTelaDoTerminal();
    exibirLogoDoJogo("PREVIA DA " + tipoDeEscolha + ": " + nomeDaEscolha);
    int larguraTerminal = obterLarguraDoTerminalEmColunas();

    int larguraArte = 0;
    for (const std::string& l : arteAsciiParaExibir) if ((int)l.length() > larguraArte) larguraArte = (int)l.length();
    
    int larguraInfo = 40;
    for (const std::string& s : informacoesParaExibir) if ((int)s.length() > larguraInfo) larguraInfo = (int)s.length();
    
    int gap = 6;
    int recuo = (larguraTerminal - (larguraInfo + gap + larguraArte)) / 2;
    if (recuo < 0) recuo = 0;

    size_t maxL = std::max(arteAsciiParaExibir.size(), informacoesParaExibir.size());
    for (size_t i = 0; i < maxL; ++i) 
    {
        std::cout << std::string(recuo, ' ');
        if (i < informacoesParaExibir.size()) std::cout << std::left << std::setw(larguraInfo) << informacoesParaExibir[i];
        else std::cout << std::string(larguraInfo, ' ');
        std::cout << std::string(gap, ' ');
        if (i < arteAsciiParaExibir.size()) std::cout << arteAsciiParaExibir[i];
        std::cout << "\n";
    }

    std::cout << "\n" << std::string(recuo, ' ') << "0. VOLTAR | 1. CONFIRMAR\n";
    std::cout << std::string(recuo, ' ') << "Escolha: ";
    int confirma; 
    if (!(std::cin >> confirma)) { std::cin.clear(); std::cin.ignore(1000, '\n'); return false; }
    return confirma == 1;
}

void Menu::imprimirLinhasCentralizadasNaTela(const std::vector<std::string>& linhasParaImprimir, int larguraVisualDaArte, const std::string& codigoDeCorAnsi) 
{
    int larguraConsole = obterLarguraDoTerminalEmColunas();
    
    if (larguraVisualDaArte <= 0) 
    {
        for (const std::string& linhaTexto : linhasParaImprimir) 
        {
            if ((int)linhaTexto.length() > larguraVisualDaArte) larguraVisualDaArte = (int)linhaTexto.length();
        }
    }
    
    int espacosPad = (larguraConsole > larguraVisualDaArte) ? (larguraConsole - larguraVisualDaArte) / 2 : 0;
    std::string padding(espacosPad > 0 ? espacosPad : 0, ' ');

    for (const std::string& linhaTexto : linhasParaImprimir) 
    {
        std::cout << padding << (codigoDeCorAnsi.empty() ? "" : codigoDeCorAnsi) << linhaTexto << (codigoDeCorAnsi.empty() ? "" : "\033[0m") << "\n";
    }
}

std::unique_ptr<Personagem> Menu::iniciarCriacaoDePersonagem() 
{
    std::string nomeDoPersonagem = "";
    std::unique_ptr<RacaBase> racaEscolhida;
    std::unique_ptr<ClasseBase> classeEscolhida;
    bool sistemaDeParryAtivado = false;
    int nivelDeDificuldadeEscolhido = 2;
    int etapaDeCriacaoAtual = 1; 

    auto formatarAtributo = [](const std::string& n, int v) 
    {
        return " - " + n + ": " + (v >= 0 ? "+" : "") + std::to_string(v);
    };

    while (etapaDeCriacaoAtual <= 5) 
    {
        if (etapaDeCriacaoAtual == 1) // --- ETAPA 1: NOME ---
        {
            limparTelaDoTerminal();
            exibirLogoDoJogo("INTRODUCAO AO RPG");
            imprimirTextoComEfeitoDeDigitacao(" [NARRACAO]: O reino clama por um novo destino...\n", 35);
            imprimirTextoComEfeitoDeDigitacao(" [NARRACAO]: E todas lendas possuem um nome.\n\n", 35);
            std::cout << " > Escolha o nome do seu personagem (ou '0' para sair): ";
            
            std::string entrada;
            std::getline(std::cin >> std::ws, entrada);

            if (entrada == "0") exit(0);
            nomeDoPersonagem = entrada;
            if (!nomeDoPersonagem.empty()) etapaDeCriacaoAtual = 2;
        }
        else if (etapaDeCriacaoAtual == 2) // --- ETAPA 2: RACA ---
        {
            limparTelaDoTerminal();
            exibirLogoDoJogo("SELECAO DE RACA");
            std::cout << "JOGADOR: " << nomeDoPersonagem << "\n";
            std::cout << std::string(obterLarguraDoTerminalEmColunas(), '-') << "\n";
            imprimirTextoComEfeitoDeDigitacao(" [NARRACAO]: Qual sua origem?\n\n", 35);
            std::cout << "  [1] Dwarf\n";
            std::cout << "  [2] Elfo\n";
            std::cout << "  [3] Humano\n";
            std::cout << "  [4] Ork\n";
            std::cout << "\n  [0] VOLTAR (selecao de nome)\n";
            std::cout << "\n > Sua escolha: ";
            
            int escolha;
            if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }
            if (escolha == 0) { etapaDeCriacaoAtual = 1; continue; }

            std::unique_ptr<RacaBase> tempRaca;
            switch(escolha) {
                case 1: tempRaca = std::make_unique<RacaDwarf>(); break;
                case 2: tempRaca = std::make_unique<RacaElfo>();  break;
                case 3: tempRaca = std::make_unique<RacaHumano>(); break;
                case 4: tempRaca = std::make_unique<RacaOrk>();   break;
            }

            if (tempRaca) 
            {
                Atributos stats = tempRaca->obterAtributosRaca();
                std::vector<std::string> info = {
                    "[ ATRIBUTOS BASE DE RAÇA ]",
                    formatarAtributo("Vida", stats.vida),
                    formatarAtributo("Forca", stats.forca),
                    formatarAtributo("Destreza", stats.destreza),
                    formatarAtributo("Resistencia", stats.resistencia),
                    formatarAtributo("Constituicao", stats.constituicao),
                    formatarAtributo("Inteligencia", stats.inteligencia),
                    formatarAtributo("Sabedoria", stats.sabedoria),
                    "",
                    "[ HABILIDADE PASSIVA ]",
                    " " + tempRaca->obterNomeHabilidadeRaca(),
                    " - " + tempRaca->obterDescricaoHabilidadeRaca()
                };
                
                if (exibirConfirmacaoDeEscolhaComArteLadoALado("RACA", tempRaca->obterNomeRaca(), info, tempRaca->obterAparenciaRaca())) 
                {
                    racaEscolhida = std::move(tempRaca); 
                    etapaDeCriacaoAtual = 3;
                }
            }
        }
        else if (etapaDeCriacaoAtual == 3) // --- ETAPA 3: CLASSE ---
        {
            limparTelaDoTerminal();
            exibirLogoDoJogo("SELECAO DE CLASSE");
            std::cout << "JOGADOR: " << nomeDoPersonagem << " | RACA: " << racaEscolhida->obterNomeRaca() << "\n";
            std::cout << std::string(obterLarguraDoTerminalEmColunas(), '-') << "\n";
            imprimirTextoComEfeitoDeDigitacao(" [NARRACAO]: Qual caminho voce seguira neste mundo?\n\n", 35);
            
            std::cout << "  [1] Arqueiro\n";
            std::cout << "  [2] Bardo\n";
            std::cout << "  [3] Guerreiro\n";
            std::cout << "  [4] Mago\n";
            std::cout << "\n  [0] VOLTAR (selecao de raca)\n";
            std::cout << "\n > Sua escolha: ";
            
            int escolha;
            if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }
            if (escolha == 0) { racaEscolhida.reset(); etapaDeCriacaoAtual = 2; continue; }

            std::unique_ptr<ClasseBase> temp;
            switch(escolha) 
            {
                case 1: temp = std::make_unique<ClasseArqueiro>(); break;
                case 2: temp = std::make_unique<ClasseBardo>(); break;
                case 3: temp = std::make_unique<ClasseGuerreiro>(); break;
                case 4: temp = std::make_unique<ClasseMago>(); break;
            }

            if (temp) 
            {
                Atributos stats = temp->obterAtributosClasse();
                
                std::string passivaNome = "Nenhuma";
                std::string passivaDesc = "";
                std::string recargaHab = "";

                if (temp->obterTipoClasse() == TipoClasse::Arqueiro) {
                    passivaNome = "Passos leves";
                    passivaDesc = "Penalidade de armaduras e debuffs de lentidao reduzidos pela metade.";
                    recargaHab = "Recarga: 1 turno.";
                } else if (temp->obterTipoClasse() == TipoClasse::Bardo) {
                    passivaNome = "Touch the sky";
                    passivaDesc = "Curas e buffs recebidos sao 40% mais fortes.";
                    recargaHab = "Recarga: 3 turnos (Individuais).";
                } else if (temp->obterTipoClasse() == TipoClasse::Guerreiro) {
                    passivaNome = "Golpe decisivo";
                    passivaDesc = "Causa +10%/+20%/+30% de dano em inimigos com menos de 30%/20%/10% de HP.";
                    recargaHab = "Recarga: 3 turnos.";
                } else if (temp->obterTipoClasse() == TipoClasse::Mago) {
                    passivaNome = "Foco arcano";
                    passivaDesc = "Ataques ressoam (25% em area) ou causam +25% de dano em alvo unico.";
                    recargaHab = "Recarga: 3 turnos.";
                }

                std::vector<std::string> info = 
                {
                    "[ ATRIBUTOS BONUS DA CLASSE ]",
                    formatarAtributo("Vida", stats.vida),
                    formatarAtributo("Forca", stats.forca),
                    formatarAtributo("Destreza", stats.destreza),
                    formatarAtributo("Resistencia", stats.resistencia),
                    formatarAtributo("Constituicao", stats.constituicao),
                    formatarAtributo("Inteligencia", stats.inteligencia),
                    formatarAtributo("Sabedoria", stats.sabedoria),
                    "",
                    "[ HABILIDADE PASSIVA DA CLASSE ]",
                    " " + passivaNome,
                    " - " + passivaDesc,
                    "",
                    "[ HABILIDADE ATIVA DA CLASSE ]",
                    " " + temp->obterNomeHabilidadeClasse(),
                    " - " + temp->obterDescricaoHabilidadeClasse(),
                    " - " + recargaHab,
                    "",
                    "[ EQUIPAMENTO INICIAL DA CLASSE ]"
                };
                
                auto kit = temp->obterEquipamentoClasse();
                std::map<std::string, std::pair<int, TipoEquipamento>> contagem;
                for (const auto& i : kit) {
                    contagem[i->obterNomeItem() + i->obterInfoStatus()].first++;
                    contagem[i->obterNomeItem() + i->obterInfoStatus()].second = i->obterTipo();
                }
                
                std::vector<TipoEquipamento> ordemPrioridade = {
                    TipoEquipamento::ARMA, TipoEquipamento::ESCUDO, 
                    TipoEquipamento::ARMADURA, TipoEquipamento::CONSUMIVEL
                };
                for (TipoEquipamento tipo : ordemPrioridade) {
                    for (auto const& [nomeI, dados] : contagem) {
                        if (dados.second == tipo) {
                            info.push_back(" - " + std::to_string(dados.first) + "x " + nomeI);
                        }
                    }
                }

                if (exibirConfirmacaoDeEscolhaComArteLadoALado("CLASSE", temp->obterNomeClasse(), info, temp->obterAparenciaClasseMenu())) 
                {
                    classeEscolhida = std::move(temp); 
                    etapaDeCriacaoAtual = 4;
                }
            }
        }
        else if (etapaDeCriacaoAtual == 4) // --- ETAPA 4: CONFIGURACOES DO JOGO ---
        {
            limparTelaDoTerminal();
            exibirLogoDoJogo("CONFIGURACOES DO JOGO");
            std::cout << "JOGADOR: " << nomeDoPersonagem << " | RACA: " << racaEscolhida->obterNomeRaca() << " | CLASSE: " << classeEscolhida->obterNomeClasse() << "\n";
            std::cout << std::string(obterLarguraDoTerminalEmColunas(), '-') << "\n";
            imprimirTextoComEfeitoDeDigitacao(" [SISTEMA]: Deseja ativar o sistema de PARRY?\n\n", 35);
            imprimirTextoComEfeitoDeDigitacao(" (Permite reduzir danos ao digitar uma sequencia de numeros num tempo limite)\n\n", 35);
            
            std::cout << "  [1] LIGAR Parry\n";
            std::cout << "  [2] DESLIGAR Parry\n";
            std::cout << "\n  [0] VOLTAR (selecao de classe)\n";
            std::cout << "\n > Sua escolha: ";
            
            int escolha;
            if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }
            if (escolha == 0) { classeEscolhida.reset(); etapaDeCriacaoAtual = 3; continue; }

            if (escolha == 1 || escolha == 2) 
            {
                sistemaDeParryAtivado = (escolha == 1);
                etapaDeCriacaoAtual = 5;
            }
        }
        else if (etapaDeCriacaoAtual == 5) // --- ETAPA 5: DIFICULDADE ---
        {
            limparTelaDoTerminal();
            exibirLogoDoJogo("DIFICULDADE DO MUNDO");
            std::cout << "JOGADOR: " << nomeDoPersonagem << " | RACA: " << racaEscolhida->obterNomeRaca() << " | CLASSE: " << classeEscolhida->obterNomeClasse() << "\n";
            std::cout << std::string(obterLarguraDoTerminalEmColunas(), '-') << "\n";
            imprimirTextoComEfeitoDeDigitacao(" [SISTEMA]: Escolha o nivel de desafio da sua jornada:\n\n", 35);
            
            std::cout << "  [1] FACIL   (Inimigos com 1x Atributos, sem habilidades de raca e sem classe)\n";
            std::cout << "  [2] NORMAL  (Inimigos com 1.5x Atributos, com habilidades de raca mas sem classes)\n";
            std::cout << "  [3] DIFICIL (Inimigos com 2x Atributos, com habilidades de raca e com classes)\n";
            std::cout << "\n  [0] VOLTAR (configuracao de parry)\n";
            std::cout << "\n > Sua escolha: ";
            
            int escolha;
            if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }
            if (escolha == 0) { etapaDeCriacaoAtual = 4; continue; }

            if (escolha >= 1 && escolha <= 3) 
            {
                nivelDeDificuldadeEscolhido = escolha;
                etapaDeCriacaoAtual = 6;
            }
        }
    }
    auto personagemCriado = std::make_unique<Personagem>(nomeDoPersonagem, std::move(racaEscolhida), std::move(classeEscolhida));
    personagemCriado->definirParryAtivado(sistemaDeParryAtivado);
    personagemCriado->definirDificuldade(nivelDeDificuldadeEscolhido);
    std::cout << "\n";
    imprimirTextoComEfeitoDeDigitacao(" [SISTEMA]: Personagem criado com sucesso! Iniciando jornada...\n", 35);
    aguardarPressionamentoDeEnter();
    return personagemCriado;
}

void Menu::exibirHordaDeInimigosLadoALado(const std::vector<Personagem*>& listaDeInimigos) 
{
    if (listaDeInimigos.empty()) return;
    int larguraTerminal = obterLarguraDoTerminalEmColunas();
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
            if (listaDeInimigos[indiceInimigo]->possuiEfeito("Sangramento")) debuffs.push_back({"[Sangramento]", "\033[31m[Sangramento]\033[0m"});
            if (listaDeInimigos[indiceInimigo]->possuiEfeito("Lentidao")) debuffs.push_back({"[Lentidao]", "\033[35m[Lentidao]\033[0m"});
            if (listaDeInimigos[indiceInimigo]->possuiEfeito("Fraqueza")) debuffs.push_back({"[Fraqueza]", "\033[33m[Fraqueza]\033[0m"});
            if (listaDeInimigos[indiceInimigo]->possuiEfeito("QuebraResistencia")) debuffs.push_back({"[Quebra Def.]", "\033[36m[Quebra Def.]\033[0m"});

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

void Menu::exibirLogoDoJogo(const std::string& tituloDaTela) 
{
#ifdef _WIN32
    SetConsoleOutputCP(65001); 
#endif
    int larguraConsole = obterLarguraDoTerminalEmColunas();
    
    std::vector<std::string> logoTexto = 
    {
        "   █████████     ███████    ██████████   ██████████       ██████    █████  █████ ██████████  █████████  ███████████  ",
        "  ███░░░░░███  ███░░░░░███ ░░███░░░░███ ░░███░░░░░█     ███░░░░███ ░░███  ░░███ ░░███░░░░░█ ███░░░░░███░█░░░███░░░█  ",
        " ███     ░░░  ███     ░░███ ░███   ░░███ ░███  █ ░     ███    ░░███ ░███   ░███  ░███  █ ░ ░███    ░░░ ░   ░███  ░   ",
        "░███         ░███      ░███ ░███    ░███ ░██████      ░███     ░███ ░███   ░███  ░██████   ░░█████████     ░███      ",
        "░███         ░███      ░███ ░███    ░███ ░███░░█      ░███   ██░███ ░███   ░███  ░███░░█    ░░░░░░░░███    ░███      ",
        "░░███     ███░░███     ███  ░███    ███  ░███ ░   █   ░░███ ░░████  ░███   ░███  ░███ ░   █ ███    ░███    ░███      ",
        " ░░█████████  ░░░███████░   ██████████   ██████████    ░░░██████░██ ░░████████   ██████████░░█████████     █████     ",
        "  ░░░░░░░░░     ░░░░░░░    ░░░░░░░░░░   ░░░░░░░░░░      ░░░░░░ ░░   ░░░░░░░░   ░░░░░░░░░░  ░░░░░░░░░     ░░░░░       "
    };

    std::vector<std::string> logoPlus = 
    {
       "                          ",
       "     ███         ███      ",
       "    ░███        ░███      ",
       " ███████████ ███████████  ",
       "░░░░░███░░░ ░░░░░███░░░   ",
       "    ░███        ░███      ",
       "    ░░░         ░░░       ",
       "                          "               
    };

    std::cout << "\n" << std::string(larguraConsole, '=') << "\n\n";

    int larguraLinhaCompleta = 140; 

    for (size_t i = 0; i < logoTexto.size(); ++i) 
    {
        // Cálculo de recuo para centralizar a linha inteira
        int recuo = (larguraConsole - larguraLinhaCompleta) / 2;
        if (recuo < 0) recuo = 0;
        std::cout << std::string(recuo, ' ');

        // Imprime a parte principal (Branco/Padrão)
        std::cout << logoTexto[i];

        // Imprime o ++ (Laranja)
        std::cout << "\x1b[38;5;208m" << logoPlus[i] << "\x1b[0m";

        std::cout << "\n";
    }

    std::cout << "\n";
    
    // 4. IMPRESSÃO DO TÍTULO (Igual ao seu original)
    if (tituloDaTela.empty()) 
    {
        std::cout << std::string(larguraConsole, '=') << "\n\n";
    } 
    else 
    {
        std::cout << std::string(larguraConsole, '=') << "\n";
        int espacos = (larguraConsole - (int)tituloDaTela.length()) / 2;
        std::cout << std::string(espacos > 0 ? espacos : 0, ' ') << tituloDaTela << "\n";
        std::cout << std::string(larguraConsole, '=') << "\n\n";
    }
}
