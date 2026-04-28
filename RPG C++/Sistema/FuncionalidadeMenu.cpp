#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <map> 
#include <chrono>
#include <thread>
#include <fstream>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

#include "FuncionalidadeMenu.h"
#include "SimplificacoesAparencia.h"
#include "../Inventario/Item.h"
#include "../Inventario/ItemConsumivel.h"
#include "../Sistema/GeradorInimigos.h"
#include "../Sistema/GerenciadorDeSave.h"

#include "../Interfaces/TelaInventario.h"
#include "../Interfaces/TelaAtributos.h"
#include "../Sistema/Tipos.h"

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

bool Menu::exibirConfirmacaoDeEscolhaComArteLadoALado(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha, const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir) 
{
    SimplificacoesAparencia::limparTela();
    exibirLogoDoJogo("PREVIA DA " + tipoDeEscolha + ": " + nomeDaEscolha);
    int larguraTerminal = SimplificacoesAparencia::obterLarguraTerminal();

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

std::unique_ptr<Personagem> Menu::menuPrincipal() 
{
    while (true) {
        SimplificacoesAparencia::limparTela();
        exibirLogoDoJogo("MENU PRINCIPAL");
        bool temSave = GerenciadorDeSave::saveExiste();
        
        std::cout << "\n";
        int espacos = std::max(0, (SimplificacoesAparencia::obterLarguraTerminal() - 20) / 2);
        std::string mE(espacos, ' ');
        std::cout << mE << "[1] Novo Jogo\n";
        if (temSave) std::cout << mE << "[2] Continuar Jogo\n";
        std::cout << mE << "[0] Sair\n\n";
        std::cout << mE << "Escolha: ";
        
        std::string escolha;
        std::cin >> escolha;
        
        if (escolha == "1") {
            return iniciarCriacaoDePersonagem();
        } else if (escolha == "2" && temSave) {
            auto saves = GerenciadorDeSave::listarSaves();
            if (saves.empty()) continue;

            SimplificacoesAparencia::limparTela();
            exibirLogoDoJogo("CARREGAR JOGO");
            std::cout << "\n" << mE << "Selecione o save que deseja carregar:\n\n";
            
            for (size_t i = 0; i < saves.size(); ++i) {
                std::ifstream arquivoSave(saves[i]);
                if (arquivoSave.is_open()) {
                    std::string nome, racaStr, classeStr;
                    std::getline(arquivoSave, nome);
                    std::getline(arquivoSave, racaStr);
                    std::getline(arquivoSave, classeStr);
                    int nivel;
                    arquivoSave >> nivel;
                    std::cout << mE << "[" << i + 1 << "] " << nome << " | Nv " << nivel << " | " << classeStr << " | " << racaStr << "\n";
                } else {
                    std::string nomeExibicao = saves[i].substr(5, saves[i].size() - 9);
                    std::cout << mE << "[" << i + 1 << "] " << nomeExibicao << "\n";
                }
            }
            std::cout << "\n" << mE << "[0] Voltar\n\n";
            std::cout << mE << "Escolha: ";
            
            int escolhaSave;
            if (!(std::cin >> escolhaSave)) { std::cin.clear(); std::cin.ignore(1000, '\n'); continue; }
            
            if (escolhaSave > 0 && escolhaSave <= (int)saves.size()) {
                auto jogador = GerenciadorDeSave::carregarJogo(saves[escolhaSave - 1]);
                if (jogador) {
                    std::cout << "\n" << mE << "[SISTEMA]: Jogo carregado com sucesso!\n";
                    SimplificacoesAparencia::aguardarEnter();
                    return jogador;
                } else {
                    std::cout << "\n" << mE << "[ERRO]: Falha ao carregar o save!\n";
                    SimplificacoesAparencia::aguardarEnter();
                }
            }
        } else if (escolha == "0") {
            return nullptr;
        }
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

    while (etapaDeCriacaoAtual <= 5) 
    {
        switch (etapaDeCriacaoAtual) 
        {
            case 1: etapaEscolherNome(nomeDoPersonagem, etapaDeCriacaoAtual); break;
            case 2: etapaEscolherRaca(nomeDoPersonagem, racaEscolhida, etapaDeCriacaoAtual); break;
            case 3: etapaEscolherClasse(nomeDoPersonagem, racaEscolhida.get(), classeEscolhida, etapaDeCriacaoAtual); break;
            case 4: etapaConfigurarParry(nomeDoPersonagem, racaEscolhida.get(), classeEscolhida.get(), sistemaDeParryAtivado, etapaDeCriacaoAtual); break;
            case 5: etapaEscolherDificuldade(nomeDoPersonagem, racaEscolhida.get(), classeEscolhida.get(), nivelDeDificuldadeEscolhido, etapaDeCriacaoAtual); break;
        }
    }
    auto personagemCriado = std::make_unique<Personagem>(nomeDoPersonagem, std::move(racaEscolhida), std::move(classeEscolhida));
    personagemCriado->definirParryAtivado(sistemaDeParryAtivado);
    personagemCriado->definirDificuldade(nivelDeDificuldadeEscolhido);
    std::cout << "\n";
    SimplificacoesAparencia::imprimirDigitando(" [SISTEMA]: Personagem criado com sucesso! Iniciando jornada...\n", 35);
    SimplificacoesAparencia::aguardarEnter();
    return personagemCriado;
}

void Menu::exibirHordaDeInimigosLadoALado(const std::vector<Personagem*>& listaDeInimigos) 
{
    if (listaDeInimigos.empty()) return;
    int larguraTerminal = SimplificacoesAparencia::obterLarguraTerminal();
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
            auto nomesEfeitos = listaDeInimigos[indiceInimigo]->obterNomesEfeitosAtivos();
            for (auto& nome : nomesEfeitos) {
                if (nome == EfeitoNomes::SANGRAMENTO) debuffs.push_back({"[Sangramento]", SimplificacoesAparencia::cor(Cor::VERMELHO) + "[Sangramento]" + SimplificacoesAparencia::cor(Cor::RESET)});
                else if (nome == EfeitoNomes::LENTIDAO) debuffs.push_back({"[Lentidao]", SimplificacoesAparencia::cor(Cor::MAGENTA) + "[Lentidao]" + SimplificacoesAparencia::cor(Cor::RESET)});
                else if (nome == EfeitoNomes::FRAQUEZA) debuffs.push_back({"[Fraqueza]", SimplificacoesAparencia::cor(Cor::AMARELO) + "[Fraqueza]" + SimplificacoesAparencia::cor(Cor::RESET)});
                else if (nome == EfeitoNomes::QUEBRA_RESISTENCIA) debuffs.push_back({"[Quebra Def.]", SimplificacoesAparencia::cor(Cor::CIANO) + "[Quebra Def.]" + SimplificacoesAparencia::cor(Cor::RESET)});
            }

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

void Menu::etapaEscolherNome(std::string& nomeDoPersonagem, int& etapaAtual)
{
    SimplificacoesAparencia::limparTela();
    exibirLogoDoJogo("INTRODUCAO AO RPG");
    SimplificacoesAparencia::imprimirDigitando(" [NARRACAO]: O reino clama por um novo destino...\n", 35);
    SimplificacoesAparencia::imprimirDigitando(" [NARRACAO]: E todas lendas possuem um nome.\n\n", 35);
    std::cout << " > Escolha o nome do seu personagem (ou '0' para sair): ";
    
    std::string entrada;
    std::getline(std::cin >> std::ws, entrada);

    if (entrada == "0") exit(0);
    nomeDoPersonagem = entrada;
    if (!nomeDoPersonagem.empty()) etapaAtual = 2;
}

void Menu::etapaEscolherRaca(const std::string& nome, std::unique_ptr<RacaBase>& racaEscolhida, int& etapaAtual)
{
    SimplificacoesAparencia::limparTela();
    exibirLogoDoJogo("SELECAO DE RACA");
    std::cout << "JOGADOR: " << nome << "\n";
    std::cout << std::string(SimplificacoesAparencia::obterLarguraTerminal(), '-') << "\n";
    SimplificacoesAparencia::imprimirDigitando(" [NARRACAO]: Qual sua origem?\n\n", 35);
    std::cout << "  [1] Dwarf\n  [2] Elfo\n  [3] Humano\n  [4] Ork\n\n  [0] VOLTAR (selecao de nome)\n\n > Sua escolha: ";
    
    int escolha;
    if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); return; }
    if (escolha == 0) { etapaAtual = 1; return; }

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
        auto formatarAtributo = [](const std::string& n, int v) { return " - " + n + ": " + (v >= 0 ? "+" : "") + std::to_string(v); };
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
            etapaAtual = 3;
        }
    }
}

