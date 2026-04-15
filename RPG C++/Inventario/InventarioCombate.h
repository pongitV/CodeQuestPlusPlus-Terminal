#pragma once

class Personagem;

class InventarioCombate
{
public:
    static void gerenciarInventario(Personagem* jogadorAtual, bool* turnoFoiConsumido = nullptr);
};
