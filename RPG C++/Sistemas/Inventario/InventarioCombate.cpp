#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "InventarioCombate.h"
#include "../../Entidades/Personagem.h"
#include "Item.h"
#include "Equipamentos/EquipamentoArma.h"
#include "Equipamentos/EquipamentoEscudo.h"
#include "Equipamentos/EquipamentoArmadura.h"
#include "../../Interface/Telas/Inventario/TelaInventario.h"
#include "../../Interface/Telas/Menu/TelaMenu.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Core/Utilidades/ControleDeInput.h"
#include "../../Interface/Telas/TelaBase.h"
#include "../../Core/Utilidades/FuncoesDialogo.h"

namespace {
}

void InventarioCombate::gerenciarInventario(Personagem* jogadorAtual, bool* turnoFoiConsumido)
{
    if (jogadorAtual == nullptr) return;
    TelaBase::executarLoop(
        [](bool animar) { TelaInventario::exibirCabecalhoInventario(animar); },
        [jogadorAtual]() {
            TelaInventario::exibirCaixaEquipados(jogadorAtual);
            std::cout << "\n";
            Aparencia::imprimirCentralizado("DINHEIRO: " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + " moedas", Aparencia::cor(Cor::AMARELO));
            std::cout << "\n";
        },
        [jogadorAtual]() {
            std::vector<std::string> opcoes;
            if (jogadorAtual->obterConsumivelRapido()) {
                int qtd = jogadorAtual->obterInventario()->contarItem(jogadorAtual->obterConsumivelRapido()->obterNomeItem());
                opcoes.push_back("Acesso Rapido: " + jogadorAtual->obterConsumivelRapido()->obterNomeItem() + " (" + std::to_string(qtd) + "x)");
            }
            opcoes.push_back("Arsenal de Equipamentos");
            opcoes.push_back("Itens Consumiveis");
            opcoes.push_back("Estoque e Materiais");
            opcoes.push_back("Itens de Missao");
            opcoes.push_back("VOLTAR");
            return opcoes;
        },
        [&](int escolha) {
            int offset = jogadorAtual->obterConsumivelRapido() ? 1 : 0;
            if (escolha < 0 || escolha == 4 + offset) return false; // VOLTAR
            
            if (offset == 1 && escolha == 0) {
                Item* rapido = jogadorAtual->obterConsumivelRapido();
                int countAntes = jogadorAtual->obterInventario()->contarItem(rapido->obterNomeItem());
                if (countAntes > 0) {
                    bool consumiu = false;
                    processarUsoDeItem(jogadorAtual, rapido, turnoFoiConsumido ? &consumiu : nullptr);
                    if (consumiu && turnoFoiConsumido) *turnoFoiConsumido = true;
                    
                    if (jogadorAtual->obterItemSelecionadoParaUso() != nullptr) {
                        return false; 
                    }
                    if (jogadorAtual->obterInventario()->contarItem(rapido->obterNomeItem()) == 0) {
                        jogadorAtual->desequiparConsumivel();
                    }
                } else {
                    jogadorAtual->desequiparConsumivel();
                }
                if (turnoFoiConsumido && *turnoFoiConsumido) return false;
                return true;
            }

            int categoria = escolha - offset;
            
            TelaBase::executarLoop(
                [](bool animar) { TelaInventario::exibirCabecalhoInventario(animar); },
                [categoria]() {
                    std::string titulo = "";
                    if (categoria == 0) titulo = "ARSENAL DE EQUIPAMENTOS";
                    else if (categoria == 1) titulo = "ITENS CONSUMIVEIS";
                    else if (categoria == 2) titulo = "ESTOQUE E MATERIAIS";
                    else if (categoria == 3) titulo = "ITENS DE MISSAO";
                    std::cout << "\n";
                    Aparencia::imprimirCentralizado(titulo, Aparencia::cor(Cor::CIANO));
                    std::cout << "\n";
                },
                [jogadorAtual, categoria]() {
                    auto itens = TelaInventario::obterListaCategoria(jogadorAtual, categoria, false);
                    std::vector<std::string> ops;
                    for(auto& p : itens) ops.push_back(p.first);
                    ops.push_back("VOLTAR");
                    return ops;
                },
                [&](int escolhaItem) {
                    auto itens = TelaInventario::obterListaCategoria(jogadorAtual, categoria, false);
                    if (escolhaItem < 0 || escolhaItem >= static_cast<int>(itens.size())) return false; // VOLTAR

                    Item* itemEncontrado = itens[escolhaItem].second;
                    
                    TelaBase::executarLoopPadrao(
                        "OPCOES DE ITEM", Cor::AMARELO,
                        [itemEncontrado]() {
                            std::cout << "\n";
                            Aparencia::imprimirCentralizado("Item Selecionado: " + Aparencia::cor(Cor::CIANO) + itemEncontrado->obterNomeItem() + Aparencia::cor(Cor::RESET));
                            std::cout << "\n";
                        },
                        [itemEncontrado]() {
                            TipoEquipamento tipo = itemEncontrado->obterTipo();
                            std::vector<std::string> ops;
                            if (tipo == TipoEquipamento::ARMA || tipo == TipoEquipamento::ESCUDO || tipo == TipoEquipamento::ARMADURA) {
                                ops = {"Equipar / Desequipar", "Inspecionar (Dano, Durabilidade, Requisitos, etc)"};
                            } else if (tipo == TipoEquipamento::MISSAO) {
                                ops = {"Usar em quantidade", "Inspecionar (Lore)"};
                            } else if (tipo == TipoEquipamento::CONSUMIVEL) {
                                ops = {"Usar em quantidade", "Equipar no Acesso Rapido", "Inspecionar Efeitos"};
                            } else {
                                ops = {"Usar", "Inspecionar Detalhes"};
                            }
                            ops.push_back("Cancelar");
                            return ops;
                        },
                        [&](int subOpcao) {
                            TipoEquipamento tipo = itemEncontrado->obterTipo();
                            
                            if (subOpcao == 0) {
                                int quantidadeParaUsar = 1;
                                if (tipo == TipoEquipamento::CONSUMIVEL || tipo == TipoEquipamento::MISSAO) {
                                    int qtdDisponivel = jogadorAtual->obterInventario()->contarItem(itemEncontrado->obterNomeItem());
                                    if (qtdDisponivel > 1 && turnoFoiConsumido == nullptr) {
                                        std::string msgQtd = "Quantidade para usar (1 a " + std::to_string(qtdDisponivel) + ", 0 cancelar): ";
                                        quantidadeParaUsar = ControleDeInput::lerInteiroComLimites(msgQtd, 0, qtdDisponivel, true);
                                    }
                                }
                                
                                if (quantidadeParaUsar == 0) return true; // Cancelado

                                std::string nomeItem = itemEncontrado->obterNomeItem();
                                bool consumiuAlgumTurno = false;
                                
                                for (int i = 0; i < quantidadeParaUsar; ++i) {
                                    Item* itemAtual = nullptr;
                                    for (auto* it : jogadorAtual->obterInventario()->obterTodosOsItens()) {
                                        if (it->obterNomeItem() == nomeItem && !jogadorAtual->isItemEquipado(it)) {
                                            itemAtual = it;
                                            break;
                                        }
                                    }
                                    if (!itemAtual && jogadorAtual->isItemEquipado(itemEncontrado)) {
                                        itemAtual = itemEncontrado;
                                    }
                                    
                                    if (itemAtual) {
                                        int countAntes = jogadorAtual->obterInventario()->contarItem(nomeItem);
                                        bool turnoLocal = false;
                                        processarUsoDeItem(jogadorAtual, itemAtual, turnoFoiConsumido ? &turnoLocal : nullptr);
                                        if (turnoLocal) consumiuAlgumTurno = true;
                                        
                                        if (jogadorAtual->obterItemSelecionadoParaUso() != nullptr) {
                                            if (quantidadeParaUsar > 1) {
                                                std::cout << "\n" << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgSistema("Este item requer selecao de alvo e sera usado apenas uma vez.", Cor::AMARELO) << "\n";
                                                ControleDeInput::aguardarEnter();
                                            }
                                            break;
                                        }
                                        
                                        int countDepois = jogadorAtual->obterInventario()->contarItem(nomeItem);
                                        if (countDepois == countAntes && !itemAtual->isEquipavel()) {
                                            break;
                                        }
                                    }
                                }
                                
                                if (jogadorAtual->obterConsumivelRapido() && jogadorAtual->obterInventario()->contarItem(jogadorAtual->obterConsumivelRapido()->obterNomeItem()) == 0) {
                                    jogadorAtual->desequiparConsumivel();
                                }

                                if (turnoFoiConsumido && consumiuAlgumTurno) {
                                    *turnoFoiConsumido = true;
                                }
                                return false; // Sai do Submenu apos o uso
                            } else if (subOpcao == 1 && tipo == TipoEquipamento::CONSUMIVEL) {
                                if (jogadorAtual->obterConsumivelRapido() == itemEncontrado) {
                                    jogadorAtual->desequiparConsumivel();
                                    std::cout << "\n" << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgSistema(itemEncontrado->obterNomeItem() + " desequipado do acesso rapido!", Cor::AMARELO) << "\n";
                                } else {
                                    jogadorAtual->equiparItem(itemEncontrado);
                                    std::cout << "\n" << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgSistema(itemEncontrado->obterNomeItem() + " equipado no acesso rapido!", Cor::VERDE) << "\n";
                                }
                                ControleDeInput::aguardarEnter();
                                return false;
                            } else if ((subOpcao == 1 && tipo != TipoEquipamento::CONSUMIVEL) || (subOpcao == 2 && tipo == TipoEquipamento::CONSUMIVEL)) {
                                Aparencia::limparTela();
                                Aparencia::exibirPainelTexto("INSPECAO DE ITEM", Cor::AMARELO);
                                TelaInventario::exibirInspecaoItem(itemEncontrado, jogadorAtual);
                                std::cout << "\n";
                                ControleDeInput::aguardarEnter();
                                return true;
                            }
                            return false; // Cancelar
                        }
                    );

                    if (turnoFoiConsumido && *turnoFoiConsumido) return false;
                    return true;
                }
            );
            if (turnoFoiConsumido && *turnoFoiConsumido) {
                return false;
            }
            return true;
        }
    );
}

