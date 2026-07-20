#include "TelaDiario.h"
#include "TelaDiarioLayout.h"
#include "TelaDiarioLogic.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include <map>
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../TelaBase.h"
#include "../Menu/TelaMenu.h"

#include "../../../Sistemas/Inventario/FabricaItens.h"
#include "../../../Sistemas/Inventario/Item.h"
#include "../../../Sistemas/Progresso/Diario.h"
#include "../../../Sistemas/Progresso/Bestiario.h"

#include "../../../Entidades/Personagem.h"
#include "../../../Entidades/Racas/Anao.h"
#include "../../../Entidades/Racas/Elfo.h"
#include "../../../Entidades/Racas/Humano.h"
#include "../../../Entidades/Racas/Orc.h"
#include "../../../Entidades/Racas/RacaBase.h"
#include "../../../Entidades/Classes/Arqueiro.h"
#include "../../../Entidades/Classes/Bardo.h"
#include "../../../Entidades/Classes/Guerreiro.h"
#include "../../../Entidades/Classes/Mago.h"
#include "../../../Entidades/Classes/Necromante.h"
#include "../../../Entidades/Classes/ClasseBase.h"
#include "../../../Entidades/NPCs/Ferreiro/NPCFerreiroLayout.h"
#include "../../../Entidades/NPCs/Mercador/NPCMercadorLayout.h"
#include "../../../Entidades/NPCs/Maga/NPCMagaLayout.h"
#include "../../../Entidades/NPCs/CavaleiroGenerico/NPCCavaleiroGenericoLayout.h"

#include "../../GerenciadorPerspectiva.h"

namespace {

const int LOGO_HEIGHT = 8;
const int MIN_Y = 11;

enum Secao { PRINCIPAL, BESTIARIO, ITENS, NPCS, RACAS, CLASSES, MISSOES, PROGRESSO };

void adicionarOpcao(std::vector<std::string>& linhas, const std::string& texto, bool selecionado) {
    if (selecionado) {
        linhas.push_back(Aparencia::cor(Cor::VERDE) + " > " + texto + Aparencia::cor(Cor::BRANCO) + "\033[48;2;25;25;25m");
    } else {
        linhas.push_back("   " + texto);
    }
}

void exibirPopupEAguarda(const std::string& titulo, const std::vector<std::string>& linhas) {
    GerenciadorPerspectiva::obterDiarioUI().renderizarPopupMensagem(titulo, linhas);
    ControleDeInput::lerTecla();
}

void exibirPopupComArteEAguarda(const std::string& titulo, const std::vector<std::string>& arte, const std::vector<std::string>& info, const std::string& subtitulo) {
    GerenciadorPerspectiva::obterDiarioUI().renderizarPopupInspecaoComArte(titulo, arte, info, subtitulo);
    ControleDeInput::lerTecla();
}

void inspecionarItem(Personagem* jogadorAtual, const std::string& nomeItem) {
    auto item = FabricaItens::criarItem(nomeItem);
    if (item) {
        std::vector<std::string> detalhes = item->obterDetalhesInspecao(jogadorAtual);
        std::vector<std::string> linhasInsp;
        linhasInsp.push_back(Aparencia::cor(Cor::AMARELO) + " >> " + item->obterNomeItem() + " <<" + Aparencia::cor(Cor::RESET));
        linhasInsp.push_back("");
        linhasInsp.insert(linhasInsp.end(), detalhes.begin(), detalhes.end());
        exibirPopupEAguarda("INSPECAO DE ITEM", linhasInsp);
    }
}

void inspecionarNPC(const std::string& nomeNPC) {
    std::vector<std::string> arte;
    std::string lore;
    if (nomeNPC.find("Bjorn") != std::string::npos) {
        arte = NPCFerreiroLayouts::arteFerreiro;
        lore = "Bjorn, o Ferreiro da Vila.\nUm anao robusto de poucas palavras.\nSempre disposto a melhorar equipamentos.";
    } else if (nomeNPC.find("Franchesco") != std::string::npos) {
        arte = NPCMercadorLayouts::arteMercador;
        lore = "Franchesco, o Mercador Ambulante.\nSempre com um sorriso no rosto.\nGosta de moedas de ouro mais do que de viver.";
    } else if (nomeNPC.find("Morgana") != std::string::npos) {
        arte = NPCMagaLayouts::arteMaga;
        lore = "Morgana, a Bruxa da Floresta.\nUm misterio, domina alquimia e encantamentos.\nSeu labirinto guarda segredos profundos.";
    } else if (nomeNPC.find("Cavaleiro Real") != std::string::npos) {
        arte = NPCCavaleiroGenericoLayouts::arteCavaleiro;
        lore = "Cavaleiro Real.\nProtetores leais do Reino.\nFortemente blindados e treinados.";
    } else {
        lore = "Informacoes sobre essa pessoa permanecem um misterio.";
    }
    std::vector<std::string> linhasLore;
    size_t pos = 0;
    std::string temp = lore;
    while ((pos = temp.find('\n')) != std::string::npos) {
        linhasLore.push_back(" > " + temp.substr(0, pos));
        temp.erase(0, pos + 1);
    }
    linhasLore.push_back(" > " + temp);
    exibirPopupComArteEAguarda("INSPECAO DE NPC", arte, linhasLore, nomeNPC);
}

void inspecionarRaca(const std::string& nomeRaca) {
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
        exibirPopupComArteEAguarda("INSPECAO DE RACA", arte, atributos, nomeRaca);
    }
}

