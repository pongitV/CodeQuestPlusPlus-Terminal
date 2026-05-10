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
    // Executa a acao diretamente e retorna true caso a propriedade possua um efeito de consumivel
    bool executarEfeitoConsumivel(Propriedade prop, SistemaPersonagem* personagemUsuario, Item* itemConsumido, bool* turnoFoiConsumido) {
        auto aplicarTalisma = [&](TipoAtributo buffAtr, TipoAtributo debuffAtr) {
            personagemUsuario->alterarAtributoEstatico(buffAtr, 5);
            personagemUsuario->alterarAtributoEstatico(debuffAtr, -5);
            std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: " << itemConsumido->obterNomeItem() << " consumido!\n";
            personagemUsuario->obterInventario()->removerItem(itemConsumido);
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
            return true;
        };

        switch(prop) {
        case Propriedade::ConsumivelCura: {
            if (personagemUsuario->obterVida() >= personagemUsuario->obterVidaMaxima()) {
                std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Sua vida ja esta cheia!\n";
                return true;
            }
            int vidaAntes = personagemUsuario->obterVida();
            int curaEstimada = static_cast<int>(personagemUsuario->obterVidaMaxima() * 0.30);
            personagemUsuario->modificarVida(curaEstimada);
            int vidaDepois = personagemUsuario->obterVida();
            int curaReal = vidaDepois - vidaAntes;
            std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: " << itemConsumido->obterNomeItem() << " usada! +" << curaReal << " HP. (Vida atual: " << vidaDepois << "/" << personagemUsuario->obterVidaMaxima() << ")\n";
            personagemUsuario->obterInventario()->removerItem(itemConsumido);
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
            return true;
        }

        case Propriedade::ConsumivelBuff:
            if (!turnoFoiConsumido) { std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Pocoes de buff so podem ser usadas em combate!\n"; return true; }
            personagemUsuario->adicionarEfeito(std::make_unique<EfeitoBuffAtributos>(2));
            personagemUsuario->definirMultiplicador(1.5);
            std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: " << itemConsumido->obterNomeItem() << " consumida! Atributos ampliados em 1.5x por 2 turnos!\n";
            personagemUsuario->obterInventario()->removerItem(itemConsumido);
            *turnoFoiConsumido = true;
            return true;

        case Propriedade::ConsumivelDebuffLentidao:
        case Propriedade::ConsumivelDebuffFraqueza:
            if (!turnoFoiConsumido) { std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Frascos de debuff so podem ser usados em combate!\n"; return true; }
            personagemUsuario->definirItemSelecionadoParaUso(itemConsumido);
            return true;

        case Propriedade::TalismaForca:        return aplicarTalisma(TipoAtributo::Forca, TipoAtributo::Inteligencia);
        case Propriedade::TalismaInteligencia: return aplicarTalisma(TipoAtributo::Inteligencia, TipoAtributo::Forca);
        case Propriedade::TalismaDestreza:     return aplicarTalisma(TipoAtributo::Destreza, TipoAtributo::Sabedoria);
        case Propriedade::TalismaSabedoria:    return aplicarTalisma(TipoAtributo::Sabedoria, TipoAtributo::Destreza);

        case Propriedade::ConsumivelPoderTroll:
            if (personagemUsuario->possuiRegeneracaoTroll()) {
                std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: O poder regenerador do Troll ja corre em suas veias!\n";
            } else {
                personagemUsuario->desbloquearRegeneracaoTroll();
                personagemUsuario->modificarVida(personagemUsuario->obterVidaMaxima());
                std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: " << itemConsumido->obterNomeItem() << " consumido! Voce agora curara 100% do seu HP apos cada combate!\n";
                personagemUsuario->obterInventario()->removerItem(itemConsumido);
            }
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
            return true;

        default:
            return false;
        }
    }
}

void InventarioCombate::gerenciarInventario(SistemaPersonagem* jogadorAtual, bool* turnoFoiConsumido)
{
    if (jogadorAtual == nullptr) return;
    std::string codigoDoItemDigitado;
    do 
    {
        TelaInventario::exibir(jogadorAtual);
        Aparencia::exibirPrompt("Digite o codigo do item para interagir ou [0] VOLTAR: \033[s");

        Item* itemEncontrado = nullptr;
        while (true) {
            codigoDoItemDigitado = ControleDeInput::lerEntradaProtegida();
            if (codigoDoItemDigitado == "0") break;
            
            itemEncontrado = jogadorAtual->obterInventario()->buscarItemPorCodigo(
                codigoDoItemDigitado, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura()
            );
            if (itemEncontrado) break;
            
            std::cout << "\033[u\033[J";
        }

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
                                    std::cout << "\n" << Aparencia::margemCombate();
                                    quantidadeParaUsar = ControleDeInput::lerInteiroComLimites("Quantidade para usar (1 a " + std::to_string(qtdDisponivel) + "): ", 1, qtdDisponivel, false, Aparencia::margemCombate());
                                }
                            }
                            
                            std::string nomeItem = itemEncontrado->obterNomeItem();
                            bool consumiuAlgumTurno = false;
                            
                            for (int i = 0; i < quantidadeParaUsar; ++i) {
                                Item* itemAtual = nullptr;
                                for (auto* it : jogadorAtual->obterInventario()->obterTodosOsItens()) {
                                    if (it->obterNomeItem() == nomeItem && it != jogadorAtual->obterArma() && it != jogadorAtual->obterEscudo() && it != jogadorAtual->obterArmadura()) {
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

    TipoEquipamento tipoDoItem = itemEncontrado->obterTipo();
    if (tipoDoItem == TipoEquipamento::ARMA || tipoDoItem == TipoEquipamento::ESCUDO || tipoDoItem == TipoEquipamento::ARMADURA)
    {
        if (itemEncontrado == jogadorAtual->obterArma()) {
            jogadorAtual->desequiparArma();
            std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: " << itemEncontrado->obterNomeItem() << " desequipado(a)!\n";
        } else if (itemEncontrado == jogadorAtual->obterEscudo()) {
            jogadorAtual->desequiparEscudo();
            std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: " << itemEncontrado->obterNomeItem() << " desequipado(a)!\n";
        } else if (itemEncontrado == jogadorAtual->obterArmadura()) {
            jogadorAtual->desequiparArmadura();
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
    
    // Executa a Estrategia O(1) do dicionario para Consumiveis
    for (Propriedade prop : itemEncontrado->obterPropriedades())
    {
        if (executarEfeitoConsumivel(prop, jogadorAtual, itemEncontrado, turnoFoiConsumido))
        {
            // Se o item for Debuff (que seta o ponteiro para pedir alvo), ignorar enter
            if (!jogadorAtual->obterItemSelecionadoParaUso()) {
                Aparencia::aguardarEnter();
            }
            return;
        }
    }

    std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Este item nao pode ser usado " << (turnoFoiConsumido ? "em combate!" : "fora de combate!") << "\n";
    Aparencia::aguardarEnter();
}
