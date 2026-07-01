#pragma once
#include <map>
#include <string>
#include <functional>
#include <vector>
#include "../../Entidades/Personagem.h"
#include "../../Sistemas/Inventario/Item.h"
#include "../Utilidades/Aparencia.h"

struct ProdutoLoja {
    ItemID idItem;
    int preco;
    int quantidade; // -1 para infinito
};

class Loja {
public:
    static void processarCompra(Personagem* jogadorAtual, const std::string& tituloLoja, Cor corLoja, 
                                std::map<int, ProdutoLoja>& estoqueAtual, 
                                const std::function<void(const std::string&)>& exibirDialogoNPC, 
                                const std::function<std::string(ItemID)>& formatadorNomeExtra = nullptr,
                                const std::vector<std::string>& arteAscii = {});
};
