#include "InventarioCombate.h"
#include "../../Perspectiva/GerenciadorPerspectiva.h"
#include "../../Entidades/Personagem.h"
#include "Item.h"
#include "Equipamentos/EquipamentoArma.h"
#include "Equipamentos/EquipamentoEscudo.h"
#include "Equipamentos/EquipamentoArmadura.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Core/Utilidades/ControleDeInput.h"
#include "../../Perspectiva/TelasBase/TelaBase.h"
#include "../../Perspectiva/TelasBase/Inventario/TelaInventario.h"
#include "../../Perspectiva/TelasBase/Combate/TelaCombate.h"
#include "../../Core/Utilidades/FuncoesDialogo.h"
#include "ControleInventario.h"
#include "../../Perspectiva/PerspectivaRaycaster/EngineRaycaster/RaycasterQuadro.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>

enum EstadoInventario { PRINCIPAL, ARSENAL, CONSUMIVEIS, ESTOQUE, MISSAO };

static int lerSelecaoPopupInventario(const std::string& titulo, const std::vector<std::string>& texto, const std::vector<std::string>& opcoes) {
    int selecaoAtual = 0;
    int totalOpcoes = opcoes.size();
    bool is3D = GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva();
    
    ControleDeInput::limparBuffer();

    while (true) {
        std::vector<std::string> linhas;
        for (const auto& t : texto) {
            linhas.push_back(" " + t + " ");
        }
        linhas.push_back("");
        for (int i = 0; i < totalOpcoes; ++i) {
            if (i == selecaoAtual) {
                linhas.push_back(Aparencia::cor(Cor::VERDE) + " > " + opcoes[i] + Aparencia::cor(Cor::BRANCO) + (is3D ? "\033[48;2;25;25;25m" : ""));
            } else {
                linhas.push_back("   " + opcoes[i]);
            }
        }

        std::vector<std::string> caixaFinal = TelaBase::criarCaixa(linhas, titulo, 0, Cor::AMARELO, is3D ? "\033[48;2;25;25;25m" : "");
        int outW = Aparencia::obterComprimentoVisual(caixaFinal[0]);
        int outH = caixaFinal.size();
        int startX = (Aparencia::obterLarguraTerminal() - outW) / 2;
        int startY = (Aparencia::obterAlturaTerminal() - outH) / 2;
        
        if (startX < 0) startX = 0;
        if (is3D && startY < 8) startY = 8;
        
        std::cout << "\033[?25l";
        for (size_t i = 0; i < caixaFinal.size(); ++i) {
            Aparencia::moverCursor(startX, startY + i);
            std::cout << caixaFinal[i];
        }
        std::cout << std::flush;

        char tecla = ControleDeInput::lerTecla();
        if (tecla == 'w' || tecla == 'W') {
            selecaoAtual--;
            if (selecaoAtual < 0) selecaoAtual = totalOpcoes - 1;
        } else if (tecla == 's' || tecla == 'S') {
            selecaoAtual++;
            if (selecaoAtual >= totalOpcoes) selecaoAtual = 0;
        } else if (tecla == '\r' || tecla == '\n') {
            return selecaoAtual;
        }
    }
}

static void exibirMensagemPopupInventario(const std::string& titulo, const std::vector<std::string>& texto) {
    lerSelecaoPopupInventario(titulo, texto, {"[ VOLTAR ]"});
}

static void exibirResultadoItem(const UsoItemInfo& info, Item* item, bool* turnoFoiConsumido) {
    switch (info.resultado) {
        case ResultadoItem::Erro_TurnoJaUsado:
            exibirMensagemPopupInventario("SISTEMA", {"Voce ja usou um item neste turno!"});
            break;
        case ResultadoItem::Erro_EscudoQuebrado: {
            std::string msg = FuncoesDialogo::formatarMsgSistema("O escudo [" + info.nomeItem + "] esta quebrado e nao pode ser equipado!", Cor::VERMELHO);
            exibirMensagemPopupInventario("SISTEMA", {msg});
            break;
        }
        case ResultadoItem::Erro_Requisitos:
            exibirMensagemPopupInventario("SISTEMA", {info.mensagemExtra});
            break;
        case ResultadoItem::Desequipou:
            exibirMensagemPopupInventario("SISTEMA", {info.nomeItem + " desequipado(a)!"});
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
            exibirMensagemPopupInventario("SISTEMA", {"Turno gasto alterando um equipamento..."});
            break;
        case ResultadoItem::Equipou:
            exibirMensagemPopupInventario("SISTEMA", {info.nomeItem + " equipado(a)!"});
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
            exibirMensagemPopupInventario("SISTEMA", {"Turno gasto alterando um equipamento..."});
            break;
        case ResultadoItem::Usou_Turno:
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
            break;
        case ResultadoItem::Usou_SemTurno:
            break;
        case ResultadoItem::Erro_NaoPodeUsar:
            exibirMensagemPopupInventario("SISTEMA", {ControleInventario::obterMensagemErro(item, turnoFoiConsumido != nullptr)});
            break;
        default: break;
    }
}

