#include "GerenciadorMenu.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

#include "../Classes/Arqueiro.h"
#include "../Classes/Bardo.h"
#include "../Classes/ClasseBase.h"
#include "../Classes/Guerreiro.h"
#include "../Classes/Mago.h"
#include "../Gerenciadores/GerenciadorInimigos.h"
#include "../Inventario/Item.h"
#include "../Inventario/ItemConsumivel.h"
#include "../Racas/Dwarf.h"
#include "../Racas/Elfo.h"
#include "../Racas/Humano.h"
#include "../Racas/Ork.h"
#include "../Racas/RacaBase.h"
#include "../Sistemas/SistemaSave.h"
#include "../Telas/TelaAtributos.h"
#include "../Telas/TelaInventario.h"
#include "../Telas/TelaMenu.h"
#include "../Utilidades/SimplificacoesAparencia.h"

namespace {
    std::vector<std::string> comporQuadroDeAtributos(const Atributos& stats, const std::string& tituloSecao, const std::string& tituloHabilidade, const std::string& nomeHab, const std::string& descHab) {
        auto formatarAtributo = [](const std::string& nomeAtr, int valorAtr) { 
            return " - " + nomeAtr + ": " + (valorAtr >= 0 ? "+" : "") + std::to_string(valorAtr); 
        };
        return {
            tituloSecao,
            formatarAtributo("Vida", stats.vida),
            formatarAtributo("Forca", stats.forca),
            formatarAtributo("Destreza", stats.destreza),
            formatarAtributo("Resistencia", stats.resistencia),
            formatarAtributo("Constituicao", stats.constituicao),
            formatarAtributo("Inteligencia", stats.inteligencia),
            formatarAtributo("Sabedoria", stats.sabedoria),
            "",
            tituloHabilidade,
            " " + nomeHab,
            " - " + descHab
        };
    }
}

std::unique_ptr<SistemaPersonagem> GerenciadorMenu::menuPrincipal() 
{
    while (true) {
        SimplificacoesAparencia::limparTela();
        TelaMenu::exibirLogoDoJogo("MENU PRINCIPAL");
        bool temSave = SistemaSave::saveExiste();
        
        std::cout << "\n";
        int espacos = std::max(0, (SimplificacoesAparencia::obterLarguraTerminal() - 20) / 2);
        std::string margemEsquerda(espacos, ' ');
        std::cout << margemEsquerda << "[1] Novo Jogo\n";
        if (temSave) std::cout << margemEsquerda << "[2] Continuar Jogo\n";
        std::cout << margemEsquerda << "[0] Sair\n\n";
        std::cout << margemEsquerda << "Escolha: ";
        
        std::string escolha;
        std::cin >> escolha;
        
        if (escolha == "1") {
            return iniciarCriacaoDeSistemaPersonagem();
        } else if (escolha == "2" && temSave) {
            auto saves = SistemaSave::listarSaves();
            if (saves.empty()) continue;

            SimplificacoesAparencia::limparTela();
            TelaMenu::exibirLogoDoJogo("CARREGAR JOGO");
            std::cout << "\n" << margemEsquerda << "Selecione o save que deseja carregar:\n\n";
            
            for (size_t i = 0; i < saves.size(); ++i) {
                std::ifstream arquivoSave(saves[i]);
                if (arquivoSave.is_open()) {
                    std::string nome, racaStr, classeStr;
                    std::getline(arquivoSave, nome);
                    std::getline(arquivoSave, racaStr);
                    std::getline(arquivoSave, classeStr);
                    int nivel;
                    arquivoSave >> nivel;
                    std::cout << margemEsquerda << "[" << i + 1 << "] " << nome << " | Nv " << nivel << " | " << classeStr << " | " << racaStr << "\n";
                } else {
                    std::string nomeExibicao = saves[i].substr(5, saves[i].size() - 9);
                    std::cout << margemEsquerda << "[" << i + 1 << "] " << nomeExibicao << "\n";
                }
            }
            std::cout << "\n" << margemEsquerda << "[0] Voltar\n\n";
            std::cout << margemEsquerda << "Escolha: ";
            
            int escolhaSave;
            while (!(std::cin >> escolhaSave) || escolhaSave < 0 || escolhaSave > static_cast<int>(saves.size())) { 
                std::cin.clear(); 
                std::cin.ignore(1000, '\n'); 
                std::cout << margemEsquerda << "Opcao invalida. Escolha: "; 
            }
            
            if (escolhaSave > 0 && escolhaSave <= (int)saves.size()) {
                auto jogador = SistemaSave::carregarJogo(saves[escolhaSave - 1]);
                if (jogador) {
                    std::cout << "\n" << margemEsquerda << "[SISTEMA]: Jogo carregado com sucesso!\n";
                    SimplificacoesAparencia::aguardarEnter();
                    return jogador;
                } else {
                    std::cout << "\n" << margemEsquerda << "[ERRO]: Falha ao carregar o save!\n";
                    SimplificacoesAparencia::aguardarEnter();
                }
            }
        } else if (escolha == "0") {
            return nullptr;
        }
    }
}

