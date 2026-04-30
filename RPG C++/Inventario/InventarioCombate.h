#pragma once

class SistemaPersonagem;
class Item;

class InventarioCombate
{
public:
    static void gerenciarInventario(SistemaPersonagem* jogadorAtual, bool* turnoFoiConsumido = nullptr);

private:
    static void processarUsoDeItem(SistemaPersonagem* jogadorAtual, Item* itemEncontrado, bool* turnoFoiConsumido);
};
