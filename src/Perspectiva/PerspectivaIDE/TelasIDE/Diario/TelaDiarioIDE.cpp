#include "TelaDiarioIDE.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include <chrono>

#include "../../../../Core/Utilidades/Aparencia.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"
#include "../../../TelasBase/Menu/TelaMenu.h"
#include "../../../TelasBase/Bestiario/TelaBestiario.h"
#include "../../../TelasBase/TelaBase.h"

#include "../../../TelasBase/Diario/TelaDiarioLayout.h"
#include "../../../TelasBase/Diario/TelaDiarioLogic.h"
#include "../../../../Sistemas/Inventario/FabricaItens.h"
#include "../../../../Sistemas/Inventario/Item.h"
#include "../../../../Sistemas/Progresso/Diario.h"
#include "../../../../Entidades/Personagem.h"
#include "../../../../Entidades/Racas/Anao.h"
#include "../../../../Entidades/Racas/Elfo.h"
#include "../../../../Entidades/Racas/Humano.h"
#include "../../../../Entidades/Racas/Orc.h"
#include "../../../../Entidades/Classes/Arqueiro.h"
#include "../../../../Entidades/Classes/Bardo.h"
#include "../../../../Entidades/Classes/Guerreiro.h"
#include "../../../../Entidades/Classes/Mago.h"
#include "../../../../Entidades/Classes/Necromante.h"
#include "../../../../Entidades/NPCs/Ferreiro/NPCFerreiroLayout.h"
#include "../../../../Entidades/NPCs/Mercador/NPCMercadorLayout.h"
#include "../../../../Entidades/NPCs/Maga/NPCMagaLayout.h"
#include "../../../../Entidades/NPCs/CavaleiroGenerico/NPCCavaleiroGenericoLayout.h"
#include "../../../TelasBase/Inventario/TelaInventario.h"
#include "../../../../Entidades/Racas/RacaBase.h"
#include "../../../../Entidades/Classes/ClasseBase.h"

namespace {

    void inspecionarItem(Personagem* jogadorAtual, const std::string& nomeItem) {
        Aparencia::limparTela();
        Aparencia::exibirPainelArte(ArtesDiario::logoDiario, 75, Cor::AMARELO, "INSPECAO DE ITEM", false);
        std::cout << "\n";
        auto item = FabricaItens::criarItem(nomeItem);
        if (item) {
            TelaInventario::exibirInspecaoItem(item.get(), jogadorAtual);
        } else {
            Aparencia::imprimirCentralizado("Detalhes indisponiveis para: " + nomeItem, Aparencia::cor(Cor::VERMELHO));
        }
        ControleDeInput::aguardarEnter();
    }

    void inspecionarNPC(const std::string& nomeNPC) {
        Aparencia::limparTela();
        Aparencia::exibirPainelArte(ArtesDiario::logoDiario, 75, Cor::AMARELO, "INSPECAO DE NPC", false);
        std::cout << "\n";

        std::vector<std::string> arte;
        std::string lore;

        if (nomeNPC.find("Bjorn") != std::string::npos) {
            arte = NPCFerreiroLayouts::arteFerreiro;
            lore = "Bjorn, o Ferreiro da Vila.\nUm anao robusto de poucas palavras, deixa o ferro a forja falarem por ele.\nSempre disposto a melhorar seus equipamentos por materiais da regiao.";
        } else if (nomeNPC.find("Franchesco") != std::string::npos) {
            arte = NPCMercadorLayouts::arteMercador;
            lore = "Franchesco, o Mercador Ambulante.\nSempre com um sorriso no rosto, ainda mais se ver sua carteira cheia.\nGosta de moedas de ouro mais do que da propria vida.";
        } else if (nomeNPC.find("Morgana") != std::string::npos) {
            arte = NPCMagaLayouts::arteMaga;
            lore = "Morgana, a Bruxa da Floresta.\nUm misterio, domina as artes da alquimia e encantamento.\nSeu labirinto guarda segredos que poucos ousam buscar.";
        } else if (nomeNPC.find("Cavaleiro Real") != std::string::npos) {
            arte = NPCCavaleiroGenericoLayouts::arteCavaleiro;
            lore = "Cavaleiro Real.\nProtetores leais do Reino e executores da vontade do Rei.\nFortemente blindados e treinados contra ameacas e monstros.";
        } else {
            lore = "Informacoes detalhadas sobre as intencoes e o passado dessa pessoa permanecem um misterio.";
        }

        if (!arte.empty()) {
            Aparencia::imprimirBlocoCentralizado(arte, Aparencia::cor(Cor::CINZA));
            std::cout << "\n";
        }

        Aparencia::imprimirCentralizado(Aparencia::cor(Cor::CIANO) + " ═══ " + nomeNPC + " ═══" + Aparencia::cor(Cor::RESET));
        std::cout << "\n";

        std::vector<std::string> linhasLore;
        size_t pos = 0;
        std::string desc = lore;
        while ((pos = desc.find('\n')) != std::string::npos) {
            linhasLore.push_back(" > " + desc.substr(0, pos));
            desc.erase(0, pos + 1);
        }
        linhasLore.push_back(" > " + desc);
        Aparencia::imprimirBlocoCentralizado(linhasLore);

        std::cout << "\n";
        ControleDeInput::aguardarEnter();
    }

