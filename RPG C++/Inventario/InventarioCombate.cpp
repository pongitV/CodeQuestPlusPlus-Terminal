#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

#include "InventarioCombate.h"
#include "../Sistemas/SistemaPersonagem.h"
#include "Item.h"
#include "EquipamentoArma.h"
#include "EquipamentoEscudo.h"
#include "EquipamentoArmadura.h"
#include "../Telas/TelaInventario.h"
#include "../Telas/TelaMenu.h"
#include "../Utilidades/SimplificacoesAparencia.h"

namespace {
    using EstrategiaUso = std::function<void(SistemaPersonagem* personagemUsuario, Item* itemConsumido, bool* turnoFoiConsumido)>;

    // Dicionario de Estrategias: Mapeia cada propriedade para sua funcao especifica
    std::unordered_map<Propriedade, EstrategiaUso> estrategiasConsumiveis = {
        {Propriedade::ConsumivelCura, [](SistemaPersonagem* personagemUsuario, Item* itemConsumido, bool* turnoFoiConsumido) {
            if (personagemUsuario->obterVida() >= personagemUsuario->obterVidaMaxima()) {
                std::cout << "\n[SISTEMA]: Sua vida ja esta cheia!\n";
                return;
            }
            int cura = static_cast<int>(personagemUsuario->obterVidaMaxima() * 0.30);
            personagemUsuario->modificarVida(cura);
            std::cout << "\n[SISTEMA]: " << itemConsumido->obterNomeItem() << " usada! +" << cura << " HP.\n";
            personagemUsuario->obterInventario()->removerItem(itemConsumido);
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
        }},
        {Propriedade::ConsumivelBuff, [](SistemaPersonagem* personagemUsuario, Item* itemConsumido, bool* turnoFoiConsumido) {
            if (!turnoFoiConsumido) { std::cout << "\n[SISTEMA]: Pocoes de buff so podem ser usadas em combate!\n"; return; }
            personagemUsuario->adicionarEfeito(std::make_unique<EfeitoBuffAtributos>(2));
            personagemUsuario->definirMultiplicador(1.5);
            std::cout << "\n[SISTEMA]: " << itemConsumido->obterNomeItem() << " consumida! Atributos ampliados em 1.5x por 2 turnos!\n";
            personagemUsuario->obterInventario()->removerItem(itemConsumido);
            *turnoFoiConsumido = true;
        }},
        {Propriedade::ConsumivelDebuffLentidao, [](SistemaPersonagem* personagemUsuario, Item* itemConsumido, bool* turnoFoiConsumido) {
            if (!turnoFoiConsumido) { std::cout << "\n[SISTEMA]: Frascos de debuff so podem ser usados em combate!\n"; return; }
            personagemUsuario->definirItemSelecionadoParaUso(itemConsumido);
        }},
        {Propriedade::ConsumivelDebuffFraqueza, [](SistemaPersonagem* personagemUsuario, Item* itemConsumido, bool* turnoFoiConsumido) {
            if (!turnoFoiConsumido) { std::cout << "\n[SISTEMA]: Frascos de debuff so podem ser usados em combate!\n"; return; }
            personagemUsuario->definirItemSelecionadoParaUso(itemConsumido);
        }},
        {Propriedade::TalismaForca, [](SistemaPersonagem* personagemUsuario, Item* itemConsumido, bool* turnoFoiConsumido) {
            personagemUsuario->alterarAtributoEstatico(TipoAtributo::Forca, 5);
            personagemUsuario->alterarAtributoEstatico(TipoAtributo::Inteligencia, -5);
            std::cout << "\n[SISTEMA]: " << itemConsumido->obterNomeItem() << " consumido!\n";
            personagemUsuario->obterInventario()->removerItem(itemConsumido);
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
        }},
        {Propriedade::TalismaInteligencia, [](SistemaPersonagem* personagemUsuario, Item* itemConsumido, bool* turnoFoiConsumido) {
            personagemUsuario->alterarAtributoEstatico(TipoAtributo::Inteligencia, 5);
            personagemUsuario->alterarAtributoEstatico(TipoAtributo::Forca, -5);
            std::cout << "\n[SISTEMA]: " << itemConsumido->obterNomeItem() << " consumido!\n";
            personagemUsuario->obterInventario()->removerItem(itemConsumido);
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
        }},
        {Propriedade::TalismaDestreza, [](SistemaPersonagem* personagemUsuario, Item* itemConsumido, bool* turnoFoiConsumido) {
            personagemUsuario->alterarAtributoEstatico(TipoAtributo::Destreza, 5);
            personagemUsuario->alterarAtributoEstatico(TipoAtributo::Sabedoria, -5);
            std::cout << "\n[SISTEMA]: " << itemConsumido->obterNomeItem() << " consumido!\n";
            personagemUsuario->obterInventario()->removerItem(itemConsumido);
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
        }},
        {Propriedade::TalismaSabedoria, [](SistemaPersonagem* personagemUsuario, Item* itemConsumido, bool* turnoFoiConsumido) {
            personagemUsuario->alterarAtributoEstatico(TipoAtributo::Sabedoria, 5);
            personagemUsuario->alterarAtributoEstatico(TipoAtributo::Destreza, -5);
            std::cout << "\n[SISTEMA]: " << itemConsumido->obterNomeItem() << " consumido!\n";
            personagemUsuario->obterInventario()->removerItem(itemConsumido);
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
        }},
        {Propriedade::ConsumivelPoderTroll, [](SistemaPersonagem* personagemUsuario, Item* itemConsumido, bool* turnoFoiConsumido) {
            if (personagemUsuario->possuiRegeneracaoTroll()) {
                std::cout << "\n[SISTEMA]: O poder regenerador do Troll ja corre em suas veias!\n";
            } else {
                personagemUsuario->desbloquearRegeneracaoTroll();
                personagemUsuario->modificarVida(personagemUsuario->obterVidaMaxima());
                std::cout << "\n[SISTEMA]: " << itemConsumido->obterNomeItem() << " consumido! Voce agora curara 100% do seu HP apos cada combate!\n";
                personagemUsuario->obterInventario()->removerItem(itemConsumido);
            }
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
        }}
    };
}

