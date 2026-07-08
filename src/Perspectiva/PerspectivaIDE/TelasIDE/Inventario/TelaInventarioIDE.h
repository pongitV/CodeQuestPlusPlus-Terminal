#pragma once

#include <string>
#include <vector>
#include <utility>

#include "../../../../Entidades/Personagem.h"

class Item;

class TelaInventarioIDE {
public:
    static void exibirCabecalhoInventario(bool animar = false);
    static void exibirCaixaEquipados(Personagem* jogadorAtual);
    static void exibirInspecaoItem(Item* item, Personagem* jogadorAtual = nullptr);
};