    void inspecionarRacaJogavel(const std::string& nomeRaca) {
        Aparencia::limparTela();
        Aparencia::exibirPainelArte(ArtesDiario::logoDiario, 75, Cor::AMARELO, "INSPECAO DE RACA", false);
        std::cout << "\n";

        std::unique_ptr<RacaBase> racaObj;
        if (nomeRaca == "Humano") racaObj = std::make_unique<Humano>();
        else if (nomeRaca == "Dwarf") racaObj = std::make_unique<Dwarf>();
        else if (nomeRaca == "Elfo") racaObj = std::make_unique<Elfo>();
        else if (nomeRaca == "Ork") racaObj = std::make_unique<Ork>();

        if (racaObj) {
            std::vector<std::string> arte = racaObj->obterAparenciaRaca();
            std::vector<std::string> atributos = TelaMenu::comporQuadroDeAtributos(
                racaObj->obterAtributosRaca(),
                "[ ATRIBUTOS BASE ]",
                "[ HABILIDADE DA RACA ]",
                racaObj->obterNomeHabilidadeRaca(),
                racaObj->obterDescricaoHabilidadeRaca()
            );

            Aparencia::imprimirLadoALado(atributos, arte, 40, 6, Cor::BRANCO, Cor::BRANCO, 0);
        }
        std::cout << "\n";
        ControleDeInput::aguardarEnter();
    }

    void inspecionarClasse(const std::string& nomeClasse) {
        Aparencia::limparTela();
        Aparencia::exibirPainelArte(ArtesDiario::logoDiario, 75, Cor::AMARELO, "INSPECAO DE CLASSE", false);
        std::cout << "\n";

        std::unique_ptr<ClasseBase> classeObj;
        if (nomeClasse == "Guerreiro") classeObj = std::make_unique<Guerreiro>();
        else if (nomeClasse == "Mago") classeObj = std::make_unique<Mago>();
        else if (nomeClasse == "Arqueiro") classeObj = std::make_unique<Arqueiro>();
        else if (nomeClasse == "Bardo") classeObj = std::make_unique<Bardo>();
        else if (nomeClasse == "Necromante") classeObj = std::make_unique<Necromante>();

        if (classeObj) {
            std::vector<std::string> arte = classeObj->obterAparenciaClasseMenu();

            std::vector<std::string> atributos = TelaMenu::comporQuadroDeAtributos(
                classeObj->obterAtributosClasse(),
                "[ ATRIBUTOS BONUS ]",
                "[ PASSIVA DA CLASSE ]",
                classeObj->obterNomePassivaClasse(),
                classeObj->obterDescricaoPassivaClasse(),
                "[ HABILIDADE ATIVA ]",
                classeObj->obterNomeHabilidadeClasse(),
                classeObj->obterDescricaoHabilidadeClasse()
            );

            Aparencia::imprimirLadoALado(atributos, arte, 40, 6, Cor::BRANCO, Cor::BRANCO, 0);
        }
        std::cout << "\n";
        ControleDeInput::aguardarEnter();
    }
}

