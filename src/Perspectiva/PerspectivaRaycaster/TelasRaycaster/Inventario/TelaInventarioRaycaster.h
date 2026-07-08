#pragma once

#include <string>
#include <vector>
#include "../../../IInventarioUI.h"

class Personagem;
class Item;

class TelaInventarioRaycaster : public IInventarioUI {
public:
    void renderizarMenu(const std::vector<std::string>& linhas, const std::string& titulo, int selecaoAtual, int& outW, int& outH) override;
    void exibirCabecalho(bool ehIde, int startY) override;
    void exibirCaixaEquipados(Personagem* jogadorAtual) override;
    void exibirDetalheItem(Item* item) override;
};
