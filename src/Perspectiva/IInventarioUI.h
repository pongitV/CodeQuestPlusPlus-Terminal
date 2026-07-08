#pragma once
#include <string>
#include <vector>
#include "../Core/Utilidades/Aparencia.h"

class Personagem;
class Item;

class IInventarioUI {
public:
    virtual ~IInventarioUI() = default;
    virtual void exibirCabecalho(bool animar, int startY) = 0;
    virtual void exibirCaixaEquipados(Personagem* jogador) = 0;
    virtual void exibirDetalheItem(Item* item) = 0;
    virtual void renderizarMenu(const std::vector<std::string>& linhas, const std::string& titulo, int selecaoAtual, int& outW, int& outH) = 0;
};
