#include "MenuJogo.h"

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

#include "../../Entidades/Classes/Arqueiro.h"
#include "../../Entidades/Classes/Bardo.h"
#include "../../Entidades/Classes/ClasseBase.h"
#include "../../Entidades/Classes/Guerreiro.h"
#include "../../Entidades/Classes/Mago.h"
#include "../../Entidades/Classes/Necromante.h"
#include "CriadorInimigos.h"
#include "../../Sistemas/Inventario/Equipamentos/EquipamentoArma.h"
#include "../../Sistemas/Inventario/Equipamentos/EquipamentoArmadura.h"
#include "../../Sistemas/Inventario/Equipamentos/EquipamentoEscudo.h"
#include "../../Sistemas/Inventario/Item.h"
#include "../../Sistemas/Inventario/Itens/ItemConsumivel.h"
#include "../../Entidades/Racas/Anao.h"
#include "../../Entidades/Racas/Elfo.h"
#include "../../Entidades/Racas/Humano.h"
#include "../../Entidades/Racas/Orc.h"
#include "../../Entidades/Racas/RacaBase.h"
#include "../../Entidades/Inimigos/Goblin.h"
#include "../../Entidades/Inimigos/Slime.h"
#include "../../Entidades/Inimigos/Troll.h"
#include "../../Entidades/Inimigos/Fada.h"
#include "../../Entidades/Inimigos/Mimico.h"
#include "../../Sistemas/Progresso/Diario.h"
#include "../../Sistemas/Progresso/Salvamento.h"
#include "../../Perspectiva/TelasBase/Atributos/TelaAtributos.h"
#include "../../Perspectiva/TelasBase/Inventario/TelaInventario.h"
#include "../../Perspectiva/TelasBase/Menu/TelaMenu.h"
#include "../../Perspectiva/TelasBase/Menu/TelaAbertura.h"
#include "../../Perspectiva/TelasBase/Menu/TelaNome.h"
#include "../../Perspectiva/TelasBase/Menu/TelaRaca.h"
#include "../../Perspectiva/TelasBase/Menu/TelaClasse.h"
#include "../../Perspectiva/TelasBase/Menu/TelaDificuldade.h"
#include "../../Perspectiva/TelasBase/Menu/TelaParry.h"
#include "../../Perspectiva/TelasBase/Menu/TelaIntroducao.h"
#include "../Utilidades/Aparencia.h"
#include "../Utilidades/FuncoesDialogo.h"
#include "../Utilidades/ControleDeInput.h"
#include "../Utilidades/GeradorAleatorio.h"
#include "../../Perspectiva/TelasBase/TelaBase.h"

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

std::vector<std::string> MenuJogo::lerInformacoesDosSaves(const std::vector<std::string>& saves) {
    return lerInfoSavesLocal(saves);
}

