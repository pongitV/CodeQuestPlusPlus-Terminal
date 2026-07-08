#pragma once

#include "../../../Entidades/Personagem.h"
#include <utility>
#include <vector>
#include <string>

class TelaInventario 
{
public:
    static void exibirCaixaEquipados(Personagem* jogadorAtual);
    static std::vector<std::pair<std::string, Item*>> obterListaCategoria(Personagem* jogadorAtual, int categoria, bool mostrarPrecos = false);
    static void exibirInspecaoItem(Item* item, Personagem* jogadorAtual = nullptr);
    static void exibirCabecalhoInventario(bool animar = false, int startY = -1);
};