static int lerInteiroPopupInventario(const std::string& titulo, const std::string& mensagem, int min, int max) {
    bool is3D = GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva();
    std::string currentInput = "";

    while (true) {
        std::vector<std::string> linhas;
        linhas.push_back(" " + mensagem + " ");
        linhas.push_back("");
        linhas.push_back(Aparencia::cor(Cor::AMARELO) + " >> " + currentInput + "_" + Aparencia::cor(Cor::BRANCO) + (is3D ? "\033[48;2;25;25;25m" : ""));
        linhas.push_back("");
        linhas.push_back(" [ENTER para confirmar]");

        std::vector<std::string> caixaFinal = TelaBase::criarCaixa(linhas, titulo, 0, Cor::AMARELO, is3D ? "\033[48;2;25;25;25m" : "");
        int outW = Aparencia::obterComprimentoVisual(caixaFinal[0]);
        int outH = caixaFinal.size();
        int startX = (Aparencia::obterLarguraTerminal() - outW) / 2;
        int startY = (Aparencia::obterAlturaTerminal() - outH) / 2;
        
        if (startX < 0) startX = 0;
        if (is3D && startY < 8) startY = 8;
        
        std::cout << "\033[?25l";
        for (size_t i = 0; i < caixaFinal.size(); ++i) {
            Aparencia::moverCursor(startX, startY + i);
            std::cout << caixaFinal[i];
        }
        std::cout << std::flush;

        char tecla = ControleDeInput::lerTecla();
        if (tecla >= '0' && tecla <= '9') {
            currentInput += tecla;
        } else if (tecla == '\b' && !currentInput.empty()) {
            currentInput.pop_back();
            if (is3D) RaycasterQuadro::restaurarUltimoQuadro();
            else Aparencia::limparTela();
        } else if (tecla == '\r' || tecla == '\n') {
            if (currentInput.empty()) return min;
            int val = std::stoi(currentInput);
            if (val < min) return min;
            if (val > max) return max;
            return val;
        }
    }
}

