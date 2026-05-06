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
#include "../Utilidades/Aparencia.h"

namespace {
}

std::unique_ptr<SistemaPersonagem> GerenciadorMenu::menuPrincipal() 
{
    while (true) {
        bool temSave = SistemaSave::saveExiste();
        TelaMenu::exibirOpcoesMenuPrincipal(temSave);
        
        std::string escolha;
        std::cin >> escolha;
        
        if (escolha == "1") {
            return iniciarCriacaoDeSistemaPersonagem();
        } else if (escolha == "2" && temSave) {
            auto saves = SistemaSave::listarSaves();
            if (saves.empty()) continue;
            
            std::vector<std::string> informacoesSaves;
            for (size_t i = 0; i < saves.size(); ++i) {
                std::ifstream arquivoSave(saves[i]);
                if (arquivoSave.is_open()) {
                    std::string nome, racaStr, classeStr;
                    std::getline(arquivoSave, nome);
                    std::getline(arquivoSave, racaStr);
                    std::getline(arquivoSave, classeStr);
                    int nivel;
                    arquivoSave >> nivel;
                    informacoesSaves.push_back("[" + std::to_string(i + 1) + "] " + nome + " | Nv " + std::to_string(nivel) + " | " + classeStr + " | " + racaStr);
                } else {
                    std::string nomeExibicao = saves[i].substr(5, saves[i].size() - 9);
                    informacoesSaves.push_back("[" + std::to_string(i + 1) + "] " + nomeExibicao);
                }
            }
            TelaMenu::exibirMenuCarregarJogo(informacoesSaves);
            
            int escolhaSave;
            while (!(std::cin >> escolhaSave) || escolhaSave < 0 || escolhaSave > static_cast<int>(saves.size())) { 
                std::cin.clear(); 
                std::cin.ignore(1000, '\n'); 
                Aparencia::exibirPrompt("Opcao invalida. Escolha: "); 
            }
            
            if (escolhaSave > 0 && escolhaSave <= (int)saves.size()) {
                auto jogador = SistemaSave::carregarJogo(saves[escolhaSave - 1]);
                if (jogador) {
                    std::cout << "\n";
                    Aparencia::imprimirCentralizado("[SISTEMA]: Jogo carregado com sucesso!");
                    Aparencia::aguardarEnter();
                    return jogador;
                } else {
                    std::cout << "\n";
                    Aparencia::imprimirCentralizado("[ERRO]: Falha ao carregar o save!");
                    Aparencia::aguardarEnter();
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
    std::string textoFinal = "[SISTEMA]: Personagem criado com sucesso! Iniciando jornada...";
    Aparencia::imprimirDigitando(Aparencia::espacosParaCentralizar(textoFinal.length()) + textoFinal + "\n", 35);
    Aparencia::aguardarEnter();
    return personagemCriado;
}

void GerenciadorMenu::etapaEscolherNome(std::string& nomeDoPersonagem, EtapaCriacao& etapaAtual)
{
    TelaMenu::exibirPromptNome();
    
    std::string entrada;
    std::getline(std::cin >> std::ws, entrada);

    if (entrada == "0") exit(0);
    nomeDoPersonagem = entrada;
    if (!nomeDoPersonagem.empty()) etapaAtual = EtapaCriacao::Raca;
}

void GerenciadorMenu::etapaEscolherRaca(const std::string& nome, std::unique_ptr<RacaBase>& racaEscolhida, EtapaCriacao& etapaAtual)
{
    TelaMenu::exibirPromptRaca(nome);
    
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
        std::vector<std::string> info = TelaMenu::comporQuadroDeAtributos(racaTemporaria->obterAtributosRaca(), "[ ATRIBUTOS BASE DE RAÇA ]", "[ HABILIDADE PASSIVA ]", racaTemporaria->obterNomeHabilidadeRaca(), racaTemporaria->obterDescricaoHabilidadeRaca());
        
        if (TelaMenu::exibirConfirmacaoDeEscolhaComArteLadoALado("RACA", racaTemporaria->obterNomeRaca(), info, racaTemporaria->obterAparenciaRaca())) 
        {
            racaEscolhida = std::move(racaTemporaria); 
            etapaAtual = EtapaCriacao::Classe;
        }
    }
}

void GerenciadorMenu::etapaEscolherClasse(const std::string& nome, RacaBase* raca, std::unique_ptr<ClasseBase>& classeEscolhida, EtapaCriacao& etapaAtual)
{
    TelaMenu::exibirPromptClasse(nome, raca->obterNomeRaca());
    
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
        std::vector<std::string> info = TelaMenu::comporQuadroDeAtributos(classeTemporaria->obterAtributosClasse(), "[ ATRIBUTOS BONUS DA CLASSE ]", "[ HABILIDADE PASSIVA DA CLASSE ]", classeTemporaria->obterNomePassivaClasse(), classeTemporaria->obterDescricaoPassivaClasse());
        
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
    TelaMenu::exibirPromptParry(nome, raca->obterNomeRaca(), classe->obterNomeClasse());
    
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
    TelaMenu::exibirPromptDificuldade(nome, raca->obterNomeRaca(), classe->obterNomeClasse());
    
    int escolha;
    if (!(std::cin >> escolha)) { std::cin.clear(); std::cin.ignore(1000, '\n'); return; }
    if (escolha == 0) { etapaAtual = EtapaCriacao::Parry; return; }

    if (escolha >= 1 && escolha <= 3) 
    {
        dificuldade = escolha;
        etapaAtual = EtapaCriacao::Concluido;
    }
}