void InventarioCombate::processarUsoDeItem(Personagem* jogadorAtual, Item* itemEncontrado, bool* turnoFoiConsumido)
{
    if (turnoFoiConsumido && *turnoFoiConsumido) 
    {
        std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Voce ja usou um item neste turno!\n";
        ControleDeInput::aguardarEnter();
        return;
    }

    if (itemEncontrado->isEquipavel())
    {
        bool desequipou = false;
        if (itemEncontrado == jogadorAtual->obterArma()) {
            jogadorAtual->desequiparArma();
            desequipou = true;
        } else if (itemEncontrado == jogadorAtual->obterEscudo()) {
            jogadorAtual->desequiparEscudo();
            desequipou = true;
        } else if (itemEncontrado == jogadorAtual->obterArmadura()) {
            jogadorAtual->desequiparArmadura();
            desequipou = true;
        }
        
        if (desequipou) {
            std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: " << itemEncontrado->obterNomeItem() << " desequipado(a)!\n";
        } else {
            if (!itemEncontrado->podeSerEquipadoPor(jogadorAtual)) {
                std::string msg = itemEncontrado->obterMensagemRequisito();
                if (msg.substr(0, 1) == "\n") std::cout << "\n" << Aparencia::margemCombate() << msg.substr(1);
                else std::cout << Aparencia::margemCombate() << msg;
                if (turnoFoiConsumido && !jogadorAtual->obterItemSelecionadoParaUso()) ControleDeInput::aguardarEnter();
                return;
            }
            jogadorAtual->equiparItem(itemEncontrado);
            std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: " << itemEncontrado->obterNomeItem() << " equipado(a)!\n";
        }

        if (turnoFoiConsumido) {
            *turnoFoiConsumido = true;
            std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Turno gasto alterando um equipamento...\n";
        }
        ControleDeInput::aguardarEnter();
        return;
    }
    
    if (itemEncontrado->usarDoInventario(jogadorAtual, turnoFoiConsumido))
    {
        // Se o item for Debuff (que seta o ponteiro para pedir alvo), ignorar enter
        if (!jogadorAtual->obterItemSelecionadoParaUso()) {
            ControleDeInput::aguardarEnter();
        }
        return;
    }

    std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Este item nao pode ser usado " << (turnoFoiConsumido ? "em combate!" : "fora de combate!") << "\n";
    ControleDeInput::aguardarEnter();
}
