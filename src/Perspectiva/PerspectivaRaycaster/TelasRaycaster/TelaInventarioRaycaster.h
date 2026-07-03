#pragma once

#include <string>
#include <vector>

class Personagem;
class Item;

class TelaInventarioRaycaster {
public:
    static void exibirCabecalho(bool animar);
    static void exibirCaixaEquipados(Personagem* jogador);
    static void exibirDetalheItem(Item* item);
};