namespace {
    std::unique_ptr<Personagem> lidarOpcoesDoSave(const std::string& saveSelecionado, const std::string& infoSave, bool& saveFoiDeletado) {
        std::unique_ptr<Personagem> charCarregado = nullptr;
        TelaBase::executarLoop(
            [](bool animar) { TelaMenu::exibirPainelLogoJogo("OPCOES DO SAVE", animar); },
            [infoSave]() {
                std::cout << "\n";
                Aparencia::imprimirCentralizado("Save selecionado: " + infoSave, Aparencia::cor(Cor::CIANO));
                std::cout << "\n";
            },
            []() { return std::vector<std::string>{"Ver Informacoes", "Carregar", "Deletar", "Voltar"}; },
            [&](int escAcao) {
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
                        Aparencia::imprimirBlocoCentralizado(TelaBase::criarCaixa(painelInfo, "DADOS DO PERSONAGEM", 40, Cor::CIANO));
                    } else {
                        std::cout << "\n";
                        Aparencia::imprimirCentralizado("Nao foi possivel ler os dados.", Aparencia::cor(Cor::VERMELHO));
                    }
                    ControleDeInput::aguardarEnter();
                }
                else if (escAcao == 1) {
                    charCarregado = Salvamento::carregarJogo(saveSelecionado);
                    if (charCarregado) {
                        std::cout << "\n";
                        Aparencia::imprimirCentralizado(FuncoesDialogo::formatarMsgSistema("Jogo carregado com sucesso!", Cor::AMARELO));
                        ControleDeInput::aguardarEnter();
                        return false; 
                    } else {
                        std::cout << "\n";
                        Aparencia::imprimirCentralizado(FuncoesDialogo::formatarMsgSistema("Falha ao carregar o save!", Cor::VERMELHO));
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
                        Salvamento::deletarSave(saveSelecionado);
                        std::cout << "\n";
                        Aparencia::imprimirCentralizado(FuncoesDialogo::formatarMsgSistema("Save deletado com sucesso!", Cor::VERDE));
                        ControleDeInput::aguardarEnter();
                        saveFoiDeletado = true;
                        return false; 
                    } else {
                        std::cout << "\n";
                        Aparencia::imprimirCentralizado(FuncoesDialogo::formatarMsgSistema("Nome incorreto. Exclusao cancelada.", Cor::VERMELHO));
                        ControleDeInput::aguardarEnter();
                    }
                }
                else if (escAcao == 3 || escAcao == -1) { return false; }
                return true;
            }
        );
        return charCarregado;
    }

    std::unique_ptr<Personagem> lidarGerenciamentoDeSaves() {
        std::unique_ptr<Personagem> charCarregado = nullptr;
        TelaBase::executarLoop(
            [](bool animar) { TelaMenu::exibirPainelLogoJogo("GERENCIAR SAVES", animar); },
            []() {
                std::cout << "\n";
                Aparencia::imprimirCentralizado("Selecione um save para gerenciar:\n");
            },
            []() {
                auto saves = Salvamento::listarSaves();
                std::vector<std::string> opcoesSave = lerInfoSavesLocal(saves);
                if (opcoesSave.empty()) return std::vector<std::string>{"Nenhum save encontrado. Voltar"};
                opcoesSave.push_back("Voltar");
                return opcoesSave;
            },
            [&](int escSave) {
                auto saves = Salvamento::listarSaves();
                if (saves.empty() || escSave >= static_cast<int>(saves.size()) || escSave == -1) return false;
                
                std::vector<std::string> infoSaves = lerInfoSavesLocal(saves);
                bool saveDeletado = false;
                charCarregado = lidarOpcoesDoSave(saves[escSave], infoSaves[escSave], saveDeletado);
                if (charCarregado) return false;
                return true;
            }
        );
        return charCarregado;
    }

    std::unique_ptr<Personagem> lidarMenuDeOpcoes() {
        std::unique_ptr<Personagem> charCarregado = nullptr;
        TelaBase::executarLoop(
            [](bool animar) { TelaMenu::exibirPainelLogoJogo("OPCOES", animar); },
            []() { std::cout << "\n"; },
            []() { return std::vector<std::string>{"Gerenciar Saves", "Voltar"}; },
            [&](int esc) {
                if (esc == 0) {
                    charCarregado = lidarGerenciamentoDeSaves();
                    if (charCarregado) return false;
                } else if (esc == 1 || esc == -1) {
                    return false;
                }
                return true;
            }
        );
        return charCarregado;
    }
}

std::unique_ptr<Personagem> MenuJogo::menuPrincipal() 
{
    TelaAbertura::exibir();

    while (true) {
        bool temSave = Salvamento::saveExiste();
        int selecao = TelaMenu::exibirOpcoesMenuPrincipal(temSave, {}, {}, {});
        
        std::string opcaoSelecionada;
        if (selecao == 0) opcaoSelecionada = "Novo Jogo";
        else if (temSave && selecao == 1) opcaoSelecionada = "Continuar Jogo";
        else if ((temSave && selecao == 2) || (!temSave && selecao == 1)) opcaoSelecionada = "Debug";
        else if ((temSave && selecao == 3) || (!temSave && selecao == 2)) opcaoSelecionada = "Opcoes";
        else opcaoSelecionada = "Sair";
        
        if (opcaoSelecionada == "Novo Jogo") {
            auto novoJogador = iniciarCriacaoDeSistemaPersonagem();
            if (novoJogador) return novoJogador;
        } else if (opcaoSelecionada == "Continuar Jogo") {
            auto saves = Salvamento::listarSaves();
            if (saves.empty()) continue;
            
            std::vector<std::string> informacoesSaves = lerInformacoesDosSaves(saves);
            int escolhaSave = TelaMenu::exibirMenuCarregarJogo(informacoesSaves);
            
            if (escolhaSave >= 0 && escolhaSave < static_cast<int>(saves.size())) {
                auto jogador = Salvamento::carregarJogo(saves[escolhaSave]);
                if (jogador) {
                    std::cout << "\n";
                    Aparencia::imprimirCentralizado(FuncoesDialogo::formatarMsgSistema("Jogo carregado com sucesso!", Cor::AMARELO));
                    ControleDeInput::aguardarEnter();
                    return jogador;
                } else {
                    std::cout << "\n";
                    Aparencia::imprimirCentralizado(FuncoesDialogo::formatarMsgSistema("Falha ao carregar o save!", Cor::VERMELHO));
                    ControleDeInput::aguardarEnter();
                }
            }
        } else if (opcaoSelecionada == "Debug") {
            auto raca = std::make_unique<Humano>();
            auto classe = std::make_unique<Mago>();
            auto personagemCriado = std::make_unique<Personagem>("Debug", std::move(raca), std::move(classe));
            personagemCriado->definirParryAtivado(true);
            personagemCriado->definirDificuldade(DificuldadeJogo::Facil);

            Diario::instancia().registrarRaca(personagemCriado->obterRaca()->obterNomeRaca());
            Diario::instancia().registrarClasse(personagemCriado->obterNomeClasse());
            for (Item* item : personagemCriado->obterInventario()->obterTodosOsItens()) {
                Diario::instancia().registrarItem(Aparencia::removerCoresANSI(item->obterNomeItem()));
            }

            std::string statusParry = Aparencia::cor(Cor::VERDE) + "Ligado" + Aparencia::cor(Cor::RESET);
            std::string nomeDificuldade = Aparencia::cor(Cor::VERDE) + "Facil" + Aparencia::cor(Cor::RESET);
            std::string infoBox = "| JOGADOR: Debug | RACA: Humano | CLASSE: Mago | DIFICULDADE: " + nomeDificuldade + " | PARRY: " + statusParry + " |";
            TelaMenu::exibirIntroducaoJornada(infoBox);

            return personagemCriado;
        } else if (opcaoSelecionada == "Opcoes") {
            auto jogador = lidarMenuDeOpcoes();
            if (jogador) return jogador;
        } else {
            std::vector<std::string> opcoesSimNao = { "NAO", "SIM" };
            std::cout << "\n";
            Aparencia::imprimirCentralizado(FuncoesDialogo::formatarMsgSistema("Deseja realmente sair do jogo?"));
            std::cout << "\n";
            if (ControleDeInput::lerSelecaoMenuComSetas(opcoesSimNao, true) == 1) {
                return nullptr;
            }
        }
    }
}

