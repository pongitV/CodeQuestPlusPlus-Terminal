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
#include "../Utilidades/ControleDeInput.h"
#include "../Utilidades/GeradorAleatorio.h"

namespace {
    void executarTutorialDeParry() {
        Aparencia::limparTela();
        TelaMenu::exibirLogoDoJogo("TUTORIAL DE PARRY");

        std::vector<std::string> explicacao = {
            "--- COMO FUNCIONA O PARRY ---",
            "Quando um inimigo atacar, voce deve estar pronto para reagir.",
            "Uma sequencia de numeros aparecera na tela com um limite de tempo.",
            "Digite os numeros rapidamente na sequencia correta e pressione ENTER.",
            "Se for rapido o suficiente e nao errar, o dano sera reduzido ou totalmente anulado!",
            "",
            "[1] INICIAR TESTE | [0] PULAR"
        };
        Aparencia::imprimirBlocoCentralizado(explicacao);
        std::string escolha = ControleDeInput::lerEntradaProtegida("> ");
        if (escolha == "0") return;

        struct NivelTutorial {
            std::string nomeInimigo;
            int digitos;
            int tempoLimiteMs;
        };

        std::vector<NivelTutorial> niveis = {
            {"(Nivel 1)", 3, 3000},
            {"(Nivel 2)", 4, 2500},
            {"(Nivel 3)", 5, 2000},
            {"(Nivel Extra)", 6, 1500}
        };

        for (size_t i = 0; i < niveis.size(); ++i) {
            Aparencia::limparTela();
            TelaMenu::exibirLogoDoJogo("TUTORIAL DE PARRY - " + niveis[i].nomeInimigo);
            
            std::cout << "\n";
            Aparencia::imprimirCentralizado("Inimigo: " + niveis[i].nomeInimigo);
            Aparencia::imprimirCentralizado("Sequencia: " + std::to_string(niveis[i].digitos) + " digitos | Tempo limite: " + std::to_string(niveis[i].tempoLimiteMs / 1000.0) + "s");
            Aparencia::aguardarEnter();

            int acertos = 0;
            for (int teste = 1; teste <= 5; ++teste) {
                std::cout << "\n";
                Aparencia::imprimirCentralizado("[ Teste " + std::to_string(teste) + "/5 ] Prepare-se...");
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                std::string sequencia = "";
                for(int d = 0; d < niveis[i].digitos; d++){
                    sequencia += std::to_string(GeradorAleatorio::obterInteiro(0, 9));
                }

                Aparencia::imprimirCentralizado("O " + niveis[i].nomeInimigo + " ataca! DIGITE: " + Aparencia::cor(Cor::CIANO) + sequencia + Aparencia::cor(Cor::RESET));
                
                auto inicio = std::chrono::steady_clock::now();
                std::string entrada = ControleDeInput::lerEntradaProtegida("> ");
                auto fim = std::chrono::steady_clock::now();
                
                int tempoGastoMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(fim - inicio).count());

                if (entrada == sequencia) {
                    if (tempoGastoMs <= niveis[i].tempoLimiteMs) {
                        Aparencia::imprimirCentralizado(Aparencia::cor(Cor::VERDE) + "Parry Perfeito! (" + std::to_string(tempoGastoMs) + "ms)" + Aparencia::cor(Cor::RESET));
                        acertos++;
                    } else {
                        Aparencia::imprimirCentralizado(Aparencia::cor(Cor::AMARELO) + "Muito lento! (" + std::to_string(tempoGastoMs) + "ms / " + std::to_string(niveis[i].tempoLimiteMs) + "ms)" + Aparencia::cor(Cor::RESET));
                    }
                } else {
                    Aparencia::imprimirCentralizado(Aparencia::cor(Cor::VERMELHO) + "Errou a sequencia! (Voce digitou: " + entrada + ")" + Aparencia::cor(Cor::RESET));
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
            }

            std::cout << "\n";
            Aparencia::imprimirCentralizado("Resultado do nivel: " + std::to_string(acertos) + "/5 acertos.");
            Aparencia::aguardarEnter();
        }

        Aparencia::limparTela();
        TelaMenu::exibirLogoDoJogo("TUTORIAL CONCLUIDO");
        Aparencia::imprimirCentralizado("Voce completou o tutorial de Parry!");
        Aparencia::aguardarEnter();
    }
}