void InventarioCombate::gerenciarInventario(SistemaPersonagem* jogadorAtual, bool* turnoFoiConsumido)
{
    if (jogadorAtual == nullptr) return;
    std::string codigoDoItemDigitado;
    do 
    {
        TelaInventario::exibir(jogadorAtual);
        TelaInventario::exibirPrompt("Digite o codigo do item para interagir ou [0] VOLTAR: ");
        std::cin >> codigoDoItemDigitado;
        std::cin.ignore(1000, '\n');

        if (codigoDoItemDigitado != "0")
        {
            Item* itemEncontrado = jogadorAtual->obterInventario()->buscarItemPorCodigo(
                codigoDoItemDigitado, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura()
            );

            if (itemEncontrado)
            {
                    bool acaoConcluida = false;
                    while (!acaoConcluida) {
                        TelaInventario::exibirMenuInteracaoItem(itemEncontrado);
                        std::string subOpcao;
                        std::cin >> subOpcao;
                        std::cin.ignore(1000, '\n');

                        if (subOpcao == "1") {
                            processarUsoDeItem(jogadorAtual, itemEncontrado, turnoFoiConsumido);
                            acaoConcluida = true;
                        } else if (subOpcao == "2") {
                            SimplificacoesAparencia::limparTela();
                            TelaMenu::exibirLogoDoJogo("INSPECAO DE ITEM");
                            itemEncontrado->exibirInspecao();
                            std::cout << "\n";
                            SimplificacoesAparencia::aguardarEnter();
                        } else if (subOpcao == "0") {
                            acaoConcluida = true;
                        }
                    }

                    if (turnoFoiConsumido && *turnoFoiConsumido) {
                        break;
                    }
            }
        }
    } while (codigoDoItemDigitado != "0" && !(turnoFoiConsumido && *turnoFoiConsumido));
}

void InventarioCombate::processarUsoDeItem(SistemaPersonagem* jogadorAtual, Item* itemEncontrado, bool* turnoFoiConsumido)
{
    if (turnoFoiConsumido && *turnoFoiConsumido) 
    {
        std::cout << "\n[SISTEMA]: Voce ja usou um item neste turno!\n";
        SimplificacoesAparencia::aguardarEnter();
        return;
    }

    TipoEquipamento tipoDoItem = itemEncontrado->obterTipo();
    if (tipoDoItem == TipoEquipamento::ARMA || tipoDoItem == TipoEquipamento::ESCUDO || tipoDoItem == TipoEquipamento::ARMADURA)
    {
        if (itemEncontrado == jogadorAtual->obterArma()) {
            jogadorAtual->desequiparArma();
            std::cout << "\n[SISTEMA]: " << itemEncontrado->obterNomeItem() << " desequipado(a)!\n";
        } else if (itemEncontrado == jogadorAtual->obterEscudo()) {
            jogadorAtual->desequiparEscudo();
            std::cout << "\n[SISTEMA]: " << itemEncontrado->obterNomeItem() << " desequipado(a)!\n";
        } else if (itemEncontrado == jogadorAtual->obterArmadura()) {
            jogadorAtual->desequiparArmadura();
            std::cout << "\n[SISTEMA]: " << itemEncontrado->obterNomeItem() << " desequipado(a)!\n";
        } else {
            if (!itemEncontrado->podeSerEquipadoPor(jogadorAtual)) {
                std::cout << itemEncontrado->obterMensagemRequisito();
                if (turnoFoiConsumido && !jogadorAtual->obterItemSelecionadoParaUso()) SimplificacoesAparencia::aguardarEnter();
                return;
            }
            jogadorAtual->equiparItem(itemEncontrado);
            std::cout << "\n[SISTEMA]: " << itemEncontrado->obterNomeItem() << " equipado(a)!\n";
        }

        if (turnoFoiConsumido) {
            *turnoFoiConsumido = true;
            std::cout << "[SISTEMA]: Turno gasto alterando um equipamento...\n";
        }
        SimplificacoesAparencia::aguardarEnter();
        return;
    }
    
    // Executa a Estrategia O(1) do dicionario para Consumiveis
    for (Propriedade prop : itemEncontrado->obterPropriedades())
    {
        auto it = estrategiasConsumiveis.find(prop);
        if (it != estrategiasConsumiveis.end())
        {
            it->second(jogadorAtual, itemEncontrado, turnoFoiConsumido);
            
            // Se o item for Debuff (que seta o ponteiro para pedir alvo), ignorar enter
            if (!jogadorAtual->obterItemSelecionadoParaUso()) {
                SimplificacoesAparencia::aguardarEnter();
            }
            return;
        }
    }

    std::cout << "\n[SISTEMA]: Este item nao pode ser usado " << (turnoFoiConsumido ? "em combate!" : "fora de combate!") << "\n";
    SimplificacoesAparencia::aguardarEnter();
}
