#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "InventarioCombate.h"
#include "../Sistemas/SistemaPersonagem.h"
#include "Item.h"
#include "EquipamentoArma.h"
#include "EquipamentoEscudo.h"
#include "EquipamentoArmadura.h"
#include "../Telas/TelaInventario.h"
#include "../Telas/TelaMenu.h"
#include "../Utilidades/Aparencia.h"
#include "../Utilidades/ControleDeInput.h"

namespace {
}

void InventarioCombate::gerenciarInventario(SistemaPersonagem* jogadorAtual, bool* turnoFoiConsumido)
{
    if (jogadorAtual == nullptr) return;
    std::string codigoDoItemDigitado;
    do 
    {
        TelaInventario::exibir(jogadorAtual);
        Aparencia::exibirPrompt("Digite o codigo do item para interagir ou [0] VOLTAR: \033[s");

        Item* itemEncontrado = TelaInventario::lerSelecaoDeItem(jogadorAtual, codigoDoItemDigitado);

        if (codigoDoItemDigitado != "0" && itemEncontrado)
        {
                    bool acaoConcluida = false;
                    while (!acaoConcluida) {
                        TelaInventario::exibirMenuInteracaoItem(itemEncontrado);
                        std::string subOpcao;
                        while (true) {
                            subOpcao = ControleDeInput::lerEntradaProtegida();
                            if (subOpcao == "0" || subOpcao == "1" || subOpcao == "2") break;
                            std::cout << "\033[u\033[J";
                        }

                        if (subOpcao == "1") {
                            TipoEquipamento tipo = itemEncontrado->obterTipo();
                            int quantidadeParaUsar = 1;
                            
                            if (tipo == TipoEquipamento::CONSUMIVEL || tipo == TipoEquipamento::MISSAO) {
                                int qtdDisponivel = jogadorAtual->obterInventario()->contarItem(itemEncontrado->obterNomeItem());
                                // Apenas permite escolher a quantidade se NÃO estiver em combate
                                if (qtdDisponivel > 1 && turnoFoiConsumido == nullptr) {
                                    std::string msgQtd = "Quantidade para usar (1 a " + std::to_string(qtdDisponivel) + "): ";
                                    quantidadeParaUsar = ControleDeInput::lerInteiroComLimites(msgQtd, 1, qtdDisponivel, true);
                                }
                            }
                            
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
                                
                                if (itemAtual) {
                                    int countAntes = jogadorAtual->obterInventario()->contarItem(nomeItem);
                                    bool turnoLocal = false;
                                    processarUsoDeItem(jogadorAtual, itemAtual, turnoFoiConsumido ? &turnoLocal : nullptr);
                                    if (turnoLocal) consumiuAlgumTurno = true;
                                    
                                    if (jogadorAtual->obterItemSelecionadoParaUso() != nullptr) {
                                        if (quantidadeParaUsar > 1) {
                                            std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Este item requer selecao de alvo e sera usado apenas uma vez.\n";
                                            Aparencia::aguardarEnter();
                                        }
                                        break;
                                    }
                                    
                                    int countDepois = jogadorAtual->obterInventario()->contarItem(nomeItem);
                                    if (countDepois == countAntes) {
                                        break;
                                    }
                                }
                            }
                            
                            if (turnoFoiConsumido && consumiuAlgumTurno) {
                                *turnoFoiConsumido = true;
                            }
                            acaoConcluida = true;
                        } else if (subOpcao == "2") {
                            Aparencia::limparTela();
                            Aparencia::exibirCabecalho("INSPECAO DE ITEM", Cor::AMARELO);
                            TelaInventario::exibirInspecaoItem(itemEncontrado);
                            std::cout << "\n";
                            Aparencia::aguardarEnter();
                        } else if (subOpcao == "0") {
                            acaoConcluida = true;
                        }
                    }

                    if (turnoFoiConsumido && *turnoFoiConsumido) {
                        break;
                    }
        }
    } while (codigoDoItemDigitado != "0" && !(turnoFoiConsumido && *turnoFoiConsumido));
}

void InventarioCombate::processarUsoDeItem(SistemaPersonagem* jogadorAtual, Item* itemEncontrado, bool* turnoFoiConsumido)
{
    if (turnoFoiConsumido && *turnoFoiConsumido) 
    {
        std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Voce ja usou um item neste turno!\n";
        Aparencia::aguardarEnter();
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
                if (turnoFoiConsumido && !jogadorAtual->obterItemSelecionadoParaUso()) Aparencia::aguardarEnter();
                return;
            }
            jogadorAtual->equiparItem(itemEncontrado);
            std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: " << itemEncontrado->obterNomeItem() << " equipado(a)!\n";
        }

        if (turnoFoiConsumido) {
            *turnoFoiConsumido = true;
            std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Turno gasto alterando um equipamento...\n";
        }
        Aparencia::aguardarEnter();
        return;
    }
    
    if (itemEncontrado->usarDoInventario(jogadorAtual, turnoFoiConsumido))
    {
        // Se o item for Debuff (que seta o ponteiro para pedir alvo), ignorar enter
        if (!jogadorAtual->obterItemSelecionadoParaUso()) {
            Aparencia::aguardarEnter();
        }
        return;
    }

    std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Este item nao pode ser usado " << (turnoFoiConsumido ? "em combate!" : "fora de combate!") << "\n";
    Aparencia::aguardarEnter();
}