static void exibirProgresso(Personagem* /*jogador*/) {
    TelaBase::executarLoopPadrao(
        "DIARIO - PROGRESSO",
        Cor::AMARELO,
        [&]() {
            std::cout << "\n";
            Aparencia::imprimirCentralizado("Seus maiores feitos nesta jornada:");
            std::cout << "\n\n";

            auto dados = TelaDiarioLogic::obterProgresso();

            if (dados.totalConquistas == 0) {
                Aparencia::imprimirCentralizado("Nenhum grande feito para registrar ainda...", Aparencia::cor(Cor::CINZA));
                std::cout << "\n";
            } else {
                std::vector<std::string> linhasParaImprimir;
                for (const auto& l : dados.linhasNPC) linhasParaImprimir.push_back(l);
                for (const auto& l : dados.linhasMonstro) linhasParaImprimir.push_back(l);
                for (const auto& l : dados.linhasItem) linhasParaImprimir.push_back(l);
                if (!linhasParaImprimir.empty()) linhasParaImprimir.pop_back();
                Aparencia::imprimirBlocoCentralizado(linhasParaImprimir);
            }
        },
        []() { return std::vector<std::string>{"VOLTAR"}; },
        [&](int id) {
            return !(id == 0 || id == -1);
        }
    );
}

static void exibirMenuItens(Personagem* jogadorAtual) {
    bool continuar = true;
    while (continuar) {
        Aparencia::limparTela();
        Aparencia::exibirPainelArte(ArtesDiario::logoDiario, 75, Cor::AMARELO, "ITENS DESCOBERTOS", false);
        std::cout << "\n";
        std::vector<std::string> itens = Diario::instancia().obterItensDescobertos();
        if (itens.empty()) {
            Aparencia::imprimirCentralizado("Nenhum item descoberto ainda.", Aparencia::cor(Cor::CINZA));
            ControleDeInput::aguardarEnter();
            return;
        }

        auto cats = TelaDiarioLogic::categorizarItens(jogadorAtual);
        std::vector<std::string> armas = cats.armas;
        std::vector<std::string> escudos = cats.escudos;
        std::vector<std::string> armaduras = cats.armaduras;
        std::vector<std::string> consumiveis = cats.consumiveis;
        std::vector<std::string> materiais = cats.materiais;
        std::vector<std::string> missoes = cats.missoes;
        std::vector<std::string> outros = cats.outros;

        std::vector<std::string> categorias = {
            "Armas (" + std::to_string(armas.size()) + ")",
            "Escudos (" + std::to_string(escudos.size()) + ")",
            "Armaduras (" + std::to_string(armaduras.size()) + ")",
            "Consumiveis (" + std::to_string(consumiveis.size()) + ")",
            "Materiais (" + std::to_string(materiais.size()) + ")",
            "Itens de Missao (" + std::to_string(missoes.size()) + ")",
            "Outros (" + std::to_string(outros.size()) + ")",
            "Voltar"
        };

        int escolhaCat = ControleDeInput::lerSelecaoMenuComSetas(categorias, true);
        if (escolhaCat == 7 || escolhaCat == -1) { continuar = false; break; }

        std::vector<std::string>* listaSelecionada = nullptr;
        if (escolhaCat == 0) listaSelecionada = &armas;
        else if (escolhaCat == 1) listaSelecionada = &escudos;
        else if (escolhaCat == 2) listaSelecionada = &armaduras;
        else if (escolhaCat == 3) listaSelecionada = &consumiveis;
        else if (escolhaCat == 4) listaSelecionada = &materiais;
        else if (escolhaCat == 5) listaSelecionada = &missoes;
        else if (escolhaCat == 6) listaSelecionada = &outros;

        if (listaSelecionada && !listaSelecionada->empty()) {
            bool lendoItens = true;
            while (lendoItens) {
                Aparencia::limparTela();
                Aparencia::exibirPainelArte(ArtesDiario::logoDiario, 75, Cor::AMARELO, categorias[escolhaCat], false);
                std::cout << "\n";

                std::vector<std::string> menuItens = *listaSelecionada;
                menuItens.push_back("Voltar");

                int escolhaItem = ControleDeInput::lerSelecaoMenuComSetas(menuItens, true);
                if (static_cast<size_t>(escolhaItem) == menuItens.size() - 1 || escolhaItem == -1) {
                    lendoItens = false;
                } else {
                    std::string nomeLimpo = (*listaSelecionada)[escolhaItem].substr(3);
                    inspecionarItem(jogadorAtual, nomeLimpo);
                }
            }
        } else {
            Aparencia::imprimirCentralizado("Nenhum item nesta categoria.", Aparencia::cor(Cor::CINZA));
            ControleDeInput::aguardarEnter();
        }
    }
}