std::unique_ptr<Personagem> MenuJogo::iniciarCriacaoDeSistemaPersonagem() 
{
    std::string nomeDoPersonagem;
    std::unique_ptr<RacaBase> racaEscolhida;
    std::unique_ptr<ClasseBase> classeEscolhida;
    bool sistemaDeParryAtivado = false;
    int nivelDeDificuldadeEscolhido = 2;
    bool fazerTutorial = false;

    for (;;) {
        {
            auto resultado = TelaNome::exibir();
            if (resultado.voltou) return nullptr;
            nomeDoPersonagem = resultado.nome;
        }

        std::string nomeRaca;
        {
            auto resultado = TelaRaca::exibir(nomeDoPersonagem);
            if (resultado.voltou) continue;

            std::vector<std::string> racas = {"Dwarf", "Elfo", "Humano", "Ork"};
            Aparencia::ordenarAlfabeticamente(racas);
            std::string racaSelecionada = racas[resultado.indice];
            nomeRaca = racaSelecionada;

            if (racaSelecionada == "Dwarf") racaEscolhida = std::make_unique<Dwarf>();
            else if (racaSelecionada == "Elfo") racaEscolhida = std::make_unique<Elfo>();
            else if (racaSelecionada == "Humano") racaEscolhida = std::make_unique<Humano>();
            else if (racaSelecionada == "Ork") racaEscolhida = std::make_unique<Ork>();
        }

        std::string nomeClasse;
        {
            auto resultado = TelaClasse::exibir(nomeDoPersonagem, nomeRaca);
            if (resultado.voltou) continue;

            std::vector<std::string> classes = {"Arqueiro", "Bardo", "Guerreiro", "Mago", "Necromante"};
            Aparencia::ordenarAlfabeticamente(classes);
            std::string classeSelecionada = classes[resultado.indice];
            nomeClasse = classeSelecionada;

            if (classeSelecionada == "Arqueiro") classeEscolhida = std::make_unique<Arqueiro>();
            else if (classeSelecionada == "Bardo") classeEscolhida = std::make_unique<Bardo>();
            else if (classeSelecionada == "Guerreiro") classeEscolhida = std::make_unique<Guerreiro>();
            else if (classeSelecionada == "Mago") classeEscolhida = std::make_unique<Mago>();
            else if (classeSelecionada == "Necromante") classeEscolhida = std::make_unique<Necromante>();
        }

        {
            auto resultado = TelaDificuldade::exibir(nomeDoPersonagem, nomeRaca, nomeClasse);
            if (resultado.voltou) continue;
            nivelDeDificuldadeEscolhido = resultado.indice + 1;
        }

        {
            auto resultado = TelaParry::exibir(nomeDoPersonagem, nomeRaca, nomeClasse);
            if (resultado.voltou) continue;
            sistemaDeParryAtivado = resultado.parryAtivado;
            fazerTutorial = resultado.fazerTutorial;
        }

        auto personagemCriado = std::make_unique<Personagem>(nomeDoPersonagem, std::move(racaEscolhida), std::move(classeEscolhida));
        personagemCriado->definirParryAtivado(sistemaDeParryAtivado);
        personagemCriado->definirDificuldade(static_cast<DificuldadeJogo>(nivelDeDificuldadeEscolhido));

        Diario::instancia().registrarRaca(personagemCriado->obterRaca()->obterNomeRaca());
        Diario::instancia().registrarClasse(personagemCriado->obterNomeClasse());

        for (Item* item : personagemCriado->obterInventario()->obterTodosOsItens()) {
            Diario::instancia().registrarItem(Aparencia::removerCoresANSI(item->obterNomeItem()));
        }

        if (fazerTutorial) {
            TelaMenu::exibirTutorialDeParry("");
        }

        TelaIntroducao::exibir();

        return personagemCriado;
    }
}