std::unique_ptr<SistemaPersonagem> GerenciadorMenu::iniciarCriacaoDeSistemaPersonagem() 
{
    std::string nomeDoPersonagem = "";
    std::unique_ptr<RacaBase> racaEscolhida;
    std::unique_ptr<ClasseBase> classeEscolhida;
    bool sistemaDeParryAtivado = false;
    int nivelDeDificuldadeEscolhido = 2;
    EtapaCriacao etapaDeCriacaoAtual = EtapaCriacao::Nome; 

    while (etapaDeCriacaoAtual != EtapaCriacao::Concluido) 
    {
        switch (etapaDeCriacaoAtual) 
        {
            case EtapaCriacao::Nome: etapaEscolherNome(nomeDoPersonagem, etapaDeCriacaoAtual); break;
            case EtapaCriacao::Raca: etapaEscolherRaca(nomeDoPersonagem, racaEscolhida, etapaDeCriacaoAtual); break;
            case EtapaCriacao::Classe: etapaEscolherClasse(nomeDoPersonagem, racaEscolhida.get(), classeEscolhida, etapaDeCriacaoAtual); break;
            case EtapaCriacao::Parry: etapaConfigurarParry(nomeDoPersonagem, racaEscolhida.get(), classeEscolhida.get(), sistemaDeParryAtivado, etapaDeCriacaoAtual); break;
            case EtapaCriacao::Dificuldade: etapaEscolherDificuldade(nomeDoPersonagem, racaEscolhida.get(), classeEscolhida.get(), nivelDeDificuldadeEscolhido, etapaDeCriacaoAtual); break;
        }
    }
    auto personagemCriado = std::make_unique<SistemaPersonagem>(nomeDoPersonagem, std::move(racaEscolhida), std::move(classeEscolhida));
    personagemCriado->definirParryAtivado(sistemaDeParryAtivado);
    personagemCriado->definirDificuldade(static_cast<DificuldadeJogo>(nivelDeDificuldadeEscolhido));
    std::cout << "\n";
    SimplificacoesAparencia::imprimirDigitando(" [SISTEMA]: Personagem criado com sucesso! Iniciando jornada...\n", 35);
    SimplificacoesAparencia::aguardarEnter();
    return personagemCriado;
}

void GerenciadorMenu::etapaEscolherNome(std::string& nomeDoPersonagem, EtapaCriacao& etapaAtual)
{
    SimplificacoesAparencia::limparTela();
    TelaMenu::exibirLogoDoJogo("INTRODUCAO AO RPG");
    SimplificacoesAparencia::imprimirDigitando(" [NARRACAO]: O reino clama por um novo destino...\n", 35);
    SimplificacoesAparencia::imprimirDigitando(" [NARRACAO]: E todas lendas possuem um nome.\n\n", 35);
    std::cout << " > Escolha o nome do seu personagem (ou '0' para sair): ";
    
    std::string entrada;
    std::getline(std::cin >> std::ws, entrada);

    if (entrada == "0") exit(0);
    nomeDoPersonagem = entrada;
    if (!nomeDoPersonagem.empty()) etapaAtual = EtapaCriacao::Raca;
}

