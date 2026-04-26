#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

#include "InventarioCombate.h"
#include "../Sistema/Personagem.h"
#include "../Sistema/Tipos.h"
#include "Item.h"
#include "../Interfaces/TelaInventario.h"
#include "../Sistema/FuncionalidadeMenu.h"
#include "../Sistema/SimplificacoesAparencia.h"

namespace {
    using EstrategiaUso = std::function<void(Personagem*, Item*, bool*)>;

    // Dicionario de Estrategias: Mapeia cada propriedade para sua funcao especifica
    std::unordered_map<Propriedade, EstrategiaUso> estrategiasConsumiveis = {
        {Propriedade::ConsumivelCura, [](Personagem* p, Item* i, bool* turno) {
            if (p->obterVida() >= p->obterVidaMaxima()) {
                std::cout << "\n[SISTEMA]: Sua vida ja esta cheia!\n";
                return;
            }
            int cura = static_cast<int>(p->obterVidaMaxima() * 0.30);
            p->modificarVida(cura);
            std::cout << "\n[SISTEMA]: " << i->obterNomeItem() << " usada! +" << cura << " HP.\n";
            p->obterInventario()->removerItem(i);
            if (turno) *turno = true;
        }},
        {Propriedade::ConsumivelBuff, [](Personagem* p, Item* i, bool* turno) {
            if (!turno) { std::cout << "\n[SISTEMA]: Pocoes de buff so podem ser usadas em combate!\n"; return; }
            p->adicionarEfeito(std::make_unique<EfeitoBuffAtributos>(2));
            p->definirMultiplicador(1.5);
            std::cout << "\n[SISTEMA]: " << i->obterNomeItem() << " consumida! Atributos ampliados em 1.5x por 2 turnos!\n";
            p->obterInventario()->removerItem(i);
            *turno = true;
        }},
        {Propriedade::ConsumivelDebuffLentidao, [](Personagem* p, Item* i, bool* turno) {
            if (!turno) { std::cout << "\n[SISTEMA]: Frascos de debuff so podem ser usados em combate!\n"; return; }
            p->definirItemSelecionadoParaUso(i);
        }},
        {Propriedade::ConsumivelDebuffFraqueza, [](Personagem* p, Item* i, bool* turno) {
            if (!turno) { std::cout << "\n[SISTEMA]: Frascos de debuff so podem ser usados em combate!\n"; return; }
            p->definirItemSelecionadoParaUso(i);
        }},
        {Propriedade::TalismaForca, [](Personagem* p, Item* i, bool* turno) {
            p->alterarAtributoEstatico(TipoAtributo::Forca, 5);
            p->alterarAtributoEstatico(TipoAtributo::Inteligencia, -5);
            std::cout << "\n[SISTEMA]: " << i->obterNomeItem() << " consumido!\n";
            p->obterInventario()->removerItem(i);
            if (turno) *turno = true;
        }},
        {Propriedade::TalismaInteligencia, [](Personagem* p, Item* i, bool* turno) {
            p->alterarAtributoEstatico(TipoAtributo::Inteligencia, 5);
            p->alterarAtributoEstatico(TipoAtributo::Forca, -5);
            std::cout << "\n[SISTEMA]: " << i->obterNomeItem() << " consumido!\n";
            p->obterInventario()->removerItem(i);
            if (turno) *turno = true;
        }},
        {Propriedade::TalismaDestreza, [](Personagem* p, Item* i, bool* turno) {
            p->alterarAtributoEstatico(TipoAtributo::Destreza, 5);
            p->alterarAtributoEstatico(TipoAtributo::Sabedoria, -5);
            std::cout << "\n[SISTEMA]: " << i->obterNomeItem() << " consumido!\n";
            p->obterInventario()->removerItem(i);
            if (turno) *turno = true;
        }},
        {Propriedade::TalismaSabedoria, [](Personagem* p, Item* i, bool* turno) {
            p->alterarAtributoEstatico(TipoAtributo::Sabedoria, 5);
            p->alterarAtributoEstatico(TipoAtributo::Destreza, -5);
            std::cout << "\n[SISTEMA]: " << i->obterNomeItem() << " consumido!\n";
            p->obterInventario()->removerItem(i);
            if (turno) *turno = true;
        }}
    };
}

void InventarioCombate::gerenciarInventario(Personagem* jogadorAtual, bool* turnoFoiConsumido)
{
    if (jogadorAtual == nullptr) return;
    int larguraDoTerminal = SimplificacoesAparencia::obterLarguraTerminal();
    std::string codigoDoItemDigitado;
    do 
    {
        TelaInventario::exibir(jogadorAtual);
        std::string mensagemDoPrompt = "Digite o codigo do item ou [0] VOLTAR: ";
        int espacos = std::max(0, (larguraDoTerminal - static_cast<int>(mensagemDoPrompt.length())) / 2);
        std::cout << "\n" << std::string(espacos, ' ') << mensagemDoPrompt;
        std::cin >> codigoDoItemDigitado;

        if (codigoDoItemDigitado != "0")
        {
            Item* itemEncontrado = jogadorAtual->obterInventario()->buscarItemPorCodigo(
                codigoDoItemDigitado, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura()
            );

            if (itemEncontrado)
            {
                processarUsoDeItem(jogadorAtual, itemEncontrado, turnoFoiConsumido);
                if (turnoFoiConsumido && *turnoFoiConsumido) {
                    break;
                }
            }
        }
    } while (codigoDoItemDigitado != "0" && !(turnoFoiConsumido && *turnoFoiConsumido));
}

void InventarioCombate::processarUsoDeItem(Personagem* jogadorAtual, Item* itemEncontrado, bool* turnoFoiConsumido)
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