void InventarioCombate::gerenciarInventario(Personagem* jogadorAtual, bool* turnoFoiConsumido)
{
    if (jogadorAtual == nullptr) return;
    
    EstadoInventario estado = PRINCIPAL;
    int selecaoAtual = 0;
    int selecaoSub = 0;
    bool executando = true;
    
    std::vector<Item*> mapIndexParaItem;

    bool redesenhoCompletoInv = true;

    while (executando) {
        std::cout << "\033[?25l";
        bool is3D = GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva();
        
        std::vector<std::string> linhas;
        std::string tituloCaixa = "";
        std::vector<std::string> interativos;
        std::vector<int> indicesReais;
        
        if (estado == PRINCIPAL) {
            tituloCaixa = " MENU DE BOLSOS ";
            std::string strBolso = "BOLSO: " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + " Moedas de Ouro [$$]";
            
            std::vector<std::string> opcoesBase;
            if (jogadorAtual->obterConsumivelRapido()) {
                int qtd = jogadorAtual->obterInventario()->contarItem(jogadorAtual->obterConsumivelRapido()->obterNomeItem());
                opcoesBase.push_back(Aparencia::cor(Cor::VERDE) + "[+] " + Aparencia::cor(Cor::BRANCO) + "Acesso Rapido: " + jogadorAtual->obterConsumivelRapido()->obterNomeItem() + " (" + std::to_string(qtd) + "x)");
            }
            opcoesBase.push_back("Arsenal de Equipamentos");
            opcoesBase.push_back("Itens Consumiveis");
            opcoesBase.push_back("Estoque e Materiais");
            opcoesBase.push_back("Itens de Missao");
            opcoesBase.push_back("");
            opcoesBase.push_back(strBolso);
            opcoesBase.push_back("");
            opcoesBase.push_back("[<] VOLTAR");
            
            for (size_t i = 0; i < opcoesBase.size(); ++i) {
                if (opcoesBase[i].empty() || opcoesBase[i].find("BOLSO:") != std::string::npos || opcoesBase[i].substr(0, 3) == "   ") {
                    linhas.push_back("   " + opcoesBase[i]);
                } else {
                    interativos.push_back(opcoesBase[i]);
                    indicesReais.push_back(i);
                    if (interativos.size() - 1 == selecaoAtual) {
                        linhas.push_back(Aparencia::cor(Cor::VERDE) + " > " + opcoesBase[i] + Aparencia::cor(Cor::BRANCO) + (is3D ? "\033[48;2;25;25;25m" : ""));
                    } else {
                        linhas.push_back("   " + opcoesBase[i]);
                    }
                }
            }
        } else if (estado == ARSENAL) {
            tituloCaixa = " ARSENAL DE EQUIPAMENTOS ";

            mapIndexParaItem.clear();

            Item* armaEq = jogadorAtual->obterArma();
            Item* armaduraEq = jogadorAtual->obterArmadura();
            Item* escudoEq = jogadorAtual->obterEscudo();

            auto todosItens = jogadorAtual->obterInventario()->obterTodosOsItens();
            std::vector<Item*> armas, armaduras, escudos;
            for (auto* item : todosItens) {
                if (item == armaEq || item == armaduraEq || item == escudoEq) continue;
                TipoEquipamento tipo = item->obterTipo();
                if (tipo == TipoEquipamento::ARMA) armas.push_back(item);
                else if (tipo == TipoEquipamento::ARMADURA) armaduras.push_back(item);
                else if (tipo == TipoEquipamento::ESCUDO) escudos.push_back(item);
            }

            Aparencia::ordenarAlfabeticamente(armas, [](Item* a) { return a->obterNomeItem(); });
            Aparencia::ordenarAlfabeticamente(armaduras, [](Item* a) { return a->obterNomeItem(); });
            Aparencia::ordenarAlfabeticamente(escudos, [](Item* a) { return a->obterNomeItem(); });

            std::string corDiv = Aparencia::cor(Cor::AMARELO);
            std::string corReset = Aparencia::cor(Cor::RESET);

            auto adicionarItem = [&](const std::string& nome, Item* item) {
                int idx = (int)interativos.size();
                interativos.push_back(nome);
                indicesReais.push_back((int)mapIndexParaItem.size());
                mapIndexParaItem.push_back(item);
                if (idx == selecaoSub)
                    linhas.push_back(Aparencia::cor(Cor::VERDE) + " > " + nome + Aparencia::cor(Cor::BRANCO) + (is3D ? "\033[48;2;25;25;25m" : ""));
                else
                    linhas.push_back("   " + nome);
            };

            auto adicionarGrupo = [&](const std::string& label, std::vector<Item*>& grupo) {
                if (grupo.empty()) return;
                linhas.push_back(" " + corDiv + "--- " + label + " ---" + corReset);
                for (auto* item : grupo) {
                    auto itensAgrupados = jogadorAtual->obterInventario()->contarItem(item->obterNomeItem());
                    std::string prefixo = (itensAgrupados > 1) ? std::to_string(itensAgrupados) + "x " : "";
                    adicionarItem(prefixo + item->obterNomeItem(), item);
                }
                linhas.push_back("");
            };

            // Equipados (non-interactive)
            linhas.push_back(" " + corDiv + "--- Equipados ---" + corReset);
            bool temEq = false;
            auto addEq = [&](const std::string& label, Item* item) {
                if (!item) return;
                temEq = true;
                std::string nome = item->obterNomeItem();
                // also add to interativos so it can be selected
                int idx = (int)interativos.size();
                interativos.push_back("(E) " + nome);
                indicesReais.push_back((int)mapIndexParaItem.size());
                mapIndexParaItem.push_back(item);
                if (idx == selecaoSub)
                    linhas.push_back(Aparencia::cor(Cor::VERDE) + " > " + Aparencia::cor(Cor::VERDE) + "[E] " + Aparencia::cor(Cor::RESET) + label + ": " + nome + Aparencia::cor(Cor::BRANCO) + (is3D ? "\033[48;2;25;25;25m" : ""));
                else
                    linhas.push_back("   " + Aparencia::cor(Cor::VERDE) + "[E] " + Aparencia::cor(Cor::RESET) + label + ": " + nome);
            };
            addEq("Arma", armaEq);
            addEq("Armadura", armaduraEq);
            addEq("Escudo", escudoEq);
            if (!temEq) linhas.push_back("   " + Aparencia::cor(Cor::CINZA) + "(Nada equipado)" + corReset);
            linhas.push_back("");

            adicionarGrupo("Armas", armas);
            adicionarGrupo("Armaduras", armaduras);
            adicionarGrupo("Escudos", escudos);

            interativos.push_back("[<] VOLTAR");
            indicesReais.push_back(-1);
            if ((int)interativos.size() - 1 == selecaoSub)
                linhas.push_back(Aparencia::cor(Cor::VERDE) + " > [<] VOLTAR" + Aparencia::cor(Cor::BRANCO) + (is3D ? "\033[48;2;25;25;25m" : ""));
            else
                linhas.push_back("   [<] VOLTAR");

        } else {
            int categoria = 0;
            if (estado == CONSUMIVEIS) { tituloCaixa = " ITENS CONSUMIVEIS "; categoria = 1; }
            else if (estado == ESTOQUE) { tituloCaixa = " ESTOQUE E MATERIAIS "; categoria = 2; }
            else if (estado == MISSAO) { tituloCaixa = " ITENS DE MISSAO "; categoria = 3; }

            auto itens = TelaInventario::obterListaCategoria(jogadorAtual, categoria, false);
            Aparencia::ordenarAlfabeticamente(itens, [](const auto& par) { return par.first; });

            mapIndexParaItem.clear();

            if (!itens.empty()) {
                for (const auto& p : itens) {
                    interativos.push_back(p.first);
                    indicesReais.push_back((int)mapIndexParaItem.size());
                    mapIndexParaItem.push_back(p.second);

                    if ((int)interativos.size() - 1 == selecaoSub) {
                        linhas.push_back(Aparencia::cor(Cor::VERDE) + " > " + p.first + Aparencia::cor(Cor::BRANCO) + (is3D ? "\033[48;2;25;25;25m" : ""));
                    } else {
                        linhas.push_back("   " + p.first);
                    }
                }
            } else {
                linhas.push_back("   " + Aparencia::cor(Cor::CINZA) + "Nenhum item nesta categoria." + Aparencia::cor(Cor::RESET));
            }
            linhas.push_back("");

            interativos.push_back("[<] VOLTAR");
            indicesReais.push_back(-1);
            if ((int)interativos.size() - 1 == selecaoSub) {
                linhas.push_back(Aparencia::cor(Cor::VERDE) + " > [<] VOLTAR" + Aparencia::cor(Cor::BRANCO) + (is3D ? "\033[48;2;25;25;25m" : ""));
            } else {
                linhas.push_back("   [<] VOLTAR");
            }
        }
        
        int totalOpcoes = interativos.size();
        int* selRef = (estado == PRINCIPAL) ? &selecaoAtual : &selecaoSub;
        if (*selRef >= totalOpcoes && totalOpcoes > 0) *selRef = totalOpcoes - 1;
        
        std::vector<std::string> caixaFinal = TelaBase::criarCaixa(linhas, tituloCaixa, 0, Cor::AMARELO, is3D ? "\033[48;2;25;25;25m" : "");
        int outW = Aparencia::obterComprimentoVisual(caixaFinal[0]);
        int outH = caixaFinal.size();
        
        if (is3D) {
            if (redesenhoCompletoInv) RaycasterQuadro::restaurarUltimoQuadro();
            int startX = (Aparencia::obterLarguraTerminal() - outW) / 2;
            int startY = (Aparencia::obterAlturaTerminal() - outH) / 2;
            if (startX < 0) startX = 0;
            if (startY < 8) startY = 8;
            
            GerenciadorPerspectiva::obterInventarioUI().exibirCabecalho(false, startY);
            
            for (size_t i = 0; i < caixaFinal.size(); ++i) {
                Aparencia::moverCursor(startX, startY + i);
                std::cout << caixaFinal[i];
            }
        } else {
            Aparencia::limparTela();
            TelaInventario::exibirCabecalhoInventario(false);
            TelaInventario::exibirCaixaEquipados(jogadorAtual);
            std::cout << "\n";
            for (const auto& l : caixaFinal) {
                std::cout << Aparencia::espacosParaCentralizar(Aparencia::obterComprimentoVisual(l)) << l << "\n";
            }
        }
        std::cout << std::flush;
        
        redesenhoCompletoInv = false;
        char tecla = ControleDeInput::lerTecla();
        if (tecla == 'w' || tecla == 'W') {
            (*selRef)--;
            if (*selRef < 0) *selRef = totalOpcoes - 1;
        } else if (tecla == 's' || tecla == 'S') {
            (*selRef)++;
            if (*selRef >= totalOpcoes) *selRef = 0;
        } else if (tecla == '\n' || tecla == '\r') {
            redesenhoCompletoInv = true;
            if (totalOpcoes > 0) {
                if (estado == PRINCIPAL) {
                    int offset = jogadorAtual->obterConsumivelRapido() ? 1 : 0;
                    int escLogica = indicesReais[*selRef];
                    
                    if (escLogica == 7 + offset) {
                        executando = false;
                    } else if (offset == 1 && escLogica == 0) {
                        // Consumivel rapido
                        Item* rapido = jogadorAtual->obterConsumivelRapido();
                        std::string nomeRapido = rapido->obterNomeItem();
                        int countAntes = jogadorAtual->obterInventario()->contarItem(nomeRapido);
                        if (countAntes > 0) {
                            bool turnoJaUsado = turnoFoiConsumido && *turnoFoiConsumido;
                            UsoItemInfo info = ControleInventario::usarOuEquipar(jogadorAtual, rapido, turnoJaUsado);
                            if (turnoFoiConsumido && info.consumiuTurno) *turnoFoiConsumido = true;
                            if (jogadorAtual->obterItemSelecionadoParaUso() != nullptr) {
                                executando = false;
                            }
                            if (jogadorAtual->obterInventario()->contarItem(nomeRapido) == 0) {
                                jogadorAtual->desequiparConsumivel();
                            }
                        } else {
                            jogadorAtual->desequiparConsumivel();
                        }
                        if (turnoFoiConsumido && *turnoFoiConsumido) executando = false;
                        
                        if (is3D) RaycasterQuadro::restaurarUltimoQuadro();
                    } else {
                        int cat = escLogica - offset;
                        if (cat == 0) estado = ARSENAL;
                        else if (cat == 1) estado = CONSUMIVEIS;
                        else if (cat == 2) estado = ESTOQUE;
                        else if (cat == 3) estado = MISSAO;
                        selecaoSub = 0;
                        if (is3D) RaycasterQuadro::restaurarUltimoQuadro();
                    }
                } else {
                    int idx = indicesReais[*selRef];
                    if (idx == -1) {
                        estado = PRINCIPAL;
                        if (is3D) RaycasterQuadro::restaurarUltimoQuadro();
                    } else {
                        Item* itemEncontrado = mapIndexParaItem[idx];
                        bool ehEquipavel = itemEncontrado->isEquipavel();
                        
                        bool submenuAberto = true;
                        while(submenuAberto) {
                            int subOpcao = lerSelecaoPopupInventario(
                                "OPCOES DE ITEM", 
                                {"O que deseja fazer com:", Aparencia::cor(Cor::AMARELO) + ">> " + itemEncontrado->obterNomeItem() + " <<" + Aparencia::cor(Cor::RESET)}, 
                                {"Usar / Equipar", "Inspecionar", "[ VOLTAR ]"}
                            );
                            
                            if (subOpcao == 2) { // VOLTAR
                                submenuAberto = false;
                                if (is3D) RaycasterQuadro::restaurarUltimoQuadro();
                                break;
                            } else if (subOpcao == 0) { // Usar / Equipar
                                bool turnoJaUsado = turnoFoiConsumido && *turnoFoiConsumido;
                                if (ehEquipavel) {
                                    UsoItemInfo info = ControleInventario::usarOuEquipar(jogadorAtual, itemEncontrado, turnoJaUsado);
                                    exibirResultadoItem(info, itemEncontrado, turnoFoiConsumido);
                                } else {
                                    int qtdDisponivel = jogadorAtual->obterInventario()->contarItem(itemEncontrado->obterNomeItem());
                                    int quantidadeParaUsar = 1;
                                    
                                    if (qtdDisponivel > 1) {
                                        int escolhaQtd = lerSelecaoPopupInventario(
                                            "QUANTIDADE: " + itemEncontrado->obterNomeItem(),
                                            {"Voce possui " + std::to_string(qtdDisponivel) + " unidades deste item."},
                                            {"Usar UMA unidade", "Usar TODAS as unidades", "Usar quantidade ESPECIFICA", "[ CANCELAR ]"}
                                        );
                                        
                                        if (escolhaQtd == 0) {
                                            quantidadeParaUsar = 1;
                                        } else if (escolhaQtd == 1) {
                                            quantidadeParaUsar = qtdDisponivel;
                                        } else if (escolhaQtd == 2) {
                                            std::string msgQtd = "Quantidade (1 a " + std::to_string(qtdDisponivel) + ", 0 cancelar): ";
                                            quantidadeParaUsar = lerInteiroPopupInventario("QUANTIDADE", msgQtd, 0, qtdDisponivel);
                                        } else {
                                            if (is3D) RaycasterQuadro::restaurarUltimoQuadro();
                                            continue; 
                                        }
                                    }
                                    
                                    if (quantidadeParaUsar <= 0) {
                                        if (is3D) RaycasterQuadro::restaurarUltimoQuadro();
                                        continue;
                                    }
                                    
                                    std::string nomeItem = itemEncontrado->obterNomeItem();
                                    int countAntes = jogadorAtual->obterInventario()->contarItem(nomeItem);
                                    bool consumiuAlgumTurno = false;
                                    
                                    for (int i = 0; i < quantidadeParaUsar; ++i) {
                                        turnoJaUsado = turnoFoiConsumido && *turnoFoiConsumido;
                                        UsoItemInfo info = ControleInventario::usarOuEquipar(jogadorAtual, itemEncontrado, turnoJaUsado);
                                        if (info.consumiuTurno) consumiuAlgumTurno = true;
                                        
                                        if (jogadorAtual->obterItemSelecionadoParaUso() != nullptr) {
                                            if (quantidadeParaUsar > 1) {
                                                exibirMensagemPopupInventario("SISTEMA", {"Este item requer selecao de alvo e", "sera usado apenas uma vez."});
                                            }
                                            break;
                                        }
                                        
                                        int countDepois = jogadorAtual->obterInventario()->contarItem(nomeItem);
                                        if (countDepois == countAntes && !ehEquipavel) {
                                            break;
                                        }
                                    }
                                    
                                    if (jogadorAtual->obterConsumivelRapido() && jogadorAtual->obterInventario()->contarItem(jogadorAtual->obterConsumivelRapido()->obterNomeItem()) == 0) {
                                        jogadorAtual->desequiparConsumivel();
                                    }

                                    if (turnoFoiConsumido && consumiuAlgumTurno) {
                                        *turnoFoiConsumido = true;
                                    }
                                }
                                submenuAberto = false; 
                                if (is3D) RaycasterQuadro::restaurarUltimoQuadro();
                            } else if (subOpcao == 1) { // Inspecionar
                                std::vector<std::string> detalhes = itemEncontrado->obterDetalhesInspecao(jogadorAtual);
                                std::vector<std::string> linhasInsp;
                                linhasInsp.push_back(Aparencia::cor(Cor::AMARELO) + " >> " + itemEncontrado->obterNomeItem() + " <<" + Aparencia::cor(Cor::RESET));
                                linhasInsp.push_back("");
                                linhasInsp.insert(linhasInsp.end(), detalhes.begin(), detalhes.end());
                                
                                exibirMensagemPopupInventario("INSPECAO DE ITEM", linhasInsp);
                                if (is3D) RaycasterQuadro::restaurarUltimoQuadro();
                            }
                        }
                        
                        if (turnoFoiConsumido && *turnoFoiConsumido) executando = false;
                        if (is3D) RaycasterQuadro::restaurarUltimoQuadro();
                    }
                }
            }
        }
    }
}


