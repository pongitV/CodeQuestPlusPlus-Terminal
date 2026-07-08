#pragma once

class Personagem;
class Item;

class InventarioCombate
{
public:
    static void gerenciarInventario(Personagem* jogadorAtual, bool* turnoFoiConsumido = nullptr);
};