static void exibirMenuNPCs(Personagem* /*jogadorAtual*/) {
    bool continuar = true;
    while (continuar) {
        Aparencia::limparTela();
        Aparencia::exibirPainelArte(ArtesDiario::logoDiario, 75, Cor::AMARELO, "NPCs CONHECIDOS", false);
        std::cout << "\n";
        std::vector<std::string> npcs = Diario::instancia().obterNPCsDescobertos();
        if (npcs.empty()) {
            Aparencia::imprimirCentralizado("Nenhum NPC encontrado ainda.", Aparencia::cor(Cor::CINZA));
            ControleDeInput::aguardarEnter();
            return;
        }

        Aparencia::ordenarAlfabeticamente(npcs);

        std::vector<std::string> opcoes = npcs;
        opcoes.push_back("Voltar");

        int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
        if (static_cast<size_t>(escolha) == opcoes.size() - 1 || escolha == -1) {
            continuar = false;
        } else {
            inspecionarNPC(npcs[escolha]);
        }
    }
}

static void exibirMenuRacas(Personagem* jogadorAtual) {
    bool continuar = true;
    while (continuar) {
        Aparencia::limparTela();
        Aparencia::exibirPainelArte(ArtesDiario::logoDiario, 75, Cor::AMARELO, "RACAS DESCOBERTAS", false);
        std::cout << "\n";
        std::vector<std::string> racas = Diario::instancia().obterRacasDescobertas();
        if (racas.empty()) {
            Aparencia::imprimirCentralizado("Nenhuma raca registrada ainda.", Aparencia::cor(Cor::CINZA));
            ControleDeInput::aguardarEnter();
            return;
        }

        std::vector<std::string> jogaveis, monstros;
        for (const auto& raca : racas) {
            if (raca == "Humano" || raca == "Dwarf" || raca == "Elfo" || raca == "Ork") {
                jogaveis.push_back(raca);
            } else {
                monstros.push_back(raca);
            }
        }

        Aparencia::ordenarAlfabeticamente(jogaveis);
        Aparencia::ordenarAlfabeticamente(monstros);

        std::vector<std::string> categorias = {
            "Racas Jogaveis (" + std::to_string(jogaveis.size()) + ")",
            "Monstros e Inimigos (" + std::to_string(monstros.size()) + ")",
            "Voltar"
        };

        int escolhaCat = ControleDeInput::lerSelecaoMenuComSetas(categorias, true);
        if (escolhaCat == 2 || escolhaCat == -1) { continuar = false; break; }

        std::vector<std::string>* listaSelecionada = (escolhaCat == 0) ? &jogaveis : &monstros;

        if (!listaSelecionada->empty()) {
            bool lendoRacas = true;
            while (lendoRacas) {
                Aparencia::limparTela();
                Aparencia::exibirPainelArte(ArtesDiario::logoDiario, 75, Cor::AMARELO, (escolhaCat == 0 ? "RACAS JOGAVEIS" : "MONSTROS E INIMIGOS"), false);
                std::cout << "\n";

                std::vector<std::string> menuRacas = *listaSelecionada;
                menuRacas.push_back("Voltar");

                int escolhaRaca = ControleDeInput::lerSelecaoMenuComSetas(menuRacas, true);
                if (static_cast<size_t>(escolhaRaca) == menuRacas.size() - 1 || escolhaRaca == -1) {
                    lendoRacas = false;
                } else {
                    if (escolhaCat == 0) {
                        inspecionarRacaJogavel((*listaSelecionada)[escolhaRaca]);
                    } else {
                        TelaBestiario::exibirFicha(jogadorAtual, (*listaSelecionada)[escolhaRaca], 0, {});
                    }
                }
            }
        } else {
            Aparencia::imprimirCentralizado("Nenhuma raca nesta categoria.", Aparencia::cor(Cor::CINZA));
            ControleDeInput::aguardarEnter();
        }
    }
}