void inspecionarClasse(const std::string& nomeClasse) {
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
        exibirPopupComArteEAguarda("INSPECAO DE CLASSE", arte, atributos, nomeClasse);
    }
}

void inspecionarBestiario(const std::string& nomeInimigo) {
    const auto* info = Bestiario::instancia().obterInfo(nomeInimigo);
    if (info) {
        std::vector<std::string> detalhes;
        if (Bestiario::instancia().jaDerrotado(nomeInimigo)) {
            detalhes.push_back(Aparencia::cor(Cor::VERMELHO) + "Derrotas: " + std::to_string(Bestiario::instancia().obterQuantidadeDerrotas(nomeInimigo)) + Aparencia::cor(Cor::RESET));
            detalhes.push_back("");
            auto loreLinhas = TelaDiarioLogic::quebrarTexto(info->lore, 50);
            for (const auto& l : loreLinhas)
                detalhes.push_back(Aparencia::cor(Cor::CINZA) + l + Aparencia::cor(Cor::RESET));
            auto fatoLinhas = TelaDiarioLogic::quebrarTexto(info->fatoCurioso, 50);
            for (const auto& f : fatoLinhas)
                detalhes.push_back(Aparencia::cor(Cor::AMARELO) + f + Aparencia::cor(Cor::RESET));
        } else {
            detalhes.push_back(Aparencia::cor(Cor::CINZA) + "Ainda nao derrotado. Pouco se sabe sobre seus costumes." + Aparencia::cor(Cor::RESET));
        }
        exibirPopupComArteEAguarda("BESTIARIO", info->aparencia, detalhes, nomeInimigo);
    }
}