void GerenciadorMenu::etapaEscolherRaca(const std::string& nome, std::unique_ptr<RacaBase>& racaEscolhida, EtapaCriacao& etapaAtual)
{
    SimplificacoesAparencia::limparTela();
    TelaMenu::exibirLogoDoJogo("SELECAO DE RACA");
    std::cout << "JOGADOR: " << nome << "\n";
    std::cout << std::string(SimplificacoesAparencia::obterLarguraTerminal(), '-') << "\n";
    SimplificacoesAparencia::imprimirDigitando(" [NARRACAO]: Qual sua origem?\n\n", 35);
    std::cout << "  [1] Dwarf\n  [2] Elfo\n  [3] Humano\n  [4] Ork\n\n  [0] VOLTAR (selecao de nome)\n\n > Sua escolha: ";
    
    int escolha;
    if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); return; }
    if (escolha == 0) { etapaAtual = EtapaCriacao::Nome; return; }

    std::unique_ptr<RacaBase> racaTemporaria;
    switch(escolha) {
        case 1: racaTemporaria = std::make_unique<Dwarf>(); break;
        case 2: racaTemporaria = std::make_unique<Elfo>();  break;
        case 3: racaTemporaria = std::make_unique<Humano>(); break;
        case 4: racaTemporaria = std::make_unique<Ork>();   break;
    }

    if (racaTemporaria) 
    {
        std::vector<std::string> info = comporQuadroDeAtributos(racaTemporaria->obterAtributosRaca(), "[ ATRIBUTOS BASE DE RAÇA ]", "[ HABILIDADE PASSIVA ]", racaTemporaria->obterNomeHabilidadeRaca(), racaTemporaria->obterDescricaoHabilidadeRaca());
        
        if (TelaMenu::exibirConfirmacaoDeEscolhaComArteLadoALado("RACA", racaTemporaria->obterNomeRaca(), info, racaTemporaria->obterAparenciaRaca())) 
        {
            racaEscolhida = std::move(racaTemporaria); 
            etapaAtual = EtapaCriacao::Classe;
        }
    }
}

void GerenciadorMenu::etapaEscolherClasse(const std::string& nome, RacaBase* raca, std::unique_ptr<ClasseBase>& classeEscolhida, EtapaCriacao& etapaAtual)
{
    SimplificacoesAparencia::limparTela();
    TelaMenu::exibirLogoDoJogo("SELECAO DE CLASSE");
    std::cout << "JOGADOR: " << nome << " | RACA: " << raca->obterNomeRaca() << "\n";
    std::cout << std::string(SimplificacoesAparencia::obterLarguraTerminal(), '-') << "\n";
    SimplificacoesAparencia::imprimirDigitando(" [NARRACAO]: Qual caminho voce seguira neste mundo?\n\n", 35);
    
    std::cout << "  [1] Arqueiro\n  [2] Bardo\n  [3] Guerreiro\n  [4] Mago\n\n  [0] VOLTAR (selecao de raca)\n\n > Sua escolha: ";
    
    int escolha;
    if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); return; }
    if (escolha == 0) { etapaAtual = EtapaCriacao::Raca; return; }

    std::unique_ptr<ClasseBase> classeTemporaria;
    switch(escolha) 
    {
        case 1: classeTemporaria = std::make_unique<Arqueiro>(); break;
        case 2: classeTemporaria = std::make_unique<Bardo>(); break;
        case 3: classeTemporaria = std::make_unique<Guerreiro>(); break;
        case 4: classeTemporaria = std::make_unique<Mago>(); break;
    }

    if (classeTemporaria) 
    {
        std::vector<std::string> info = comporQuadroDeAtributos(classeTemporaria->obterAtributosClasse(), "[ ATRIBUTOS BONUS DA CLASSE ]", "[ HABILIDADE PASSIVA DA CLASSE ]", classeTemporaria->obterNomePassivaClasse(), classeTemporaria->obterDescricaoPassivaClasse());
        
        info.push_back("");
        info.insert(info.end(), {
            "[ HABILIDADE ATIVA DA CLASSE ]",
            " " + classeTemporaria->obterNomeHabilidadeClasse(),
            " - " + classeTemporaria->obterDescricaoHabilidadeClasse(),
            " - " + classeTemporaria->obterRecargaHabilidadeClasse(),
            "",
            "[ EQUIPAMENTO INICIAL DA CLASSE ]" 
        });
        
        auto kit = classeTemporaria->obterEquipamentoClasse();
        std::map<std::string, std::pair<int, TipoEquipamento>> contagem;
        for (const auto& itemDoKit : kit) {
            if (itemDoKit) {
                contagem[itemDoKit->obterNomeItem() + itemDoKit->obterInfoStatus()].first++;
                contagem[itemDoKit->obterNomeItem() + itemDoKit->obterInfoStatus()].second = itemDoKit->obterTipo();
            }
        }
        
        std::vector<TipoEquipamento> ordemPrioridade = { TipoEquipamento::ARMA, TipoEquipamento::ESCUDO, TipoEquipamento::ARMADURA, TipoEquipamento::CONSUMIVEL };
        for (TipoEquipamento tipo : ordemPrioridade) {
            for (auto const& [nomeDoItem, dadosDoItem] : contagem) {
                if (dadosDoItem.second == tipo) info.push_back(" - " + std::to_string(dadosDoItem.first) + "x " + nomeDoItem);
            }
        }

        if (TelaMenu::exibirConfirmacaoDeEscolhaComArteLadoALado("CLASSE", classeTemporaria->obterNomeClasse(), info, classeTemporaria->obterAparenciaClasseMenu())) 
        {
            classeEscolhida = std::move(classeTemporaria); 
            etapaAtual = EtapaCriacao::Parry;
        }
    }
}

