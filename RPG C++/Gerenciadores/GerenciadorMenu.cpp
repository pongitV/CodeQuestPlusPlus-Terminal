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
#include "../Inventario/EquipamentoArma.h"
#include "../Inventario/EquipamentoArmadura.h"
#include "../Inventario/EquipamentoEscudo.h"
#include "../Inventario/Item.h"
#include "../Inventario/ItemConsumivel.h"
#include "../Racas/Dwarf.h"
#include "../Racas/Elfo.h"
#include "../Racas/Humano.h"
#include "../Racas/Ork.h"
#include "../Racas/RacaBase.h"
#include "../Sistemas/SistemaDiario.h"
#include "../Sistemas/SistemaSave.h"
#include "../Telas/TelaAtributos.h"
#include "../Telas/TelaInventario.h"
#include "../Telas/TelaMenu.h"
#include "../Utilidades/Aparencia.h"
#include "../Utilidades/ControleDeInput.h"
#include "../Utilidades/GeradorAleatorio.h"

namespace {
    std::vector<std::string> lerInfoSavesLocal(const std::vector<std::string>& saves) {
        std::vector<std::string> informacoesSaves;
        informacoesSaves.reserve(saves.size());
        for (const auto& save : saves) {
            std::ifstream arquivoSave(save);
            if (arquivoSave.is_open()) {
                std::string nome, racaStr, classeStr;
                std::getline(arquivoSave, nome);
                std::getline(arquivoSave, racaStr);
                std::getline(arquivoSave, classeStr);
                int nivel;
                arquivoSave >> nivel;
                informacoesSaves.push_back(nome + " | Nv " + std::to_string(nivel) + " | " + classeStr + " | " + racaStr);
            } else {
                std::string nomeExibicao = save.substr(5, save.size() - 9);
                informacoesSaves.push_back(nomeExibicao);
            }
        }
        return informacoesSaves;
    }
}

std::vector<std::string> GerenciadorMenu::lerInformacoesDosSaves(const std::vector<std::string>& saves) {
    return lerInfoSavesLocal(saves);
}

