#include "ControleInventario.h"
#include "Item.h"
#include "Equipamentos/EquipamentoArma.h"
#include "Equipamentos/EquipamentoEscudo.h"
#include "Equipamentos/EquipamentoArmadura.h"
#include "../../Entidades/Personagem.h"
#include <string>

UsoItemInfo ControleInventario::usarOuEquipar(Personagem* jogador, Item* item, bool turnoJaFoiConsumido) {
    if (turnoJaFoiConsumido) {
        return {ResultadoItem::Erro_TurnoJaUsado, "", "", false};
    }

    if (item->isEquipavel()) {
        if (item->obterTipo() == TipoEquipamento::ESCUDO && item->obterDurabilidadeAtualEscudo() <= 0) {
            return {ResultadoItem::Erro_EscudoQuebrado, item->obterNomeItem(), "", false};
        }

        bool desequipou = false;
        if (item == jogador->obterArma()) {
            jogador->desequiparArma();
            desequipou = true;
        } else if (item == jogador->obterEscudo()) {
            jogador->desequiparEscudo();
            desequipou = true;
        } else if (item == jogador->obterArmadura()) {
            jogador->desequiparArmadura();
            desequipou = true;
        }

        if (desequipou) {
            return {ResultadoItem::Desequipou, item->obterNomeItem(), "", true};
        }

        if (!item->podeSerEquipadoPor(jogador)) {
            return {ResultadoItem::Erro_Requisitos, item->obterNomeItem(), item->obterMensagemRequisito(), false};
        }

        jogador->equiparItem(item);
        return {ResultadoItem::Equipou, item->obterNomeItem(), "", true};
    }

    bool consumiu = false;
    if (item->usarDoInventario(jogador, &consumiu)) {
        return {ResultadoItem::Usou_Turno, item->obterNomeItem(), "", consumiu};
    }

    return {ResultadoItem::Erro_NaoPodeUsar, item->obterNomeItem(), "", false};
}

std::string ControleInventario::obterMensagemErro(Item* item, bool emCombate) {
    switch (item->obterTipo()) {
        case TipoEquipamento::MATERIAL:
            return "Materiais sao utilizados para NPCs especializados.";
        case TipoEquipamento::MISSAO:
            return "Itens de missao sao ativados automaticamente.";
        case TipoEquipamento::CONSUMIVEL:
            return "Este consumivel nao pode ser usado " + std::string(emCombate ? "no combate!" : "fora de combate!");
        default:
            return "Este item nao possui uso direto no inventario.";
    }
}