void exibirRaycaster(Personagem* jogadorAtual) {
    Secao secao = PRINCIPAL;
    int sel = 0;
    int selSub = 0;
    bool executando = true;

    bool emLista = false;
    int idxGrupo = 0;
    std::vector<GrupoCategorizado> grupos;
    std::vector<std::string> listaAtual;

    bool redesenhoCompleto = true;

    while (executando) {
        if (redesenhoCompleto) {
            GerenciadorPerspectiva::obterDiarioUI().renderizarFundo();
        }
        std::cout << "\033[?25l";

        std::vector<std::string> linhas;
        std::string tituloCaixa;
        std::vector<std::string> interativos;

        bool isGroupView = !emLista && (secao == BESTIARIO || secao == ITENS || secao == NPCS || secao == RACAS);

        if (secao == PRINCIPAL) {
            tituloCaixa = " DIARIO DE JORNADA ";
            interativos = {"Bestiario (Inimigos)", "Itens (Descobertos)", "NPCs Conhecidos",
                           "Racas do Mundo", "Classes Jogaveis", "Missoes (Diario)",
                           "Progresso e Feitos", "Voltar"};
            for (size_t i = 0; i < interativos.size(); ++i)
                adicionarOpcao(linhas, interativos[i], (int)i == sel);
        } else if (isGroupView) {
            if (secao == BESTIARIO) {
                tituloCaixa = " BESTIARIO ";
                grupos = TelaDiarioLogic::categorizarBestiario();
            } else if (secao == ITENS) {
                tituloCaixa = " ITENS DESCOBERTOS ";
            } else if (secao == NPCS) {
                tituloCaixa = " NPCS CONHECIDOS ";
                grupos = TelaDiarioLogic::categorizarNPCs();
            } else if (secao == RACAS) {
                tituloCaixa = " RACAS DESCOBERTAS ";
                auto todasRacas = Diario::instancia().obterRacasDescobertas();
                grupos = TelaDiarioLogic::categorizarRacas(todasRacas);
            }

            if (secao == ITENS) {
                auto cats = TelaDiarioLogic::categorizarItens(jogadorAtual);
                struct CatInfo { const char* nome; std::vector<std::string>* lista; };
                CatInfo todasCats[] = {
                    {"Armas", &cats.armas}, {"Escudos", &cats.escudos},
                    {"Armaduras", &cats.armaduras}, {"Consumiveis", &cats.consumiveis},
                    {"Materiais", &cats.materiais}, {"Missoes", &cats.missoes},
                    {"Outros", &cats.outros}
                };
                grupos.clear();
                for (auto& ci : todasCats) {
                    if (!ci.lista->empty())
                        grupos.push_back({ci.nome, *ci.lista});
                }
            }

            if (grupos.empty()) {
                linhas.push_back("   " + Aparencia::cor(Cor::CINZA) + "Nenhum registro encontrado ainda." + Aparencia::cor(Cor::RESET));
                linhas.push_back("");
                interativos.push_back("Voltar");
                adicionarOpcao(linhas, "Voltar", selSub == 0);
            } else {
                for (size_t i = 0; i < grupos.size(); ++i) {
                    interativos.push_back(grupos[i].nome);
                    std::string txt = grupos[i].nome + " (" + std::to_string(grupos[i].itens.size()) + ")";
                    adicionarOpcao(linhas, txt, (int)i == selSub);
                }
                linhas.push_back("");
                interativos.push_back("Voltar");
                adicionarOpcao(linhas, "Voltar", selSub == (int)grupos.size());
            }
        } else {
            if (secao == MISSOES) tituloCaixa = " DIARIO DE MISSOES ";
            else if (secao == PROGRESSO) tituloCaixa = " PROGRESSO E FEITOS ";
            else if (secao == CLASSES) tituloCaixa = " CLASSES JOGAVEIS ";
            else if (secao == BESTIARIO) tituloCaixa = grupos.empty() ? " BESTIARIO " : (" " + grupos[idxGrupo].nome + " ");
            else if (secao == ITENS) tituloCaixa = grupos.empty() ? " ITENS " : (" " + grupos[idxGrupo].nome + " ");
            else if (secao == NPCS) tituloCaixa = grupos.empty() ? " NPCS " : (" " + grupos[idxGrupo].nome + " ");
            else if (secao == RACAS) tituloCaixa = grupos.empty() ? " RACAS " : (" " + grupos[idxGrupo].nome + " ");

            if (secao == PROGRESSO) {
                auto dados = TelaDiarioLogic::obterProgresso();
                if (dados.totalConquistas == 0) {
                    linhas.push_back("   " + Aparencia::cor(Cor::CINZA) + "Nenhum grande feito para registrar ainda..." + Aparencia::cor(Cor::RESET));
                } else {
                    for (const auto& l : dados.linhasNPC) linhas.push_back(l);
                    for (const auto& l : dados.linhasMonstro) linhas.push_back(l);
                    for (const auto& l : dados.linhasItem) linhas.push_back(l);
                    if (!linhas.empty()) linhas.pop_back();
                }
                linhas.push_back("");
                interativos.push_back("Voltar");
                adicionarOpcao(linhas, "Voltar", selSub == 0);
            } else if (secao == MISSOES) {
                auto cats = TelaDiarioLogic::categorizarMissoes(jogadorAtual);
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
                linhas.push_back("");
                interativos.push_back("Voltar");
                adicionarOpcao(linhas, "Voltar", selSub == 0);
            } else {
                if (secao == CLASSES) {
                    listaAtual = TelaDiarioLogic::obterTodasClasses();
                } else if (!grupos.empty() && idxGrupo < (int)grupos.size()) {
                    listaAtual = grupos[idxGrupo].itens;
                }

                if (listaAtual.empty()) {
                    linhas.push_back("   " + Aparencia::cor(Cor::CINZA) + "Nenhum registro encontrado ainda." + Aparencia::cor(Cor::RESET));
                } else {
                    for (size_t i = 0; i < listaAtual.size(); ++i) {
                        interativos.push_back(listaAtual[i]);
                        adicionarOpcao(linhas, listaAtual[i], (int)i == selSub);
                    }
                }
                linhas.push_back("");
                interativos.push_back("Voltar");
                adicionarOpcao(linhas, "Voltar", selSub == (int)listaAtual.size());
            }
        }

        int totalOpcoes = interativos.size();

        if (secao == PRINCIPAL) {
            if (sel >= totalOpcoes && totalOpcoes > 0) sel = totalOpcoes - 1;
        } else {
            if (selSub >= totalOpcoes && totalOpcoes > 0) selSub = totalOpcoes - 1;
        }

        std::vector<std::string> caixaPrevia = TelaBase::criarCaixa(linhas, tituloCaixa, 0, Cor::AMARELO, "\033[48;2;25;25;25m");
        int startYCaixa = (Aparencia::obterAlturaTerminal() - (int)caixaPrevia.size()) / 2;
        if (startYCaixa < MIN_Y) startYCaixa = MIN_Y;
        if (startYCaixa < 8) startYCaixa = 8;

        GerenciadorPerspectiva::obterDiarioUI().exibirCabecalho(startYCaixa);
        GerenciadorPerspectiva::obterDiarioUI().renderizarCaixa(linhas, tituloCaixa, Cor::AMARELO, MIN_Y, startYCaixa);

        redesenhoCompleto = false;
        char tecla = ControleDeInput::lerTecla();

        auto moverCursor = [&](int dir) {
            int& s = (secao == PRINCIPAL) ? sel : selSub;
            s += dir;
            if (s < 0) s = totalOpcoes - 1;
            if (s >= totalOpcoes) s = 0;
        };

        if (tecla == 'w' || tecla == 'W') moverCursor(-1);
        else if (tecla == 's' || tecla == 'S') moverCursor(1);
        else if (tecla == '\n' || tecla == '\r') {
            redesenhoCompleto = true;
            if (secao == PRINCIPAL) {
                switch (sel) {
                    case 0: secao = BESTIARIO; break;
                    case 1: secao = ITENS; break;
                    case 2: secao = NPCS; break;
                    case 3: secao = RACAS; break;
                    case 4: secao = CLASSES; break;
                    case 5: secao = MISSOES; break;
                    case 6: secao = PROGRESSO; break;
                    case 7: executando = false; break;
                }
                emLista = false;
                selSub = 0;
                idxGrupo = 0;
                grupos.clear();
                listaAtual.clear();
            } else if (isGroupView) {
                if (selSub == (int)grupos.size()) {
                    secao = PRINCIPAL;
                    emLista = false;
                } else if (selSub >= 0 && selSub < (int)grupos.size()) {
                    idxGrupo = selSub;
                    emLista = true;
                    selSub = 0;
                }
    } else {
            int ultimoIdx = (int)listaAtual.size();
            if (selSub == ultimoIdx || listaAtual.empty()) {
                if (secao == MISSOES || secao == PROGRESSO || secao == CLASSES) {
                    secao = PRINCIPAL;
                } else {
                    emLista = false;
                    selSub = 0;
                }
                } else if (selSub >= 0 && selSub < ultimoIdx) {
                    std::string itemSel = listaAtual[selSub];
                    if (secao == ITENS) {
                        std::string nomeItem = (itemSel.size() > 3) ? itemSel.substr(3) : itemSel;
                        inspecionarItem(jogadorAtual, nomeItem);
                    } else if (secao == NPCS) {
                        inspecionarNPC(itemSel);
                    } else if (secao == RACAS) {
                        inspecionarRaca(itemSel);
                    } else if (secao == CLASSES) {
                        inspecionarClasse(itemSel);
                    } else if (secao == BESTIARIO) {
                        inspecionarBestiario(itemSel);
                    }
                }
            }
        }
    }
}

} // anonymous namespace

void TelaDiario::exibir(Personagem* jogadorAtual) {
    if (!jogadorAtual) return;

    exibirRaycaster(jogadorAtual);
}
