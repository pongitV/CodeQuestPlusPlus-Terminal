#include "ItemConsumivel.h"
#include "../Sistema/Personagem.h"
#include <iostream>

ItemConsumivel::ItemConsumivel(std::string nome, int precoVenda) : Item(nome, precoVenda) {}

Raridade ItemConsumivel::obterRaridade() const {
    if (temPropriedade(Propriedade::TalismaForca) || temPropriedade(Propriedade::TalismaInteligencia) || 
        temPropriedade(Propriedade::TalismaDestreza) || temPropriedade(Propriedade::TalismaSabedoria)) return Raridade::RARO;
    return Raridade::COMUM;
}

TipoEquipamento ItemConsumivel::obterTipo() const { return TipoEquipamento::CONSUMIVEL; }

bool ItemConsumivel::aoUsar(Personagem& jogador) {
    if (temPropriedade(Propriedade::ConsumivelBuff)) {
        jogador.definirTurnosBuff(2);
        jogador.definirMultiplicador(1.5);
        std::cout << "\n[SISTEMA]: " << obterNomeItem() << " consumida! Atributos ampliados em 1.5x por 2 turnos!\n";
        return true;
    }
    if (temPropriedade(Propriedade::TalismaForca)) {
        jogador.alterarAtributoEstatico("forca", 5);
        jogador.alterarAtributoEstatico("inteligencia", -5);
        std::cout << "\n[SISTEMA]: " << obterNomeItem() << " consumido!\n";
        return true;
    }
    if (temPropriedade(Propriedade::TalismaInteligencia)) {
        jogador.alterarAtributoEstatico("inteligencia", 5);
        jogador.alterarAtributoEstatico("forca", -5);
        std::cout << "\n[SISTEMA]: " << obterNomeItem() << " consumido!\n";
        return true;
    }
    if (temPropriedade(Propriedade::TalismaDestreza)) {
        jogador.alterarAtributoEstatico("destreza", 5);
        jogador.alterarAtributoEstatico("sabedoria", -5);
        std::cout << "\n[SISTEMA]: " << obterNomeItem() << " consumido!\n";
        return true;
    }
    if (temPropriedade(Propriedade::TalismaSabedoria)) {
        jogador.alterarAtributoEstatico("sabedoria", 5);
        jogador.alterarAtributoEstatico("destreza", -5);
        std::cout << "\n[SISTEMA]: " << obterNomeItem() << " consumido!\n";
        return true;
    }
    if (temPropriedade(Propriedade::ConsumivelCura)) {
        if (jogador.obterVida() >= jogador.obterVidaMaxima()) {
            std::cout << "\n[SISTEMA]: Sua vida ja esta cheia!\n";
            return false;
        }
        int quantidadeDeCura = static_cast<int>(jogador.obterVidaMaxima() * 0.30);
        jogador.modificarVida(quantidadeDeCura);
        std::cout << "\n[SISTEMA]: " << obterNomeItem() << " usada! +" << quantidadeDeCura << " HP.\n";
        return true;
    }
    return false;
}