namespace {
    std::unique_ptr<SistemaPersonagem> lidarOpcoesDoSave(const std::string& saveSelecionado, const std::string& infoSave, bool& saveFoiDeletado) {
        bool animar = false;
        while (true) {
            Aparencia::limparTela();
            TelaMenu::exibirPainelLogoJogo("OPCOES DO SAVE", animar);
            animar = false;
            std::cout << "\n";
            Aparencia::imprimirCentralizado("Save selecionado: " + infoSave, Aparencia::cor(Cor::CIANO));
            std::cout << "\n";
            
            std::vector<std::string> acoesSave = {"Ver Informacoes", "Carregar", "Deletar", "Voltar"};
            int escAcao = ControleDeInput::lerSelecaoMenuComSetas(acoesSave, true);
            
            if (escAcao == 0) {
                Aparencia::limparTela();
                TelaMenu::exibirPainelLogoJogo("INFORMACOES DO SAVE", true);
                
                std::ifstream arquivoSave(saveSelecionado);
                if (arquivoSave.is_open()) {
                    std::string nome, raca, classe;
                    std::getline(arquivoSave, nome);
                    std::getline(arquivoSave, raca);
                    std::getline(arquivoSave, classe);
                    int nivel, xp, xpMax, vida, ouro, dif, lab;
                    arquivoSave >> nivel >> xp >> xpMax >> vida >> ouro >> dif >> lab;
                    
                    std::vector<std::string> painelInfo = {
                        "NOME:       " + nome, "RACA:       " + raca, "CLASSE:     " + classe,
                        "NIVEL:      " + std::to_string(nivel), "XP:         " + std::to_string(xp) + " / " + std::to_string(xpMax),
                        "VIDA SALVA: " + std::to_string(vida), "OURO:       " + std::to_string(ouro) + "G",
                        "DIFICULDADE:" + std::string(dif == 1 ? " Facil" : (dif == 2 ? " Normal" : " Dificil"))
                    };
                    std::cout << "\n";
                    Aparencia::imprimirBlocoCentralizado(Aparencia::criarCaixa(painelInfo, "DADOS DO PERSONAGEM", 40, Cor::CIANO));
                } else {
                    std::cout << "\n";
                    Aparencia::imprimirCentralizado("Nao foi possivel ler os dados.", Aparencia::cor(Cor::VERMELHO));
                }
                ControleDeInput::aguardarEnter();
            }
            else if (escAcao == 1) {
                auto jogador = SistemaSave::carregarJogo(saveSelecionado);
                if (jogador) {
                    std::cout << "\n";
                    Aparencia::imprimirCentralizado("[SISTEMA]: Jogo carregado com sucesso!");
                    ControleDeInput::aguardarEnter();
                    return jogador;
                } else {
                    std::cout << "\n";
                    Aparencia::imprimirCentralizado("[ERRO]: Falha ao carregar o save!", Aparencia::cor(Cor::VERMELHO));
                    ControleDeInput::aguardarEnter();
                }
            }
            else if (escAcao == 2) {
                Aparencia::limparTela();
                TelaMenu::exibirPainelLogoJogo("DELETAR SAVE", true);
                std::string nomeCorreto = "";
                std::ifstream arquivoSave(saveSelecionado);
                if (arquivoSave.is_open()) { std::getline(arquivoSave, nomeCorreto); arquivoSave.close(); }
                
                std::cout << "\n";
                Aparencia::imprimirCentralizado("ATENCAO: Esta acao e irreversivel!", Aparencia::cor(Cor::VERMELHO));
                Aparencia::imprimirCentralizado("Para confirmar a exclusao, digite exatamente o nome do personagem:");
                std::cout << "\n" << Aparencia::espacosParaCentralizar(nomeCorreto.length() + 2) << "[" << Aparencia::cor(Cor::AMARELO) << nomeCorreto << Aparencia::cor(Cor::RESET) << "]\n";
                
                std::string digitado = ControleDeInput::lerEntradaProtegida("> ");
                if (digitado == nomeCorreto) {
                    SistemaSave::deletarSave(saveSelecionado);
                    std::cout << "\n";
                    Aparencia::imprimirCentralizado("[SISTEMA]: Save deletado com sucesso!", Aparencia::cor(Cor::VERDE));
                    ControleDeInput::aguardarEnter();
                    saveFoiDeletado = true;
                    break; 
                } else {
                    std::cout << "\n";
                    Aparencia::imprimirCentralizado("[SISTEMA]: Nome incorreto. Exclusao cancelada.", Aparencia::cor(Cor::VERMELHO));
                    ControleDeInput::aguardarEnter();
                }
            }
            else if (escAcao == 3) { break; }
        }
        return nullptr;
    }

    std::unique_ptr<SistemaPersonagem> lidarGerenciamentoDeSaves() {
        bool animar = false;
        while (true) {
            auto saves = SistemaSave::listarSaves();
            if (saves.empty()) {
                Aparencia::limparTela();
                TelaMenu::exibirPainelLogoJogo("GERENCIAR SAVES", animar);
                std::cout << "\n";
                Aparencia::imprimirCentralizado("Nenhum save encontrado.", Aparencia::cor(Cor::AMARELO));
                ControleDeInput::aguardarEnter();
                break; 
            }
            
            std::vector<std::string> infoSaves = lerInfoSavesLocal(saves);
            
            Aparencia::limparTela();
            TelaMenu::exibirPainelLogoJogo("GERENCIAR SAVES", animar);
            animar = false;
            std::cout << "\n";
            Aparencia::imprimirCentralizado("Selecione um save para gerenciar:\n");
            
            std::vector<std::string> opcoesSave = infoSaves;
            opcoesSave.push_back("Voltar");
            int escSave = ControleDeInput::lerSelecaoMenuComSetas(opcoesSave, true);
            
            if (escSave >= 0 && escSave < static_cast<int>(saves.size())) {
                bool saveDeletado = false;
                auto jogador = lidarOpcoesDoSave(saves[escSave], infoSaves[escSave], saveDeletado);
                if (jogador) return jogador;
            } else { break; }
        }
        return nullptr;
    }