void Menu::etapaEscolherClasse(const std::string& nome, RacaBase* raca, std::unique_ptr<ClasseBase>& classeEscolhida, int& etapaAtual)
{
    SimplificacoesAparencia::limparTela();
    exibirLogoDoJogo("SELECAO DE CLASSE");
    std::cout << "JOGADOR: " << nome << " | RACA: " << raca->obterNomeRaca() << "\n";
    std::cout << std::string(SimplificacoesAparencia::obterLarguraTerminal(), '-') << "\n";
    SimplificacoesAparencia::imprimirDigitando(" [NARRACAO]: Qual caminho voce seguira neste mundo?\n\n", 35);
    
    std::cout << "  [1] Arqueiro\n  [2] Bardo\n  [3] Guerreiro\n  [4] Mago\n\n  [0] VOLTAR (selecao de raca)\n\n > Sua escolha: ";
    
    int escolha;
    if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); return; }
    if (escolha == 0) { etapaAtual = 2; return; }

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
        auto formatarAtributo = [](const std::string& n, int v) { return " - " + n + ": " + (v >= 0 ? "+" : "") + std::to_string(v); };
        
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
            " " + temp->obterNomePassivaClasse(),
            " - " + temp->obterDescricaoPassivaClasse(),
            "",
            "[ HABILIDADE ATIVA DA CLASSE ]",
            " " + temp->obterNomeHabilidadeClasse(),
            " - " + temp->obterDescricaoHabilidadeClasse(),
            " - " + temp->obterRecargaHabilidadeClasse(),
            "",
            "[ EQUIPAMENTO INICIAL DA CLASSE ]"
        };
        
        auto kit = temp->obterEquipamentoClasse();
        std::map<std::string, std::pair<int, TipoEquipamento>> contagem;
        for (const auto& i : kit) {
            contagem[i->obterNomeItem() + i->obterInfoStatus()].first++;
            contagem[i->obterNomeItem() + i->obterInfoStatus()].second = i->obterTipo();
        }
        
        std::vector<TipoEquipamento> ordemPrioridade = { TipoEquipamento::ARMA, TipoEquipamento::ESCUDO, TipoEquipamento::ARMADURA, TipoEquipamento::CONSUMIVEL };
        for (TipoEquipamento tipo : ordemPrioridade) {
            for (auto const& [nomeI, dados] : contagem) {
                if (dados.second == tipo) info.push_back(" - " + std::to_string(dados.first) + "x " + nomeI);
            }
        }

        if (exibirConfirmacaoDeEscolhaComArteLadoALado("CLASSE", temp->obterNomeClasse(), info, temp->obterAparenciaClasseMenu())) 
        {
            classeEscolhida = std::move(temp); 
            etapaAtual = 4;
        }
    }
}