std::unique_ptr<SistemaPersonagem> GerenciadorMenu::menuPrincipal() 
{
    while (true) {
        bool temSave = SistemaSave::saveExiste();
        TelaMenu::exibirOpcoesMenuPrincipal(temSave);
        
        std::string escolha = ControleDeInput::lerEntradaProtegida();
        
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
            while (true) {
                std::string entradaSave = ControleDeInput::lerEntradaProtegida();
                try {
                    escolhaSave = std::stoi(entradaSave);
                    if (escolhaSave >= 0 && escolhaSave <= static_cast<int>(saves.size())) {
                        break;
                    }
                } catch (...) {}
                
                std::cout << "\033[u\033[J"; 
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
            case EtapaCriacao::Dificuldade: etapaEscolherDificuldade(nomeDoPersonagem, racaEscolhida.get(), classeEscolhida.get(), nivelDeDificuldadeEscolhido, etapaDeCriacaoAtual); break;
            case EtapaCriacao::Parry: etapaConfigurarParry(nomeDoPersonagem, racaEscolhida.get(), classeEscolhida.get(), nivelDeDificuldadeEscolhido, sistemaDeParryAtivado, etapaDeCriacaoAtual); break;
        }
    }
    auto personagemCriado = std::make_unique<SistemaPersonagem>(nomeDoPersonagem, std::move(racaEscolhida), std::move(classeEscolhida));
    personagemCriado->definirParryAtivado(sistemaDeParryAtivado);
    personagemCriado->definirDificuldade(static_cast<DificuldadeJogo>(nivelDeDificuldadeEscolhido));
    std::cout << "\n";
    std::string textoFinal = "[SISTEMA]: Personagem criado com sucesso!";
    Aparencia::imprimirCentralizadoDigitando(textoFinal);
    Aparencia::aguardarEnter();

    Aparencia::limparTela();
    TelaMenu::exibirLogoDoJogo("SISTEMA DE SAVE");
    std::vector<std::string> avisoSave = {
        "--- AVISO IMPORTANTE ---",
        "O jogo NAO possui salvamento automatico (auto-save).",
        "Para salvar o seu progresso, abra a sua Ficha de Jogador",
        "durante a exploracao ou durante uma batalha,",
        "e escolha a opcao de Voltar ao Menu Principal."
    };
    Aparencia::imprimirBlocoCentralizado(avisoSave);
    Aparencia::aguardarEnter();

    Aparencia::limparTela();
    TelaMenu::exibirLogoDoJogo("INICIO DA JORNADA");
    std::vector<std::string> dialogoInicio = {
        "[NARRACAO]: Voce desperta nos arredores de um lugar desconhecido...",
        "[NARRACAO]: Na sua vista, uma pequena vila sendo atacada por monstros.",
        "[NARRACAO]: Empunhando seu equipamento, voce sente que seu destino o aguarda.",
        "[NARRACAO]: Um novo capitulo se inicia agora."
    };
    Aparencia::imprimirBlocoCentralizadoDigitando(dialogoInicio);
    Aparencia::aguardarEnter("Pressione ENTER para iniciar...");

    return personagemCriado;
}

void GerenciadorMenu::etapaEscolherNome(std::string& nomeDoPersonagem, EtapaCriacao& etapaAtual)
{
    TelaMenu::exibirPromptNome();
    
    std::string entrada = ControleDeInput::lerEntradaProtegida();

    if (entrada == "0") exit(0);
    nomeDoPersonagem = entrada;
    if (!nomeDoPersonagem.empty()) etapaAtual = EtapaCriacao::Raca;
}

void GerenciadorMenu::etapaEscolherRaca(const std::string& nome, std::unique_ptr<RacaBase>& racaEscolhida, EtapaCriacao& etapaAtual)
{
    TelaMenu::exibirPromptRaca(nome);
    
    std::string entrada = ControleDeInput::lerEntradaProtegida();
    int escolha;
    try { escolha = std::stoi(entrada); } catch (...) { return; }
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
    
    std::string entrada = ControleDeInput::lerEntradaProtegida();
    int escolha;
    try { escolha = std::stoi(entrada); } catch (...) { return; }
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
            etapaAtual = EtapaCriacao::Dificuldade;
        }
    }
}

void GerenciadorMenu::etapaConfigurarParry(const std::string& nome, RacaBase* raca, ClasseBase* classe, int dificuldade, bool& parry, EtapaCriacao& etapaAtual)
{
    std::string nomeDificuldade = "Normal";
    if (dificuldade == 1) nomeDificuldade = "Facil";
    else if (dificuldade == 3) nomeDificuldade = "Dificil";

    TelaMenu::exibirPromptParry(nome, raca->obterNomeRaca(), classe->obterNomeClasse() + " | DIFICULDADE: " + nomeDificuldade);
    
    std::string entrada = ControleDeInput::lerEntradaProtegida();
    int escolha;
    try { escolha = std::stoi(entrada); } catch (...) { return; }
    if (escolha == 0) { etapaAtual = EtapaCriacao::Dificuldade; return; }

    if (escolha == 1) {
        parry = true;
        executarTutorialDeParry();
        etapaAtual = EtapaCriacao::Concluido;
    } else if (escolha == 2) {
        parry = false;
        etapaAtual = EtapaCriacao::Concluido;
    }
}

void GerenciadorMenu::etapaEscolherDificuldade(const std::string& nome, RacaBase* raca, ClasseBase* classe, int& dificuldade, EtapaCriacao& etapaAtual)
{
    TelaMenu::exibirPromptDificuldade(nome, raca->obterNomeRaca(), classe->obterNomeClasse());
    
    std::string entrada = ControleDeInput::lerEntradaProtegida();
    int escolha;
    try { escolha = std::stoi(entrada); } catch (...) { return; }
    if (escolha == 0) { etapaAtual = EtapaCriacao::Classe; return; }

    if (escolha >= 1 && escolha <= 3) 
    {
        dificuldade = escolha;
        etapaAtual = EtapaCriacao::Parry;
    }
}