static void exibirMenuMissoes(Personagem* jogadorAtual) {
    bool continuar = true;
    while (continuar) {
        Aparencia::limparTela();
        Aparencia::exibirPainelArte(ArtesDiario::logoDiario, 75, Cor::AMARELO, "DIARIO DE MISSOES", false);
        std::cout << "\n";

        auto cats = TelaDiarioLogic::categorizarMissoes(jogadorAtual);

        std::vector<std::string> linhas;

        linhas.push_back(Aparencia::cor(Cor::AMARELO) + "Em andamento" + Aparencia::cor(Cor::RESET));
        if (cats.emAndamento.empty()) linhas.push_back("  (Nenhuma)");
        for (const auto& m : cats.emAndamento) linhas.push_back("  " + Aparencia::cor(Cor::BRANCO) + m + Aparencia::cor(Cor::RESET));
        linhas.push_back("");

        linhas.push_back(Aparencia::cor(Cor::AMARELO) + "Prontas" + Aparencia::cor(Cor::RESET));
        if (cats.prontas.empty()) linhas.push_back("  (Nenhuma)");
        for (const auto& m : cats.prontas) linhas.push_back("  " + Aparencia::cor(Cor::VERDE) + m + Aparencia::cor(Cor::RESET));
        linhas.push_back("");

        linhas.push_back(Aparencia::cor(Cor::AMARELO) + "Completas" + Aparencia::cor(Cor::RESET));
        if (cats.completas.empty()) linhas.push_back("  (Nenhuma)");
        for (const auto& m : cats.completas) linhas.push_back("  " + Aparencia::cor(Cor::CINZA) + m + Aparencia::cor(Cor::RESET));

        Aparencia::imprimirBlocoCentralizado(linhas);
        std::cout << "\n";

        std::vector<std::string> opcoes = {"Voltar"};
        int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
        if (escolha == 0 || escolha == -1) continuar = false;
    }
}

static void exibirMenuClasses(Personagem* /*jogadorAtual*/) {
    bool continuar = true;
    while (continuar) {
        Aparencia::limparTela();
        Aparencia::exibirPainelArte(ArtesDiario::logoDiario, 75, Cor::AMARELO, "CLASSES DESCOBERTAS", false);
        std::cout << "\n";
        std::vector<std::string> classes = Diario::instancia().obterClassesDescobertas();
        if (classes.empty()) {
            Aparencia::imprimirCentralizado("Nenhuma classe registrada ainda.", Aparencia::cor(Cor::CINZA));
            ControleDeInput::aguardarEnter();
            return;
        }

        Aparencia::ordenarAlfabeticamente(classes);

        std::vector<std::string> opcoes = classes;
        opcoes.push_back("Voltar");

        int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
        if (static_cast<size_t>(escolha) == opcoes.size() - 1 || escolha == -1) {
            continuar = false;
        } else {
            inspecionarClasse(classes[escolha]);
        }
    }
}

void TelaDiarioIDE::exibir(Personagem* jogadorAtual) {
    if (jogadorAtual == nullptr) return;

    static auto ultimoAcesso = std::chrono::steady_clock::now() - std::chrono::hours(1);
    bool animarEntrada = TelaBase::deveAnimarEntradaDaTela(ultimoAcesso, 300);

    bool continuar = true;
    while (continuar) {
        Aparencia::limparTela();
        Aparencia::exibirPainelArte(ArtesDiario::logoDiario, 75, Cor::AMARELO, "DIARIO DE JORNADA", animarEntrada);
        animarEntrada = false;

        std::vector<std::string> opcoes = {
            "Bestiario (Inimigos)",
            "Itens (Equipamentos, Consumiveis, Materiais, Missoes)",
            "NPCs Conhecidos",
            "Racas do Mundo",
            "Classes Jogaveis",
            "Missoes (Em andamento, Prontas, Completas)",
            "Progresso",
            "Voltar"
        };

        std::cout << "\n";
        Aparencia::imprimirCentralizado("O que voce deseja consultar?");
        std::cout << "\n";

        int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);

        switch (escolha) {
            case 0:
                TelaBestiario::exibirLista(jogadorAtual);
                break;
            case 1:
                exibirMenuItens(jogadorAtual);
                break;
            case 2:
                exibirMenuNPCs(jogadorAtual);
                break;
            case 3:
                exibirMenuRacas(jogadorAtual);
                break;
            case 4:
                exibirMenuClasses(jogadorAtual);
                break;
            case 5:
                exibirMenuMissoes(jogadorAtual);
                break;
            case 6:
                exibirProgresso(jogadorAtual);
                break;
            case 7:
            case -1:
                continuar = false;
                break;
        }
    }
}