void Menu::etapaConfigurarParry(const std::string& nome, RacaBase* raca, ClasseBase* classe, bool& parry, int& etapaAtual)
{
    SimplificacoesAparencia::limparTela();
    exibirLogoDoJogo("CONFIGURACOES DO JOGO");
    std::cout << "JOGADOR: " << nome << " | RACA: " << raca->obterNomeRaca() << " | CLASSE: " << classe->obterNomeClasse() << "\n";
    std::cout << std::string(SimplificacoesAparencia::obterLarguraTerminal(), '-') << "\n";
    SimplificacoesAparencia::imprimirDigitando(" [SISTEMA]: Deseja ativar o sistema de PARRY?\n\n", 35);
    SimplificacoesAparencia::imprimirDigitando(" (Permite reduzir danos ao digitar uma sequencia de numeros num tempo limite)\n\n", 35);
    
    std::cout << "  [1] LIGAR Parry\n  [2] DESLIGAR Parry\n\n  [0] VOLTAR (selecao de classe)\n\n > Sua escolha: ";
    
    int escolha;
    if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); return; }
    if (escolha == 0) { etapaAtual = 3; return; }

    if (escolha == 1 || escolha == 2) 
    {
        parry = (escolha == 1);
        etapaAtual = 5;
    }
}

void Menu::etapaEscolherDificuldade(const std::string& nome, RacaBase* raca, ClasseBase* classe, int& dificuldade, int& etapaAtual)
{
    SimplificacoesAparencia::limparTela();
    exibirLogoDoJogo("DIFICULDADE DO MUNDO");
    std::cout << "JOGADOR: " << nome << " | RACA: " << raca->obterNomeRaca() << " | CLASSE: " << classe->obterNomeClasse() << "\n";
    std::cout << std::string(SimplificacoesAparencia::obterLarguraTerminal(), '-') << "\n";
    SimplificacoesAparencia::imprimirDigitando(" [SISTEMA]: Escolha o nivel de desafio da sua jornada:\n\n", 35);
    
    std::cout << "  [1] FACIL   (Inimigos com 1x Atributos, sem habilidades de raca e sem classe)\n";
    std::cout << "  [2] NORMAL  (Inimigos com 1.5x Atributos, com habilidades de raca mas sem classes)\n";
    std::cout << "  [3] DIFICIL (Inimigos com 2x Atributos, com habilidades de raca e com classes)\n";
    std::cout << "\n  [0] VOLTAR (configuracao de parry)\n\n > Sua escolha: ";
    
    int escolha;
    if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); return; }
    if (escolha == 0) { etapaAtual = 4; return; }

    if (escolha >= 1 && escolha <= 3) 
    {
        dificuldade = escolha;
        etapaAtual = 6;
    }
}

void Menu::exibirLogoDoJogo(const std::string& tituloDaTela) 
{
#ifdef _WIN32
    SetConsoleOutputCP(65001); 
#endif
    int larguraConsole = SimplificacoesAparencia::obterLarguraTerminal();
    
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
        std::cout << SimplificacoesAparencia::corRGBTexto(208) << logoPlus[i] << SimplificacoesAparencia::cor(Cor::RESET);

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