    std::unique_ptr<SistemaPersonagem> lidarMenuDeOpcoes() {
        bool animar = false;
        while (true) {
            Aparencia::limparTela();
            TelaMenu::exibirPainelLogoJogo("OPCOES", animar);
            animar = false;
            std::cout << "\n";
            int esc = ControleDeInput::lerSelecaoMenuComSetas({"Gerenciar Saves", "Voltar"}, true);
            if (esc == 0) {
                auto jogador = lidarGerenciamentoDeSaves();
                if (jogador) return jogador;
            } else if (esc == 1) { break; }
        }
        return nullptr;
    }
}

std::unique_ptr<SistemaPersonagem> GerenciadorMenu::menuPrincipal() 
{
    while (true) {
        bool temSave = SistemaSave::saveExiste();
        int selecao = TelaMenu::exibirOpcoesMenuPrincipal(temSave);
        
        std::string opcaoSelecionada;
        if (selecao == 0) opcaoSelecionada = "Novo Jogo";
        else if (temSave && selecao == 1) opcaoSelecionada = "Continuar Jogo";
        else if ((temSave && selecao == 2) || (!temSave && selecao == 1)) opcaoSelecionada = "Opcoes";
        else opcaoSelecionada = "Sair";
        
        if (opcaoSelecionada == "Novo Jogo") {
            auto novoJogador = iniciarCriacaoDeSistemaPersonagem();
            if (novoJogador) return novoJogador;
        } else if (opcaoSelecionada == "Continuar Jogo") {
            auto saves = SistemaSave::listarSaves();
            if (saves.empty()) continue;
            
            std::vector<std::string> informacoesSaves = lerInformacoesDosSaves(saves);
            int escolhaSave = TelaMenu::exibirMenuCarregarJogo(informacoesSaves);
            
            if (escolhaSave >= 0 && escolhaSave < static_cast<int>(saves.size())) {
                auto jogador = SistemaSave::carregarJogo(saves[escolhaSave]);
                if (jogador) {
                    std::cout << "\n";
                    Aparencia::imprimirCentralizado("[SISTEMA]: Jogo carregado com sucesso!");
                    ControleDeInput::aguardarEnter();
                    return jogador;
                } else {
                    std::cout << "\n";
                    Aparencia::imprimirCentralizado("[ERRO]: Falha ao carregar o save!", Aparencia::cor(Cor::VERMELHO));
                    ControleDeInput::aguardarEnter();
                }
            }
        } else if (opcaoSelecionada == "Opcoes") {
            auto jogador = lidarMenuDeOpcoes();
            if (jogador) return jogador;
        } else {
            std::vector<std::string> opcoesSimNao = { "NAO", "SIM" };
            std::cout << "\n";
            Aparencia::imprimirCentralizado("[SISTEMA]: Deseja realmente sair do jogo?", Aparencia::cor(Cor::AMARELO));
            std::cout << "\n";
            if (ControleDeInput::lerSelecaoMenuComSetas(opcoesSimNao, true) == 1) {
                return nullptr;
            }
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
            case EtapaCriacao::Nome: 
                etapaEscolherNome(nomeDoPersonagem, etapaDeCriacaoAtual); 
                if (nomeDoPersonagem == "0") return nullptr;
                break;
            case EtapaCriacao::Raca: etapaEscolherRaca(nomeDoPersonagem, racaEscolhida, etapaDeCriacaoAtual); break;
            case EtapaCriacao::Classe: etapaEscolherClasse(nomeDoPersonagem, racaEscolhida.get(), classeEscolhida, etapaDeCriacaoAtual); break;
            case EtapaCriacao::Dificuldade: etapaEscolherDificuldade(nomeDoPersonagem, racaEscolhida.get(), classeEscolhida.get(), nivelDeDificuldadeEscolhido, etapaDeCriacaoAtual); break;
            case EtapaCriacao::Parry: etapaConfigurarParry(nomeDoPersonagem, racaEscolhida.get(), classeEscolhida.get(), nivelDeDificuldadeEscolhido, sistemaDeParryAtivado, etapaDeCriacaoAtual); break;
        }
    }
    auto personagemCriado = std::make_unique<SistemaPersonagem>(nomeDoPersonagem, std::move(racaEscolhida), std::move(classeEscolhida));
    personagemCriado->definirParryAtivado(sistemaDeParryAtivado);
    personagemCriado->definirDificuldade(static_cast<DificuldadeJogo>(nivelDeDificuldadeEscolhido));
    
    SistemaDiario::instancia().registrarRaca(personagemCriado->obterRaca()->obterNomeRaca());
    SistemaDiario::instancia().registrarClasse(personagemCriado->obterNomeClasse());
    
    for (Item* item : personagemCriado->obterInventario()->obterTodosOsItens()) {
        SistemaDiario::instancia().registrarItem(Aparencia::removerCoresANSI(item->obterNomeItem()));
    }

    std::string nomeDificuldade = Aparencia::cor(Cor::AMARELO) + "Normal" + Aparencia::cor(Cor::RESET);
    if (nivelDeDificuldadeEscolhido == 1) nomeDificuldade = Aparencia::cor(Cor::VERDE) + "Facil" + Aparencia::cor(Cor::RESET);
    else if (nivelDeDificuldadeEscolhido == 3) nomeDificuldade = Aparencia::cor(Cor::VERMELHO) + "Dificil" + Aparencia::cor(Cor::RESET);

    std::string statusParry = sistemaDeParryAtivado ? (Aparencia::cor(Cor::VERDE) + "Ligado" + Aparencia::cor(Cor::RESET)) : (Aparencia::cor(Cor::CINZA) + "Desligado" + Aparencia::cor(Cor::RESET));
    std::string infoBox = "| JOGADOR: " + personagemCriado->obterNome() + " | RACA: " + personagemCriado->obterRaca()->obterNomeRaca() + " | CLASSE: " + personagemCriado->obterNomeClasse() + " | DIFICULDADE: " + nomeDificuldade + " | PARRY: " + statusParry + " |";

    TelaMenu::exibirIntroducaoJornada(infoBox);

    return personagemCriado;
}