void GerenciadorMenu::etapaConfigurarParry(const std::string& nome, RacaBase* raca, ClasseBase* classe, bool& parry, EtapaCriacao& etapaAtual)
{
    SimplificacoesAparencia::limparTela();
    TelaMenu::exibirLogoDoJogo("CONFIGURACOES DO JOGO");
    std::cout << "JOGADOR: " << nome << " | RACA: " << raca->obterNomeRaca() << " | CLASSE: " << classe->obterNomeClasse() << "\n";
    std::cout << std::string(SimplificacoesAparencia::obterLarguraTerminal(), '-') << "\n";
    SimplificacoesAparencia::imprimirDigitando(" [SISTEMA]: Deseja ativar o sistema de PARRY?\n\n", 35);
    SimplificacoesAparencia::imprimirDigitando(" (Permite reduzir danos ao digitar uma sequencia de numeros num tempo limite)\n\n", 35);
    
    std::cout << "  [1] LIGAR Parry\n  [2] DESLIGAR Parry\n\n  [0] VOLTAR (selecao de classe)\n\n > Sua escolha: ";
    
    int escolha;
    if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); return; }
    if (escolha == 0) { etapaAtual = EtapaCriacao::Classe; return; }

    if (escolha == 1 || escolha == 2) 
    {
        parry = (escolha == 1);
        etapaAtual = EtapaCriacao::Dificuldade;
    }
}

void GerenciadorMenu::etapaEscolherDificuldade(const std::string& nome, RacaBase* raca, ClasseBase* classe, int& dificuldade, EtapaCriacao& etapaAtual)
{
    SimplificacoesAparencia::limparTela();
    TelaMenu::exibirLogoDoJogo("DIFICULDADE DO MUNDO");
    std::cout << "JOGADOR: " << nome << " | RACA: " << raca->obterNomeRaca() << " | CLASSE: " << classe->obterNomeClasse() << "\n";
    std::cout << std::string(SimplificacoesAparencia::obterLarguraTerminal(), '-') << "\n";
    SimplificacoesAparencia::imprimirDigitando(" [SISTEMA]: Escolha o nivel de desafio da sua jornada:\n\n", 35);
    
    std::cout << "  [1] FACIL   (Inimigos com 1x Atributos, sem habilidades de raca e sem classe)\n";
    std::cout << "  [2] NORMAL  (Inimigos com 1.5x Atributos, com habilidades de raca mas sem classes)\n";
    std::cout << "  [3] DIFICIL (Inimigos com 2x Atributos, com habilidades de raca e com classes)\n";
    std::cout << "\n  [0] VOLTAR (configuracao de parry)\n\n > Sua escolha: ";
    
    int escolha;
    if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); return; }
    if (escolha == 0) { etapaAtual = EtapaCriacao::Parry; return; }

    if (escolha >= 1 && escolha <= 3) 
    {
        dificuldade = escolha;
        etapaAtual = EtapaCriacao::Concluido;
    }
}