void GerenciadorMenu::etapaEscolherNome(std::string& nomeDoPersonagem, EtapaCriacao& etapaAtual)
{
    TelaMenu::exibirPromptNome();
    
    std::string entrada = ControleDeInput::lerEntradaProtegida();

    if (entrada == "0") {
        nomeDoPersonagem = "0";
        return;
    }
    nomeDoPersonagem = entrada;
    if (!nomeDoPersonagem.empty()) etapaAtual = EtapaCriacao::Raca;
}

void GerenciadorMenu::etapaEscolherRaca(const std::string& nome, std::unique_ptr<RacaBase>& racaEscolhida, EtapaCriacao& etapaAtual)
{
    int escolha = TelaMenu::exibirPromptRaca(nome);
    if (escolha == 4) { etapaAtual = EtapaCriacao::Nome; return; }

    std::unique_ptr<RacaBase> racaTemporaria;
    switch(escolha) {
        case 0: racaTemporaria = std::make_unique<Dwarf>(); break;
        case 1: racaTemporaria = std::make_unique<Elfo>();  break;
        case 2: racaTemporaria = std::make_unique<Humano>(); break;
        case 3: racaTemporaria = std::make_unique<Ork>();   break;
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
    int escolha = TelaMenu::exibirPromptClasse(nome, raca->obterNomeRaca());
    if (escolha == 4) { etapaAtual = EtapaCriacao::Raca; return; }

    std::unique_ptr<ClasseBase> classeTemporaria;
    switch(escolha) 
    {
        case 0: classeTemporaria = std::make_unique<Arqueiro>(); break;
        case 1: classeTemporaria = std::make_unique<Bardo>(); break;
        case 2: classeTemporaria = std::make_unique<Guerreiro>(); break;
        case 3: classeTemporaria = std::make_unique<Mago>(); break;
    }

    if (classeTemporaria) 
    {
        std::vector<std::string> atrDestaque;
        std::string descCombatStyle = "";
        TipoClasse tipo = classeTemporaria->obterTipoClasse();
        if (tipo == TipoClasse::Guerreiro || tipo == TipoClasse::Arqueiro) atrDestaque = {"Forca", "Destreza"};
        else if (tipo == TipoClasse::Mago || tipo == TipoClasse::Bardo) atrDestaque = {"Inteligencia", "Sabedoria"};

        if (tipo == TipoClasse::Guerreiro) descCombatStyle = "Estilo de combate: Equilibrado em dano e resistencia.";
        else if (tipo == TipoClasse::Bardo) descCombatStyle = "Estilo de combate: Focado em buffs e curas.";
        else if (tipo == TipoClasse::Mago) descCombatStyle = "Estilo de combate: Estilo Glass Cannon.";
        else if (tipo == TipoClasse::Arqueiro) descCombatStyle = "Estilo de combate: Focado em dano critico e desvios.";

        std::vector<std::string> info = TelaMenu::comporQuadroDeAtributos(classeTemporaria->obterAtributosClasse(), "[ ATRIBUTOS BONUS DA CLASSE ]", "[ HABILIDADE PASSIVA DA CLASSE ]", classeTemporaria->obterNomePassivaClasse(), classeTemporaria->obterDescricaoPassivaClasse(), atrDestaque);
        
        info.insert(info.begin(), "");
        info.insert(info.begin(), Aparencia::cor(Cor::CINZA) + descCombatStyle + Aparencia::cor(Cor::RESET));

        info.push_back("");
        info.insert(info.end(), {
            Aparencia::cor(Cor::BRANCO) + "[ HABILIDADE ATIVA DA CLASSE ]" + Aparencia::cor(Cor::RESET),
            " " + Aparencia::cor(Cor::CIANO) + classeTemporaria->obterNomeHabilidadeClasse() + Aparencia::cor(Cor::RESET),
            " - " + Aparencia::cor(Cor::CINZA) + classeTemporaria->obterDescricaoHabilidadeClasse() + Aparencia::cor(Cor::RESET),
            " - " + Aparencia::cor(Cor::CINZA) + classeTemporaria->obterRecargaHabilidadeClasse() + Aparencia::cor(Cor::RESET),
            "",
            Aparencia::cor(Cor::BRANCO) + "[ EQUIPAMENTO INICIAL DA CLASSE ]" + Aparencia::cor(Cor::RESET)
        });
        
        auto kit = classeTemporaria->obterEquipamentoClasse();
        std::map<std::string, std::pair<int, TipoEquipamento>> contagem;
        for (const auto& itemDoKit : kit) {
            if (itemDoKit) {
                std::string nomeCompleto = itemDoKit->obterNomeItem() + itemDoKit->obterInfoStatus();
                
                if (itemDoKit->obterTipo() == TipoEquipamento::ARMA) {
                    if (auto* arma = dynamic_cast<EquipamentoArma*>(itemDoKit.get())) {
                        int dFis = arma->obterDanoFisico();
                        int dMag = arma->obterDanoMagico();
                        if (dFis > 0 && dMag > 0) nomeCompleto += Aparencia::cor(Cor::CINZA) + " [" + std::to_string(dFis) + " Fis | " + std::to_string(dMag) + " Mag]" + Aparencia::cor(Cor::RESET);
                        else if (dFis > 0) nomeCompleto += Aparencia::cor(Cor::CINZA) + " [" + std::to_string(dFis) + " Dano Fis]" + Aparencia::cor(Cor::RESET);
                        else if (dMag > 0) nomeCompleto += Aparencia::cor(Cor::CINZA) + " [" + std::to_string(dMag) + " Dano Mag]" + Aparencia::cor(Cor::RESET);
                    }
                } else if (itemDoKit->obterTipo() == TipoEquipamento::ARMADURA) {
                    if (auto* armadura = dynamic_cast<EquipamentoArmadura*>(itemDoKit.get())) {
                        nomeCompleto += Aparencia::cor(Cor::CINZA) + " [" + std::to_string(armadura->obterReducaoFixa()) + " Defesa]" + Aparencia::cor(Cor::RESET);
                    }
                } else if (itemDoKit->obterTipo() == TipoEquipamento::ESCUDO) {
                    if (auto* escudo = dynamic_cast<EquipamentoEscudo*>(itemDoKit.get())) {
                        nomeCompleto += Aparencia::cor(Cor::CINZA) + " [" + std::to_string(escudo->obterReducaoDanoFixaEscudo()) + " Bloqueio]" + Aparencia::cor(Cor::RESET);
                    }
                }

                contagem[nomeCompleto].first++;
                contagem[nomeCompleto].second = itemDoKit->obterTipo();
            }
        }
        
        std::vector<TipoEquipamento> ordemPrioridade = { TipoEquipamento::ARMA, TipoEquipamento::ESCUDO, TipoEquipamento::ARMADURA, TipoEquipamento::CONSUMIVEL };
        for (TipoEquipamento tipo : ordemPrioridade) {
            for (auto const& [nomeDoItem, dadosDoItem] : contagem) {
                if (dadosDoItem.second == tipo) {
                    info.push_back(" - " + std::to_string(dadosDoItem.first) + "x " + nomeDoItem);
                }
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
    std::string nomeDificuldade = Aparencia::cor(Cor::AMARELO) + "Normal" + Aparencia::cor(Cor::RESET);
    if (dificuldade == 1) nomeDificuldade = Aparencia::cor(Cor::VERDE) + "Facil" + Aparencia::cor(Cor::RESET);
    else if (dificuldade == 3) nomeDificuldade = Aparencia::cor(Cor::VERMELHO) + "Dificil" + Aparencia::cor(Cor::RESET);

    int escolha = TelaMenu::exibirPromptParry(nome, raca->obterNomeRaca(), classe->obterNomeClasse() + " | DIFICULDADE: " + nomeDificuldade);
    if (escolha == 2) { etapaAtual = EtapaCriacao::Dificuldade; return; }

    if (escolha == 0) {
        parry = true;
        std::string statusParry = Aparencia::cor(Cor::VERDE) + "Ligado" + Aparencia::cor(Cor::RESET);
        std::string infoBox = "| JOGADOR: " + nome + " | RACA: " + raca->obterNomeRaca() + " | CLASSE: " + classe->obterNomeClasse() + " | DIFICULDADE: " + nomeDificuldade + " | PARRY: " + statusParry + " |";
        TelaMenu::exibirTutorialDeParry(infoBox);
        etapaAtual = EtapaCriacao::Concluido;
    } else if (escolha == 1) {
        parry = false;
        etapaAtual = EtapaCriacao::Concluido;
    }
}

void GerenciadorMenu::etapaEscolherDificuldade(const std::string& nome, RacaBase* raca, ClasseBase* classe, int& dificuldade, EtapaCriacao& etapaAtual)
{
    int escolha = TelaMenu::exibirPromptDificuldade(nome, raca->obterNomeRaca(), classe->obterNomeClasse());
    if (escolha == 3) { etapaAtual = EtapaCriacao::Classe; return; }

    dificuldade = escolha + 1;
    etapaAtual = EtapaCriacao::Parry;
}
